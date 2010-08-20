#ifndef GC_MEMORY_H_
#define GC_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef void* (*_gcaligned_malloc_fn_ptr)(size_t size, size_t align_size);
typedef void (*_gcaligned_free_fn_ptr)(void* pointer);

extern _gcaligned_malloc_fn_ptr gcheap_alloc;
extern _gcaligned_free_fn_ptr gcheap_free;

#ifdef __cplusplus
}
#endif

#endif