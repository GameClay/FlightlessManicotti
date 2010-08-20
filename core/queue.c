#include "core/queue.h"

void gcinit_queue(gcqueue* queue, size_t size, _gcaligned_malloc_fn_ptr _allocator)
{
   queue->_buffer = _allocator(size, 16);
   queue->start = queue->_buffer;
   queue->end = queue->_buffer;
   queue->size = 0;
}

void gcdestroy_queue(gcqueue* queue, _gcaligned_free_fn_ptr _freeer)
{
   _freeer(queue->_buffer);
   queue->_buffer = NULL;
   queue->start = queue->_buffer;
   queue->end = queue->_buffer;
   queue->size = 0;
}