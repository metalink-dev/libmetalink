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

#include <libxml/parser.h>

#include "metalink_pstm.h"
#include "metalink_pstate.h"
#include "metalink_parser_common.h"
#include "metalink_session_data.h"
#include "metalink_stack.h"
#include "metalink_string_buffer.h"

static void start_element_handler(void* user_data,
                                  const xmlChar* localname,
                                  const xmlChar* prefix,
                                  const xmlChar* ns_uri,
                                  int numNamespaces,
                                  const xmlChar** namespaces,
                                  int numAttrs,
                                  int numDefaulted,
                                  const xmlChar** attrs)
{
  metalink_session_data_t* session_data = (metalink_session_data_t*)user_data;
  metalink_string_buffer_t* str_buf = metalink_string_buffer_new(128);
  char* attrblock;
  const char** attr_index;
  char* value_dst_ptr;
  size_t value_alloc_space = 0;
  int i, j;
  
  for(i = 0; i < numAttrs*5; i += 5) {
    value_alloc_space += attrs[i+4] - attrs[i+3]+1;
  }

  attrblock = malloc((numAttrs*2+1)*sizeof(char*)+value_alloc_space);
  attr_index = (const char**)attrblock;
  value_dst_ptr = attrblock+(numAttrs*2+1)*sizeof(char*);

  for(i = 0, j = 0; i < numAttrs*5; i += 5, j += 2) {
    size_t value_len = attrs[i+4] - attrs[i+3];
    memcpy(value_dst_ptr, attrs[i+3], value_len);
    value_dst_ptr[value_len] = '\0';
    attr_index[j] = (const char*)attrs[i];
    attr_index[j+1] = value_dst_ptr;
    value_dst_ptr += value_len+1;
  }
  attr_index[j] = NULL;
  /* TODO evaluate return value of stack_push; non-zero value is error. */
  metalink_stack_push(session_data->characters_stack, str_buf);

  session_data->stm->state->start_fun(session_data->stm,
				      (const char*)localname,
                                      (const char*)ns_uri,
				      attr_index);
  free(attrblock);
}

static void end_element_handler(void* user_data,
                                const xmlChar* localname,
                                const xmlChar* prefix,
                                const xmlChar* ns_uri)
{
  metalink_session_data_t* session_data = (metalink_session_data_t*)user_data;
  metalink_string_buffer_t* str_buf = metalink_stack_pop(session_data->characters_stack);
  
  session_data->stm->state->end_fun(session_data->stm,
				    (const char*)localname,
                                    (const char*)ns_uri,
				    metalink_string_buffer_str(str_buf));

  metalink_string_buffer_delete(str_buf);	     
}

static void characters_handler(void* user_data, const xmlChar* chars,
			       int length)
{
  metalink_session_data_t* session_data = (metalink_session_data_t*)user_data;
  metalink_string_buffer_t* str_buf =
    metalink_stack_top(session_data->characters_stack);

  metalink_string_buffer_append(str_buf, (const char*)chars, length);
}

static xmlSAXHandler mySAXHandler = {
  0, /* internalSubsetSAXFunc */
  0, /* isStandaloneSAXFunc */
  0, /* hasInternalSubsetSAXFunc */
  0, /* hasExternalSubsetSAXFunc */
  0, /* resolveEntitySAXFunc */
  0, /* getEntitySAXFunc */
  0, /* entityDeclSAXFunc */
  0, /* notationDeclSAXFunc */
  0, /* attributeDeclSAXFunc */
  0, /* elementDeclSAXFunc */
  0, /*   unparsedEntityDeclSAXFunc */
  0, /*   setDocumentLocatorSAXFunc */
  0, /*   startDocumentSAXFunc */
  0, /*   endDocumentSAXFunc */
  0, /*   startElementSAXFunc */
  0, /*   endElementSAXFunc */
  0, /*   referenceSAXFunc */
  &characters_handler, /*   charactersSAXFunc */
  0, /*   ignorableWhitespaceSAXFunc */
  0, /*   processingInstructionSAXFunc */
  0, /*   commentSAXFunc */
  0, /*   warningSAXFunc */
  0, /*   errorSAXFunc */
  0, /*   fatalErrorSAXFunc */
  0, /*   getParameterEntitySAXFunc */
  0, /*   cdataBlockSAXFunc */
  0, /*   externalSubsetSAXFunc */
  XML_SAX2_MAGIC, /*   unsigned int  initialized */
  0, /*   void *        _private */
  &start_element_handler, /*   startElementNsSAX2Func */
  &end_element_handler, /*   endElementNsSAX2Func */
  0, /*   xmlStructuredErrorFunc */
};

struct _metalink_parser_context
{
  metalink_session_data_t* session_data;
  xmlParserCtxtPtr parser;
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
  xmlFreeParserCtxt(ctx->parser);
  free(ctx);
}

static metalink_error_t
metalink_parse_update_internal(metalink_parser_context_t* ctx,
			       const char* buf, size_t len, int terminate)
{
  metalink_error_t r;

  if(ctx->parser == NULL) {
    int inilen = 4 < len ? 4 : len;
    ctx->parser = xmlCreatePushParserCtxt(&mySAXHandler, ctx->session_data,
					  buf, inilen, NULL);
    if(ctx->parser == NULL) {
      r = METALINK_ERR_PARSER_ERROR;
    } else {
      r = xmlParseChunk(ctx->parser, buf+inilen, len-inilen, terminate);
    }
  } else {
    r = xmlParseChunk(ctx->parser, buf, len, terminate);
  }
  return r;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_update(metalink_parser_context_t* ctx,
		      const char* buf, size_t len)
{
  metalink_error_t r;
  r = metalink_parse_update_internal(ctx, buf, len, 0);
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
  metalink_error_t r,
		   retval;

  r = metalink_parse_update_internal(ctx, buf, len, 1);
  if(r == 0) {
    r = metalink_pctrl_get_error(ctx->session_data->stm->ctrl);
  }

  retval = metalink_handle_parse_result(res, ctx->session_data, r);

  metalink_parser_context_delete(ctx);

  return retval; 
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_file(const char* filename, metalink_t** res)
{
  metalink_session_data_t* session_data;
  metalink_error_t r,
  		   retval;

  session_data = metalink_session_data_new();
  
  r = xmlSAXUserParseFile(&mySAXHandler, session_data, filename);

  retval = metalink_handle_parse_result(res, session_data, r);

  metalink_session_data_delete(session_data);

  return retval;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_fp(FILE* docfp, metalink_t** res)
{
  metalink_session_data_t* session_data;
  metalink_error_t r = 0,
  		   retval;
  size_t num_read;
  char buff[BUFSIZ];

  xmlParserCtxtPtr ctxt;

  session_data = metalink_session_data_new();
  
  num_read = fread(buff, 1, 4, docfp);
  ctxt = xmlCreatePushParserCtxt(&mySAXHandler, session_data, buff, num_read, NULL);
  if(ctxt == NULL)
    r = METALINK_ERR_PARSER_ERROR;

  while(!feof(docfp) && !r) {
    num_read = fread(buff, 1, BUFSIZ, docfp);
    if(num_read == 0) {
      if(ferror(docfp)) {
        r = METALINK_ERR_PARSER_ERROR;
      }
    } else if(xmlParseChunk(ctxt, buff, num_read, 0))
      r = METALINK_ERR_PARSER_ERROR;
  }
  xmlParseChunk(ctxt, buff, 0, 1);
  xmlFreeParserCtxt(ctxt);

  retval = metalink_handle_parse_result(res, session_data, r);

  metalink_session_data_delete(session_data);

  return retval;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_fd(int fd, metalink_t** res)
{
  metalink_error_t r = 0;
  metalink_parser_context_t* context;

  context = metalink_parser_context_new();
  if(context == NULL) {
    return METALINK_ERR_BAD_ALLOC;
  }

  while(1) {
    ssize_t len;
    char buf[BUFSIZ];
    while((len = read(fd, buf, BUFSIZ)) == -1 && errno == EINTR);
    if(len == 0) {
      r = metalink_parse_final(context, NULL, 0, res);
      break;
    }
    r = metalink_parse_update(context, buf, len);
    if(r != 0) {
      break;
    }
  }
  return r;
}

metalink_error_t
METALINK_PUBLIC
metalink_parse_memory(const char* buf, size_t len, metalink_t** res)
{
  metalink_session_data_t* session_data;
  metalink_error_t r,
		   retval;

  session_data = metalink_session_data_new();

  r = xmlSAXUserParseMemory(&mySAXHandler, session_data, buf, len);

  retval = metalink_handle_parse_result(res, session_data, r);

  metalink_session_data_delete(session_data);

  return retval;
}
