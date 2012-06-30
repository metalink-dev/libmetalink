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
#ifndef _D_METALINK_PARSER_COMMON_H_
#define _D_METALINK_PARSER_COMMON_H_

#include "metalink_config.h"

#include <metalink/metalink.h>

#include "metalink_session_data.h"

/*
 * See session_data and parser_retval which is a return value of parser object
 * and decide the actual return value of metalink_parse_* function and fills
 * res.
 * @param res If the function decided the parse operation is succeeded, then
 *  res is filled with actual metalink_t pointer which is detached from
 *  session_data.
 * @param session_data session_data used in parser operation.
 * @param parser_retval return value of parser object.
 * @return error code listed in metalink_error.h if error, otherwise 0 for
 *  success.
 * 
 */
metalink_error_t metalink_handle_parse_result(metalink_t** res,
				 metalink_session_data_t* session_data,
				 metalink_error_t parser_retval);


#endif /* _D_METALINK_PARSER_COMMON_H_ */
