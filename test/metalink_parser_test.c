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
#include "metalink/metalink_parser.h"

static size_t count_array(void** array)
{
  size_t count = 0;
  while(*array) {
    ++count;
    ++array;
  }
  return count;
}

static void validate_result(metalink_t* metalink)
{
  metalink_file_t* file;
  metalink_checksum_t* checksum;
  metalink_resource_t* resource;
  metalink_piece_hash_t* piece_hash;

  /* count files */
  CU_ASSERT_EQUAL_FATAL(3, count_array((void**)metalink->files));

  /* check 1st file */
  file = metalink->files[0];
  CU_ASSERT_STRING_EQUAL("libmetalink-0.0.1.tar.bz2", file->name);
  CU_ASSERT_EQUAL(0, file->size); /* no size specified */
  CU_ASSERT_STRING_EQUAL("0.0.1", file->version);
  CU_ASSERT_STRING_EQUAL("en-US", file->language);
  CU_ASSERT_STRING_EQUAL("Linux-x86", file->os);
  CU_ASSERT_EQUAL(1, file->maxconnections);

  /* There is one checksum which has no type specified. Let's see it is
     excluded. */
  CU_ASSERT_EQUAL_FATAL(2, count_array((void**)file->checksums));
  
  checksum = file->checksums[0];
  CU_ASSERT_STRING_EQUAL("sha1", checksum->type);
  CU_ASSERT_STRING_EQUAL("a96cf3f0266b91d87d5124cf94326422800b627d",
			 checksum->hash);
  checksum = file->checksums[1];
  CU_ASSERT_STRING_EQUAL("md5", checksum->type);
  CU_ASSERT_STRING_EQUAL("fc4d834e89c18c99b2615d902750948c", checksum->hash);

  CU_ASSERT_PTR_NULL(file->chunk_checksum); /* no chunk checksum */

  CU_ASSERT_EQUAL_FATAL(2, count_array((void**)file->resources));

  resource = file->resources[0];
  CU_ASSERT_STRING_EQUAL("ftp://ftphost/libmetalink-0.0.1.tar.bz2",
			 resource->url);
  CU_ASSERT_STRING_EQUAL("ftp", resource->type);
  CU_ASSERT_STRING_EQUAL("jp", resource->location);
  CU_ASSERT_EQUAL(100, resource->preference);
  CU_ASSERT_EQUAL(1, resource->maxconnections);

  resource = file->resources[1];
  CU_ASSERT_STRING_EQUAL("http://httphost/libmetalink-0.0.1.tar.bz2",
			 resource->url);
  CU_ASSERT_STRING_EQUAL("http", resource->type);
  CU_ASSERT_PTR_NULL(resource->location); /* no location */
  CU_ASSERT_EQUAL(99, resource->preference);
  CU_ASSERT_EQUAL(0, resource->maxconnections); /* bad maxconnections, fallback to 0 */

  /* check 2nd file */
  file = metalink->files[1];
  CU_ASSERT_STRING_EQUAL("libmetalink-0.0.2a.tar.bz2", file->name);
  CU_ASSERT_EQUAL(4294967296LL, file->size);
  CU_ASSERT_EQUAL(0, file->maxconnections) /* bad maxconnections, fallback to 0 */

  CU_ASSERT_PTR_NULL(file->checksums); /* no checksums */

  CU_ASSERT_STRING_EQUAL("sha1", file->chunk_checksum->type);
  CU_ASSERT_EQUAL(262144, file->chunk_checksum->length);
  /* Check that the entry which doesn't have type attribute is skipped. */
  CU_ASSERT_PTR_NOT_NULL_FATAL(file->chunk_checksum);
  CU_ASSERT_EQUAL(2, count_array((void**)file->chunk_checksum->piece_hashes));

  piece_hash = file->chunk_checksum->piece_hashes[0];
  CU_ASSERT_EQUAL(0, piece_hash->piece);
  CU_ASSERT_STRING_EQUAL("179463a88d79cbf0b1923991708aead914f26142",
			 piece_hash->hash);

  piece_hash = file->chunk_checksum->piece_hashes[1];
  CU_ASSERT_EQUAL(1, piece_hash->piece);
  CU_ASSERT_STRING_EQUAL("fecf8bc9a1647505fe16746f94e97a477597dbf3",
			 piece_hash->hash);

  /* Check that entry which doesn't have type attribute is skipped. */
  CU_ASSERT_EQUAL_FATAL(5, count_array((void**)file->resources));

  resource = file->resources[0];
  CU_ASSERT_STRING_EQUAL("ftp://ftphost/libmetalink-0.0.2a.tar.bz2",
			 resource->url);
  resource = file->resources[1];
  CU_ASSERT_STRING_EQUAL("http://httphost/libmetalink-0.0.2a.tar.bz2",
			 resource->url);
  CU_ASSERT_STRING_EQUAL("http", resource->type);
  CU_ASSERT_EQUAL(0, resource->preference); /* no preference */

  resource = file->resources[2];
  CU_ASSERT_STRING_EQUAL("http://badpreference/", resource->url);
  CU_ASSERT_STRING_EQUAL("http", resource->type);
  CU_ASSERT_EQUAL(0, resource->preference); /* bad preference, fallback to 0. */

  resource = file->resources[3];
  CU_ASSERT_STRING_EQUAL("http://mirror1/libmetalink-0.0.2a.tar.bz2",
			 resource->url);
  
  resource = file->resources[4];
  CU_ASSERT_STRING_EQUAL("http://mirror2/libmetalink-0.0.2a.tar.bz2.torrent",
			 resource->url);
  CU_ASSERT_STRING_EQUAL("bittorrent", resource->type);

  /* Check 3rd file */
  file = metalink->files[2];
  CU_ASSERT_STRING_EQUAL("NoVerificationHash", file->name);
  CU_ASSERT_EQUAL(0, file->size); /* bad size, fallback to 0 */
  CU_ASSERT_PTR_NULL(file->checksums);
  CU_ASSERT_PTR_NULL(file->chunk_checksum);
  
  CU_ASSERT_EQUAL_FATAL(1, count_array((void**)file->resources));
  resource = file->resources[0];
  CU_ASSERT_STRING_EQUAL("ftp://host/file", resource->url);

  delete_metalink(metalink);
}

static int openfile(const char* filepath, int flags)
{
  int fd = open(filepath, flags);
  if(fd == -1) {
    CU_FAIL_FATAL("opening file failed");
  }
  return fd;
}

static void getfstat(struct stat* stptr, int fd)
{
  if(fstat(fd, stptr) == -1) {
    CU_FAIL_FATAL("fstat failed");
  }
}

static void* mapfile(int fd, off_t size)
{
  char* addr;

  addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if(addr == MAP_FAILED) {
    CU_FAIL_FATAL("mmap failed");
  }
  return addr;
}


void test_metalink_parse_file()
{
  int r;
  metalink_t* metalink;

  r = metalink_parse_file("test1.xml", &metalink);
  CU_ASSERT_EQUAL(0, r);

  validate_result(metalink);
}

void test_metalink_parse_memory()
{
  int r;
  metalink_t* metalink;
  int fd;
  struct stat st;
  char* addr;

  fd = openfile("test1.xml", O_RDONLY);
  getfstat(&st, fd);

  addr = mapfile(fd, st.st_size);

  r = metalink_parse_memory(addr, st.st_size, &metalink);
  CU_ASSERT_EQUAL(0, r);

  CU_ASSERT_EQUAL(0, close(fd));

  validate_result(metalink);
}

void test_metalink_parse_update()
{
  int r;
  metalink_t* metalink;
  int fd;
  struct stat st;
  char* addr;
  char* ptr;
  char* last;
  const int advance = 256;
  metalink_parser_context_t* ctx;

  fd = openfile("test1.xml", O_RDONLY);
  getfstat(&st, fd);
  addr = mapfile(fd, st.st_size);

  ctx = new_metalink_parser_context();
  CU_ASSERT_FATAL(NULL != ctx);

  last = addr+st.st_size;
  for(ptr = addr; ptr+advance < last; ptr += advance) {
    r = metalink_parse_update(ctx, ptr, advance);
    CU_ASSERT_EQUAL_FATAL(r, 0);
  }

  r = metalink_parse_final(ctx, ptr, last-ptr, &metalink);
  CU_ASSERT_EQUAL(r, 0);

  CU_ASSERT_EQUAL(0, close(fd));

  validate_result(metalink);
}


void test_metalink_parse_update_fail()
{
  int r;
  metalink_t* metalink;
  int fd;
  struct stat st;
  char* addr;
  char* ptr;
  char* last;
  const int advance = 256;
  metalink_parser_context_t* ctx;

  // Initialize ctx
  ctx = new_metalink_parser_context();
  CU_ASSERT_FATAL(NULL != ctx);

  // feed bad formed data
  r = metalink_parse_update(ctx, "<a><b></a>", 10);
  CU_ASSERT(0 != r);

  delete_metalink_parser_context(ctx);

  // Initialize ctx
  ctx = new_metalink_parser_context();
  CU_ASSERT_FATAL(NULL != ctx);

  fd = openfile("test1.xml", O_RDONLY);
  getfstat(&st, fd);
  addr = mapfile(fd, st.st_size);

  last = addr+st.st_size;
  for(ptr = addr; ptr+advance < last; ptr += advance) {
    r = metalink_parse_update(ctx, ptr, advance);
    CU_ASSERT_EQUAL_FATAL(r, 0);
  }
  // See when prematured XML data is supplied
  r = metalink_parse_final(ctx, 0, 0, &metalink);

  CU_ASSERT(0 != r);

  CU_ASSERT_EQUAL(0, close(fd));
}
