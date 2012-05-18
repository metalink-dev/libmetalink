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
#include "metalink_pstm.h"
#include "metalink_config.h"

#include <string.h>

metalink_pstm_t* new_metalink_pstm(void)
{
  metalink_pstm_t* stm;

  stm = malloc(sizeof(metalink_pstm_t));
  if(!stm) {
    return NULL;
  }
  stm->ctrl = new_metalink_pctrl();
  if(!stm->ctrl) {
    goto NEW_METALINK_PSTM_ERROR;
  }
  stm->state = new_metalink_pstate();
  if(!stm->state) {
    goto NEW_METALINK_PSTM_ERROR;
  }

  metalink_pstm_set_fun(stm, &initial_state_start_fun, &initial_state_end_fun);

  return stm;

 NEW_METALINK_PSTM_ERROR:
  delete_metalink_pstm(stm);
  return NULL;
}

void delete_metalink_pstm(metalink_pstm_t* stm)
{
  if(!stm) {
    return;
  }
  delete_metalink_pctrl(stm->ctrl);
  delete_metalink_pstate(stm->state);
  free(stm);
}

int metalink_pstm_character_buffering_enabled(const metalink_pstm_t* stm)
{
  return stm->state->character_buffering;
}

void metalink_pstm_enable_character_buffering(metalink_pstm_t* stm)
{
  stm->state->character_buffering = 1;
}

void metalink_pstm_disable_character_buffering(metalink_pstm_t* stm)
{
  stm->state->character_buffering = 0;
}

void metalink_pstm_set_fun(metalink_pstm_t* stm,
			   metalink_start_fun start_fun,
			   metalink_end_fun end_fun)
{
  stm->state->start_fun = start_fun;
  stm->state->end_fun = end_fun;
}

void metalink_pstm_enter_null_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &null_state_start_fun, &null_state_end_fun);
  metalink_pstm_disable_character_buffering(stm);
}

void metalink_pstm_enter_metalink_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &metalink_state_start_fun_v3, &metalink_state_end_fun_v3);
  metalink_pstm_disable_character_buffering(stm);
}

void metalink_pstm_enter_metalink_state_v4(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &metalink_state_start_fun_v4, &metalink_state_end_fun_v4);
  metalink_pstm_disable_character_buffering(stm);
}

void metalink_pstm_enter_identity_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &identity_state_start_fun_v3, &identity_state_end_fun_v3);
  metalink_pstm_disable_character_buffering(stm);
}

void metalink_pstm_enter_tags_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &tags_state_start_fun_v3, &tags_state_end_fun_v3);
  metalink_pstm_disable_character_buffering(stm);
}

void metalink_pstm_enter_files_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &files_state_start_fun_v3, &files_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_file_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &file_state_start_fun_v3, &file_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_size_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &size_state_start_fun_v3, &size_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_version_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &version_state_start_fun_v3, &version_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_language_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &language_state_start_fun_v3, &language_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_os_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &os_state_start_fun_v3, &os_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_resources_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &resources_state_start_fun_v3, &resources_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_url_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &url_state_start_fun_v3, &url_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_verification_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &verification_state_start_fun_v3,
			&verification_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_hash_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &hash_state_start_fun_v3, &hash_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_pieces_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &pieces_state_start_fun_v3, &pieces_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_piece_hash_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &piece_hash_state_start_fun_v3,
			&piece_hash_state_end_fun_v3);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_fin_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &fin_state_start_fun_v3, &fin_state_end_fun_v3);
  metalink_pstm_disable_character_buffering(stm);
}

void metalink_pstm_enter_skip_state(metalink_pstm_t* stm)
{					   
  stm->state->before_skip_state_start_fun = stm->state->start_fun;
  stm->state->before_skip_state_end_fun = stm->state->end_fun;

  metalink_pstm_set_fun(stm, &skip_state_start_fun, &skip_state_end_fun);

  metalink_pstm_disable_character_buffering(stm);
  stm->state->skip_depth = 1;
}

void metalink_pstm_exit_skip_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm,
			stm->state->before_skip_state_start_fun,
			stm->state->before_skip_state_end_fun);

  if(stm->state->before_skip_character_buffering) {
    metalink_pstm_enable_character_buffering(stm);
  } else {
    metalink_pstm_disable_character_buffering(stm);
  }
}

/* Metalink 4 states */
void metalink_pstm_enter_generator_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &generator_state_start_fun_v4,
			&generator_state_end_fun_v4);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_origin_state(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &origin_state_start_fun_v4,
			&origin_state_end_fun_v4);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_file_state_v4(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &file_state_start_fun_v4,
			&file_state_end_fun_v4);
  metalink_pstm_disable_character_buffering(stm);
}

void metalink_pstm_enter_url_state_v4(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &url_state_start_fun_v4,
			&url_state_end_fun_v4);
  metalink_pstm_enable_character_buffering(stm);
}

void metalink_pstm_enter_version_state_v4(metalink_pstm_t* stm)
{
  metalink_pstm_set_fun(stm, &version_state_start_fun_v4,
			&version_state_end_fun_v4);
  metalink_pstm_enable_character_buffering(stm);
}

