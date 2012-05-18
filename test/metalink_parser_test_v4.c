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
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <CUnit/CUnit.h>
#include "metalink_parser_test.h"
#include "metalink_parser_test_v4.h"
#include "metalink/metalink_parser.h"

static void validate_result(volatile metalink_t* metalink)
{
  metalink_file_t* file;
  metalink_resource_t* resource;

  CU_ASSERT_STRING_EQUAL("MetalinkEditor/2.0dev", metalink->generator);

  /* count files */
  CU_ASSERT_EQUAL_FATAL(3, count_array((void**)metalink->files));

  /* check 1st file */
  file = metalink->files[0];
  CU_ASSERT_STRING_EQUAL("libmetalink-0.0.1.tar.bz2", file->name);
  CU_ASSERT_STRING_EQUAL("0.0.1", file->version);

  resource = file->resources[0];
  CU_ASSERT_EQUAL(100, resource->priority);

  resource = file->resources[1];
  CU_ASSERT_EQUAL(99, resource->priority);
}

void test_metalink_parse_file_v4(void)
{
  metalink_error_t r;
  metalink_t* metalink;

  r = metalink_parse_file("test2.xml", &metalink);
  CU_ASSERT_EQUAL_FATAL(0, r);

  validate_result(metalink);
}
