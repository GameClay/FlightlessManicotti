#include "core/queue.h"
#include "core/memory.h"

void gcinit_queue(gcqueue* queue, size_t element_size, size_t num_elements, _gcaligned_malloc_fn_ptr _allocator)
{
   queue->buffer = _allocator(element_size * num_elements, 16);
   queue->start = 0;
   queue->end = 0;
   queue->element_size = element_size;
   queue->size = num_elements;
}

void gcdestroy_queue(gcqueue* queue, _gcaligned_free_fn_ptr _freeer)
{
   _freeer(queue->buffer);
   queue->buffer = NULL;
   queue->start = 0;
   queue->end = 0;
   queue->size = 0;
   queue->element_size = 0;
}

int gcenqueue(gcqueue* queue, void* item)
{
   if((queue->end + 1) % queue->size != queue->start) 
   {
      gcmicrorcpy(((char*)queue->buffer) + queue->end * queue->element_size, item, queue->element_size);
      queue->end = (queue->end + 1) % queue->size;
      return 1;
   }
   return 0;
}

int gcdequeue(gcqueue* queue, void* item)
{
   if(queue->start % queue->size != queue->end) 
   {
      gcmicrorcpy(item, gcpeek_queue(queue), queue->element_size);
      queue->start = (queue->start + 1) % queue->size;
      return 1;
   }
   return 0;
}