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
#include "metalink_list_test.h"

#include <CUnit/CUnit.h>

#include "metalink_list.h"

void test_metalink_list(void)
{
  metalink_list_t* l;
  int a, b, c;
  int* int_ptr_array[3];

  l = metalink_list_new();
  CU_ASSERT_PTR_NOT_NULL(l);

  /* Add 3 ints. */
  a = 1;
  b = 2;
  c = 4;
  metalink_list_append(l, &a);
  metalink_list_append(l, &b);
  metalink_list_append(l, &c);
  CU_ASSERT_EQUAL(3, metalink_list_length(l));

  /* get 2nd(index = 1) element = (2) */
  CU_ASSERT_EQUAL(2, *(int*)metalink_list_get_data(l, 1));
 
  /* dump the contents of list to array */
  metalink_list_to_array(l, (void**)int_ptr_array);
  CU_ASSERT_EQUAL(1, *int_ptr_array[0]);
  CU_ASSERT_EQUAL(2, *int_ptr_array[1]);
  CU_ASSERT_EQUAL(4, *int_ptr_array[2]);

  /* clear all data */
  metalink_list_clear(l);
  CU_ASSERT_EQUAL(0, metalink_list_length(l));

  /* delete list */
  metalink_list_delete(l);
}
