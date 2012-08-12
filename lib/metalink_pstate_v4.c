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
#include "metalink_pstate_v4.h"

#include <time.h>
#ifdef HAVE_TIME64_H
#  include <time64.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "metalink_pstm.h"
#include "metalink_helper.h"
#ifndef HAVE_STRPTIME
#  include "strptime.h"
#endif /* !HAVE_STRPTIME */
#ifndef HAVE_TIMEGM
#  include "timegm.h"
#endif /* !HAVE_TIMEGM */

static time_t my_timegm(struct tm *tm)
{
#ifdef HAVE__MKGMTIME
  /* WINE does not implement _mkgmtime() yet */
  return _mkgmtime(tm);
#elif defined(HAVE_MKGMTIME)
  return mkgmtime(tm);
#elif defined(HAVE_TIMEGM64)
  return (time_t)timegm64(tm);
#else
  return timegm(tm);
#endif
}

/* parse a RFC3339 formatted date, ie. 2010-05-01T12:15:02Z or
   2010-05-01T12:16:02+01:00 */
static time_t parse_date(const char* date)
{
  time_t t = 0;
  int sign = 0;
  struct tm tm = {0}, interval = {0};
  char* rest;

  if(strlen(date) < 20) 
  {
    return t;
  }

  rest = strptime(date, "%Y-%m-%dT%H:%M:%S", &tm);

  if(rest == NULL)
  {
    return t;
  }

  t = my_timegm(&tm);

  while(*rest != 'Z' && *rest != '+' && *rest != '-' && *rest != '\0')
  {
    rest++;
  }

  if(*rest == '\0')
  {
    /* not a valid RFC3339 date, but we can still return what has already been
     * parsed */
    return t;
  }
  if(*rest == '+') {
    sign = -1; 
  } else if(*rest == '-') {
    sign = +1; 
  }

  if(sign != 0) {
    if(strptime(rest+1, "%H:%M", &interval) == NULL)
      return t;
    t += sign*(interval.tm_hour*3600+interval.tm_min*60);
  }

  return t;
}

/* metalink state <metalink> */
void metalink_state_start_fun_v4(metalink_pstm_t* stm,
				 const char* name, const char* ns_uri,
				 const char** attrs)
{
  metalink_error_t r;

  if(strcmp("file", name) == 0) {
    const char* fname;
    metalink_file_t* file;

    fname = get_attribute_value(attrs, "name");
    if(!metalink_check_safe_path(fname)) {
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    file = metalink_pctrl_new_file_transaction(stm->ctrl);
    if(!file) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_pctrl_file_set_name(stm->ctrl, fname);
    if(r !=  0) {
      error_handler(stm, r);
      return;
    }

    metalink_pstm_enter_file_state_v4(stm);
  } else if(strcmp("generator", name) == 0) {
    metalink_pstm_enter_generator_state(stm);
  } else if(strcmp("origin", name) == 0) {
    const char* dynamic_attr;
    dynamic_attr = get_attribute_value(attrs, "dynamic");
    if(dynamic_attr && strcmp("true", dynamic_attr) == 0) {
      metalink_pctrl_set_origin_dynamic(stm->ctrl, 1);
    }
    metalink_pstm_enter_origin_state(stm);
  } else if(strcmp("published", name) == 0) {
    metalink_pstm_enter_published_state_v4(stm);
  } else if(strcmp("updated", name) == 0) {
    metalink_pstm_enter_updated_state_v4(stm);
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void metalink_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_metalink_accumulate_files(stm->ctrl);
  if (r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_fin_state(stm);
}

/* generator state <generator> */
void generator_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void generator_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters)
{
  if(strcmp("generator", name) == 0) {
    metalink_pctrl_set_generator(stm->ctrl, characters);
  } else {
    metalink_pstm_enter_skip_state(stm);
    return;
  }
  metalink_pstm_enter_metalink_state_v4(stm);
}

/* origin state <origin> */
void origin_state_start_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void origin_state_end_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char* characters)
{
  if(strcmp("origin", name) == 0) {
    metalink_pctrl_set_origin(stm->ctrl, characters);
  } else {
    metalink_pstm_enter_skip_state(stm);
    return;
  }
  metalink_pstm_enter_metalink_state_v4(stm);
}

/* file state <file> */
void file_state_start_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char** attrs)
{
  metalink_error_t r;

  if(strcmp("url", name) == 0) {
    const char* location;
    const char* value;
    long int priority = 999999;
    metalink_resource_t* resource;

    resource = metalink_pctrl_new_resource_transaction(stm->ctrl);
    if(!resource) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }

    location = get_attribute_value(attrs, "location");
    if(location) {
      r = metalink_pctrl_resource_set_location(stm->ctrl, location);
      if(r != 0) {
	error_handler(stm, r);
	return;
      }
    }

    value = get_attribute_value(attrs, "priority");
    if(value) {
      errno = 0;
      priority = strtol(value, 0, 10);
      if(errno == ERANGE || priority < 0 || priority > INT_MAX) {
	priority = 999999;
      }
    }
    metalink_pctrl_resource_set_priority(stm->ctrl, priority);

    metalink_pstm_enter_url_state(stm);
  } else if(strcmp("metaurl", name) == 0) {
    const char* mediatype;
    const char* name;
    const char* value;
    long int priority = 999999;
    metalink_metaurl_t* metaurl;

    metaurl = metalink_pctrl_new_metaurl_transaction(stm->ctrl);
    if(!metaurl) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }

    mediatype = get_attribute_value(attrs, "mediatype");
    if(mediatype) {
      r = metalink_pctrl_metaurl_set_mediatype(stm->ctrl, mediatype);
      if(r != 0) {
	error_handler(stm, r);
	return;
      }
    } else {
      /* mediatype argument is mandatory, skip if not present */
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    name = get_attribute_value(attrs, "name");
    if(name) {
      r = metalink_pctrl_metaurl_set_name(stm->ctrl, name);
      if(r != 0) {
	error_handler(stm, r);
	return;
      }
    }

    value = get_attribute_value(attrs, "priority");
    if(value) {
      errno = 0;
      priority = strtol(value, 0, 10);
      if(errno == ERANGE || priority < 0 || priority > INT_MAX) {
	priority = 999999;
      }
    }
    metalink_pctrl_metaurl_set_priority(stm->ctrl, priority);

    metalink_pstm_enter_metaurl_state_v4(stm);
  } else if(strcmp("hash", name) == 0) {
    const char* type;
    metalink_checksum_t* checksum;

    type = get_attribute_value(attrs, "type");
    if(!type) {
      metalink_pstm_enter_skip_state(stm);
      return;
    }
    checksum = metalink_pctrl_new_checksum_transaction(stm->ctrl);
    if(!checksum) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_checksum_set_type(checksum, type);
    if(r != 0) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    metalink_pstm_enter_hash_state(stm);
  } else if(strcmp("pieces", name) == 0) {
    const char* type;
    const char* value;
    long int length;
    metalink_chunk_checksum_t* chunk_checksum;

    type = get_attribute_value(attrs, "type");
    if(!type) {
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    value = get_attribute_value(attrs, "length");
    if(value) {
      errno = 0;
      length = strtol(value, 0, 10);
      if(errno == ERANGE || length < 0 || length > INT_MAX) {
	/* Invalid piece size: skip this tag */
	metalink_pstm_enter_skip_state(stm);
	return;
      }
    } else {
      metalink_pstm_enter_skip_state(stm);
      return;
    }

    chunk_checksum = metalink_pctrl_new_chunk_checksum_transaction(stm->ctrl);
    if(!chunk_checksum) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_chunk_checksum_set_type(chunk_checksum, type);
    if(r != 0) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    metalink_chunk_checksum_set_length(chunk_checksum, length);

    metalink_pstm_enter_pieces_state_v4(stm);
  } else if(strcmp("signature", name) == 0) {
    const char* mediatype;
    metalink_checksum_t* signature;

    mediatype = get_attribute_value(attrs, "mediatype");
    if(!mediatype) {
      metalink_pstm_enter_skip_state(stm);
      return;
    }
    signature = metalink_pctrl_new_checksum_transaction(stm->ctrl);
    if(!signature) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    r = metalink_checksum_set_type(signature, mediatype);
    if(r != 0) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }
    metalink_pstm_enter_signature_state_v4(stm);
  } else if(strcmp("publisher", name) == 0) {
    const char* name;
    const char* url;

    name = get_attribute_value(attrs, "name");
    if(!name) {
      /* name is mandatory */
      metalink_pstm_enter_skip_state(stm);
      return;
    }
    r = metalink_pctrl_file_set_publisher_name(stm->ctrl, name);
    if(r != 0) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }

    url = get_attribute_value(attrs, "url");
    if(url) {
      /* url is optional */
      r = metalink_pctrl_file_set_publisher_url(stm->ctrl, url);
      if(r != 0) {
        error_handler(stm, METALINK_ERR_BAD_ALLOC);
        return;
      }
    }
    metalink_pstm_enter_skip_state(stm);
  } else if(strcmp("description", name) == 0) {
    metalink_pstm_enter_description_state_v4(stm);
  } else if(strcmp("copyright", name) == 0) {
    metalink_pstm_enter_copyright_state_v4(stm);
  } else if(strcmp("identity", name) == 0) {
    metalink_pstm_enter_identity_state_v4(stm);
  } else if(strcmp("logo", name) == 0) {
    metalink_pstm_enter_logo_state_v4(stm);
  } else if(strcmp("language", name) == 0) {
    metalink_pstm_enter_language_state(stm);
  } else if(strcmp("os", name) == 0) {
    metalink_pstm_enter_os_state(stm);
  } else if(strcmp("size", name) == 0) {
    metalink_pstm_enter_size_state(stm);
  } else if(strcmp("version", name) == 0) {
    metalink_pstm_enter_version_state(stm);
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void file_state_end_fun_v4(metalink_pstm_t* stm,
			   const char* name, const char* ns_uri,
			   const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_commit_file_transaction(stm->ctrl);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_metalink_state_v4(stm);
}

/* description state <description> */
void description_state_start_fun_v4(metalink_pstm_t* stm,
				    const char* name, const char* ns_uri,
				    const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void description_state_end_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_file_set_description(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_file_state_v4(stm);
}

/* copyright state <copyright> */
void copyright_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void copyright_state_end_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_file_set_copyright(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_file_state_v4(stm);
}

/* identity state <identity> */
void identity_state_start_fun_v4(metalink_pstm_t* stm,
				 const char* name, const char* ns_uri,
				 const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void identity_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_file_set_identity(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_file_state_v4(stm);
}

/* logo state <logo> */
void logo_state_start_fun_v4(metalink_pstm_t* stm,
			    const char* name, const char* ns_uri,
			    const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void logo_state_end_fun_v4(metalink_pstm_t* stm,
			   const char* name, const char* ns_uri,
			   const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_file_set_logo(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_file_state_v4(stm);
}

/* signature state <signature> */
void signature_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void signature_state_end_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_checksum_set_hash(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }
  metalink_pstm_enter_file_state_v4(stm);
}

/* pieces state <pieces> */
void pieces_state_start_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char** attrs)
{
  if(strcmp("hash", name) == 0) {
    metalink_piece_hash_t* piece_hash;

    
    piece_hash = metalink_pctrl_new_piece_hash_transaction(stm->ctrl);
    if(!piece_hash) {
      error_handler(stm, METALINK_ERR_BAD_ALLOC);
      return;
    }

    metalink_pstm_enter_piece_hash_state(stm);
  } else {
    metalink_pstm_enter_skip_state(stm);
  }
}

void pieces_state_end_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_commit_chunk_checksum_transaction(stm->ctrl);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }

  metalink_pstm_enter_file_state_v4(stm);
}

/* metaurl state <metaurl> */
void metaurl_state_start_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void metaurl_state_end_fun_v4(metalink_pstm_t* stm,
			      const char* name, const char* ns_uri,
			      const char* characters)
{
  metalink_error_t r;
  r = metalink_pctrl_metaurl_set_url(stm->ctrl, characters);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }

  r = metalink_pctrl_commit_metaurl_transaction(stm->ctrl);
  if(r != 0) {
    error_handler(stm, r);
    return;
  }

  metalink_pstm_enter_file_state_v4(stm);
}

/* published state <published> */
void published_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void published_state_end_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char* characters)
{
  time_t t = parse_date(characters);
  metalink_pctrl_set_published(stm->ctrl, t);
  metalink_pstm_enter_metalink_state_v4(stm);
}

/* updated state <updated> */
void updated_state_start_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char** attrs)
{
  metalink_pstm_enter_skip_state(stm);
}

void updated_state_end_fun_v4(metalink_pstm_t* stm,
			      const char* name, const char* ns_uri,
			      const char* characters)
{
  time_t t = parse_date(characters);
  metalink_pctrl_set_updated(stm->ctrl, t);
  metalink_pstm_enter_metalink_state_v4(stm);
}
