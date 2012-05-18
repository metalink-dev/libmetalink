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
#include "metalink_config.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "metalink_pstm.h"
#include "metalink_helper.h"

/* metalink state <metalink> */
void metalink_state_start_fun_v4(metalink_pstm_t* stm,
				 const char* name, const char* ns_uri,
				 const char** attrs)
{
  metalink_error_t r;

  if(strcmp("file", name) == 0) {
    const char* fname;
    metalink_file_t* file;

    fname = get_attribute_value(attrs, "name");
    if(!metalink_check_safe_path(fname)) {
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    file = metalink_pctrl_new_file_transaction(stm->ctrl);
    if(!file) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_pctrl_file_set_name(stm->ctrl, fname);
    if(r !=  0) {
      error_handler(stm, r);
      return;
    }

    metalink_pstm_enter_file_state_v4(stm);
  } else if(strcmp("generator", name) == 0) {
    metalink_pstm_enter_generator_state(stm);
  } else if(strcmp("origin", name) == 0) {
    metalink_pstm_enter_origin_state(stm);
  } else if(strcmp("published", name) == 0) {
    /*metalink_pstm_enter_published_state_v4(stm);*/
  } else if(strcmp("updated", name) == 0) {
    /*metalink_pstm_enter_updated_state_v4(stm);*/
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void metalink_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_metalink_accumulate_files(stm->ctrl);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_fin_state(stm);
}

/* generator state <generator> */
void generator_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void generator_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters)
{
  if(strcmp("generator", name) == 0) {
    metalink_pctrl_set_generator(stm->ctrl, characters);
  } else {
    metalink_pstm_enter_skip_state(stm);
    return;
  }
  metalink_pstm_enter_metalink_state_v4(stm);
}

/* origin state <origin> */
void origin_state_start_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void origin_state_end_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char* characters)
{
  if(strcmp("origin", name) == 0) {
    metalink_pctrl_set_origin(stm->ctrl, characters);
  } else {
    metalink_pstm_enter_skip_state(stm);
    return;
  }
  metalink_pstm_enter_metalink_state_v4(stm);
}

/* file state <file> */
void file_state_start_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char** attrs)
{
  metalink_error_t r;

  if(strcmp("url", name) == 0) {
    const char* location;
    const char* value;
    long int priority = 0;
    metalink_resource_t* resource;

    resource = metalink_pctrl_new_resource_transaction(stm->ctrl);
    if(!resource) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }

    location = get_attribute_value(attrs, "location");
    if(location) {
      r = metalink_pctrl_resource_set_location(stm->ctrl, location);
      if(r != 0) {
        error_handler(stm, r);
        return;
      }
    }

    value = get_attribute_value(attrs, "priority");
    if(value) {
      errno = 0;
      priority = strtol(value, 0, 10);
      if(errno == ERANGE || priority < 0 || priority > INT_MAX) {
        priority = 0;
      }
    }
    metalink_pctrl_resource_set_priority(stm->ctrl, priority);

    metalink_pstm_enter_url_state(stm);
  } else if(strcmp("description", name) == 0) {
    metalink_pstm_enter_description_state_v4(stm);
  } else if(strcmp("hash", name) == 0) {
    const char* type;
    metalink_checksum_t* checksum;

    type = get_attribute_value(attrs, "type");
    if(!type) {
      metalink_pstm_enter_skip_state(stm);
      return;
    }
    checksum = metalink_pctrl_new_checksum_transaction(stm->ctrl);
    if(!checksum) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_checksum_set_type(checksum, type);
    if(r != 0) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    metalink_pstm_enter_hash_state(stm);
  } else if(strcmp("size", name) == 0) {
    metalink_pstm_enter_size_state(stm);
  } else if(strcmp("version", name) == 0) {
    metalink_pstm_enter_version_state(stm);
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void file_state_end_fun_v4(metalink_pstm_t* stm,
			   const char* name, const char* ns_uri,
			   const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_commit_file_transaction(stm->ctrl);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_metalink_state_v4(stm);
}

/* url state <url> */
void url_state_start_fun_v4(metalink_pstm_t* stm,
			    const char* name, const char* ns_uri,
			    const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void url_state_end_fun_v4(metalink_pstm_t* stm,
			  const char* name, const char* ns_uri,
			  const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_resource_set_url(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  r = metalink_pctrl_commit_resource_transaction(stm->ctrl);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_file_state_v4(stm);
}

/* description state <description> */
void description_state_start_fun_v4(metalink_pstm_t* stm,
				    const char* name, const char* ns_uri,
				    const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void description_state_end_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_file_set_description(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_file_state_v4(stm);
}

/* hash state <hash> */
void hash_state_start_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void hash_state_end_fun_v4(metalink_pstm_t* stm,
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
  metalink_pstm_enter_file_state_v4(stm);
}

/* size state <size> */
void size_state_start_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void size_state_end_fun_v4(metalink_pstm_t* stm,
			   const char* name, const char* ns_uri,
			   const char* characters)
{
  long long int size = 0;

  errno = 0;
  size = strtoll(characters, 0, 10);
  if(errno == ERANGE || size < 0) {
    size = 0;
  }
  metalink_pctrl_file_set_size(stm->ctrl, size);

  metalink_pstm_enter_file_state_v4(stm);
}

/* version state <version> */
void version_state_start_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void version_state_end_fun_v4(metalink_pstm_t* stm,
			      const char* name, const char* ns_uri,
			      const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_file_set_version(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_file_state_v4(stm);
}
