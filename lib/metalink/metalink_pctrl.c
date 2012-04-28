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
#include "metalink_pctrl.h"
#include "metalink_config.h"

#include <string.h>

metalink_pctrl_t* new_metalink_pctrl(void)
{
  metalink_pctrl_t* ctrl;
  ctrl = malloc(sizeof(metalink_pctrl_t));
  if(!ctrl) {
    return NULL;
  }
  memset(ctrl, 0, sizeof(metalink_pctrl_t));
  ctrl->metalink = metalink_new();
  if(!ctrl->metalink) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->files = metalink_list_new();
  if(!ctrl->files) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->resources = metalink_list_new();
  if(!ctrl->resources) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->checksums = metalink_list_new();
  if(!ctrl->checksums) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->piece_hashes = metalink_list_new();
  if(!ctrl->piece_hashes) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  return ctrl;
 NEW_METALINK_PCTRL_ERROR:
  delete_metalink_pctrl(ctrl);
  return NULL;
}

void delete_metalink_pctrl(metalink_pctrl_t* ctrl)
{
  if(!ctrl) {
    return;
  }
  metalink_delete(ctrl->metalink);
  
  metalink_list_for_each(ctrl->files, (void (*)(void*))&metalink_file_delete);
  metalink_list_delete(ctrl->files);
  metalink_file_delete(ctrl->temp_file);
  
  metalink_list_for_each(ctrl->resources,
			 (void (*)(void*))&metalink_resource_delete);
  metalink_list_delete(ctrl->resources);
  metalink_resource_delete(ctrl->temp_resource);

  metalink_list_for_each(ctrl->checksums,
			 (void (*)(void*))&metalink_checksum_delete);
  metalink_list_delete(ctrl->checksums);
  metalink_checksum_delete(ctrl->temp_checksum);

  metalink_chunk_checksum_delete(ctrl->temp_chunk_checksum);

  metalink_list_for_each(ctrl->piece_hashes,
			 (void (*)(void*))&metalink_piece_hash_delete);
  metalink_list_delete(ctrl->piece_hashes);
  metalink_piece_hash_delete(ctrl->temp_piece_hash);

  free(ctrl);
}

metalink_t* metalink_pctrl_detach_metalink(metalink_pctrl_t* ctrl)
{
  metalink_t* metalink;
  metalink = ctrl->metalink;
  ctrl->metalink = NULL;
  return metalink;
}

void metalink_pctrl_set_error(metalink_pctrl_t* ctrl, metalink_error_t error)
{
  ctrl->error = error;
}

metalink_error_t metalink_pctrl_get_error(metalink_pctrl_t* ctrl)
{
  return ctrl->error;
}

metalink_error_t metalink_pctrl_metalink_accumulate_files(metalink_pctrl_t* ctrl)
{
  size_t files_length;
  files_length = metalink_list_length(ctrl->files);
  if(files_length) {
    ctrl->metalink->files = calloc(files_length+1, sizeof(metalink_file_t*));
    if(!ctrl->metalink->files) {
      return METALINK_ERR_BAD_ALLOC;
    }
    metalink_list_to_array(ctrl->files, (void**)ctrl->metalink->files);
    ctrl->metalink->files[files_length] = NULL;

    metalink_list_clear(ctrl->files);
  }
  return 0;
}

static metalink_error_t commit_list_to_array(void*** array_ptr, metalink_list_t* src, size_t ele_size)
{
  size_t size;
  size = metalink_list_length(src);
  if(size) {
    *array_ptr = calloc(size+1, ele_size);
    if(!*array_ptr) {
      return METALINK_ERR_BAD_ALLOC;
    }
    metalink_list_to_array(src, *array_ptr);
    (*array_ptr)[size] = NULL;
    metalink_list_clear(src);
  }
  return 0;
}

/* transaction functions */
metalink_file_t* metalink_pctrl_new_file_transaction(metalink_pctrl_t* ctrl)
{
  if(ctrl->temp_file) {
    metalink_file_delete(ctrl->temp_file);
  }
  ctrl->temp_file = metalink_file_new();

  metalink_list_for_each(ctrl->resources,
			 (void (*)(void*))&metalink_resource_delete);
  metalink_list_clear(ctrl->resources);

  metalink_list_for_each(ctrl->checksums,
			 (void (*)(void*))&metalink_checksum_delete);
  metalink_list_clear(ctrl->checksums);

  return ctrl->temp_file;
}

metalink_error_t metalink_pctrl_commit_file_transaction(metalink_pctrl_t* ctrl)
{
  metalink_error_t r;
  if(!ctrl->temp_file) {
    return METALINK_ERR_NO_FILE_TRANSACTION;
  }

  /* copy ctrl->resources to ctrl->temp_file->resources */
  r = commit_list_to_array((void***)&ctrl->temp_file->resources,
			   ctrl->resources, sizeof(metalink_resource_t*));
  if(r != 0) {
    return r;
  }
  /* copy ctrl->checksums to ctrl->temp_file->checksums */
  r = commit_list_to_array((void***)&ctrl->temp_file->checksums,
			   ctrl->checksums, sizeof(metalink_checksum_t*));
  if(r != 0) {
    return r;
  }

  if(metalink_list_append(ctrl->files, ctrl->temp_file) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }

  ctrl->temp_file = NULL;
  return 0;
}

metalink_resource_t* metalink_pctrl_new_resource_transaction(metalink_pctrl_t* ctrl)
{
  if(ctrl->temp_resource) {
    metalink_resource_delete(ctrl->temp_resource);
  }
  ctrl->temp_resource = metalink_resource_new();
  return ctrl->temp_resource;
}

metalink_error_t metalink_pctrl_commit_resource_transaction(metalink_pctrl_t* ctrl)
{
  if(!ctrl->temp_resource) {
    return METALINK_ERR_NO_RESOURCE_TRANSACTION;
  }

  if(metalink_list_append(ctrl->resources, ctrl->temp_resource) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }

  ctrl->temp_resource = NULL;
  return 0;
}

metalink_checksum_t* metalink_pctrl_new_checksum_transaction(metalink_pctrl_t* ctrl)
{
  if(ctrl->temp_checksum) {
    metalink_checksum_delete(ctrl->temp_checksum);
  }
  ctrl->temp_checksum = metalink_checksum_new();
  return ctrl->temp_checksum;
}

metalink_error_t metalink_pctrl_commit_checksum_transaction(metalink_pctrl_t* ctrl)
{
  if(!ctrl->temp_checksum) {
    return METALINK_ERR_NO_CHECKSUM_TRANSACTION;
  }

  if(metalink_list_append(ctrl->checksums, ctrl->temp_checksum) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }
  ctrl->temp_checksum = NULL;
  return 0;
}

metalink_chunk_checksum_t*
metalink_pctrl_new_chunk_checksum_transaction(metalink_pctrl_t* ctrl)
{
  if(ctrl->temp_chunk_checksum) {
    metalink_chunk_checksum_delete(ctrl->temp_chunk_checksum);
  }

  ctrl->temp_chunk_checksum = metalink_chunk_checksum_new();
  metalink_list_for_each(ctrl->piece_hashes,
			 (void (*)(void*))&metalink_piece_hash_delete);
  metalink_list_clear(ctrl->piece_hashes);
  
  return ctrl->temp_chunk_checksum;
}

metalink_error_t metalink_pctrl_commit_chunk_checksum_transaction(metalink_pctrl_t* ctrl)
{
  metalink_error_t r;
  if(!ctrl->temp_chunk_checksum) {
    return METALINK_ERR_NO_CHUNK_CHECKSUM_TRANSACTION;
  }
  if(!ctrl->temp_file) {
    return METALINK_ERR_NO_FILE_TRANSACTION;
  }
  r = commit_list_to_array((void***)&ctrl->temp_chunk_checksum->piece_hashes,
			   ctrl->piece_hashes, sizeof(metalink_piece_hash_t*));
  if(r != 0) {
    return r;
  }
  metalink_list_clear(ctrl->piece_hashes);
  ctrl->temp_file->chunk_checksum = ctrl->temp_chunk_checksum;
  ctrl->temp_chunk_checksum = NULL;
  return 0;
}

metalink_piece_hash_t* metalink_pctrl_new_piece_hash_transaction(metalink_pctrl_t* ctrl)
{
  if(ctrl->temp_piece_hash) {
    metalink_piece_hash_delete(ctrl->temp_piece_hash);
  }
  ctrl->temp_piece_hash = metalink_piece_hash_new();
  return ctrl->temp_piece_hash;
}

metalink_error_t metalink_pctrl_commit_piece_hash_transaction(metalink_pctrl_t* ctrl)
{
  if(!ctrl->temp_piece_hash) {
    return METALINK_ERR_NO_PIECE_HASH_TRANSACTION;
  }
  if(metalink_list_append(ctrl->piece_hashes,
			  (void*)ctrl->temp_piece_hash) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }
  ctrl->temp_piece_hash = NULL;
  return 0;
}

/* metalink manipulation functions */
metalink_error_t metalink_pctrl_set_identity(metalink_pctrl_t* ctrl, const char* identity)
{
  return metalink_set_identity(ctrl->metalink, identity);
}

metalink_error_t metalink_pctrl_set_tags(metalink_pctrl_t* ctrl, const char* tags)
{
  return metalink_set_tags(ctrl->metalink, tags);
}

/* file manipulation functions*/
metalink_error_t metalink_pctrl_file_set_name(metalink_pctrl_t* ctrl, const char* name)
{
  return metalink_file_set_name(ctrl->temp_file, name);
}

void metalink_pctrl_file_set_size(metalink_pctrl_t* ctrl, long long int size)
{
  metalink_file_set_size(ctrl->temp_file, size);
}

metalink_error_t metalink_pctrl_file_set_version(metalink_pctrl_t* ctrl, const char* version)
{
  return metalink_file_set_version(ctrl->temp_file, version);
}

metalink_error_t metalink_pctrl_file_set_language(metalink_pctrl_t* ctrl, const char* language)
{
  return metalink_file_set_language(ctrl->temp_file, language);
}

metalink_error_t metalink_pctrl_file_set_os(metalink_pctrl_t* ctrl, const char* os)
{
  return metalink_file_set_os(ctrl->temp_file, os);
}

void metalink_pctrl_file_set_maxconnections(metalink_pctrl_t* ctrl, int maxconnections)
{
  metalink_file_set_maxconnections(ctrl->temp_file, maxconnections);
}


/* resource manipulation functions */
metalink_error_t metalink_pctrl_resource_set_type(metalink_pctrl_t* ctrl, const char* type)
{
  return metalink_resource_set_type(ctrl->temp_resource, type);
}

metalink_error_t metalink_pctrl_resource_set_location(metalink_pctrl_t* ctrl,
					 const char* location)
{
  return metalink_resource_set_location(ctrl->temp_resource, location);
}

void metalink_pctrl_resource_set_preference(metalink_pctrl_t* ctrl,
					    int preference)
{
  metalink_resource_set_preference(ctrl->temp_resource, preference);
}

void metalink_pctrl_resource_set_maxconnections(metalink_pctrl_t* ctrl,
						int maxconnections)
{
  metalink_resource_set_maxconnections(ctrl->temp_resource, maxconnections);
}

metalink_error_t metalink_pctrl_resource_set_url(metalink_pctrl_t* ctrl, const char* url)
{
  return metalink_resource_set_url(ctrl->temp_resource, url);
}

/* checksum manipulation functions */
metalink_error_t metalink_pctrl_checksum_set_type(metalink_pctrl_t* ctrl, const char* type)
{
  return metalink_checksum_set_type(ctrl->temp_checksum, type);
}

metalink_error_t metalink_pctrl_checksum_set_hash(metalink_pctrl_t* ctrl, const char* hash)
{
  return metalink_checksum_set_hash(ctrl->temp_checksum, hash);
}

/* piece hash manipulation functions */
void metalink_pctrl_piece_hash_set_piece(metalink_pctrl_t* ctrl, int piece)
{
  metalink_piece_hash_set_piece(ctrl->temp_piece_hash, piece);
}

metalink_error_t metalink_pctrl_piece_hash_set_hash(metalink_pctrl_t* ctrl, const char* hash)
{
  return metalink_piece_hash_set_hash(ctrl->temp_piece_hash, hash);
}

/* chunk checksum manipulation functions */
metalink_error_t metalink_pctrl_chunk_checksum_set_type(metalink_pctrl_t* ctrl, const char* type)
{
  return metalink_chunk_checksum_set_type(ctrl->temp_chunk_checksum, type);
}

void metalink_pctrl_chunk_checksum_set_length(metalink_pctrl_t* ctrl, int length)
{
  metalink_chunk_checksum_set_length(ctrl->temp_chunk_checksum, length);
}

void metalink_pctrl_chunk_checksum_set_piece_hashes
(metalink_pctrl_t* ctrl, metalink_piece_hash_t** piece_hashes)
{
  metalink_chunk_checksum_set_piece_hashes(ctrl->temp_chunk_checksum,
					   piece_hashes);
}
