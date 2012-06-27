#ifndef _KL_MATRIX_INTERNAL_H_
#define _KL_MATRIX_INTERNAL_H_

extern void kl_matrix_mul_matrix_sse(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT c);
extern void kl_matrix_mul_matrix_c(const float* KL_RESTRICT a, const float* KL_RESTRICT b, float* KL_RESTRICT mresult);
extern void kl_matrix_mul_vector_sse3(const float* KL_RESTRICT a, const float* KL_RESTRICT vec, float* KL_RESTRICT o);
extern void kl_matrix_mul_vector_c(const float* KL_RESTRICT m, const float* KL_RESTRICT p, float* KL_RESTRICT presult);
extern void kl_matrix_mul_vector_batch_sse3(const float* KL_RESTRICT a, const float* KL_RESTRICT vec, float* KL_RESTRICT o, uint32_t n);
extern void kl_matrix_mul_vector_batch_c(const float* KL_RESTRICT m, const float* KL_RESTRICT vec, float* KL_RESTRICT o, uint32_t n);

extern float kl_matrix_mul_matrix_sse_timing(uint32_t num);
extern float kl_matrix_mul_matrix_c_timing(uint32_t num);
extern float kl_matrix_mul_vector_sse3_timing(uint32_t num);
extern float kl_matrix_mul_vector_c_timing(uint32_t num);
extern float kl_matrix_mul_vector_batch_sse3_timing(uint32_t num);
extern float kl_matrix_mul_vector_batch_c_timing(uint32_t num);

#endif
