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
    metalink_pstm_enter_metalink_state(stm);
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
