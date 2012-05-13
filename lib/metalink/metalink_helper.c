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
#include "metalink_helper.h"
#include "metalink_config.h"

#include <string.h>
#include <stdlib.h>

static int starts_with(const char* a, const char *b)
{
  for(; *a && *b && *a == *b; ++a, ++b);
  return *b == '\0';
}

static int ends_with(const char* a, const char *b)
{
  size_t alen = strlen(a);
  size_t blen = strlen(b);
  if(alen < blen) {
    return 0;
  }
  a += alen-blen;
  return strcmp(a, b) == 0;
}

int metalink_check_safe_path(const char* path)
{
  size_t len;
  size_t i;
  if(!path) {
    return 0;
  }
  len = strlen(path);
  if(len == 0 || strcmp(path, ".") == 0 || strcmp(path, "..") == 0 ||
     path[0] == '/' || path[len-1] == '/') {
    return 0;
  }
  for(i = 0; i < len; ++i) {
    unsigned char c = path[i];
    if((0x00 <= c && c <= 0x1f) || c == 0x7f) {
      return 0;
    }
  }
  if(starts_with(path, "./") ||
     starts_with(path, "../") ||
     strstr(path, "/./") != NULL ||
     strstr(path, "/../") != NULL ||
     ends_with(path, "/.") ||
     ends_with(path, "/..")) {
    return 0;
  }
  return 1;
}
