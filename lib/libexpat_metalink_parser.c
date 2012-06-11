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
#include <metalink/metalink_parser.h>

#include "metalink_config.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <expat.h>

#include "metalink_pstm.h"
#include "metalink_pstate.h"
#include "metalink_parser_common.h"
#include "metalink_session_data.h"
#include "metalink_stack.h"
#include "metalink_string_buffer.h"

#define NAMESPACE_SEPARATOR '\t'

static void split_ns_name(const char** localname,
			  const char** ns_uri,
			  const char* src)
{
  const char* sep;
  char* temp;
  size_t len;

  sep = strchr(src, NAMESPACE_SEPARATOR);
  if (sep) {
    *localname = sep+1;
    len = sep-src;
    temp = malloc((len+1) * sizeof **ns_uri);
    memcpy(temp, src, len);
    temp[len] = '\0';
    *ns_uri = temp;
  } else {
    *localname = src;
  }
}

static void start_element_handler(void* user_data,
				  const char* name,
				  const char** attrs)
{
  const char* localname = NULL;
  const char* ns_uri = NULL;

  metalink_session_data_t* session_data = (metalink_session_data_t*)user_data;
  metalink_string_buffer_t* str_buf = metalink_string_buffer_new(128);

  /* TODO evaluate return value of stack_push; non-zero value is error. */
  metalink_stack_push(session_data->characters_stack, str_buf);

  split_ns_name(&localname, &ns_uri, name);
  session_data->stm->state->start_fun(session_data->stm,
				      localname,
				      ns_uri,
				      (const char**)attrs);
  free((void *)ns_uri);
}

static void end_element_handler(void* user_data, const char* name)
{
  const char* localname = NULL;
  const char* ns_uri = NULL;

  metalink_session_data_t* session_data = (metalink_session_data_t*)user_data;
  metalink_string_buffer_t* str_buf = metalink_stack_pop(session_data->characters_stack);
  
  split_ns_name(&localname, &ns_uri, name);
  session_data->stm->state->end_fun(session_data->stm,
				    localname,
				    ns_uri,
				    metalink_string_buffer_str(str_buf));
  free((void *)ns_uri);

  metalink_string_buffer_delete(str_buf);	     
}

static void characters_handler(void* user_data, const char* chars,
			       int length)
{
  metalink_session_data_t* session_data = (metalink_session_data_t*)user_data;
  metalink_string_buffer_t* str_buf = metalink_stack_top(session_data->characters_stack);

  metalink_string_buffer_append(str_buf, (const char*)chars, length);
}

static XML_Parser setup_parser(metalink_session_data_t* session_data)
{
  XML_Parser parser;
  
  parser = XML_ParserCreateNS(NULL, NAMESPACE_SEPARATOR);
  
  XML_SetUserData(parser, session_data);
  XML_SetElementHandler(parser, &start_element_handler, &end_element_handler);
  XML_SetCharacterDataHandler(parser, &characters_handler);

  return parser;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_file(const char* filename, metalink_t** res)
{
  metalink_error_t r;
  FILE *docfp = fopen(filename, "rb");
  if(docfp == NULL)
    return METALINK_ERR_CANNOT_OPEN_FILE;
  r = metalink_parse_fp(docfp, res);
  fclose(docfp);   
  return r;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_fp(FILE* docfp, metalink_t** res)
{
  metalink_session_data_t* session_data;
  metalink_error_t r = 0,
		   retval;
  XML_Parser parser;

  session_data = metalink_session_data_new();
  
  parser = setup_parser(session_data);

  while(!feof(docfp)) {
    size_t num_read;
    void* buff = XML_GetBuffer(parser, BUFSIZ);
    if(buff == NULL) {
      r = METALINK_ERR_PARSER_ERROR;
      break;
    }
    num_read = fread(buff, 1, BUFSIZ, docfp);
    if(num_read == 0) {
      if(feof(docfp)) {
        break;
      } else if(ferror(docfp)) {
        r = METALINK_ERR_PARSER_ERROR;
        break;
      } else {
        assert(0);
      }
    }
    if(!XML_ParseBuffer(parser, num_read, 0)) {
      r = METALINK_ERR_PARSER_ERROR;
      break;
    }
  }
  XML_ParserFree(parser);
  
  retval = metalink_handle_parse_result(res, session_data, r);

  metalink_session_data_delete(session_data);

  return retval;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_fd(int fd, metalink_t** res)
{
  metalink_session_data_t* session_data;
  metalink_error_t r = 0;
  metalink_error_t retval;
  XML_Parser parser;

  session_data = metalink_session_data_new();

  parser = setup_parser(session_data);

  while(1) {
    ssize_t num_read;
    void* buff = XML_GetBuffer(parser, BUFSIZ);
    if(buff == NULL) {
      r = METALINK_ERR_PARSER_ERROR;
      break;
    }
    while((num_read = read(fd, buff, BUFSIZ)) == -1 && errno == EINTR);
    if(num_read == -1) {
      r = METALINK_ERR_PARSER_ERROR;
      break;
    } else if(num_read == 0) {
      break;
    }
    if(!XML_ParseBuffer(parser, num_read, 0)) {
      r = METALINK_ERR_PARSER_ERROR;
      break;
    }
  }
  XML_ParserFree(parser);

  retval = metalink_handle_parse_result(res, session_data, r);

  metalink_session_data_delete(session_data);

  return retval;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_memory(const char* buf, size_t len, metalink_t** res)
{
  metalink_session_data_t* session_data;
  metalink_error_t r = 0,
		   retval;
  XML_Parser parser;
  
  session_data = metalink_session_data_new();

  parser = setup_parser(session_data);

  if(!XML_Parse(parser, buf, len, 1)) {
    r = METALINK_ERR_PARSER_ERROR;
  }

  XML_ParserFree(parser);

  retval = metalink_handle_parse_result(res, session_data, r);

  metalink_session_data_delete(session_data);

  return retval; 
}

struct _metalink_parser_context
{
  metalink_session_data_t* session_data;
  XML_Parser parser;
  metalink_t* res;
};

metalink_parser_context_t
METALINK_PUBLIC
* metalink_parser_context_new(void)
{
  metalink_parser_context_t* ctx;
  ctx = malloc(sizeof(metalink_parser_context_t));
  if(ctx == NULL) {
    return NULL;
  }
  memset(ctx, 0, sizeof(metalink_parser_context_t));

  ctx->session_data = metalink_session_data_new();
  if(ctx->session_data == NULL) {
    metalink_parser_context_delete(ctx);
    return NULL;
  }

  ctx->parser = setup_parser(ctx->session_data);
  if(ctx->parser == NULL) {
    metalink_parser_context_delete(ctx);
    return NULL;
  }
  return ctx;
}

void
METALINK_PUBLIC
metalink_parser_context_delete(metalink_parser_context_t* ctx)
{
  if(ctx == NULL) {
    return;
  }
  metalink_session_data_delete(ctx->session_data);
  XML_ParserFree(ctx->parser);
  free(ctx);
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_update(metalink_parser_context_t* ctx,
		      const char* buf, size_t len)
{
  metalink_error_t r = 0;

  if(!XML_Parse(ctx->parser, buf, len, 0)) {
    r = METALINK_ERR_PARSER_ERROR;
  }

  if(r == 0) {
    r = metalink_pctrl_get_error(ctx->session_data->stm->ctrl);
  }
  return r;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_final(metalink_parser_context_t* ctx,
		     const char* buf, size_t len, metalink_t** res)
{
  metalink_error_t r = 0,
		   retval;

  if(!XML_Parse(ctx->parser, buf, len, 1)) {
    r = METALINK_ERR_PARSER_ERROR;
  }

  retval = metalink_handle_parse_result(res, ctx->session_data, r);

  metalink_parser_context_delete(ctx);

  return retval; 
}
