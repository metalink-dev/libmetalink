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
#include "list.h"

list_t* new_list()
{
  list_t* l = malloc(sizeof(list_t));
  if(l) {
    l->head = NULL;
    l->tail = NULL;
  }
  return l;
}

void delete_list(list_t* list)
{
  list_clear(list);
  free(list);
}

void list_clear(list_t* list)
{
  list_entry_t* e = list->head;
  list_entry_t* next;
  while(e) {
    next = e->next;
    free(e);
    e = next;
  }
  list->head = NULL;
  list->tail = NULL;
}

void list_clear_data(list_t* list)
{
  list_entry_t* e = list->head;
  list_entry_t* next;
  while(e) {
    next = e->next;
    free(e->data);
    free(e);
    e = next;
  }
  list->head = NULL;
  list->tail = NULL;
}

void* list_get_data(list_t* list, size_t index)
{
  list_entry_t* e = list->head;
  while(index-- && e) {
    e = e->next;
  }
  if(e) {
    return e->data;
  } else {
    return 0;
  }
}

size_t list_length(list_t* list)
{
  size_t length = 0;
  list_entry_t* e = list->head;
  while(e) {
    e = e->next;
    ++length;
  }
  return length;
}

int list_append(list_t* list, void* data)
{
  list_entry_t* new_entry = malloc(sizeof(list_entry_t));

  if(!new_entry) {
    return 1;
  }
  new_entry->data = data;
  new_entry->next = NULL;

  if(!list->head) {
    list->head = new_entry;
  }
  if(list->tail) {
    list->tail->next = new_entry;
  }
  list->tail = new_entry;
  return 0;
}

void list_to_array(list_t* list, void** array)
{
  void** p = array;
  list_entry_t* e = list->head;
  while(e) {
    *p++ = e->data;
    e = e->next;
  }
}

void list_for_each(list_t* list, void (*fun)(void* data))
{
  list_entry_t* e = list->head;
  while(e) {
    fun(e->data);
    e = e->next;
  }
}
