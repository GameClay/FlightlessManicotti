#ifndef _INLINE_H
#define _INLINE_H

// http://en.wikipedia.org/wiki/C99
#if __STDC_VERSION__ >= 199901L
   // "inline" is a keyword
#else
#  define inline static
#endif

#endif