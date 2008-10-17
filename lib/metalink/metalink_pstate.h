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
#ifndef _D_METALINK_PSTATE_H_
#define _D_METALINK_PSTATE_H_

typedef struct _metalink_pstm metalink_pstm_t;

typedef void (*metalink_start_fun) (metalink_pstm_t* stm,
				    const char* name,
				    const char** attr);

typedef void (*metalink_end_fun) (metalink_pstm_t* stm,
				  const char* name,
				  const char* characters);

typedef struct _metalink_pstate
{
  metalink_start_fun start_fun;

  metalink_end_fun end_fun;

  int character_buffering;

  /** follow three elements are used in skip state **/
  metalink_start_fun before_skip_state_start_fun;

  metalink_end_fun before_skip_state_end_fun;

  int before_skip_character_buffering;

  int skip_depth;

} metalink_pstate_t;

/* constructor */
metalink_pstate_t* new_metalink_pstate();

/* destructor */
void delete_metalink_pstate(metalink_pstate_t* state);

/* null handler doing nothing */
void null_state_start_fun(metalink_pstm_t* stm,
			  const char* name, const char** attrs);

void null_state_end_fun(metalink_pstm_t* stm,
			const char* name, const char* characters);

/* initial state */
void initial_state_start_fun(metalink_pstm_t* stm,
			     const char* name, const char** attrs);

void initial_state_end_fun(metalink_pstm_t* stm,
			   const char* name, const char* characters);

/* metalink state <metalink> */
void metalink_state_start_fun(metalink_pstm_t* stm,
			      const char* name, const char** attrs);

void metalink_state_end_fun(metalink_pstm_t* stm,
			    const char* name, const char* characters);

/* identity state <identity> */
void identity_state_start_fun(metalink_pstm_t* stm,
			const char* name, const char** attrs);

void identity_state_end_fun(metalink_pstm_t* stm,
		      const char* name, const char* characters);

/* identity state <tags> */
void tags_state_start_fun(metalink_pstm_t* stm,
			const char* name, const char** attrs);

void tags_state_end_fun(metalink_pstm_t* stm,
		      const char* name, const char* characters);

/* files state <files> */
void files_state_start_fun(metalink_pstm_t* stm,
			   const char* name, const char** attrs);

void files_state_end_fun(metalink_pstm_t* stm,
			 const char* name, const char* characters);

/* file state <file> */
void file_state_start_fun(metalink_pstm_t* stm,
			  const char* name, const char** attrs);

void file_state_end_fun(metalink_pstm_t* stm,
			const char* name, const char* characters);

/* size state <size> */
void size_state_start_fun(metalink_pstm_t* stm,
			  const char* name, const char** attrs);

void size_state_end_fun(metalink_pstm_t* stm,
			const char* name, const char* characters);

/* version state <version> */
void version_state_start_fun(metalink_pstm_t* stm,
			     const char* name, const char** attrs);

void version_state_end_fun(metalink_pstm_t* stm,
			   const char* name, const char* characters);

/* language state <language> */
void language_state_start_fun(metalink_pstm_t* stm,
			      const char* name, const char** attrs);

void language_state_end_fun(metalink_pstm_t* stm,
			    const char* name, const char* characters);

/* os state <os> */
void os_state_start_fun(metalink_pstm_t* stm,
			const char* name, const char** attrs);

void os_state_end_fun(metalink_pstm_t* stm,
		      const char* name, const char* characters);

/* resources state <resources> */
void resources_state_start_fun(metalink_pstm_t* stm,
			       const char* name, const char** attrs);

void resources_state_end_fun(metalink_pstm_t* stm,
			     const char* name, const char* characters);

/* url state <url> */
void url_state_start_fun(metalink_pstm_t* stm,
			 const char* name, const char** attrs);

void url_state_end_fun(metalink_pstm_t* stm,
		       const char* name, const char* characters);

/* verification state <verification> */
void verification_state_start_fun(metalink_pstm_t* stm,
				  const char* name, const char** attrs);

void verification_state_end_fun(metalink_pstm_t* stm,
				const char* name, const char* characters);

/* hash state <hash> */
void hash_state_start_fun(metalink_pstm_t* stm,
			  const char* name, const char** attrs);

void hash_state_end_fun(metalink_pstm_t* stm,
			const char* name, const char* characters);

/* pieces state <pieces> */
void pieces_state_start_fun(metalink_pstm_t* stm,
			    const char* name, const char** attrs);

void pieces_state_end_fun(metalink_pstm_t* stm,
			  const char* name, const char* characters);

/* piece hash state <hash> inside of <pieces> */
void piece_hash_state_start_fun(metalink_pstm_t* stm,
				const char* name, const char** attrs);

void piece_hash_state_end_fun(metalink_pstm_t* stm,
			      const char* name, const char* characters);

/* fin state */
void fin_state_start_fun(metalink_pstm_t* stm,
			 const char* name, const char** attrs);

void fin_state_end_fun(metalink_pstm_t* stm,
		       const char* name, const char* characters);

/* skip state */
void skip_state_start_fun(metalink_pstm_t* stm,
			  const char* name, const char** attrs);

void skip_state_end_fun(metalink_pstm_t* stm,
			const char* name, const char* characters);

#endif // _D_METALINK_PARSER_STATE_H_
