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
#include "metalink_helper_test.h"

#include <CUnit/CUnit.h>

#include "metalink/metalink_helper.h"

void test_metalink_check_safe_path(void)
{
  char ctrlchars[] = { 0x1f, 0x7f, 0x00 };
  char safechars[] = { 0xff, 0xff, 0x00 };
  CU_ASSERT(!metalink_check_safe_path(NULL));
  CU_ASSERT(!metalink_check_safe_path(""));
  CU_ASSERT(!metalink_check_safe_path("."));
  CU_ASSERT(!metalink_check_safe_path(".."));
  CU_ASSERT(!metalink_check_safe_path("/foo"));
  CU_ASSERT(!metalink_check_safe_path("foo/"));
  CU_ASSERT(!metalink_check_safe_path("foo/"));
  CU_ASSERT(!metalink_check_safe_path(ctrlchars));
  CU_ASSERT(!metalink_check_safe_path("./foo"));
  CU_ASSERT(!metalink_check_safe_path("../foo"));
  CU_ASSERT(!metalink_check_safe_path("foo/../bar"));
  CU_ASSERT(!metalink_check_safe_path("foo/./bar"));
  CU_ASSERT(!metalink_check_safe_path("foo/."));
  CU_ASSERT(!metalink_check_safe_path("foo/.."));

  CU_ASSERT(metalink_check_safe_path(safechars));
  CU_ASSERT(metalink_check_safe_path("foo"));
  CU_ASSERT(metalink_check_safe_path("foo/bar"));
  CU_ASSERT(metalink_check_safe_path("foo/bar.baz"));
  CU_ASSERT(metalink_check_safe_path(".foo"));
}
