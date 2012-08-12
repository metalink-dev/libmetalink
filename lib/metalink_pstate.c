/* <!-- copyright */
/*
 * libmetalink
 *
 * Copyright (c) 2008 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/* copyright --> */
#include "metalink_pstate.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "metalink_pstm.h"
#include "metalink_helper.h"

metalink_pstate_t* new_metalink_pstate(void)
{
  metalink_pstate_t* state;
  state = malloc(sizeof(metalink_pstate_t));
  if(state) {
    memset(state, 0, sizeof(metalink_pstate_t));
  }
  return state;
}

void delete_metalink_pstate(metalink_pstate_t* state)
{
  free(state);
}

const char* get_attribute_value(const char** attrs, const char* name)
{
  const char** p;
  const char* cname;

  if(attrs == NULL) {
    return NULL;
  }

  p = attrs;
  while(*p) {
    cname = *p++;
    if(*p == 0) {
      break;
    }
    if(strcmp(cname, name) == 0) {
      break;
    } else {
      ++p;
    }
  }
  return *p;
}

/**
 * set error code to metalink_pctrl and transit to null state, where no further
 * state transition takes place.
 */
void error_handler(metalink_pstm_t* stm, metalink_error_t error)
{
  metalink_pctrl_set_error(stm->ctrl, error);
  metalink_pstm_enter_null_state(stm);
}


/* null handler doing nothing */
void null_state_start_fun(metalink_pstm_t* stm,
			  const char* name,
			  const char* ns_uri,
			  const char** attrs) {}

void null_state_end_fun(metalink_pstm_t* stm,
			const char* name,
			const char* ns_uri,
			const char* characters) {}

/* initial state */
void initial_state_start_fun(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char** attrs)
{
  if(strcmp("metalink", name) == 0) {
    if (strcmp(METALINK_V3_NS_URI, ns_uri) == 0) {
      const char* type;
      const char* origin;
      metalink_pctrl_set_version(stm->ctrl, METALINK_VERSION_3);

      type = get_attribute_value(attrs, "type");
      if(type && strcmp("dynamic", type) == 0) {
        metalink_pctrl_set_origin_dynamic(stm->ctrl, 1);
      }
      origin = get_attribute_value(attrs, "origin");
      if(origin) {
        metalink_pctrl_set_origin(stm->ctrl, origin);
      }

      metalink_pstm_enter_metalink_state(stm);
    }
    else if (strcmp(METALINK_V4_NS_URI, ns_uri) == 0) {
      metalink_pctrl_set_version(stm->ctrl, METALINK_VERSION_4);
      metalink_pstm_enter_metalink_state_v4(stm);
    } else {
      metalink_pctrl_set_version(stm->ctrl, METALINK_VERSION_UNKNOWN);
      metalink_pstm_enter_skip_state(stm);
    }
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void initial_state_end_fun(metalink_pstm_t* stm,
			   const char* name, const char* ns_uri,
			   const char* characters) {}

/* skip state */
void skip_state_start_fun(metalink_pstm_t* stm,
			  const char* name, const char* ns_uri,
			  const char** attrs)
{
  ++stm->state->skip_depth;
}

void skip_state_end_fun(metalink_pstm_t* stm,
			const char* name, const char* ns_uri,
			const char* characters)
{
  if(--stm->state->skip_depth == 0) {
    metalink_pstm_exit_skip_state(stm);
  }
}

/* size state <size> */
void size_state_start_fun(metalink_pstm_t* stm,
			  const char* name, const char* ns_uri,
			  const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void size_state_end_fun(metalink_pstm_t* stm,
		        const char* name, const char* ns_uri,
		        const char* characters)
{
  long long int size = 0;

  /* TODO evaluate endptr(2nd argument) */
  errno = 0;
  size = strtoll(characters, 0, 10);
  if(errno == ERANGE || size < 0) {
    /* overflow or parse error or negative integer detected. */
    /* current Metalink specification does not require size. */
    size = 0;
  }
  metalink_pctrl_file_set_size(stm->ctrl, size);

  if(strcmp(METALINK_V3_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state(stm);
  } else if(strcmp(METALINK_V4_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state_v4(stm);
  } else {
    error_handler(stm, METALINK_ERR_NAMESPACE_ERROR);
  }
}

/* version state <version> */
void version_state_start_fun(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void version_state_end_fun(metalink_pstm_t* stm,
			   const char* name, const char* ns_uri,
			   const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_file_set_version(stm->ctrl, characters);
  if(r == 0) {
    metalink_pstm_enter_file_state(stm);
  } else {
    error_handler(stm, r);
  }

  if(strcmp(METALINK_V3_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state(stm);
  } else if(strcmp(METALINK_V4_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state_v4(stm);
  } else {
    error_handler(stm, METALINK_ERR_NAMESPACE_ERROR);
  }
}

/* language state <language> */
void language_state_start_fun(metalink_pstm_t* stm,
			      const char* name, const char* ns_uri,
			      const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void language_state_end_fun(metalink_pstm_t* stm,
			    const char* name, const char* ns_uri,
			    const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_add_language(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }

  if(strcmp(METALINK_V3_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state(stm);
  } else if(strcmp(METALINK_V4_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state_v4(stm);
  } else {
    error_handler(stm, METALINK_ERR_NAMESPACE_ERROR);
  }
}

/* os state <os> */
void os_state_start_fun(metalink_pstm_t* stm,
		        const char* name, const char* ns_uri,
		        const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void os_state_end_fun(metalink_pstm_t* stm,
		      const char* name, const char* ns_uri,
		      const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_add_os(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }

  if(strcmp(METALINK_V3_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state(stm);
  } else if(strcmp(METALINK_V4_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state_v4(stm);
  } else {
    error_handler(stm, METALINK_ERR_NAMESPACE_ERROR);
  }
}

/* url state <url> */
void url_state_start_fun(metalink_pstm_t* stm,
			 const char* name, const char* ns_uri,
			 const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void url_state_end_fun(metalink_pstm_t* stm,
		       const char* name, const char* ns_uri,
		       const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_resource_set_url(stm->ctrl, characters);
  if(r != 0) {
    /* TODO clear intermidiate resource transaction. */
    error_handler(stm, r);
    return;
  }
  r = metalink_pctrl_commit_resource_transaction(stm->ctrl);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }

  if(strcmp(METALINK_V3_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_resources_state(stm);
  } else if(strcmp(METALINK_V4_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state_v4(stm);
  } else {
    error_handler(stm, METALINK_ERR_NAMESPACE_ERROR);
  }
}

/* hash state <hash> */
void hash_state_start_fun(metalink_pstm_t* stm,
			  const char* name, const char* ns_uri,
			  const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void hash_state_end_fun(metalink_pstm_t* stm,
		        const char* name, const char* ns_uri,
		        const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_checksum_set_hash(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  r = metalink_pctrl_commit_checksum_transaction(stm->ctrl);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }

  if(strcmp(METALINK_V3_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_verification_state(stm);
  } else if(strcmp(METALINK_V4_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_file_state_v4(stm);
  } else {
    error_handler(stm, METALINK_ERR_NAMESPACE_ERROR);
  }
}

/* piece hash state <hash> inside of <pieces> */
void piece_hash_state_start_fun(metalink_pstm_t* stm,
			        const char* name, const char* ns_uri,
			        const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void piece_hash_state_end_fun(metalink_pstm_t* stm,
			      const char* name, const char* ns_uri,
			      const char* characters)
{
  metalink_error_t r;
  metalink_pctrl_piece_hash_set_hash(stm->ctrl, characters);
  r = metalink_pctrl_commit_piece_hash_transaction(stm->ctrl);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }

  if(strcmp(METALINK_V3_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_pieces_state(stm);
  } else if(strcmp(METALINK_V4_NS_URI, ns_uri) == 0) {
    metalink_pstm_enter_pieces_state_v4(stm);
  } else {
    error_handler(stm, METALINK_ERR_NAMESPACE_ERROR);
  }
}

/* fin state */
void fin_state_start_fun(metalink_pstm_t* stm,
			 const char* name, const char* ns_uri,
			 const char** attrs) {}

void fin_state_end_fun(metalink_pstm_t* stm,
		       const char* name, const char* ns_uri,
		       const char* characters) {}

