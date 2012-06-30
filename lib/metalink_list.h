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
#ifndef _D_METALINK_LIST_H_
#define _D_METALINK_LIST_H_

#include "metalink_config.h"

#include <stdlib.h>

#include <metalink/metalink.h>

typedef struct _metalink_list_entry {
  void* data;
  struct _metalink_list_entry* next;
} metalink_list_entry_t;

typedef struct _metalink_list {
  metalink_list_entry_t* head;
  metalink_list_entry_t* tail;
} metalink_list_t;

metalink_list_t* metalink_list_new(void);

void metalink_list_delete(metalink_list_t* list);

void* metalink_list_get_data(metalink_list_t* list, size_t index);

size_t metalink_list_length(metalink_list_t* list);

void metalink_list_clear(metalink_list_t* list);

void metalink_list_clear_data(metalink_list_t* list);

void metalink_list_to_array(metalink_list_t* list, void** array);

int metalink_list_append(metalink_list_t* list, void* data);

void metalink_list_insert(metalink_list_t* list, size_t index);

void metalink_list_remove(metalink_list_t* list, size_t index);

void metalink_list_for_each(metalink_list_t* list, void (*fun)(void* data));

#endif /* _D_METALINK_LIST_H_ */
