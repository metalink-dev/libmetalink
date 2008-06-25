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
#ifndef _D_METALINK_PARSER_H_
#define _D_METALINK_PARSER_H__

#include <metalink/metalink_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Parses metalink XML file.
 * @param filename path to Metalink XML file to be parsed.
 * @param res a dynamically allocated metalink_t structure as a result of
 * parsing.
 * @return 0 for success, non-zero for error. See metalink_error.h for
 * the meaning of error code.
 */
int metalink_parse_file(const char* filename, metalink_t** res);

/*
 * Frees the memory allocated for res
 */
void metalink_free(metalink_t* res);

#ifdef __cplusplus
}
#endif

#endif // _D_METALINK_PARSER_H_
