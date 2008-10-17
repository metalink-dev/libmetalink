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
#ifndef _D_METALINK_TYPES_H_
#define _D_METALINK_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "metalink_error.h"

typedef struct _metalink_resource {
  /* url, null terminated string */
  char* url;
  /* type of resources, like "http", "ftp", null terminated string */
  char* type;
  /* location, this is 2-characther country code, like "JP",
   * null terminated string
   */
  char* location;
  /* preference of this resource, higher value has bigger preference. */
  int preference;
  /* max connections that a client can establish to this resource */
  int maxconnections;
} metalink_resource_t;

metalink_resource_t* new_metalink_resource();

void delete_metalink_resource(metalink_resource_t* resource);

/* mutators */
metalink_error_t metalink_resource_set_type(metalink_resource_t* resource, const char* type);

metalink_error_t metalink_resource_set_location(metalink_resource_t* resource,
				   const char* location);

void metalink_resource_set_preference(metalink_resource_t* resource,
				      int preference);

void metalink_resource_set_maxconnections(metalink_resource_t* resource,
					  int maxconnections);

metalink_error_t metalink_resource_set_url(metalink_resource_t* resource, const char* url);

typedef struct _metalink_checksum {
  /* message digest algorithm, for example, sha1, null terminated string */
  char* type;
  /* message digest in a ASCII hexadecimal notation, null terminated string */
  char* hash;
} metalink_checksum_t;

metalink_checksum_t* new_metalink_checksum();

void delete_metalink_checksum(metalink_checksum_t* checksum);

/* mutators */
metalink_error_t metalink_checksum_set_type(metalink_checksum_t* checksum, const char* type);

metalink_error_t metalink_checksum_set_hash(metalink_checksum_t* checksum, const char* hash);

/**
 * hash value of each piece.
 */
typedef struct _metalink_piece_hash {
  int piece;
  /* hash value in a ASCII hexadecimal notation */
  char* hash;
} metalink_piece_hash_t;

/* constructor */
metalink_piece_hash_t* new_metalink_piece_hash();

/* destructor */
void delete_metalink_piece_hash(metalink_piece_hash_t* piece_hash);

/* mutators */
void metalink_piece_hash_set_piece(metalink_piece_hash_t* piece_hash, int piece);

metalink_error_t metalink_piece_hash_set_hash(metalink_piece_hash_t* piece_hash, const char* hash);

/**
 * Piece hash; containing type(hash algorithm) and piece size and
 * hashes.
 */
typedef struct _metalink_chunk_checksum {
  /* message digest algorithm, for example, sha1, null terminated string */
  char* type;
  /* length of piece */
  int length;
  /* list of hash. Iterate until you get NULL */
  metalink_piece_hash_t** piece_hashes;
} metalink_chunk_checksum_t;

/* constructor */
metalink_chunk_checksum_t* new_metalink_chunk_checksum();

/* destructor */
void delete_metalink_chunk_checksum(metalink_chunk_checksum_t* chunk_checksum);

/* mutators */
metalink_error_t metalink_chunk_checksum_set_type(metalink_chunk_checksum_t* chunk_checksum,
				     const char* type);

void metalink_chunk_checksum_set_length(metalink_chunk_checksum_t* chunk_checksum,
					int length);

void metalink_chunk_checksum_set_piece_hashes(metalink_chunk_checksum_t* chunk_checksum,
					      metalink_piece_hash_t** piece_hashes);

typedef struct _metalink_file {
  /* filename, null terminated string */
  char* name;
  /* file size */
  long long int size;
  /* version, null terminated string */
  char* version;
  /* language, null terminated string */
  char* language;
  /* os, null terminated string */
  char* os;
  /* list of metalink_resource_t */
  metalink_resource_t** resources;
  int maxconnections;
  /* list of metalink_checksum_t. It is possible to include multiple message
   * digest algorithms
   */
  metalink_checksum_t** checksums;

  /* chunk checksum */
  metalink_chunk_checksum_t* chunk_checksum;

} metalink_file_t;

/* constructor */
metalink_file_t* new_metalink_file();

/* destructor */
void delete_metalink_file(metalink_file_t* file);

/* mutators */
metalink_error_t metalink_file_set_name(metalink_file_t* file, const char* name);

void metalink_file_set_size(metalink_file_t* file, long long int size);

metalink_error_t metalink_file_set_version(metalink_file_t* file, const char* version);

metalink_error_t metalink_file_set_language(metalink_file_t* file, const char* language);

metalink_error_t metalink_file_set_os(metalink_file_t* file, const char* os);

metalink_error_t metalink_file_set_resources(metalink_file_t* file, metalink_resource_t** resources);

void metalink_file_set_maxconnections(metalink_file_t* file, int maxconnections);

typedef struct _metalink {
  /* put more descriptable information here... */
  /* date, publisher or something useful */

  /* list of metalink_file_t */
  metalink_file_t** files;
  char* identity;
  char* tags;
} metalink_t;

metalink_error_t metalink_set_identity(metalink_t* metalink, const char* identity);
metalink_error_t metalink_set_tags(metalink_t* metalink, const char* tags);


metalink_t* new_metalink();

void delete_metalink(metalink_t* metalink);

#ifdef __cplusplus
}
#endif

#endif // _D_METALINK_TYPES_H_
