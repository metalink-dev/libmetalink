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
#ifndef _D_METALINK_PSTATE_H_
#define _D_METALINK_PSTATE_H_

#include "metalink_config.h"

#include <metalink/metalink.h>

#define METALINK_V3_NS_URI "http://www.metalinker.org/"
#define METALINK_V4_NS_URI "urn:ietf:params:xml:ns:metalink"

typedef enum { METALINK_NS_NONE, METALINK_NS_V3, METALINK_NS_V4 } metalink_ns;

typedef enum {
  METALINK_TOKEN_COPYRIGHT,
  METALINK_TOKEN_DESCRIPTION,
  METALINK_TOKEN_FILE,
  METALINK_TOKEN_FILES,
  METALINK_TOKEN_GENERATOR,
  METALINK_TOKEN_HASH,
  METALINK_TOKEN_IDENTITY,
  METALINK_TOKEN_LANGUAGE,
  METALINK_TOKEN_LOGO,
  METALINK_TOKEN_METALINK,
  METALINK_TOKEN_METAURL,
  METALINK_TOKEN_ORIGIN,
  METALINK_TOKEN_OS,
  METALINK_TOKEN_PIECES,
  METALINK_TOKEN_PUBLISHED,
  METALINK_TOKEN_PUBLISHER,
  METALINK_TOKEN_RESOURCES,
  METALINK_TOKEN_SIGNATURE,
  METALINK_TOKEN_SIZE,
  METALINK_TOKEN_TAGS,
  METALINK_TOKEN_UPDATED,
  METALINK_TOKEN_URL,
  METALINK_TOKEN_VERIFICATION,
  METALINK_TOKEN_VERSION
} metalink_token;

typedef enum {
  METALINK_ATTR_TOKEN_DYNAMIC,
  METALINK_ATTR_TOKEN_LENGTH,
  METALINK_ATTR_TOKEN_LOCATION,
  METALINK_ATTR_TOKEN_MAXCONNECTIONS,
  METALINK_ATTR_TOKEN_MEDIATYPE,
  METALINK_ATTR_TOKEN_NAME,
  METALINK_ATTR_TOKEN_ORIGIN,
  METALINK_ATTR_TOKEN_PIECE,
  METALINK_ATTR_TOKEN_PREFERENCE,
  METALINK_ATTR_TOKEN_PRIORITY,
  METALINK_ATTR_TOKEN_TYPE,
  METALINK_ATTR_TOKEN_URL,
  METALINK_ATTR_TOKEN_MAX
} metalink_attr_token;

int metalink_lookup_token(const char *name, size_t namelen);

int metalink_lookup_attr_token(const char *name, size_t namelen);

typedef struct _metalink_pstm metalink_pstm_t;

typedef void (*metalink_start_fun)(metalink_pstm_t *stm, int name, int ns_uri,
                                   const char **attr);

typedef void (*metalink_end_fun)(metalink_pstm_t *stm, int name, int ns_uri,
                                 const char *characters);

typedef struct _metalink_pstate {
  metalink_start_fun start_fun;

  metalink_end_fun end_fun;

  int character_buffering;

  /** follow three elements are used in skip state **/
  metalink_start_fun before_skip_state_start_fun;

  metalink_end_fun before_skip_state_end_fun;

  int before_skip_character_buffering;

  int skip_depth;

} metalink_pstate_t;

/* constructor */
metalink_pstate_t *new_metalink_pstate(void);

/* destructor */
void delete_metalink_pstate(metalink_pstate_t *state);

void error_handler(metalink_pstm_t *stm, metalink_error_t error);

/* null handler doing nothing */
void null_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                          const char **attrs);

void null_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char *characters);

/* initial state */
void initial_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                             const char **attrs);

void initial_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                           const char *characters);

/* skip state */
void skip_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                          const char **attrs);

void skip_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char *characters);

/* size state <size> */
void size_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                          const char **attrs);

void size_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char *characters);

/* version state <version> */
void version_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                             const char **attrs);

void version_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                           const char *characters);

/* language state <language> */
void language_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                              const char **attrs);

void language_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                            const char *characters);

/* os state <os> */
void os_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char **attrs);

void os_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                      const char *characters);

/* url state <url> */
void url_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                         const char **attrs);

void url_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                       const char *characters);

/* hash state <hash> */
void hash_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                          const char **attrs);

void hash_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                        const char *characters);

/* piece hash state <hash> inside of <pieces> */
void piece_hash_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                                const char **attrs);

void piece_hash_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                              const char *characters);

/* fin state */
void fin_state_start_fun(metalink_pstm_t *stm, int name, int ns_uri,
                         const char **attrs);

void fin_state_end_fun(metalink_pstm_t *stm, int name, int ns_uri,
                       const char *characters);

#endif /* _D_METALINK_PARSER_STATE_H_ */
