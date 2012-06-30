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
#ifndef _D_METALINK_STACK_H_
#define _D_METALINK_STACK_H_

#include "metalink_config.h"

#include <metalink/metalink.h>

typedef struct _metalink_stack_entry {
  void* data;
  struct _metalink_stack_entry* next;
} metalink_stack_entry_t;

typedef struct _metalink_stack {
  metalink_stack_entry_t* entry;
} metalink_stack_t;

metalink_stack_t* metalink_stack_new(void);

void metalink_stack_delete(metalink_stack_t* stack);

int metalink_stack_empty(const metalink_stack_t* stack);

void* metalink_stack_top(metalink_stack_t* stack);

/* returns data */
void* metalink_stack_pop(metalink_stack_t* stack);

int metalink_stack_push(metalink_stack_t* stack, void* data);

#endif /* _D_METALINK_STACK_H_ */
