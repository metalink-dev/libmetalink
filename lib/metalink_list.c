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
#include "metalink_list.h"

metalink_list_t* metalink_list_new(void)
{
  metalink_list_t* l = malloc(sizeof(metalink_list_t));
  if(l) {
    l->head = NULL;
    l->tail = NULL;
  }
  return l;
}

void metalink_list_delete(metalink_list_t* list)
{
  metalink_list_clear(list);
  free(list);
}

void metalink_list_clear(metalink_list_t* list)
{
  metalink_list_entry_t* e = list->head;
  metalink_list_entry_t* next;
  while(e) {
    next = e->next;
    free(e);
    e = next;
  }
  list->head = NULL;
  list->tail = NULL;
}

void metalink_list_clear_data(metalink_list_t* list)
{
  metalink_list_entry_t* e = list->head;
  metalink_list_entry_t* next;
  while(e) {
    next = e->next;
    free(e->data);
    free(e);
    e = next;
  }
  list->head = NULL;
  list->tail = NULL;
}

void* metalink_list_get_data(metalink_list_t* list, size_t index)
{
  metalink_list_entry_t* e = list->head;
  while(index-- && e) {
    e = e->next;
  }
  if(e) {
    return e->data;
  } else {
    return 0;
  }
}

size_t metalink_list_length(metalink_list_t* list)
{
  size_t length = 0;
  metalink_list_entry_t* e = list->head;
  while(e) {
    e = e->next;
    ++length;
  }
  return length;
}

int metalink_list_append(metalink_list_t* list, void* data)
{
  metalink_list_entry_t* new_entry = malloc(sizeof(metalink_list_entry_t));

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

void metalink_list_to_array(metalink_list_t* list, void** array)
{
  void** p = array;
  metalink_list_entry_t* e = list->head;
  while(e) {
    *p++ = e->data;
    e = e->next;
  }
}

void metalink_list_for_each(metalink_list_t* list, void (*fun)(void* data))
{
  metalink_list_entry_t* e = list->head;
  while(e) {
    fun(e->data);
    e = e->next;
  }
}
