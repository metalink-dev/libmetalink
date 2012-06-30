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
#ifndef _D_METALINK_STRING_BUFFER_H_
#define _D_METALINK_STRING_BUFFER_H_

#include "metalink_config.h"

#include <stdlib.h>

#include <metalink/metalink.h>

typedef struct metalink_string_buffer_t {
  char* buffer;
  size_t length;
  size_t capacity;
} metalink_string_buffer_t;

metalink_string_buffer_t* metalink_string_buffer_new(size_t initial_capacity);

void metalink_string_buffer_delete(metalink_string_buffer_t* str_buf);

void metalink_string_buffer_append(metalink_string_buffer_t* str_buf,
				   const char* str, size_t length);

const char* metalink_string_buffer_str(const metalink_string_buffer_t* str_buf);

size_t metalink_string_buffer_capacity(const metalink_string_buffer_t* str_buf);

size_t metalink_string_buffer_strlen(const metalink_string_buffer_t* str_buf);

#endif /* _D_METALINK_STRING_BUFFER_H_ */
