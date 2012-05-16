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
  if(strcmp("file", name) == 0) {
    //metalink_pstm_enter_file_state_v4(stm);
  }else if(strcmp("generator", name) == 0) {
    metalink_pstm_enter_generator_state(stm);
  }else if(strcmp("origin", name) == 0) {
    metalink_pstm_enter_origin_state(stm);
  }else if(strcmp("published", name) == 0) {
    //metalink_pstm_enter_published_state_v4(stm);
  }else if(strcmp("updated", name) == 0) {
    //metalink_pstm_enter_updated_state_v4(stm);
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void metalink_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters)
{
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
  if (strcmp("generator", name) == 0) {
    metalink_pctrl_set_generator(stm->ctrl, characters);
  }else {
    metalink_pstm_enter_skip_state(stm);
  }
}

/* origin state <generator> */
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
  if (strcmp("origin", name) == 0) {
    metalink_pctrl_set_origin(stm->ctrl, characters);
  }else {
    metalink_pstm_enter_skip_state(stm);
  }
}
