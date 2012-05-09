/* -*- Mode: C; tab-width: 3; c-basic-offset: 3; indent-tabs-mode: nil -*- */
/* vim: set filetype=C tabstop=3 softtabstop=3 shiftwidth=3 expandtab: */

/* FlightlessManicotti -- Copyright (C) 2010-2012 GameClay LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <FlightlessManicotti/core/rarray.h>
#include <string.h>

int kl_rarray_init(kl_rarray_t* array, size_t element_sz, size_t num_elements)
{
   int ret = KL_ERROR;

   if(array != NULL)
   {
      array->num_elements = 0;
      array->element_sz = element_sz;
      array->max_idx = 0;
      array->elements = kl_heap_alloc(num_elements * element_sz);
      if(array->elements != NULL)
      {
         array->num_elements = num_elements;
         ret = KL_SUCCESS;
      }
   }

   return ret;
}

void kl_rarray_destroy(kl_rarray_t* array)
{
   if(array != NULL)
   {
      kl_heap_free(array->elements);
      array->num_elements = 0;
      array->element_sz = 0;
      array->max_idx = 0;
   }
}

void kl_rarray_resize(kl_rarray_t* array, size_t num_elements)
{
   if(array != NULL)
   {
      void* old_elements = array->elements;
      array->elements = kl_heap_alloc(num_elements * array->element_sz);
      memcpy(array->elements, old_elements,
             num_elements > array->num_elements ? array->num_elements : num_elements);
      kl_heap_free(old_elements);

      array->num_elements = num_elements;
      array->max_idx = (array->max_idx >= num_elements ? num_elements - 1 : array->max_idx);
   }
}

void kl_rarray_append(kl_rarray_t* array, const void* element)
{
   if(array != NULL)
   {
      uint8_t* ptr;
      if(array->max_idx + 1 == array->num_elements)
      {
         kl_rarray_resize(array, array->num_elements + array->num_elements / 2);
      }

      ptr = array->elements;
      memcpy(&ptr[array->max_idx * array->element_sz], element, array->element_sz);
      array->max_idx++;
   }
}
