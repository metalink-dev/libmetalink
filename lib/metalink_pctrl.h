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
#ifndef _D_METALINK_PCTRL_H_
#define _D_METALINK_PCTRL_H_

#include "metalink_config.h"

#include <metalink/metalink.h>

#include "metalink_list.h"

typedef struct metalink_pctrl_t {
  metalink_error_t error;

  metalink_t* metalink;

  metalink_list_t/* <metalink_file_t*> */ * files;

  metalink_file_t* temp_file;
  
  metalink_list_t/* <char*> */ *languages;

  metalink_list_t/* <char*> */ *oses;

  metalink_list_t/* <metalink_resource_t*> */ * resources;

  metalink_resource_t* temp_resource;

  metalink_list_t/* <metalink_metaurl_t*> */ * metaurls;

  metalink_metaurl_t* temp_metaurl;

  metalink_list_t/* <metalink_checksum_t*> */ * checksums;

  metalink_checksum_t* temp_checksum;

  metalink_chunk_checksum_t* temp_chunk_checksum;

  metalink_list_t/* <metalink_piece_hash_t*> */ * piece_hashes;

  metalink_piece_hash_t* temp_piece_hash;

} metalink_pctrl_t;

metalink_pctrl_t* new_metalink_pctrl(void);

void delete_metalink_pctrl(metalink_pctrl_t* ctrl);

/**
 * detach metalink member: return ctrl->metalink and set NULL to
 * ctrl->metalink.
 */
metalink_t* metalink_pctrl_detach_metalink(metalink_pctrl_t* ctrl);

/* Set error code */
void metalink_pctrl_set_error(metalink_pctrl_t* ctrl, metalink_error_t error);

metalink_error_t metalink_pctrl_get_error(metalink_pctrl_t* ctrl);

/* metalink manipulation functions */
metalink_error_t metalink_pctrl_metalink_accumulate_files(metalink_pctrl_t* ctrl);

/* transaction functions */
metalink_file_t* metalink_pctrl_new_file_transaction(metalink_pctrl_t* ctrl);

metalink_error_t metalink_pctrl_commit_file_transaction(metalink_pctrl_t* ctrl);

metalink_resource_t* metalink_pctrl_new_resource_transaction(metalink_pctrl_t* ctrl);

metalink_error_t metalink_pctrl_commit_resource_transaction(metalink_pctrl_t* ctrl);

metalink_metaurl_t* metalink_pctrl_new_metaurl_transaction(metalink_pctrl_t* ctrl);

metalink_error_t metalink_pctrl_commit_metaurl_transaction(metalink_pctrl_t* ctrl);

metalink_checksum_t* metalink_pctrl_new_checksum_transaction(metalink_pctrl_t* ctrl);

metalink_error_t metalink_pctrl_commit_checksum_transaction(metalink_pctrl_t* ctrl);

metalink_chunk_checksum_t* metalink_pctrl_new_chunk_checksum_transaction(metalink_pctrl_t* ctrl);

metalink_error_t metalink_pctrl_commit_chunk_checksum_transaction(metalink_pctrl_t* ctrl);

metalink_piece_hash_t* metalink_pctrl_new_piece_hash_transaction(metalink_pctrl_t* ctrl);

metalink_error_t metalink_pctrl_commit_piece_hash_transaction(metalink_pctrl_t* ctrl);

/* metalink manipulation functions */
void metalink_pctrl_set_version(metalink_pctrl_t* ctrl, metalink_version_t version);
metalink_error_t metalink_pctrl_add_language(metalink_pctrl_t* ctrl, const char* language);
metalink_error_t metalink_pctrl_add_os(metalink_pctrl_t* ctrl, const char* os);
metalink_error_t metalink_pctrl_set_identity(metalink_pctrl_t* ctrl, const char* identity);
metalink_error_t metalink_pctrl_set_tags(metalink_pctrl_t* ctrl, const char* tags);

/* file manipulation functions */
metalink_error_t metalink_pctrl_file_set_language(metalink_pctrl_t* ctrl, const char* language);

metalink_error_t metalink_pctrl_file_set_os(metalink_pctrl_t* ctrl, const char* os);

metalink_error_t metalink_pctrl_file_set_name(metalink_pctrl_t* ctrl, const char* name);

metalink_error_t metalink_pctrl_file_set_description(metalink_pctrl_t* ctrl, const char* description);

metalink_error_t metalink_pctrl_file_set_copyright(metalink_pctrl_t* ctrl, const char* copyright);

metalink_error_t metalink_pctrl_file_set_identity(metalink_pctrl_t* ctrl, const char* identity);

metalink_error_t metalink_pctrl_file_set_logo(metalink_pctrl_t* ctrl, const char* logo);

metalink_error_t metalink_pctrl_file_set_publisher_name(metalink_pctrl_t* ctrl, const char* name);

metalink_error_t metalink_pctrl_file_set_publisher_url(metalink_pctrl_t* ctrl, const char* url);

void metalink_pctrl_file_set_size(metalink_pctrl_t* ctrl, long long int size);

metalink_error_t metalink_pctrl_file_set_version(metalink_pctrl_t* ctrl, const char* version);

void metalink_pctrl_file_set_maxconnections(metalink_pctrl_t* ctrl, int maxconnections);

/* resource manipulation functions */
metalink_error_t metalink_pctrl_resource_set_type(metalink_pctrl_t* ctrl, const char* type);

metalink_error_t metalink_pctrl_resource_set_location(metalink_pctrl_t* ctrl,
					 const char* location);

void metalink_pctrl_resource_set_preference(metalink_pctrl_t* ctrl,
					    int preference);

void metalink_pctrl_resource_set_priority(metalink_pctrl_t* ctrl,
					  int priority);

void metalink_pctrl_resource_set_maxconnections(metalink_pctrl_t* ctrl,
						int maxconnections);

metalink_error_t metalink_pctrl_resource_set_url(metalink_pctrl_t* ctrl, const char* url);

/* metaurl manipulation functions */
metalink_error_t metalink_pctrl_metaurl_set_mediatype(metalink_pctrl_t* ctrl,
						      const char* mediatype);

metalink_error_t metalink_pctrl_metaurl_set_name(metalink_pctrl_t* ctrl,
						 const char* name);

void metalink_pctrl_metaurl_set_priority(metalink_pctrl_t* ctrl,
					 int priority);

metalink_error_t metalink_pctrl_metaurl_set_url(metalink_pctrl_t* ctrl,
						const char* url);

/* checksum manipulation functions */
metalink_error_t metalink_pctrl_checksum_set_type(metalink_pctrl_t* ctrl, const char* type);

metalink_error_t metalink_pctrl_checksum_set_hash(metalink_pctrl_t* ctrl, const char* hash);

/* piece hash manipulation functions */
void metalink_pctrl_piece_hash_set_piece(metalink_pctrl_t* ctrl, int piece);

metalink_error_t metalink_pctrl_piece_hash_set_hash(metalink_pctrl_t* ctrl, const char* hash);

/* chunk checksum manipulation functions */
metalink_error_t metalink_pctrl_chunk_checksum_set_type(metalink_pctrl_t* ctrl, const char* type);

void metalink_pctrl_chunk_checksum_set_length(metalink_pctrl_t* ctrl, int length);

/* Unlike other mutator functions, this function doesn't create copy of
   piece_hashes. So don't free piece_hashes manually after this call.*/
void metalink_pctrl_chunk_checksum_set_piece_hashes(metalink_pctrl_t* ctrl,
						    metalink_piece_hash_t** piece_hashes);

/* information functions */
metalink_error_t metalink_pctrl_set_generator(metalink_pctrl_t* ctrl, const char* generator);

metalink_error_t metalink_pctrl_set_origin(metalink_pctrl_t* ctrl, const char* origin);

/* Sets the dynamic attribute of the origin element as per Metalink
   version 4. If it is "true", origin_dynamic must be 1. Otherwise, it
   must be 0. For Metalink version 3, origin_dynamic must be 1 if the
   type attribute of metalink element is "dynamic". Otherwise, it must
   be 0.
 */
void metalink_pctrl_set_origin_dynamic(metalink_pctrl_t* ctrl,
                                       int origin_dynamic);

void metalink_pctrl_set_published(metalink_pctrl_t* ctrl, int published);

void metalink_pctrl_set_updated(metalink_pctrl_t* ctrl, int updated);

#endif /* _D_METALINK_PCTRL_H_ */
