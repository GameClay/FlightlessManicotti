#ifndef GC_QUEUE_H_
#define GC_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include "core/memory.h"

typedef struct
{
   void* _buffer;
   void* start;
   void* end;
   size_t size;
} gcqueue;

void gcinit_queue(gcqueue* queue, size_t size, _gcaligned_malloc_fn_ptr _allocator);
void gcdestroy_queue(gcqueue* queue, _gcaligned_free_fn_ptr _freeer);

#ifdef __cplusplus
}
#endif

#endif