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

#include <metalink/metalink_error.h>

typedef struct _metalink_resource {
  /* url, null terminated string */
  char* url;
  /* type of resources, like "http", "ftp", null terminated string */
  char* type;
  /* location, this is 2-characther country code, like "JP",
   * null terminated string
   */
  char* location;
  /* priority of this resource, higher value has bigger priority. */
  int priority;
  /* max connections that a client can establish to this resource */
  int maxconnections;
} metalink_resource_t;

metalink_resource_t* metalink_resource_new(void);

void metalink_resource_delete(metalink_resource_t* resource);

/* mutators */
metalink_error_t metalink_resource_set_type(metalink_resource_t* resource, const char* type);

metalink_error_t metalink_resource_set_location(metalink_resource_t* resource,
				   const char* location);

void metalink_resource_set_priority(metalink_resource_t* resource,
				      int priority);

void metalink_resource_set_maxconnections(metalink_resource_t* resource,
					  int maxconnections);

metalink_error_t metalink_resource_set_url(metalink_resource_t* resource, const char* url);

typedef struct _metalink_checksum {
  /* message digest algorithm, for example, sha1, null terminated string */
  char* type;
  /* message digest in a ASCII hexadecimal notation, null terminated string */
  char* hash;
} metalink_checksum_t;

metalink_checksum_t* metalink_checksum_new(void);

void metalink_checksum_delete(metalink_checksum_t* checksum);

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
metalink_piece_hash_t* metalink_piece_hash_new(void);

/* destructor */
void metalink_piece_hash_delete(metalink_piece_hash_t* piece_hash);

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
metalink_chunk_checksum_t* metalink_chunk_checksum_new(void);

/* destructor */
void metalink_chunk_checksum_delete(metalink_chunk_checksum_t* chunk_checksum);

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
  /* file description */
  char* description;
  /* file size */
  long long int size;
  /* version, null terminated string */
  char* version;
  /* list of language, null terminated list of null terminated string */
  char** languages;
  /* list of os, null terminated list of null terminated string */
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
metalink_file_t* metalink_file_new(void);

/* destructor */
void metalink_file_delete(metalink_file_t* file);

/* mutators */
metalink_error_t metalink_file_set_name(metalink_file_t* file, const char* name);

metalink_error_t metalink_file_set_description(metalink_file_t* file, const char* description);

void metalink_file_set_size(metalink_file_t* file, long long int size);

metalink_error_t metalink_file_set_version(metalink_file_t* file, const char* version);

metalink_error_t metalink_file_add_language(metalink_file_t* file, const char* language);

metalink_error_t metalink_file_set_os(metalink_file_t* file, const char* os);

metalink_error_t metalink_file_set_resources(metalink_file_t* file, metalink_resource_t** resources);

void metalink_file_set_maxconnections(metalink_file_t* file, int maxconnections);

typedef struct _metalink {
  /* put more descriptable information here... */
  /* date, publisher or something useful */
  char* generator;
  char* origin;
  int published;
  int updated;

  /* list of metalink_file_t */
  metalink_file_t** files;
  char* identity;
  char* tags;
} metalink_t;

metalink_error_t metalink_set_identity(metalink_t* metalink, const char* identity);
metalink_error_t metalink_set_tags(metalink_t* metalink, const char* tags);
metalink_error_t metalink_set_generator(metalink_t* metalink, const char* generator);
metalink_error_t metalink_set_origin(metalink_t* metalink, const char* origin);
void metalink_set_published(metalink_t* metalink, int published);
void metalink_set_updated(metalink_t* metalink, int updated);

metalink_t* metalink_new(void);

void metalink_delete(metalink_t* metalink);

#ifdef __cplusplus
}
#endif

#endif /* _D_METALINK_TYPES_H_ */
