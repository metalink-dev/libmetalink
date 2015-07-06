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
#include "metalink_pstate_v3.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "metalink_pstm.h"
#include "metalink_helper.h"

/* metalink state <metalink> */
void metalink_state_start_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                                 const char **attrs) {
  (void)attrs;

  if (ns_uri != METALINK_NS_V3) {
    metalink_pstm_enter_skip_state(stm);
    return;
  }

  switch (name) {
  case METALINK_TOKEN_TAGS:
    metalink_pstm_enter_tags_state(stm);
    break;
  case METALINK_TOKEN_IDENTITY:
    metalink_pstm_enter_identity_state(stm);
    break;
  case METALINK_TOKEN_FILES:
    metalink_pstm_enter_files_state(stm);
    break;
  default:
    metalink_pstm_enter_skip_state(stm);
  }
}

void metalink_state_end_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                               const char *characters) {
  (void)name;
  (void)ns_uri;
  (void)characters;

  metalink_pstm_enter_fin_state(stm);
}

/* identity state <identity> */
void identity_state_start_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                                 const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void identity_state_end_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                               const char *characters) {
  metalink_error_t r;

  (void)name;
  (void)ns_uri;

  r = metalink_pctrl_set_identity(stm->ctrl, characters);
  if (r == 0) {
    metalink_pstm_enter_metalink_state(stm);
  } else {
    error_handler(stm, r);
  }
}

/* tags state <tags> */
void tags_state_start_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                             const char **attrs) {
  (void)name;
  (void)ns_uri;
  (void)attrs;

  metalink_pstm_enter_skip_state(stm);
}

void tags_state_end_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                           const char *characters) {
  metalink_error_t r;

  (void)name;
  (void)ns_uri;

  r = metalink_pctrl_set_tags(stm->ctrl, characters);
  if (r == 0) {
    metalink_pstm_enter_metalink_state(stm);
  } else {
    error_handler(stm, r);
  }
}

/* files state <files> */
void files_state_start_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                              const char **attrs) {
  metalink_error_t r;

  if (ns_uri != METALINK_NS_V3) {
    metalink_pstm_enter_skip_state(stm);
    return;
  }

  if (name == METALINK_TOKEN_FILE) {
    const char *fname;
    metalink_file_t *file;

    fname = attrs[METALINK_ATTR_TOKEN_NAME];
    if (!metalink_check_safe_path(fname)) {
      /* name is required attribute. If name is NULL or it is not
         safe, skip this entry. */
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    file = metalink_pctrl_new_file_transaction(stm->ctrl);
    if (!file) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_pctrl_file_set_name(stm->ctrl, fname);
    if (r != 0) {
      error_handler(stm, r);
      return;
    }
    metalink_pstm_enter_file_state(stm);
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void files_state_end_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                            const char *characters) {
  metalink_error_t r;

  (void)name;
  (void)ns_uri;
  (void)characters;

  r = metalink_pctrl_metalink_accumulate_files(stm->ctrl);
  if (r == 0) {
    metalink_pstm_enter_metalink_state(stm);
  } else {
    error_handler(stm, r);
  }
}

/* file state <file> */
void file_state_start_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                             const char **attrs) {
  if (ns_uri != METALINK_NS_V3) {
    metalink_pstm_enter_skip_state(stm);
    return;
  }

  switch (name) {
  case METALINK_TOKEN_SIZE:
    metalink_pstm_enter_size_state(stm);
    break;
  case METALINK_TOKEN_VERSION:
    metalink_pstm_enter_version_state(stm);
    break;
  case METALINK_TOKEN_LANGUAGE:
    metalink_pstm_enter_language_state(stm);
    break;
  case METALINK_TOKEN_OS:
    metalink_pstm_enter_os_state(stm);
    break;
  case METALINK_TOKEN_VERIFICATION:
    metalink_pstm_enter_verification_state(stm);
    break;
  case METALINK_TOKEN_RESOURCES: {
    const char *value;
    long int maxconnections = 0;

    value = attrs[METALINK_ATTR_TOKEN_MAXCONNECTIONS];
    if (value) {
      errno = 0;
      maxconnections = strtol(value, 0, 10);
      if (errno == ERANGE || maxconnections < 0 || maxconnections > INT_MAX) {
        /* error, maxconnection is not positive integer. */
        maxconnections = 0;
      }
    }
    metalink_pctrl_file_set_maxconnections(stm->ctrl, (int)maxconnections);

    metalink_pstm_enter_resources_state(stm);
    break;
  }
  default:
    metalink_pstm_enter_skip_state(stm);
  }
}

void file_state_end_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                           const char *characters) {
  metalink_error_t r;

  (void)name;
  (void)ns_uri;
  (void)characters;

  r = metalink_pctrl_commit_file_transaction(stm->ctrl);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_files_state(stm);
}

/* resources state <resources> */
void resources_state_start_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                                  const char **attrs) {
  metalink_error_t r;

  if (ns_uri != METALINK_NS_V3) {
    metalink_pstm_enter_skip_state(stm);
    return;
  }

  if (name == METALINK_TOKEN_URL) {
    const char *type;
    const char *location;
    const char *value;
    long int preference = 0;
    long int maxconnections = 0;
    metalink_resource_t *resource;

    resource = metalink_pctrl_new_resource_transaction(stm->ctrl);
    if (!resource) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }

    type = attrs[METALINK_ATTR_TOKEN_TYPE];
    if (!type) {
      /* type attribute is required, but not found. Skip current url tag. */
      metalink_pstm_enter_skip_state(stm);
      return;
    }
    r = metalink_pctrl_resource_set_type(stm->ctrl, type);
    if (r != 0) {
      error_handler(stm, r);
      return;
    }

    location = attrs[METALINK_ATTR_TOKEN_LOCATION];
    if (location) {
      r = metalink_pctrl_resource_set_location(stm->ctrl, location);
      if (r != 0) {
        error_handler(stm, r);
        return;
      }
    }

    value = attrs[METALINK_ATTR_TOKEN_PREFERENCE];
    if (value) {
      errno = 0;
      preference = strtol(value, 0, 10);
      if (errno == ERANGE || preference < 0 || preference > INT_MAX) {
        /* error, preference is not positive integer. */
        preference = 0;
      }
    }
    metalink_pctrl_resource_set_preference(stm->ctrl, (int)preference);

    value = attrs[METALINK_ATTR_TOKEN_MAXCONNECTIONS];
    if (value) {
      errno = 0;
      maxconnections = strtol(value, 0, 10);
      if (errno == ERANGE || maxconnections < 0 || maxconnections > INT_MAX) {
        /* error, maxconnections is not positive integer. */
        maxconnections = 0;
      }
    }
    metalink_pctrl_resource_set_maxconnections(stm->ctrl, (int)maxconnections);

    metalink_pstm_enter_url_state(stm);
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void resources_state_end_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                                const char *characters) {
  (void)name;
  (void)ns_uri;
  (void)characters;

  metalink_pstm_enter_file_state(stm);
}

/* verification state <verification> */
void verification_state_start_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                                     const char **attrs) {
  metalink_error_t r;

  if (ns_uri != METALINK_NS_V3) {
    metalink_pstm_enter_skip_state(stm);
    return;
  }

  switch (name) {
  case METALINK_TOKEN_HASH: {
    const char *type;
    metalink_checksum_t *checksum;

    type = attrs[METALINK_ATTR_TOKEN_TYPE];
    if (!type) {
      /* type is required attribute, if not specified, then skip this tag */
      metalink_pstm_enter_skip_state(stm);
      return;
    }
    checksum = metalink_pctrl_new_checksum_transaction(stm->ctrl);
    if (!checksum) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_checksum_set_type(checksum, type);
    if (r != 0) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    metalink_pstm_enter_hash_state(stm);
    break;
  }
  case METALINK_TOKEN_PIECES: {
    const char *type;
    const char *value;
    long int length;
    metalink_chunk_checksum_t *chunk_checksum;

    type = attrs[METALINK_ATTR_TOKEN_TYPE];
    if (!type) {
      /* type is required attribute, so if not specified, then skip this tag. */
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    value = attrs[METALINK_ATTR_TOKEN_LENGTH];
    if (value) {
      errno = 0;
      length = strtol(value, 0, 10);
      if (errno == ERANGE || length < 0 || length > INT_MAX) {
        /* error, length is not positive integer. */
        /* length is required attribute, so if not specified, then skip this
         * tag*/
        metalink_pstm_enter_skip_state(stm);
        return;
      }
    } else {
      /* length is required attribute, so if not specified, then skip this tag*/
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    chunk_checksum = metalink_pctrl_new_chunk_checksum_transaction(stm->ctrl);
    if (!chunk_checksum) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_chunk_checksum_set_type(chunk_checksum, type);
    if (r != 0) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    metalink_chunk_checksum_set_length(chunk_checksum, (int)length);

    metalink_pstm_enter_pieces_state(stm);
    break;
  }
  default:
    metalink_pstm_enter_skip_state(stm);
  }
}

void verification_state_end_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                                   const char *characters) {
  (void)name;
  (void)ns_uri;
  (void)characters;

  metalink_pstm_enter_file_state(stm);
}

/* pieces state <pieces> */
void pieces_state_start_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                               const char **attrs) {
  if (ns_uri != METALINK_NS_V3) {
    metalink_pstm_enter_skip_state(stm);
    return;
  }

  if (name == METALINK_TOKEN_HASH) {
    const char *value;
    long int piece;
    metalink_piece_hash_t *piece_hash;

    value = attrs[METALINK_ATTR_TOKEN_PIECE];
    if (value) {
      errno = 0;
      piece = strtol(value, 0, 10);
      if (errno == ERANGE || piece < 0 || piece > INT_MAX) {
        /* error, piece is not positive integer. */
        /* piece is required attribute, but it is missing. Skip this tag. */
        metalink_pstm_enter_skip_state(stm);
        return;
      }
    } else {
      /* value is required attribute, but it is missing. Skip this tag. */
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    piece_hash = metalink_pctrl_new_piece_hash_transaction(stm->ctrl);
    if (!piece_hash) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    metalink_pctrl_piece_hash_set_piece(stm->ctrl, (int)piece);

    metalink_pstm_enter_piece_hash_state(stm);
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void pieces_state_end_fun_v3(metalink_pstm_t *stm, int name, int ns_uri,
                             const char *characters) {
  metalink_error_t r;

  (void)name;
  (void)ns_uri;
  (void)characters;

  r = metalink_pctrl_commit_chunk_checksum_transaction(stm->ctrl);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }

  metalink_pstm_enter_verification_state(stm);
}
