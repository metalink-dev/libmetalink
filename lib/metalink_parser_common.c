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
#include "metalink_parser_common.h"
#include "metalink_pctrl.h"

metalink_error_t metalink_handle_parse_result(metalink_t** res,
				 metalink_session_data_t* session_data,
				 metalink_error_t parser_retval)
{
  metalink_error_t retval;
  if(parser_retval == 0 && session_data->stm->ctrl->error == 0) {
    *res = metalink_pctrl_detach_metalink(session_data->stm->ctrl);
  }

  if(parser_retval != 0) {
    /* TODO more detailed error handling for parser is desired. */
    retval = METALINK_ERR_PARSER_ERROR;
  } else {
    retval = metalink_pctrl_get_error(session_data->stm->ctrl);
  }
  return retval;
}
