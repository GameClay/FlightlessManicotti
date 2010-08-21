#ifndef _ATOMIC_H_
#define _ATOMIC_H_

#ifdef __cplusplus
extern "C" {
#endif

//If C1x
//include <stdatomic.h>
//Else
#include <stdint.h>
#include "inline.h"

// If GCC
#  ifndef _Bool
#     define _Bool int
#  endif

inline _Bool atomic_compare_exchange_weak(volatile size_t* object, size_t* expected, size_t desired)
{
   return __sync_bool_compare_and_swap(object, *expected, desired);
}
// Endif

//Endif

#ifdef __cplusplus
}
#endif

#endif