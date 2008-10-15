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
#include <session_data.h>
#include <string_buffer.h>

session_data_t* new_session_data()
{
  session_data_t* sd;
  sd = malloc(sizeof(session_data_t));
  if(!sd) {
    return NULL;
  }
  sd->stm = new_metalink_pstm();
  if(!sd->stm) {
    goto NEW_SESSION_DATA_ERROR;
  }
  sd->characters_stack = new_stack();
  if(!sd->characters_stack) {
    goto NEW_SESSION_DATA_ERROR;
  }
  return sd;
 NEW_SESSION_DATA_ERROR:
  delete_session_data(sd);
  return NULL;
}

void delete_session_data(session_data_t* sd)
{
  if(!sd) {
    return;
  }
  delete_metalink_pstm(sd->stm);
  while(!is_stack_empty(sd->characters_stack)) {
    delete_string_buffer(stack_pop(sd->characters_stack));
  }
  release_stack(sd->characters_stack);
  free(sd);
}
