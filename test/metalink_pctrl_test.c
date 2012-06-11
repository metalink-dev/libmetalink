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
#include "metalink_pctrl_test.h"

#include <CUnit/CUnit.h>

#include <metalink/metalink_error.h>
#include "metalink_pctrl.h"

void test_metalink_pctrl_file_transaction(void)
{
  metalink_pctrl_t* ctrl;
  metalink_file_t* file;

  ctrl = new_metalink_pctrl();
  CU_ASSERT_PTR_NOT_NULL(ctrl);

  /* start new file transaction */
  file = metalink_pctrl_new_file_transaction(ctrl);
  CU_ASSERT_PTR_NOT_NULL(file);

  /* Set values to file. */
  CU_ASSERT_EQUAL(0, metalink_pctrl_file_set_name(ctrl, "libmetalink.tar.bz2"));
  metalink_pctrl_file_set_size(ctrl, 4294967296LL);
  CU_ASSERT_EQUAL(0, metalink_pctrl_file_set_version(ctrl, "1.0.0"));
  CU_ASSERT_EQUAL(0, metalink_pctrl_file_set_language(ctrl, "en_US"));
  CU_ASSERT_EQUAL(0, metalink_pctrl_file_set_os(ctrl, "linux"));
  metalink_pctrl_file_set_maxconnections(ctrl, 5);

  /* Commit */
  CU_ASSERT_EQUAL(0, metalink_pctrl_commit_file_transaction(ctrl));
  file = NULL;

  CU_ASSERT_PTR_NULL(ctrl->temp_file);
  CU_ASSERT_EQUAL(1, metalink_list_length(ctrl->files));

  file = metalink_list_get_data(ctrl->files, 0);
  CU_ASSERT_STRING_EQUAL("libmetalink.tar.bz2", file->name);
  CU_ASSERT_EQUAL(4294967296LL, file->size);
  CU_ASSERT_STRING_EQUAL("1.0.0", file->version);
  CU_ASSERT_STRING_EQUAL("en_US", file->language);
  CU_ASSERT_STRING_EQUAL("linux", file->os);
  CU_ASSERT_EQUAL(5, file->maxconnections);

  delete_metalink_pctrl(ctrl);
}

void test_metalink_pctrl_resource_transaction(void)
{
  metalink_pctrl_t* ctrl;
  metalink_resource_t* resource;

  ctrl = new_metalink_pctrl();
  CU_ASSERT_PTR_NOT_NULL(ctrl);

  /* start new resource transaction */
  resource = metalink_pctrl_new_resource_transaction(ctrl);
  CU_ASSERT_PTR_NOT_NULL(resource);

  /* Set several values to resource object.
   * These values are stored in ctrl->temp_resource, which is the return value
   * of metalink_pctrl_new_resource_transaction(), thus resource.
   */
  metalink_pctrl_resource_set_type(ctrl, "http");
  metalink_pctrl_resource_set_location(ctrl, "FI");
  metalink_pctrl_resource_set_priority(ctrl, 100);
  metalink_pctrl_resource_set_maxconnections(ctrl, 4);
  metalink_pctrl_resource_set_url(ctrl, "http://host/dir/file");

  /* Commit appends ctrl->temp_resource to ctrl->resources. */
  metalink_pctrl_commit_resource_transaction(ctrl);
  /* at this point, resource is undefined. So set it to NULL */
  resource = NULL;

  CU_ASSERT_PTR_NULL(ctrl->temp_resource);
  CU_ASSERT_EQUAL(1, metalink_list_length(ctrl->resources));

  resource = metalink_list_get_data(ctrl->resources, 0);
  CU_ASSERT_STRING_EQUAL("http", resource->type);
  CU_ASSERT_STRING_EQUAL("FI", resource->location);
  CU_ASSERT_EQUAL(100, resource->priority);
  CU_ASSERT_EQUAL(4, resource->maxconnections);
  CU_ASSERT_STRING_EQUAL("http://host/dir/file", resource->url);

  delete_metalink_pctrl(ctrl);
}

void test_metalink_pctrl_checksum_transaction(void)
{
  metalink_pctrl_t* ctrl;
  metalink_checksum_t* checksum;

  ctrl = new_metalink_pctrl();
  CU_ASSERT_PTR_NOT_NULL(ctrl);

  /* start new checksum transaction */
  checksum = metalink_pctrl_new_checksum_transaction(ctrl);
  CU_ASSERT_PTR_NOT_NULL(checksum);

  /* Set type, hash to checksum */
  CU_ASSERT_EQUAL(0, metalink_pctrl_checksum_set_type(ctrl, "sha1"));
  CU_ASSERT_EQUAL(0, metalink_pctrl_checksum_set_hash
	    (ctrl, "234f3611ad77aaf1241a0dc8ac708007935844d5"));

  CU_ASSERT_STRING_EQUAL("sha1", checksum->type);
  CU_ASSERT_STRING_EQUAL("234f3611ad77aaf1241a0dc8ac708007935844d5",
			 checksum->hash);

  /* Commit appends ctrl->temp_checksum to ctrl->checksums. */
  CU_ASSERT_EQUAL(0, metalink_pctrl_commit_checksum_transaction(ctrl));
  checksum = NULL;

  CU_ASSERT_PTR_NULL(ctrl->temp_checksum);
  CU_ASSERT_EQUAL(1, metalink_list_length(ctrl->checksums));

  checksum = metalink_list_get_data(ctrl->checksums, 0);
  CU_ASSERT_STRING_EQUAL("sha1", checksum->type);
  CU_ASSERT_STRING_EQUAL("234f3611ad77aaf1241a0dc8ac708007935844d5",
			 checksum->hash);

  /* Try to commit while transaction is not started */
  CU_ASSERT_EQUAL(METALINK_ERR_NO_CHECKSUM_TRANSACTION,
		  metalink_pctrl_commit_checksum_transaction(ctrl));

  delete_metalink_pctrl(ctrl);
}

void test_metalink_pctrl_chunk_checksum_transaction(void)
{
  metalink_pctrl_t* ctrl;
  metalink_chunk_checksum_t* chunk_checksum;
  metalink_file_t* file;

  ctrl = new_metalink_pctrl();
  CU_ASSERT_PTR_NOT_NULL(ctrl);

  /* start new file transaction, chunk checksum transaction needs this */
  file = metalink_pctrl_new_file_transaction(ctrl);
  CU_ASSERT_PTR_NOT_NULL(file);

  /* start new chunk checksum transaction */
  chunk_checksum = metalink_pctrl_new_chunk_checksum_transaction(ctrl);
  CU_ASSERT_PTR_NOT_NULL(chunk_checksum);

  /* Set type and length */
  CU_ASSERT_EQUAL(0, metalink_pctrl_chunk_checksum_set_type(ctrl, "sha1"));
  metalink_pctrl_chunk_checksum_set_length(ctrl, 65536);

  /* Commit */
  CU_ASSERT_EQUAL(0, metalink_pctrl_commit_chunk_checksum_transaction(ctrl));
  chunk_checksum = NULL;

  CU_ASSERT_PTR_NULL(ctrl->temp_chunk_checksum);
  CU_ASSERT_PTR_NOT_NULL(ctrl->temp_file->chunk_checksum);

  chunk_checksum = ctrl->temp_file->chunk_checksum;
  CU_ASSERT_STRING_EQUAL("sha1", chunk_checksum->type);
  CU_ASSERT_EQUAL(65536, chunk_checksum->length);

  /* Try to commit while transaction is not started */
  CU_ASSERT_EQUAL(METALINK_ERR_NO_CHUNK_CHECKSUM_TRANSACTION,
		  metalink_pctrl_commit_chunk_checksum_transaction(ctrl));

  delete_metalink_pctrl(ctrl);
}

void test_metalink_pctrl_piece_hash_transaction(void)
{
  metalink_pctrl_t* ctrl;
  metalink_piece_hash_t* piece_hash;

  ctrl = new_metalink_pctrl();
  CU_ASSERT_PTR_NOT_NULL(ctrl);

  /* start new piece hash transaction */
  piece_hash = metalink_pctrl_new_piece_hash_transaction(ctrl);
  CU_ASSERT_PTR_NOT_NULL(piece_hash);

  /* Set piece and hash */
  metalink_pctrl_piece_hash_set_piece(ctrl, 100);
  CU_ASSERT_EQUAL(0, metalink_pctrl_piece_hash_set_hash
		  (ctrl, "234f3611ad77aaf1241a0dc8ac708007935844d5"));

  /* Commit */
  CU_ASSERT_EQUAL(0, metalink_pctrl_commit_piece_hash_transaction(ctrl));
  piece_hash = NULL;

  CU_ASSERT_PTR_NULL(ctrl->temp_piece_hash);
  CU_ASSERT_EQUAL(1, metalink_list_length(ctrl->piece_hashes));

  piece_hash = metalink_list_get_data(ctrl->piece_hashes, 0);
  CU_ASSERT_EQUAL(100, piece_hash->piece);
  CU_ASSERT_STRING_EQUAL("234f3611ad77aaf1241a0dc8ac708007935844d5",
			 piece_hash->hash);

  /* Try to commit while transaction is not started */
  CU_ASSERT_EQUAL(METALINK_ERR_NO_PIECE_HASH_TRANSACTION,
		  metalink_pctrl_commit_piece_hash_transaction(ctrl));

  delete_metalink_pctrl(ctrl);
}
