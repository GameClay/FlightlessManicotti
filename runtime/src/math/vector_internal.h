#ifndef _KL_VECTOR_INTERNAL_H_
#define _KL_VECTOR_INTERNAL_H_

extern float kl_vector_dot_sse3(const float* KL_RESTRICT a, const float* KL_RESTRICT b);
extern float kl_vector_dot_c(const float* KL_RESTRICT a, const float* KL_RESTRICT b);
extern void kl_vector_cross_sse(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c);
extern void kl_vector_cross_c(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c);

#endif
