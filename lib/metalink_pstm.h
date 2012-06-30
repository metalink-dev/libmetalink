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
#ifndef _D_METALINK_PSTM_H_
#define _D_METALINK_PSTM_H_

#include "metalink_config.h"

#include <metalink/metalink.h>

#include "metalink_pstate.h"
#include "metalink_pstate_v3.h"
#include "metalink_pstate_v4.h"
#include "metalink_pctrl.h"

/* parser state machine */
struct _metalink_pstm {
  metalink_pctrl_t* ctrl;
  metalink_pstate_t* state;
} /* metalink_pstm_t */;

/* constructor */
metalink_pstm_t* new_metalink_pstm(void);

/* destructor */
void delete_metalink_pstm(metalink_pstm_t* stm);

/* Setting callback functions for start element and end element */
void metalink_pstm_set_fun(metalink_pstm_t* stm,
			   metalink_start_fun start_fun,
			   metalink_end_fun end_fun);

/**
 * Returns 1 if character buffering in XML parser is enabled, 
 * otherwise returns 0.
 */
int metalink_pstm_character_buffering_enabled(const metalink_pstm_t* stm);

/**
 * Enable character buffersing in XML parser.
 */
void metalink_pstm_enable_character_buffering(metalink_pstm_t* stm);

/**
 * Disable character buffersing in XML parser.
 */
void metalink_pstm_disable_character_buffering(metalink_pstm_t* stm);

/* functions for state transition */
void metalink_pstm_enter_null_state(metalink_pstm_t* stm);

void metalink_pstm_enter_metalink_state(metalink_pstm_t* stm);

void metalink_pstm_enter_metalink_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_identity_state(metalink_pstm_t* stm);

void metalink_pstm_enter_tags_state(metalink_pstm_t* stm);

void metalink_pstm_enter_files_state(metalink_pstm_t* stm);

void metalink_pstm_enter_file_state(metalink_pstm_t* stm);

void metalink_pstm_enter_size_state(metalink_pstm_t* stm);

void metalink_pstm_enter_version_state(metalink_pstm_t* stm);

void metalink_pstm_enter_language_state(metalink_pstm_t* stm);

void metalink_pstm_enter_os_state(metalink_pstm_t* stm);

void metalink_pstm_enter_resources_state(metalink_pstm_t* stm);

void metalink_pstm_enter_url_state(metalink_pstm_t* stm);

void metalink_pstm_enter_verification_state(metalink_pstm_t* stm);

void metalink_pstm_enter_hash_state(metalink_pstm_t* stm);

void metalink_pstm_enter_pieces_state(metalink_pstm_t* stm);

void metalink_pstm_enter_piece_hash_state(metalink_pstm_t* stm);

void metalink_pstm_enter_fin_state(metalink_pstm_t* stm);

void metalink_pstm_enter_skip_state(metalink_pstm_t* stm);

void metalink_pstm_exit_skip_state(metalink_pstm_t* stm);

/* Metalink 4 states */
void metalink_pstm_enter_file_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_generator_state(metalink_pstm_t* stm);

void metalink_pstm_enter_origin_state(metalink_pstm_t* stm);

void metalink_pstm_enter_description_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_copyright_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_identity_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_logo_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_signature_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_pieces_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_metaurl_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_published_state_v4(metalink_pstm_t* stm);

void metalink_pstm_enter_updated_state_v4(metalink_pstm_t* stm);

#endif /* _D_METALINK_PSTM_H_ */
