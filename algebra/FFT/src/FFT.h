/*
 * FFT.h
 *
 *  Created on: Jul 2, 2014
 *      Author: matan
 */

/*
 * This file is the API for the FFT algorithms.
 */
#ifndef FFT_H_
#define FFT_H_

#include "Basis.h"
#ifdef __GPU
#include "Chunk.cuh"
#include <cuda_runtime.h>
#include <cuda.h>
#endif	// #ifdef __GPU
#include "Definitions.h"
#include "Polynomials.h"
#include "Element.h"

namespace FFF {
/*
 * This class is for the FFT algorithm.
 * When creating an instance of the class, it gets as input a basis for the FFT over which the polynomial (that will be given later) will be evaluated
 * and preprocesses some of the data needed for it.
 */
typedef enum{FFT_OP,IFFT_OP} fft_operation_t;
class FFT {
public:
	/*
	 * This is the basis over which the polynomial will be evaluated, could be affine.
	 */
	Basis basis;
	/*
	 * This is the series of bases over which the polynomial is actually recursively evaluated.
	 */
	Basis* bases;
	/*
	 * This is an array of the last element in each of the bases that evaluated.
	 */
	Element* bms;
	/*
	 * This is an array containing the inverse of the elements in bms.
	 */
	Element* i_bms;
	/*
	 * This is the elements that spans the last subspace to evaluate at the bottom of the recursion.
	 * This is an affine subspace of dimension 1.
	 */
	Element lastD;
	/*
	 * This is the inverse of lastD.
	 */
	Element i_lastD;
	/*
	 * This is the shift of the last affine subspace.
	 */
	Element lastShift;
	/*
	 * This is a two-dimensional array s.t. exps[i] will contain a multiexponentiation of the i-th  element bms up the needed degree
	 *  (2^ (sizeOfOriginalBasis-i))-1.
	 */
	Element** exps;
	/*
	 * This are the subspaces needed to multiply at the WFromUV and UVFromW phases.
	 */
	Element** subspaces;
#ifdef __GPU
	/*
	 * These are the chunks used to calculate the FFT on GPU.
	 */
	Chunk** gpu_exp;
	Chunk** gpu_i_exp;
	/*
	 * This is the chunk that is used for the last phase, every even element in the chunk contains lastD+lastShift, every odd one will contain only lastD.
	 */
	Chunk linear_mul;
	Chunk ilinear_mul;
	/*
	 * These are the subspaces needed for the GPU phase of WFromUV.
	 */
	Chunk** gpu_subspace;
#endif
	/*
	 * Gets a polynomial P and evaluates the FFT over P.
	 */
	void fft_cpu(Polynomial P)const;
	/*
	 * This is the serial version of the FFT.
	 * log_size is log the numebr of processors.
	 * c_p_cpy is an auxilliary free space in memory.
	 */
	void fft_serial(Polynomial P,len_t log_size, Element* c_p_cpy)const;
	/*
	 * Gets a polynomial P and evaluated the iFFT over P.
	 */
	void ifft_cpu(Polynomial P);
	/*
	 * This is the serial version of the iFFT.
	 * log_size is log the numebr of processors.
	 * c_p_cpy is an auxilliary free space in memory.
	 */
	void ifft_serial(Polynomial P,len_t log_size, Element* c_p_cpy);
	/*
	 * Calculates e^0,e^1,...,e^(t-1).
	 * Returns them in an array.
	 * The array has to be unallocated by the user using "free()".
	 */
	void multiExponentiate_cpu(Element& e, len_t t, Element* res);
	/*
	 * GPartition phase on CPU.
	 */
	void GPartition_cpu(Element* G, len_t log_g_len,Element* res)const;
	/*
	 * The inverse of the partition on cpu.
	 */
	void iGPartition_cpu(Element* G, len_t log_g_len,Element* res);
	/*
	 * This is an implementation of the partition phase with openMP, relevant for upper levels of the FFT.
	 */
	void GPartition_cpuOMP(Element* G, len_t log_g_len,Element* res)const;
	/*
	 * This is an implementation of the inverse partition phase with openMP, relevant for upper levels of the iFFT.
	 */
	void iGPartition_cpuOMP(Element* G, len_t log_g_len,Element* res);
	/*
	 * Parallel WFromUV implementation.
	 */
	void WFromUV_cpu(Polynomial P, len_t log_g_len, len_t size)const;
	/*
	 * Serial WFromUV implementation - for lower levels of FFT.
	 */
	void WFromUV_cpu_serial(Polynomial P, len_t log_g_len, len_t size)const;
	/*
	 * Parallel UVFromW implementation.
	 */
	void UVFromW_cpu(Polynomial P, len_t log_g_len,  len_t size);
	/*
	 * Serial UVFromW implementation - for lower levels of iFFT.
	 */
	void UVFromW_cpu_serial(Polynomial P, len_t log_g_len, len_t size);
#ifdef __GPU
	void fft_gpu(Polynomial P);
#endif	// #ifdef __GPU
	/*
	 * This function used at preprocessing level to generate all elements of a subspaces.
	 * Given array b of length b_len , writes in res[i] the i-th element of the subspace spanned by b with shift s.
	 *
	 * This function doesn't check the elements in b are linearly independent.
	 */
static void generateSubspaceElements_cpu(const Element* b,const Element& s,const len_t b_len,Element* const  res);
static void updateShiftSubspaceElements_cpu(const Element& s,const len_t b_len,Element* const  res);
	FFT(Basis& b,const fft_operation_t operation);
	virtual ~FFT();

    void changeShift(const Element& newShift);

	/*
	 * Non-allocating version of the i/FFT Algorithms.
	 */
	void AlgFFT(Polynomial* P, len_t p_len)const;
	void AlgIFFT(Element* P);
	/*
	 * Allocating Version of the i/FFT Algoirthms, where result is not written in place but it written to *res (you don't have to malloc *res).
	 */
	void AlgFFT(const Polynomial* P, len_t p_len,  Element** res);
	void AlgIFFT(Element* P,  Polynomial* res);
};
	void fft_gpu_aux(FFT* fft, Polynomial P);
} /* namespace FFF */
#endif /* FFT_H_ */
