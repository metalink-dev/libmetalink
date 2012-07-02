/*
 * Sample for libmetalink parser. This program prints the parsed
 * result of given Metalink files.
 *
 * To compile:
 * gcc -Wall -g -O2 -o sample sample.c -lmetalink
 *
 * Usage: metalinkcat <METALINK_FILE>...
 */
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#include <metalink/metalink.h>

/* 
 * Usually on most platforms one would just include inttypes.h,
 * but unfortunately some platforms either might lack this
 * header, or do not define the PRI* macros, so we do it here.
 */ 
#ifndef PRId64
#ifdef _WIN32
#define PRId64 "I64d"
#else
#define PRId64 "lld"
#endif  /* _WIN32 */
#endif  /* PRId64 */


int main(int argc, char** argv)
{
  int i;
  if(argc < 2) {
    printf("Usage: %s <METALINK_FILE> [...]\n", argv[0]);
    return EXIT_SUCCESS;
  }
  for(i = 1; i < argc; ++i) {
    metalink_error_t r;
    metalink_t* metalink;
    metalink_file_t* file;
    metalink_checksum_t** checksums;

    r = metalink_parse_file(argv[i], &metalink);

    if(r != 0) {
      fprintf(stderr, "ERROR (%d): %s\n", r, metalink_strerror(r));
      exit(EXIT_FAILURE);
    }
 
    file = metalink->files[0];
    printf("name: %s\n", file->name);
    printf("size: %" PRId64 "\n", (int64_t)file->size);
    printf("os  : %s\n", file->os);

    if(file->checksums) {
      checksums = file->checksums;
      while(*checksums) {
        printf("hash: %s %s\n", (*checksums)->type, (*checksums)->hash);
        ++checksums;
      }
    }
    if(file->chunk_checksum) {
      size_t count = 0;
      metalink_piece_hash_t** piece_hashes;
      printf("chunk checksum: size=%d, type=%s\n", file->chunk_checksum->length,
             file->chunk_checksum->type);
      printf("first 5 piece hashes...\n");
      piece_hashes = file->chunk_checksum->piece_hashes;
      while(*piece_hashes && count < 5) {
        printf("piece=%d, hash=%s\n",
               (*piece_hashes)->piece, (*piece_hashes)->hash);
        ++piece_hashes;
        ++count;
      }
      printf("...\n");
    }
    if(file->resources) {
      size_t count = 0;
      metalink_resource_t** resources;
      printf("first 5 resources...\n");
      resources = file->resources;
      while(*resources && count < 5) {
        printf("type=%s, location=%s, preference=%d, url=%s\n",
               (*resources)->type, (*resources)->location,
               (*resources)->preference, (*resources)->url);
        ++resources;
        ++count;
      }
      printf("...\n");
    }

    /* delete metalink_t */
    metalink_delete(metalink);
  }
  return EXIT_SUCCESS;
}
