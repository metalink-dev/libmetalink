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
#include <string.h>
#include <string_buffer.h>
#include <stdio.h>

string_buffer_t* new_string_buffer(size_t initial_capacity)
{
  string_buffer_t* sbuf = malloc(sizeof(string_buffer_t));
  sbuf->buffer = calloc(sizeof(char), initial_capacity+1);
  sbuf->length = 0;
  sbuf->buffer[sbuf->length] = '\0';
  sbuf->capacity = initial_capacity;
  return sbuf;
}

void delete_string_buffer(string_buffer_t* str_buf)
{
  if(str_buf) {
    free(str_buf->buffer);
    free(str_buf);
  }
}

static void string_buffer_resize(string_buffer_t* str_buf, size_t new_capacity)
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

void string_buffer_append(string_buffer_t* str_buf, const char* str, size_t length)
{
  if(str_buf->length+length > str_buf->capacity) {
    string_buffer_resize(str_buf, str_buf->length+length);
  }

  memcpy(str_buf->buffer+str_buf->length, str, length);
  str_buf->length += length;
  str_buf->buffer[str_buf->length] = '\0';
}

const char* string_buffer_str(const string_buffer_t* str_buf)
{
  return str_buf->buffer;
}

size_t string_buffer_capacity(const string_buffer_t* str_buf)
{
  return str_buf->capacity;
}

size_t string_buffer_strlen(const string_buffer_t* str_buf)
{
  return str_buf->length;
}
