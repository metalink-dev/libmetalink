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

#include <metalink/metalink.h>

#include "metalink_helper.h"

void test_metalink_check_safe_path(void)
{
  char ctrlchars[] = { 0x1f, 0x7f, 0x00 };
  CU_ASSERT(!metalink_check_safe_path(NULL));
  CU_ASSERT(!metalink_check_safe_path(""));
  CU_ASSERT(!metalink_check_safe_path(" "));
  CU_ASSERT(!metalink_check_safe_path("  "));
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
  CU_ASSERT(!metalink_check_safe_path("A:"));
  CU_ASSERT(!metalink_check_safe_path("C:/foo"));
  CU_ASSERT(!metalink_check_safe_path("Z:/foo"));
  CU_ASSERT(!metalink_check_safe_path("a:/foo"));
  CU_ASSERT(!metalink_check_safe_path("c:/foo"));
  CU_ASSERT(!metalink_check_safe_path("z:/foo"));
  CU_ASSERT(!metalink_check_safe_path("C:\\foo"));
  CU_ASSERT(!metalink_check_safe_path("foo\\bar"));
  CU_ASSERT(!metalink_check_safe_path(" foo"));
  CU_ASSERT(!metalink_check_safe_path("foo "));
  CU_ASSERT(!metalink_check_safe_path(".foo"));
  CU_ASSERT(!metalink_check_safe_path("..foo"));
  CU_ASSERT(!metalink_check_safe_path("~foo"));
  CU_ASSERT(!metalink_check_safe_path("|foo"));
  CU_ASSERT(!metalink_check_safe_path("foo/.bar"));
  CU_ASSERT(!metalink_check_safe_path("foo/~bar"));
  CU_ASSERT(!metalink_check_safe_path("foo/|bar"));

  CU_ASSERT(metalink_check_safe_path("foo"));
  CU_ASSERT(metalink_check_safe_path("foo."));
  CU_ASSERT(metalink_check_safe_path("foo~"));
  CU_ASSERT(metalink_check_safe_path("foo/bar"));
  CU_ASSERT(metalink_check_safe_path("foo/bar.baz"));
}

static void format_version(char* buf, size_t len,
                           int major, int minor, int patch)
{
  snprintf(buf, len, "%d.%d.%d", major, minor, patch);
}

void test_metalink_check_version(void)
{
  char req_version[20];
  const char* res;
  format_version(req_version, sizeof(req_version),
                 LIBMETALINK_VERSION_MAJOR,
                 LIBMETALINK_VERSION_MINOR,
                 LIBMETALINK_VERSION_PATCH);
  res = metalink_check_version(req_version);
  CU_ASSERT(res);
  if(res) {
    CU_ASSERT(0 == strcmp(PACKAGE_VERSION, res));
  }

  format_version(req_version, sizeof(req_version),
                 LIBMETALINK_VERSION_MAJOR,
                 LIBMETALINK_VERSION_MINOR,
                 LIBMETALINK_VERSION_PATCH-1);
  res = metalink_check_version(req_version);
  CU_ASSERT(res);

  format_version(req_version, sizeof(req_version),
                 LIBMETALINK_VERSION_MAJOR,
                 LIBMETALINK_VERSION_MINOR-1,
                 LIBMETALINK_VERSION_PATCH);
  res = metalink_check_version(req_version);
  CU_ASSERT(res);

  format_version(req_version, sizeof(req_version),
                 LIBMETALINK_VERSION_MAJOR-1,
                 LIBMETALINK_VERSION_MINOR,
                 LIBMETALINK_VERSION_PATCH);
  res = metalink_check_version(req_version);
  CU_ASSERT(res);

  format_version(req_version, sizeof(req_version),
                 LIBMETALINK_VERSION_MAJOR,
                 LIBMETALINK_VERSION_MINOR,
                 LIBMETALINK_VERSION_PATCH+1);
  res = metalink_check_version(req_version);
  CU_ASSERT(!res);

  format_version(req_version, sizeof(req_version),
                 LIBMETALINK_VERSION_MAJOR,
                 LIBMETALINK_VERSION_MINOR+1,
                 LIBMETALINK_VERSION_PATCH);
  res = metalink_check_version(req_version);
  CU_ASSERT(!res);

  format_version(req_version, sizeof(req_version),
                 LIBMETALINK_VERSION_MAJOR+1,
                 LIBMETALINK_VERSION_MINOR,
                 LIBMETALINK_VERSION_PATCH);
  res = metalink_check_version(req_version);
  CU_ASSERT(!res);

  res = metalink_check_version("0.0");
  CU_ASSERT(!res);

  res = metalink_check_version(NULL);
  CU_ASSERT(res);
}
