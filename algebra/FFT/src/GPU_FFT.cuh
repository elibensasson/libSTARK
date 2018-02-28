/*
 * GPU_FFT.h
 *
 *  Created on: Aug 26, 2014
 *      Author: matan
 */
#ifdef __GPU
#ifndef GPU_FFT_H_
#define GPU_FFT_H_

#include "FFT.h"
namespace FFF{

 class GPU_FFT{
	 public:
	 static void multiExp_mult(len_t a_len, Chunk* d_a , Chunk* d_b , len_t b_len);
	 static void linearEvaluation(Chunk* d_chunk_p,Chunk* d_chunk_p_cpy, len_t p_len);
	 static void ilinearEvaluation(Chunk* d_chunk_p,Chunk* d_chunk_p_cpy, len_t p_len);
	 static void WFromUV(Chunk* d_chunk_p, len_t p_len, Chunk* subspace, len_t dim);
	 static void UVFromW(Chunk* d_chunk_p, len_t p_len, Chunk* subspace, len_t dim);
	 static void multiExponentiate_gpu(const FFT* fft,Chunk* d_chunk_P, len_t p_len, len_t dim, Chunk* d_res );
	 static void imultiExponentiate_gpu(const FFT* fft,Chunk* d_chunk_P, len_t p_len, len_t dim, Chunk* d_exp );
	 static void taylorExpansion_gpu(const FFT* fft, Chunk* d_chunk_P, len_t p_len , len_t dim);
	 static void itaylorExpansion_gpu(const FFT* fft, Chunk* d_chunk_P, len_t p_len , len_t dim);
	 static void setUpConstantMemory(const FFT* fft);
	 static bool partition(Chunk* d_chunk_p_src, Chunk* d_chunk_p_dst, len_t p_len, len_t t_dim);
	 static bool ipartition(Chunk* d_chunk_p_src, Chunk* d_chunk_p_dst, len_t p_len, len_t t_dim);
	 static void fft_gpu(const FFT* fft,Polynomial* P);
	 static void ifft_gpu(const FFT* const fft,Polynomial* P);
	 static void fft_gpu_InTB(len_t dim, const FFT* const fft, Chunk* p, len_t p_len);
 };
}


#endif /* GPU_FFT_H_ */
#endif // #ifdef __GPU
