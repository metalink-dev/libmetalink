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

#include <string.h>

metalink_pctrl_t *new_metalink_pctrl(void) {
  metalink_pctrl_t *ctrl;
  ctrl = malloc(sizeof(metalink_pctrl_t));
  if (!ctrl) {
    return NULL;
  }
  memset(ctrl, 0, sizeof(metalink_pctrl_t));
  ctrl->metalink = metalink_new();
  if (!ctrl->metalink) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->files = metalink_list_new();
  if (!ctrl->files) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->languages = metalink_list_new();
  if (!ctrl->languages) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->oses = metalink_list_new();
  if (!ctrl->oses) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->resources = metalink_list_new();
  if (!ctrl->resources) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->metaurls = metalink_list_new();
  if (!ctrl->metaurls) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->checksums = metalink_list_new();
  if (!ctrl->checksums) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  ctrl->piece_hashes = metalink_list_new();
  if (!ctrl->piece_hashes) {
    goto NEW_METALINK_PCTRL_ERROR;
  }
  return ctrl;
NEW_METALINK_PCTRL_ERROR:
  delete_metalink_pctrl(ctrl);
  return NULL;
}

void delete_metalink_pctrl(metalink_pctrl_t *ctrl) {
  if (!ctrl) {
    return;
  }
  metalink_delete(ctrl->metalink);

  metalink_list_for_each(ctrl->files,
                         (void (*)(void *)) & metalink_file_delete);
  metalink_list_delete(ctrl->files);
  metalink_file_delete(ctrl->temp_file);

  /* metalink_list_delete takes care of freeing the pointers in those lists */
  metalink_list_delete(ctrl->languages);
  metalink_list_delete(ctrl->oses);

  metalink_list_for_each(ctrl->resources,
                         (void (*)(void *)) & metalink_resource_delete);
  metalink_list_delete(ctrl->resources);
  metalink_resource_delete(ctrl->temp_resource);

  metalink_list_for_each(ctrl->metaurls,
                         (void (*)(void *)) & metalink_metaurl_delete);
  metalink_list_delete(ctrl->metaurls);
  metalink_metaurl_delete(ctrl->temp_metaurl);

  metalink_list_for_each(ctrl->checksums,
                         (void (*)(void *)) & metalink_checksum_delete);
  metalink_list_delete(ctrl->checksums);
  metalink_checksum_delete(ctrl->temp_checksum);

  metalink_chunk_checksum_delete(ctrl->temp_chunk_checksum);

  metalink_list_for_each(ctrl->piece_hashes,
                         (void (*)(void *)) & metalink_piece_hash_delete);
  metalink_list_delete(ctrl->piece_hashes);
  metalink_piece_hash_delete(ctrl->temp_piece_hash);

  metalink_signature_delete(ctrl->temp_signature);

  free(ctrl);
}

metalink_t *metalink_pctrl_detach_metalink(metalink_pctrl_t *ctrl) {
  metalink_t *metalink;
  metalink = ctrl->metalink;
  ctrl->metalink = NULL;
  return metalink;
}

void metalink_pctrl_set_error(metalink_pctrl_t *ctrl, metalink_error_t error) {
  ctrl->error = error;
}

metalink_error_t metalink_pctrl_get_error(metalink_pctrl_t *ctrl) {
  return ctrl->error;
}

metalink_error_t
metalink_pctrl_metalink_accumulate_files(metalink_pctrl_t *ctrl) {
  size_t files_length;
  files_length = metalink_list_length(ctrl->files);
  if (files_length) {
    ctrl->metalink->files = calloc(files_length + 1, sizeof(metalink_file_t *));
    if (!ctrl->metalink->files) {
      return METALINK_ERR_BAD_ALLOC;
    }
    metalink_list_to_array(ctrl->files, (void **)ctrl->metalink->files);
    ctrl->metalink->files[files_length] = NULL;

    metalink_list_clear(ctrl->files);
  }
  return 0;
}

static metalink_error_t
commit_list_to_array(void ***array_ptr, metalink_list_t *src, size_t ele_size) {
  size_t size;
  size = metalink_list_length(src);
  if (size) {
    *array_ptr = calloc(size + 1, ele_size);
    if (!*array_ptr) {
      return METALINK_ERR_BAD_ALLOC;
    }
    metalink_list_to_array(src, *array_ptr);
    (*array_ptr)[size] = NULL;
    metalink_list_clear(src);
  }
  return 0;
}

/* transaction functions */
metalink_file_t *metalink_pctrl_new_file_transaction(metalink_pctrl_t *ctrl) {
  if (ctrl->temp_file) {
    metalink_file_delete(ctrl->temp_file);
  }
  ctrl->temp_file = metalink_file_new();

  metalink_list_clear(ctrl->languages);
  metalink_list_clear(ctrl->oses);

  metalink_list_for_each(ctrl->resources,
                         (void (*)(void *)) & metalink_resource_delete);
  metalink_list_clear(ctrl->resources);

  metalink_list_for_each(ctrl->metaurls,
                         (void (*)(void *)) & metalink_metaurl_delete);
  metalink_list_clear(ctrl->metaurls);

  metalink_list_for_each(ctrl->checksums,
                         (void (*)(void *)) & metalink_checksum_delete);
  metalink_list_clear(ctrl->checksums);

  return ctrl->temp_file;
}

static int resource_pri_comp(const void *lhs, const void *rhs) {
  return (*(const metalink_resource_t **)lhs)->priority -
         (*(const metalink_resource_t **)rhs)->priority;
}

static int metaurl_pri_comp(const void *lhs, const void *rhs) {
  return (*(const metalink_metaurl_t **)lhs)->priority -
         (*(const metalink_metaurl_t **)rhs)->priority;
}

metalink_error_t
metalink_pctrl_commit_file_transaction(metalink_pctrl_t *ctrl) {
  metalink_error_t r;
  if (!ctrl->temp_file) {
    return METALINK_ERR_NO_FILE_TRANSACTION;
  }

  /* copy ctrl->languages to ctrl->temp_file->languages */
  r = commit_list_to_array((void *)&ctrl->temp_file->languages, ctrl->languages,
                           sizeof(char *));
  if (r != 0) {
    return r;
  }
  if (ctrl->temp_file->languages) {
    ctrl->temp_file->language = ctrl->temp_file->languages[0];
  }

  /* copy ctrl->oses to ctrl->temp_file->oses */
  r = commit_list_to_array((void *)&ctrl->temp_file->oses, ctrl->oses,
                           sizeof(char *));
  if (r != 0) {
    return r;
  }
  if (ctrl->temp_file->oses) {
    ctrl->temp_file->os = ctrl->temp_file->oses[0];
  }

  /* copy ctrl->resources to ctrl->temp_file->resources */
  r = commit_list_to_array((void *)&ctrl->temp_file->resources, ctrl->resources,
                           sizeof(metalink_resource_t *));
  if (r != 0) {
    return r;
  }
  if (ctrl->temp_file->resources) {
    size_t size;
    for (size = 0; ctrl->temp_file->resources[size]; ++size)
      ;
    /* Sort by priority */
    qsort(ctrl->temp_file->resources, size, sizeof(metalink_resource_t *),
          resource_pri_comp);
  }

  /* copy ctrl->metaurls to ctrl->temp_file->metaurls */
  r = commit_list_to_array((void *)&ctrl->temp_file->metaurls, ctrl->metaurls,
                           sizeof(metalink_metaurl_t *));
  if (r != 0) {
    return r;
  }
  if (ctrl->temp_file->metaurls) {
    size_t size;
    for (size = 0; ctrl->temp_file->metaurls[size]; ++size)
      ;
    /* Sort by priority */
    qsort(ctrl->temp_file->metaurls, size, sizeof(metalink_metaurl_t *),
          metaurl_pri_comp);
  }

  /* copy ctrl->checksums to ctrl->temp_file->checksums */
  r = commit_list_to_array((void *)&ctrl->temp_file->checksums, ctrl->checksums,
                           sizeof(metalink_checksum_t *));
  if (r != 0) {
    return r;
  }

  if (metalink_list_append(ctrl->files, ctrl->temp_file) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }

  ctrl->temp_file = NULL;
  return 0;
}

metalink_resource_t *
metalink_pctrl_new_resource_transaction(metalink_pctrl_t *ctrl) {
  if (ctrl->temp_resource) {
    metalink_resource_delete(ctrl->temp_resource);
  }
  ctrl->temp_resource = metalink_resource_new();
  return ctrl->temp_resource;
}

metalink_error_t
metalink_pctrl_commit_resource_transaction(metalink_pctrl_t *ctrl) {
  if (!ctrl->temp_resource) {
    return METALINK_ERR_NO_RESOURCE_TRANSACTION;
  }

  if (metalink_list_append(ctrl->resources, ctrl->temp_resource) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }

  ctrl->temp_resource = NULL;
  return 0;
}

metalink_metaurl_t *
metalink_pctrl_new_metaurl_transaction(metalink_pctrl_t *ctrl) {
  if (ctrl->temp_metaurl) {
    metalink_metaurl_delete(ctrl->temp_metaurl);
  }
  ctrl->temp_metaurl = metalink_metaurl_new();
  return ctrl->temp_metaurl;
}

metalink_error_t
metalink_pctrl_commit_metaurl_transaction(metalink_pctrl_t *ctrl) {
  if (!ctrl->temp_metaurl) {
    return METALINK_ERR_NO_RESOURCE_TRANSACTION;
  }

  if (metalink_list_append(ctrl->metaurls, ctrl->temp_metaurl) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }

  ctrl->temp_metaurl = NULL;
  return 0;
}

metalink_checksum_t *
metalink_pctrl_new_checksum_transaction(metalink_pctrl_t *ctrl) {
  if (ctrl->temp_checksum) {
    metalink_checksum_delete(ctrl->temp_checksum);
  }
  ctrl->temp_checksum = metalink_checksum_new();
  return ctrl->temp_checksum;
}

metalink_error_t
metalink_pctrl_commit_checksum_transaction(metalink_pctrl_t *ctrl) {
  if (!ctrl->temp_checksum) {
    return METALINK_ERR_NO_CHECKSUM_TRANSACTION;
  }

  if (metalink_list_append(ctrl->checksums, ctrl->temp_checksum) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }
  ctrl->temp_checksum = NULL;
  return 0;
}

metalink_chunk_checksum_t *
metalink_pctrl_new_chunk_checksum_transaction(metalink_pctrl_t *ctrl) {
  if (ctrl->temp_chunk_checksum) {
    metalink_chunk_checksum_delete(ctrl->temp_chunk_checksum);
  }

  ctrl->temp_chunk_checksum = metalink_chunk_checksum_new();
  metalink_list_for_each(ctrl->piece_hashes,
                         (void (*)(void *)) & metalink_piece_hash_delete);
  metalink_list_clear(ctrl->piece_hashes);

  return ctrl->temp_chunk_checksum;
}

metalink_error_t
metalink_pctrl_commit_chunk_checksum_transaction(metalink_pctrl_t *ctrl) {
  metalink_error_t r;
  if (!ctrl->temp_chunk_checksum) {
    return METALINK_ERR_NO_CHUNK_CHECKSUM_TRANSACTION;
  }
  if (!ctrl->temp_file) {
    return METALINK_ERR_NO_FILE_TRANSACTION;
  }
  r = commit_list_to_array((void *)&ctrl->temp_chunk_checksum->piece_hashes,
                           ctrl->piece_hashes, sizeof(metalink_piece_hash_t *));
  if (r != 0) {
    return r;
  }
  metalink_list_clear(ctrl->piece_hashes);
  ctrl->temp_file->chunk_checksum = ctrl->temp_chunk_checksum;
  ctrl->temp_chunk_checksum = NULL;
  return 0;
}

metalink_piece_hash_t *
metalink_pctrl_new_piece_hash_transaction(metalink_pctrl_t *ctrl) {
  if (ctrl->temp_piece_hash) {
    metalink_piece_hash_delete(ctrl->temp_piece_hash);
  }
  ctrl->temp_piece_hash = metalink_piece_hash_new();
  return ctrl->temp_piece_hash;
}

metalink_error_t
metalink_pctrl_commit_piece_hash_transaction(metalink_pctrl_t *ctrl) {
  if (!ctrl->temp_piece_hash) {
    return METALINK_ERR_NO_PIECE_HASH_TRANSACTION;
  }
  if (metalink_list_append(ctrl->piece_hashes, (void *)ctrl->temp_piece_hash) !=
      0) {
    return METALINK_ERR_BAD_ALLOC;
  }
  ctrl->temp_piece_hash = NULL;
  return 0;
}

metalink_signature_t *
metalink_pctrl_new_signature_transaction(metalink_pctrl_t *ctrl) {
  if (ctrl->temp_signature) {
    metalink_signature_delete(ctrl->temp_signature);
  }
  ctrl->temp_signature = metalink_signature_new();
  return ctrl->temp_signature;
}

metalink_error_t
metalink_pctrl_commit_signature_transaction(metalink_pctrl_t *ctrl) {
  if (!ctrl->temp_signature) {
    return METALINK_ERR_NO_SIGNATURE_TRANSACTION;
  }
  if (ctrl->temp_file->signature) {
    metalink_signature_delete(ctrl->temp_file->signature);
  }
  ctrl->temp_file->signature = ctrl->temp_signature;
  ctrl->temp_signature = NULL;
  return 0;
}

/* metalink manipulation functions */
void metalink_pctrl_set_version(metalink_pctrl_t *ctrl,
                                metalink_version_t version) {
  metalink_set_version(ctrl->metalink, version);
}

metalink_error_t metalink_pctrl_add_language(metalink_pctrl_t *ctrl,
                                             const char *language) {
  char *l;

  l = strdup(language);
  if (!l || metalink_list_append(ctrl->languages, l) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }
  return 0;
}

metalink_error_t metalink_pctrl_add_os(metalink_pctrl_t *ctrl, const char *os) {
  char *o;

  o = strdup(os);
  if (!o || metalink_list_append(ctrl->oses, o) != 0) {
    return METALINK_ERR_BAD_ALLOC;
  }
  return 0;
}

metalink_error_t metalink_pctrl_set_identity(metalink_pctrl_t *ctrl,
                                             const char *identity) {
  return metalink_set_identity(ctrl->metalink, identity);
}

metalink_error_t metalink_pctrl_set_tags(metalink_pctrl_t *ctrl,
                                         const char *tags) {
  return metalink_set_tags(ctrl->metalink, tags);
}

/* file manipulation functions*/
metalink_error_t metalink_pctrl_file_set_language(metalink_pctrl_t *ctrl,
                                                  const char *language) {
  char *l;

  if (ctrl->languages) {
    metalink_list_delete(ctrl->languages);
  }

  l = strdup(language);
  ctrl->languages = metalink_list_new();
  if (!ctrl->languages || !l || metalink_list_append(ctrl->languages, l) != 0) {
    if (l) free(l);
    return METALINK_ERR_BAD_ALLOC;
  }

  return 0;
}

metalink_error_t metalink_pctrl_file_set_os(metalink_pctrl_t *ctrl,
                                            const char *os) {
  char *o;

  if (ctrl->oses) {
    metalink_list_delete(ctrl->oses);
  }

  o = strdup(os);
  ctrl->oses = metalink_list_new();
  if (!ctrl->oses || !o || metalink_list_append(ctrl->oses, o) != 0) {
    if (o) free(o);
    return METALINK_ERR_BAD_ALLOC;
  }

  return 0;
}

metalink_error_t metalink_pctrl_file_set_name(metalink_pctrl_t *ctrl,
                                              const char *name) {
  return metalink_file_set_name(ctrl->temp_file, name);
}

metalink_error_t metalink_pctrl_file_set_description(metalink_pctrl_t *ctrl,
                                                     const char *description) {
  return metalink_file_set_description(ctrl->temp_file, description);
}

metalink_error_t metalink_pctrl_file_set_copyright(metalink_pctrl_t *ctrl,
                                                   const char *copyright) {
  return metalink_file_set_copyright(ctrl->temp_file, copyright);
}

metalink_error_t metalink_pctrl_file_set_identity(metalink_pctrl_t *ctrl,
                                                  const char *identity) {
  return metalink_file_set_identity(ctrl->temp_file, identity);
}

metalink_error_t metalink_pctrl_file_set_logo(metalink_pctrl_t *ctrl,
                                              const char *logo) {
  return metalink_file_set_logo(ctrl->temp_file, logo);
}

metalink_error_t metalink_pctrl_file_set_publisher_name(metalink_pctrl_t *ctrl,
                                                        const char *name) {
  return metalink_file_set_publisher_name(ctrl->temp_file, name);
}

metalink_error_t metalink_pctrl_file_set_publisher_url(metalink_pctrl_t *ctrl,
                                                       const char *url) {
  return metalink_file_set_publisher_url(ctrl->temp_file, url);
}

void metalink_pctrl_file_set_size(metalink_pctrl_t *ctrl, long long int size) {
  metalink_file_set_size(ctrl->temp_file, size);
}

metalink_error_t metalink_pctrl_file_set_version(metalink_pctrl_t *ctrl,
                                                 const char *version) {
  return metalink_file_set_version(ctrl->temp_file, version);
}

void metalink_pctrl_file_set_maxconnections(metalink_pctrl_t *ctrl,
                                            int maxconnections) {
  metalink_file_set_maxconnections(ctrl->temp_file, maxconnections);
}

/* resource manipulation functions */
metalink_error_t metalink_pctrl_resource_set_type(metalink_pctrl_t *ctrl,
                                                  const char *type) {
  return metalink_resource_set_type(ctrl->temp_resource, type);
}

metalink_error_t metalink_pctrl_resource_set_location(metalink_pctrl_t *ctrl,
                                                      const char *location) {
  return metalink_resource_set_location(ctrl->temp_resource, location);
}

void metalink_pctrl_resource_set_preference(metalink_pctrl_t *ctrl,
                                            int preference) {
  metalink_resource_set_preference(ctrl->temp_resource, preference);
}

void metalink_pctrl_resource_set_priority(metalink_pctrl_t *ctrl,
                                          int priority) {
  metalink_resource_set_priority(ctrl->temp_resource, priority);
}

void metalink_pctrl_resource_set_maxconnections(metalink_pctrl_t *ctrl,
                                                int maxconnections) {
  metalink_resource_set_maxconnections(ctrl->temp_resource, maxconnections);
}

metalink_error_t metalink_pctrl_resource_set_url(metalink_pctrl_t *ctrl,
                                                 const char *url) {
  return metalink_resource_set_url(ctrl->temp_resource, url);
}

/* metaurl manipulation functions */
metalink_error_t metalink_pctrl_metaurl_set_mediatype(metalink_pctrl_t *ctrl,
                                                      const char *mediatype) {
  return metalink_metaurl_set_mediatype(ctrl->temp_metaurl, mediatype);
}

metalink_error_t metalink_pctrl_metaurl_set_name(metalink_pctrl_t *ctrl,
                                                 const char *name) {
  return metalink_metaurl_set_name(ctrl->temp_metaurl, name);
}

void metalink_pctrl_metaurl_set_priority(metalink_pctrl_t *ctrl, int priority) {
  metalink_metaurl_set_priority(ctrl->temp_metaurl, priority);
}

metalink_error_t metalink_pctrl_metaurl_set_url(metalink_pctrl_t *ctrl,
                                                const char *url) {
  return metalink_metaurl_set_url(ctrl->temp_metaurl, url);
}

/* checksum manipulation functions */
metalink_error_t metalink_pctrl_checksum_set_type(metalink_pctrl_t *ctrl,
                                                  const char *type) {
  return metalink_checksum_set_type(ctrl->temp_checksum, type);
}

metalink_error_t metalink_pctrl_checksum_set_hash(metalink_pctrl_t *ctrl,
                                                  const char *hash) {
  return metalink_checksum_set_hash(ctrl->temp_checksum, hash);
}

/* piece hash manipulation functions */
void metalink_pctrl_piece_hash_set_piece(metalink_pctrl_t *ctrl, int piece) {
  metalink_piece_hash_set_piece(ctrl->temp_piece_hash, piece);
}

metalink_error_t metalink_pctrl_piece_hash_set_hash(metalink_pctrl_t *ctrl,
                                                    const char *hash) {
  return metalink_piece_hash_set_hash(ctrl->temp_piece_hash, hash);
}

/* chunk checksum manipulation functions */
metalink_error_t metalink_pctrl_chunk_checksum_set_type(metalink_pctrl_t *ctrl,
                                                        const char *type) {
  return metalink_chunk_checksum_set_type(ctrl->temp_chunk_checksum, type);
}

void metalink_pctrl_chunk_checksum_set_length(metalink_pctrl_t *ctrl,
                                              int length) {
  metalink_chunk_checksum_set_length(ctrl->temp_chunk_checksum, length);
}

void metalink_pctrl_chunk_checksum_set_piece_hashes(
    metalink_pctrl_t *ctrl, metalink_piece_hash_t **piece_hashes) {
  metalink_chunk_checksum_set_piece_hashes(ctrl->temp_chunk_checksum,
                                           piece_hashes);
}

/* signature manipulation functions */
metalink_error_t metalink_pctrl_signature_set_signature(metalink_pctrl_t *ctrl,
                                                        const char *signature) {
  return metalink_signature_set_signature(ctrl->temp_signature, signature);
}

/* information functions */
metalink_error_t metalink_pctrl_set_generator(metalink_pctrl_t *ctrl,
                                              const char *generator) {
  return metalink_set_generator(ctrl->metalink, generator);
}

metalink_error_t metalink_pctrl_set_origin(metalink_pctrl_t *ctrl,
                                           const char *origin) {
  return metalink_set_origin(ctrl->metalink, origin);
}

void metalink_pctrl_set_origin_dynamic(metalink_pctrl_t *ctrl,
                                       int origin_dynamic) {
  metalink_set_origin_dynamic(ctrl->metalink, origin_dynamic);
}

void metalink_pctrl_set_published(metalink_pctrl_t *ctrl, time_t published) {
  metalink_set_published(ctrl->metalink, published);
}

void metalink_pctrl_set_updated(metalink_pctrl_t *ctrl, time_t updated) {
  metalink_set_updated(ctrl->metalink, updated);
}
