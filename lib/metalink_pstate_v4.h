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
#ifndef _D_METALINK_PSTATE_V4_H_
#define _D_METALINK_PSTATE_V4_H_

#include "metalink_config.h"

#include <metalink/metalink.h>

#include "metalink_pstate.h"

/* metalink state <metalink> */
void metalink_state_start_fun_v4(metalink_pstm_t* stm,
				 const char* name, const char* ns_uri,
				 const char** attrs);

void metalink_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters);

/* generator state <generator> */
void generator_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs);

void generator_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters);

/* origin state <origin> */
void origin_state_start_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char** attrs);

void origin_state_end_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char* characters);

/* file state <file> */
void file_state_start_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char** attrs);

void file_state_end_fun_v4(metalink_pstm_t* stm,
			   const char* name, const char* ns_uri,
			   const char* characters);

/* url state <url> */
void url_state_start_fun_v4(metalink_pstm_t* stm,
			    const char* name, const char* ns_uri,
			    const char** attrs);

void url_state_end_fun_v4(metalink_pstm_t* stm,
			  const char* name, const char* ns_uri,
			  const char* characters);

/* description state <description> */
void description_state_start_fun_v4(metalink_pstm_t* stm,
				    const char* name, const char* ns_uri,
				    const char** attrs);

void description_state_end_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char* characters);

/* copyright state <copyright> */
void copyright_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs);

void copyright_state_end_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char* characters);

/* identity state <identity> */
void identity_state_start_fun_v4(metalink_pstm_t* stm,
				 const char* name, const char* ns_uri,
				 const char** attrs);

void identity_state_end_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char* characters);

/* logo state <logo> */
void logo_state_start_fun_v4(metalink_pstm_t* stm,
			    const char* name, const char* ns_uri,
			    const char** attrs);

void logo_state_end_fun_v4(metalink_pstm_t* stm,
			   const char* name, const char* ns_uri,
			   const char* characters);

/* signature state <signature> */
void signature_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs);

void signature_state_end_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char* characters);

/* pieces state <pieces> */
void pieces_state_start_fun_v4(metalink_pstm_t* stm,
			       const char* name, const char* ns_uri,
			       const char** attrs);

void pieces_state_end_fun_v4(metalink_pstm_t* stm,
			     const char* name, const char* ns_uri,
			     const char* characters);

/* metaurl state <metaurl> */
void metaurl_state_start_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char** attrs);

void metaurl_state_end_fun_v4(metalink_pstm_t* stm,
			      const char* name, const char* ns_uri,
			      const char* characters);

/* published state <published> */
void published_state_start_fun_v4(metalink_pstm_t* stm,
				  const char* name, const char* ns_uri,
				  const char** attrs);

void published_state_end_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char* characters);

/* updated state <updated> */
void updated_state_start_fun_v4(metalink_pstm_t* stm,
				const char* name, const char* ns_uri,
				const char** attrs);

void updated_state_end_fun_v4(metalink_pstm_t* stm,
			      const char* name, const char* ns_uri,
			      const char* characters);

#endif /* _D_METALINK_PARSER_STATE_V4_H_ */
