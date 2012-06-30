/* <!-- copyright */
/*
 * libmetalink
 *
 * Copyright (c) 2012 Tatsuhiro Tsujikawa
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
#ifndef _D_METALINK_H_
#define _D_METALINK_H_

/* Convenient header file to include all libmetalink include files. */

#include <metalink/metalinkver.h>
#include <metalink/metalink_error.h>
#include <metalink/metalink_types.h>
#include <metalink/metalink_parser.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Checks that the version of the library is equal or later than given
 * |req_version|. If |req_version| is NULL, this function just returns
 * the version string of the library. The |req_version| must be in the
 * format "$MAJOR.$MINOR.$PATCH".
 *
 * This function returns the version string of the library if the
 * version requirement is satisfied, or NULL.
 */
const char* metalink_check_version(const char* req_version);

#ifdef __cplusplus
}
#endif

#endif /* _D_METALINK_H_ */
