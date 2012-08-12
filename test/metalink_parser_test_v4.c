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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <CUnit/CUnit.h>
#include "metalink_parser_test.h"
#include "metalink_parser_test_v4.h"
#include "metalink/metalink_parser.h"

static void validate_result(metalink_t* metalink)
{
  metalink_file_t* file;
  metalink_checksum_t* checksum;
  metalink_resource_t* resource;
  metalink_metaurl_t* metaurl;
  metalink_piece_hash_t* piece_hash;

  CU_ASSERT_STRING_EQUAL("MetalinkEditor/2.0dev", metalink->generator);
  /* UTC time */
  CU_ASSERT_EQUAL(metalink->published, 1242356003);
  CU_ASSERT_EQUAL(metalink->version, METALINK_VERSION_4);

  CU_ASSERT_STRING_EQUAL("http://example.org/foo.metalink", metalink->origin);
  CU_ASSERT(metalink->origin_dynamic);

  /* count files */
  CU_ASSERT_EQUAL_FATAL(4, count_array((void**)metalink->files));

  /* check 1st file */
  file = metalink->files[0];
  CU_ASSERT_STRING_EQUAL("libmetalink-0.0.1.tar.bz2", file->name);
  CU_ASSERT_EQUAL(0, file->size); /* no size specified */
  CU_ASSERT_STRING_EQUAL("0.0.1", file->version);
  CU_ASSERT_STRING_EQUAL("libmetalink", file->identity);

  CU_ASSERT_STRING_EQUAL("name", file->publisher_name);
  CU_ASSERT_STRING_EQUAL("url", file->publisher_url);

  CU_ASSERT_EQUAL(1, count_array((void**)file->languages));
  CU_ASSERT_STRING_EQUAL("en-US", file->languages[0]);

  CU_ASSERT_EQUAL(1, count_array((void**)file->oses));
  CU_ASSERT_STRING_EQUAL("Linux-x86", file->oses[0]);

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

  /* file->resources were sorted by priority */
  resource = file->resources[0];
  CU_ASSERT_STRING_EQUAL("http://httphost/libmetalink-0.0.1.tar.bz2",
			 resource->url);
  CU_ASSERT_PTR_NULL(resource->location); /* no location */
  CU_ASSERT_EQUAL(99, resource->priority);

  resource = file->resources[1];
  CU_ASSERT_STRING_EQUAL("ftp://ftphost/libmetalink-0.0.1.tar.bz2",
			 resource->url);
  CU_ASSERT_STRING_EQUAL("jp", resource->location);
  CU_ASSERT_EQUAL(100, resource->priority);


  /* check 2nd file */
  file = metalink->files[1];
  CU_ASSERT_STRING_EQUAL("libmetalink-0.0.2a.tar.bz2", file->name);
  CU_ASSERT_EQUAL(4294967296LL, file->size);

  CU_ASSERT_PTR_NULL(file->checksums); /* no checksums */

  CU_ASSERT_STRING_EQUAL("sha1", file->chunk_checksum->type);
  CU_ASSERT_EQUAL(262144, file->chunk_checksum->length);
  /* Check that the entry which doesn't have type attribute is skipped. */
  CU_ASSERT_PTR_NOT_NULL_FATAL(file->chunk_checksum);
  CU_ASSERT_EQUAL(2, count_array((void**)file->chunk_checksum->piece_hashes));

  piece_hash = file->chunk_checksum->piece_hashes[0];
  CU_ASSERT_STRING_EQUAL("179463a88d79cbf0b1923991708aead914f26142",
			 piece_hash->hash);

  piece_hash = file->chunk_checksum->piece_hashes[1];
  CU_ASSERT_STRING_EQUAL("fecf8bc9a1647505fe16746f94e97a477597dbf3",
			 piece_hash->hash);

  CU_ASSERT_EQUAL_FATAL(3, count_array((void**)file->resources));

  resource = file->resources[0];
  CU_ASSERT_STRING_EQUAL("ftp://ftphost/libmetalink-0.0.2a.tar.bz2",
			 resource->url);

  resource = file->resources[1];
  CU_ASSERT_STRING_EQUAL("http://mirror1/libmetalink-0.0.2a.tar.bz2",
			 resource->url);

  resource = file->resources[2];
  CU_ASSERT_STRING_EQUAL("http://httphost/libmetalink-0.0.2a.tar.bz2",
			 resource->url);
  CU_ASSERT_EQUAL(999999, resource->priority); /* no priority */

  CU_ASSERT_EQUAL_FATAL(3, count_array((void**)file->metaurls));
  metaurl = file->metaurls[0];
  CU_ASSERT_STRING_EQUAL("http://mirror3/libmetalink-0.0.2a.tar.bz2.torrent",
			 metaurl->url);
  metaurl = file->metaurls[1];
  CU_ASSERT_STRING_EQUAL("http://mirror1/libmetalink-0.0.2a.tar.bz2.torrent",
			 metaurl->url);
  metaurl = file->metaurls[2];
  CU_ASSERT_STRING_EQUAL("http://mirror2/libmetalink-0.0.2a.tar.bz2.torrent",
			 metaurl->url);
  CU_ASSERT_EQUAL(999999, metaurl->priority);

  /* Check 3rd file */
  file = metalink->files[2];
  CU_ASSERT_STRING_EQUAL("NoVerificationHash", file->name);
  CU_ASSERT_EQUAL(0, file->size); /* bad size, fallback to 0 */
  CU_ASSERT_PTR_NULL(file->checksums);
  CU_ASSERT_PTR_NULL(file->chunk_checksum);
  
  CU_ASSERT_EQUAL_FATAL(1, count_array((void**)file->resources));
  resource = file->resources[0];
  CU_ASSERT_STRING_EQUAL("ftp://host/file", resource->url);

  /* Check 4th file */
  file = metalink->files[3];
  CU_ASSERT_STRING_EQUAL("badpri", file->name);
  resource = file->resources[0];
  CU_ASSERT_STRING_EQUAL("http://badpriority/", resource->url);
  /* bad priority, fallback to 999999. */
  CU_ASSERT_EQUAL(999999, resource->priority);

  metalink_delete(metalink);
}

void test_metalink_parse_file_v4(void)
{
  metalink_error_t r;
  metalink_t* metalink;

  r = metalink_parse_file(LIBMETALINK_TEST_DIR"test2.xml", &metalink);
  CU_ASSERT_EQUAL_FATAL(0, r);

  validate_result(metalink);
}
