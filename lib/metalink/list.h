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
#ifndef _D_LIST_H_
#define _D_LIST_H_

#include <stdlib.h>

typedef struct _list_entry {
  void* data;
  struct _list_entry* next;
} list_entry_t;

typedef struct _list {
  list_entry_t* head;
  list_entry_t* tail;
} list_t;

list_t* new_list();

void delete_list(list_t* list);

void* list_get_data(list_t* list, size_t index);

size_t list_length(list_t* list);

void list_clear(list_t* list);

void list_clear_data(list_t* list);

void list_to_array(list_t* list, void** array);

int list_append(list_t* list, void* data);

void list_insert(list_t* list, size_t index);

void list_remove(list_t* list, size_t index);

void list_for_each(list_t* list, void (*fun)(void* data));

#endif // _D_LIST_H_
