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
#include "metalink_string_buffer.h"

#include <string.h>
#include <stdio.h>

metalink_string_buffer_t* metalink_string_buffer_new(size_t initial_capacity)
{
  metalink_string_buffer_t* sbuf = malloc(sizeof(metalink_string_buffer_t));
  sbuf->buffer = calloc(sizeof(char), initial_capacity+1);
  sbuf->length = 0;
  sbuf->buffer[sbuf->length] = '\0';
  sbuf->capacity = initial_capacity;
  return sbuf;
}

void metalink_string_buffer_delete(metalink_string_buffer_t* str_buf)
{
  if(str_buf) {
    free(str_buf->buffer);
    free(str_buf);
  }
}

static void metalink_string_buffer_resize(metalink_string_buffer_t* str_buf,
					  size_t new_capacity)
{
  char* new_buffer;

  new_buffer = realloc(str_buf->buffer, new_capacity+1);

  str_buf->buffer = new_buffer;
  str_buf->capacity = new_capacity;
  if(str_buf->length > new_capacity) {
    str_buf->length = new_capacity;
    str_buf->buffer[str_buf->length] = '\0';
  }
}

void metalink_string_buffer_append(metalink_string_buffer_t* str_buf,
				   const char* str, size_t length)
{
  if(str_buf->length+length > str_buf->capacity) {
    metalink_string_buffer_resize(str_buf, str_buf->length+length);
  }

  memcpy(str_buf->buffer+str_buf->length, str, length);
  str_buf->length += length;
  str_buf->buffer[str_buf->length] = '\0';
}

const char* metalink_string_buffer_str(const metalink_string_buffer_t* str_buf)
{
  return str_buf->buffer;
}

size_t metalink_string_buffer_capacity(const metalink_string_buffer_t* str_buf)
{
  return str_buf->capacity;
}

size_t metalink_string_buffer_strlen(const metalink_string_buffer_t* str_buf)
{
  return str_buf->length;
}
