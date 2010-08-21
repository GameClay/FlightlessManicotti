#include "core/queue.h"
#include "core/memory.h"
#include "core/atomic.h"

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
   size_t qend = queue->end;
   size_t end = (qend + 1) % queue->size;
   if((qend+ 1) % queue->size != queue->start) 
   {
      if(atomic_compare_exchange_weak(&queue->end, &qend, end))
      {
         gcmicrorcpy(((char*)queue->buffer) + qend * queue->element_size, item, queue->element_size);
         return 1;
      }
      return -1;
   }
   return 0;
}

int gcdequeue(gcqueue* queue, void* item)
{
   size_t qstart = queue->start;
   size_t start = (qstart + 1) % queue->size;
   void* peekaddr = gcpeek_queue(queue);
   if(qstart % queue->size != queue->end) 
   {
      if(atomic_compare_exchange_weak(&queue->start, &qstart, start))
      {
         gcmicrorcpy(item, peekaddr, queue->element_size);
         return 1;
      }
      return -1;
   }
   return 0;
}