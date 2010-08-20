#ifndef GC_QUEUE_H_
#define GC_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "core/memory.h"
#include "inline.h"

typedef struct
{
   void* buffer;
   size_t start;
   size_t end;
   size_t size;
   size_t element_size;
} gcqueue;

void gcinit_queue(gcqueue* queue, size_t element_size, size_t num_elements, _gcaligned_malloc_fn_ptr _allocator);
void gcdestroy_queue(gcqueue* queue, _gcaligned_free_fn_ptr _freeer);

inline void* gcpeek_queue(const gcqueue* queue)
{
   return ((char*)queue->buffer) + queue->start * queue->element_size;
}

int gcenqueue(gcqueue* queue, void* item);

#ifdef __cplusplus
}
#endif

#endif