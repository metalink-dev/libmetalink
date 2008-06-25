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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <expat.h>
#include "metalink_parser.h"
#include "metalink_pstm.h"
#include "metalink_pstate.h"
#include "metalink_error.h"
#include "metalink_parser_common.h"
#include "session_data.h"
#include "stack.h"
#include "string_buffer.h"

static void start_element_handler(void* user_data,
				  const char* name,
				  const char** attrs)
{
  session_data_t* session_data = (session_data_t*)user_data;
  string_buffer_t* str_buf = new_string_buffer(128);

  /* TODO evaluate return value of stack_push; non-zero value is error. */
  stack_push(session_data->characters_stack, str_buf);

  session_data->stm->state->start_fun(session_data->stm,
				      (const char*)name,
				      (const char**)attrs);
}

static void end_element_handler(void* user_data, const char* name)
{
  session_data_t* session_data = (session_data_t*)user_data;
  string_buffer_t* str_buf = stack_pop(session_data->characters_stack);
  
  session_data->stm->state->end_fun(session_data->stm,
				    (const char*)name,
				    string_buffer_str(str_buf));

  delete_string_buffer(str_buf);	     
}

static void characters_handler(void* user_data, const char* chars,
			       int length)
{
  session_data_t* session_data = (session_data_t*)user_data;
  string_buffer_t* str_buf = stack_top(session_data->characters_stack);

  string_buffer_append(str_buf, (const char*)chars, length);
}

static XML_Parser setup_parser(session_data_t* session_data)
{
  XML_Parser parser;
  
  parser = XML_ParserCreate(NULL);
  
  XML_SetUserData(parser, session_data);
  XML_SetElementHandler(parser, &start_element_handler, &end_element_handler);
  XML_SetCharacterDataHandler(parser, &characters_handler);

  return parser;
}

int metalink_parse_file(const char* filename, metalink_t** res)
{
  session_data_t* session_data;
  int r = 0;
  int retval;
  XML_Parser parser;
  int docfd;
  const size_t BUFF_SIZE = 4096;

  docfd = open(filename, O_RDONLY);
  if(docfd == -1) {
    return METALINK_ERR_CANNOT_OPEN_FILE;
  }

  session_data = new_session_data();
  
  parser = setup_parser(session_data);

  while(1) {
    int num_read;
    void* buff = XML_GetBuffer(parser, BUFF_SIZE);
    if(buff == NULL) {
      r = METALINK_ERR_PARSER_ERROR;
      break;
    }
    num_read = read(docfd, buff, BUFF_SIZE);
    if(num_read < 0) {
      r = METALINK_ERR_PARSER_ERROR;
      break;
    }
    if(!XML_ParseBuffer(parser, num_read, num_read == 0)) {
      r = METALINK_ERR_PARSER_ERROR;
      break;
    }
    if(num_read == 0) {
      break;
    }
  }
  XML_ParserFree(parser);
  close(docfd);
  
  retval = metalink_handle_parse_result(res, session_data, r);

  delete_session_data(session_data);

  return retval;
}

int metalink_parse_memory(const char* buf, size_t len, metalink_t** res)
{
  session_data_t* session_data;
  int r = 0;
  int retval;
  XML_Parser parser;
  
  session_data = new_session_data();

  parser = setup_parser(session_data);

  if(!XML_Parse(parser, buf, len, 1)) {
    r = METALINK_ERR_PARSER_ERROR;
  }

  XML_ParserFree(parser);

  retval = metalink_handle_parse_result(res, session_data, r);

  delete_session_data(session_data);

  return retval; 
}
