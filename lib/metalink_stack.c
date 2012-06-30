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
#include "metalink_stack.h"

#include <stdlib.h>

static void init_stack(metalink_stack_t* s)
{
  s->entry = NULL;
}

metalink_stack_t* metalink_stack_new(void)
{
  metalink_stack_t* s = malloc(sizeof(metalink_stack_t));
  if(s) {
    init_stack(s);
  }
  return s;
}

void metalink_stack_delete(metalink_stack_t* stack)
{
  metalink_stack_entry_t* e;
  metalink_stack_entry_t* next;

  e = stack->entry;
  while(e) {
    next = e->next;
    free(e);
    e = next;
  }
  free(stack);
}


int metalink_stack_empty(const metalink_stack_t* stack)
{
  return stack->entry == NULL;
}

int metalink_stack_push(metalink_stack_t* stack, void* data)
{
  metalink_stack_entry_t* new_entry;

  new_entry = malloc(sizeof(metalink_stack_entry_t));
  if(!new_entry) {
    return 1;
  }
  new_entry->data = data;
  new_entry->next = stack->entry;
  stack->entry = new_entry;
  return 0;
}

void* metalink_stack_pop(metalink_stack_t* stack)
{
  metalink_stack_entry_t* pop_entry;
  void* data;

  pop_entry = stack->entry;
  stack->entry = pop_entry->next;
  data = pop_entry->data;
  free(pop_entry);
  return data;
}

void* metalink_stack_top(metalink_stack_t* stack)
{
  if(metalink_stack_empty(stack)) {
    return NULL;
  } else {
    return stack->entry->data;
  }
}
