#ifndef GC_MEMORY_H_
#define GC_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

// Function pointer types
typedef void* (*_gcaligned_malloc_fn_ptr)(size_t size, size_t align_size);
typedef void (*_gcaligned_free_fn_ptr)(void* pointer);
typedef void* (*_gcmicrorcpy_fn_ptr)(void* __restrict dest, const void* __restrict src, size_t size);

extern _gcaligned_malloc_fn_ptr gcheap_alloc;
extern _gcaligned_free_fn_ptr gcheap_free;
extern _gcmicrorcpy_fn_ptr gcmicrorcpy;

#ifdef __cplusplus
}
#endif

#endif