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

#include <string.h>
#include <stdlib.h>

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
  /* See discussions of treating filename from the remote server in
     http://tools.ietf.org/html/rfc6266#section-4.3,
     http://tools.ietf.org/html/rfc2183#section-5 and
     http://tools.ietf.org/html/rfc5854.html#section-4.1.2.1
   */
  size_t len, i;
  ssize_t filename_idx = 0;
  /* If path or filename (string following the final '/' in path)
     start with one of the characters in bad_prefix, we consider it as
     invalid. */
  const char bad_prefix[] = " .~/";
  /* If path ends with one of the characters in bad_suffix, we
     consider it as invalid. */
  const char bad_suffix[] = " /";
  if(!path || !path[0]) {
    return 0;
  }
  for(i = 0; path[i]; ++i) {
    unsigned char c = path[i];
    if(c <= 0x1f || c == 0x7f || c == '\\' || c == '<' || c == '>' ||
       c == '|') {
      return 0;
    }
    if(path[i] == '/') {
      filename_idx = i + 1;
    }
  }
  len = i;
  if(filename_idx == len) {
    return 0;
  }
  if(strchr(bad_prefix, path[0]) != NULL) {
    return 0;
  }
  if(filename_idx != 0) {
    if(strchr(bad_prefix, path[filename_idx]) != NULL) {
      return 0;
    }
  }
  if(strchr(bad_suffix, path[len-1]) != NULL) {
    return 0;
  }
  /* Don't allow DOS drive letter (e.g., C:) */
  if(len >= 2 &&
     (('A' <= path[0] && path[0] <= 'Z') ||
      ('a' <= path[0] && path[0] <= 'z')) && path[1] == ':') {
    return 0;
  }
  /* "." and ".." and prefix "./" and "../" are considered in
     bad_prefix */
  if(strstr(path, "/./") != NULL ||
     strstr(path, "/../") != NULL ||
     ends_with(path, "/.") ||
     ends_with(path, "/..")) {
    return 0;
  }
  return 1;
}

void metalink_get_version(int* major, int* minor, int* patch)
{
  *major = LIBMETALINK_VERSION_MAJOR;
  *minor = LIBMETALINK_VERSION_MINOR;
  *patch = LIBMETALINK_VERSION_PATCH;
}

const char* metalink_strerror(int error_code)
{
  switch(error_code) {
  case 0:
    return "success";
  case METALINK_ERR_BAD_ALLOC:
    return "out of memory";
  case METALINK_ERR_CANNOT_OPEN_FILE:
    return "could not open file";
  case METALINK_ERR_MISSING_REQUIRED_ATTR:
    return "required attribute not found";
  case METALINK_ERR_NAMESPACE_ERROR:
    return "unexpected namespace";
  case METALINK_ERR_PARSER_ERROR:
    return "xml parser failure";
    /* METALINK_ERR_NO_*_TRANSACTION error code should not be returned
       to the application code. If they are, it is a bug of
       libmetalink. In the future release, they will be removed and
       assert() will be used instead. */
  case METALINK_ERR_NO_FILE_TRANSACTION:
    return "no file transaction";
  case METALINK_ERR_NO_RESOURCE_TRANSACTION:
    return "no resource transaction";
  case METALINK_ERR_NO_CHECKSUM_TRANSACTION:
    return "no checksum transaction";
  case METALINK_ERR_NO_CHUNK_CHECKSUM_TRANSACTION:
    return "no chunk checksum transaction";
  case METALINK_ERR_NO_PIECE_HASH_TRANSACTION:
    return "no piece hash transaction";
  default:
    return "unknown error code";
  }
}
