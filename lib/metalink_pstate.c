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
#include <assert.h>

#include "metalink_pstm.h"
#include "metalink_helper.h"

metalink_pstate_t *new_metalink_pstate(void) {
  metalink_pstate_t *state;
  state = malloc(sizeof(metalink_pstate_t));
  if (state) {
    memset(state, 0, sizeof(metalink_pstate_t));
  }
  return state;
}

void delete_metalink_pstate(metalink_pstate_t *state) { free(state); }

/**
 * set error code to metalink_pctrl and transit to null state, where no further
 * state transition takes place.
 */
void error_handler(metalink_pstm_t *stm, metalink_error_t error) {
  metalink_pctrl_set_error(stm->ctrl, error);
  metalink_pstm_enter_null_state(stm);
}

/* null handler doing nothing */
void null_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                          const char **attrs) {
  (void)stm;
  (void)name;
  (void)ns_uri;
  (void)attrs;
}

void null_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char *characters) {
  (void)stm;
  (void)name;
  (void)ns_uri;
  (void)characters;
}

/* initial state */
void initial_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                             const char **attrs) {
  (void)stm;
  (void)ns_uri;

  if (name == METALINK_TOKEN_METALINK) {
    switch (ns_uri) {
    case METALINK_NS_V3: {
      const char *type;
      const char *origin;
      metalink_pctrl_set_version(stm->ctrl, METALINK_VERSION_3);

      type = attrs[METALINK_ATTR_TOKEN_TYPE];
      if (type && strcmp("dynamic", type) == 0) {
        metalink_pctrl_set_origin_dynamic(stm->ctrl, 1);
      }
      origin = attrs[METALINK_ATTR_TOKEN_ORIGIN];
      if (origin) {
        metalink_pctrl_set_origin(stm->ctrl, origin);
      }

      metalink_pstm_enter_metalink_state(stm);
      break;
    }
    case METALINK_NS_V4:
      metalink_pctrl_set_version(stm->ctrl, METALINK_VERSION_4);
      metalink_pstm_enter_metalink_state_v4(stm);
      break;
    default:
      metalink_pctrl_set_version(stm->ctrl, METALINK_VERSION_UNKNOWN);
      metalink_pstm_enter_skip_state(stm);
    }
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void initial_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                           const char *characters) {
  (void)stm;
  (void)name;
  (void)ns_uri;
  (void)characters;
}

/* skip state */
void skip_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                          const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  ++stm->state->skip_depth;
}

void skip_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char *characters) {
  (void)name;
  (void)ns_uri;
  (void)characters;

  if (--stm->state->skip_depth == 0) {
    metalink_pstm_exit_skip_state(stm);
  }
}

/* size state <size> */
void size_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                          const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void size_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char *characters) {
  long long int size = 0;

  (void)name;

  /* TODO evaluate endptr(2nd argument) */
  errno = 0;
  size = strtoll(characters, 0, 10);
  if (errno == ERANGE || size < 0) {
    /* overflow or parse error or negative integer detected. */
    /* current Metalink specification does not require size. */
    size = 0;
  }
  metalink_pctrl_file_set_size(stm->ctrl, size);

  switch (ns_uri) {
  case METALINK_NS_V3:
    metalink_pstm_enter_file_state(stm);
    break;
  case METALINK_NS_V4:
    metalink_pstm_enter_file_state_v4(stm);
    break;
  default:
    assert(0);
  }
}

/* version state <version> */
void version_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                             const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void version_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                           const char *characters) {
  metalink_error_t r;

  (void)name;

  r = metalink_pctrl_file_set_version(stm->ctrl, characters);
  if (r == 0) {
    metalink_pstm_enter_file_state(stm);
  } else {
    error_handler(stm, r);
  }

  switch (ns_uri) {
  case METALINK_NS_V3:
    metalink_pstm_enter_file_state(stm);
    break;
  case METALINK_NS_V4:
    metalink_pstm_enter_file_state_v4(stm);
    break;
  default:
    assert(0);
  }
}

/* language state <language> */
void language_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                              const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void language_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                            const char *characters) {
  metalink_error_t r;

  (void)name;

  r = metalink_pctrl_add_language(stm->ctrl, characters);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }

  switch (ns_uri) {
  case METALINK_NS_V3:
    metalink_pstm_enter_file_state(stm);
    break;
  case METALINK_NS_V4:
    metalink_pstm_enter_file_state_v4(stm);
    break;
  default:
    assert(0);
  }
}

/* os state <os> */
void os_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void os_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                      const char *characters) {
  metalink_error_t r;

  (void)name;

  r = metalink_pctrl_add_os(stm->ctrl, characters);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }

  switch (ns_uri) {
  case METALINK_NS_V3:
    metalink_pstm_enter_file_state(stm);
    break;
  case METALINK_NS_V4:
    metalink_pstm_enter_file_state_v4(stm);
    break;
  default:
    assert(0);
  }
}

/* url state <url> */
void url_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                         const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void url_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                       const char *characters) {
  metalink_error_t r;

  (void)name;

  r = metalink_pctrl_resource_set_url(stm->ctrl, characters);
  if (r != 0) {
    /* TODO clear intermidiate resource transaction. */
    error_handler(stm, r);
    return;
  }
  r = metalink_pctrl_commit_resource_transaction(stm->ctrl);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }

  switch (ns_uri) {
  case METALINK_NS_V3:
    metalink_pstm_enter_resources_state(stm);
    break;
  case METALINK_NS_V4:
    metalink_pstm_enter_file_state_v4(stm);
    break;
  default:
    assert(0);
  }
}

/* hash state <hash> */
void hash_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                          const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void hash_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char *characters) {
  metalink_error_t r;

  (void)name;

  r = metalink_pctrl_checksum_set_hash(stm->ctrl, characters);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }
  r = metalink_pctrl_commit_checksum_transaction(stm->ctrl);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }

  switch (ns_uri) {
  case METALINK_NS_V3:
    metalink_pstm_enter_verification_state(stm);
    break;
  case METALINK_NS_V4:
    metalink_pstm_enter_file_state_v4(stm);
    break;
  default:
    assert(0);
  }
}

/* piece hash state <hash> inside of <pieces> */
void piece_hash_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                                const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void piece_hash_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                              const char *characters) {
  metalink_error_t r;

  (void)name;

  metalink_pctrl_piece_hash_set_hash(stm->ctrl, characters);
  r = metalink_pctrl_commit_piece_hash_transaction(stm->ctrl);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }

  switch (ns_uri) {
  case METALINK_NS_V3:
    metalink_pstm_enter_pieces_state(stm);
    break;
  case METALINK_NS_V4:
    metalink_pstm_enter_pieces_state_v4(stm);
    break;
  default:
    assert(0);
  }
}

/* fin state */
void fin_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                         const char **attrs) {
  (void)stm;
  (void)name;
  (void)ns_uri;
  (void)attrs;
}

void fin_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                       const char *characters) {
  (void)stm;
  (void)name;
  (void)ns_uri;
  (void)characters;
}

#define lstreq(A, B, N) ((sizeof((A)) - 1) == (N) && memcmp((A), (B), (N)) == 0)

int metalink_lookup_token(const char *name, size_t namelen) {
  switch (namelen) {
  case 2:
    switch (name[1]) {
    case 's':
      if (lstreq("o", name, 1)) {
        return METALINK_TOKEN_OS;
      }
      break;
    }
    break;
  case 3:
    switch (name[2]) {
    case 'l':
      if (lstreq("ur", name, 2)) {
        return METALINK_TOKEN_URL;
      }
      break;
    }
    break;
  case 4:
    switch (name[3]) {
    case 'e':
      if (lstreq("fil", name, 3)) {
        return METALINK_TOKEN_FILE;
      }
      if (lstreq("siz", name, 3)) {
        return METALINK_TOKEN_SIZE;
      }
      break;
    case 'h':
      if (lstreq("has", name, 3)) {
        return METALINK_TOKEN_HASH;
      }
      break;
    case 'o':
      if (lstreq("log", name, 3)) {
        return METALINK_TOKEN_LOGO;
      }
      break;
    case 's':
      if (lstreq("tag", name, 3)) {
        return METALINK_TOKEN_TAGS;
      }
      break;
    }
    break;
  case 5:
    switch (name[4]) {
    case 's':
      if (lstreq("file", name, 4)) {
        return METALINK_TOKEN_FILES;
      }
      break;
    }
    break;
  case 6:
    switch (name[5]) {
    case 'n':
      if (lstreq("origi", name, 5)) {
        return METALINK_TOKEN_ORIGIN;
      }
      break;
    case 's':
      if (lstreq("piece", name, 5)) {
        return METALINK_TOKEN_PIECES;
      }
      break;
    }
    break;
  case 7:
    switch (name[6]) {
    case 'd':
      if (lstreq("update", name, 6)) {
        return METALINK_TOKEN_UPDATED;
      }
      break;
    case 'l':
      if (lstreq("metaur", name, 6)) {
        return METALINK_TOKEN_METAURL;
      }
      break;
    case 'n':
      if (lstreq("versio", name, 6)) {
        return METALINK_TOKEN_VERSION;
      }
      break;
    }
    break;
  case 8:
    switch (name[7]) {
    case 'e':
      if (lstreq("languag", name, 7)) {
        return METALINK_TOKEN_LANGUAGE;
      }
      break;
    case 'k':
      if (lstreq("metalin", name, 7)) {
        return METALINK_TOKEN_METALINK;
      }
      break;
    case 'y':
      if (lstreq("identit", name, 7)) {
        return METALINK_TOKEN_IDENTITY;
      }
      break;
    }
    break;
  case 9:
    switch (name[8]) {
    case 'd':
      if (lstreq("publishe", name, 8)) {
        return METALINK_TOKEN_PUBLISHED;
      }
      break;
    case 'e':
      if (lstreq("signatur", name, 8)) {
        return METALINK_TOKEN_SIGNATURE;
      }
      break;
    case 'r':
      if (lstreq("generato", name, 8)) {
        return METALINK_TOKEN_GENERATOR;
      }
      if (lstreq("publishe", name, 8)) {
        return METALINK_TOKEN_PUBLISHER;
      }
      break;
    case 's':
      if (lstreq("resource", name, 8)) {
        return METALINK_TOKEN_RESOURCES;
      }
      break;
    case 't':
      if (lstreq("copyrigh", name, 8)) {
        return METALINK_TOKEN_COPYRIGHT;
      }
      break;
    }
    break;
  case 11:
    switch (name[10]) {
    case 'n':
      if (lstreq("descriptio", name, 10)) {
        return METALINK_TOKEN_DESCRIPTION;
      }
      break;
    }
    break;
  case 12:
    switch (name[11]) {
    case 'n':
      if (lstreq("verificatio", name, 11)) {
        return METALINK_TOKEN_VERIFICATION;
      }
      break;
    }
    break;
  }
  return -1;
}

int metalink_lookup_attr_token(const char *name, size_t namelen) {
  switch (namelen) {
  case 3:
    switch (name[2]) {
    case 'l':
      if (lstreq("ur", name, 2)) {
        return METALINK_ATTR_TOKEN_URL;
      }
      break;
    }
    break;
  case 4:
    switch (name[3]) {
    case 'e':
      if (lstreq("nam", name, 3)) {
        return METALINK_ATTR_TOKEN_NAME;
      }
      if (lstreq("typ", name, 3)) {
        return METALINK_ATTR_TOKEN_TYPE;
      }
      break;
    }
    break;
  case 5:
    switch (name[4]) {
    case 'e':
      if (lstreq("piec", name, 4)) {
        return METALINK_ATTR_TOKEN_PIECE;
      }
      break;
    }
    break;
  case 6:
    switch (name[5]) {
    case 'h':
      if (lstreq("lengt", name, 5)) {
        return METALINK_ATTR_TOKEN_LENGTH;
      }
      break;
    case 'n':
      if (lstreq("origi", name, 5)) {
        return METALINK_ATTR_TOKEN_ORIGIN;
      }
      break;
    }
    break;
  case 7:
    switch (name[6]) {
    case 'c':
      if (lstreq("dynami", name, 6)) {
        return METALINK_ATTR_TOKEN_DYNAMIC;
      }
      break;
    }
    break;
  case 8:
    switch (name[7]) {
    case 'n':
      if (lstreq("locatio", name, 7)) {
        return METALINK_ATTR_TOKEN_LOCATION;
      }
      break;
    case 'y':
      if (lstreq("priorit", name, 7)) {
        return METALINK_ATTR_TOKEN_PRIORITY;
      }
      break;
    }
    break;
  case 9:
    switch (name[8]) {
    case 'e':
      if (lstreq("mediatyp", name, 8)) {
        return METALINK_ATTR_TOKEN_MEDIATYPE;
      }
      break;
    }
    break;
  case 10:
    switch (name[9]) {
    case 'e':
      if (lstreq("preferenc", name, 9)) {
        return METALINK_ATTR_TOKEN_PREFERENCE;
      }
      break;
    }
    break;
  case 14:
    switch (name[13]) {
    case 's':
      if (lstreq("maxconnection", name, 13)) {
        return METALINK_ATTR_TOKEN_MAXCONNECTIONS;
      }
      break;
    }
    break;
  }
  return -1;
}
