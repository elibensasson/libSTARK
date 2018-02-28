/*
 * Chunk.cuh
 *
 *  Created on: Jul 29, 2014
 *      Author: matan
 */

#ifndef CHUNK_CUH_
#define CHUNK_CUH_
#ifdef __CUDACC__
#define __CUDA_RUNNABLE__ __device__ __host__
#define __CUDA_DEVICE__ __device__
#else
#define __CUDA_RUNNABLE__
#define __CUDA_DEVICE__
#endif
#include "Element.h"
#include <cstdlib>
#include "Definitions.h"
namespace FFF {
class Elements_Chunk;

class Chunk {
public:
	Chunk(){};
	~Chunk(){};
	chunk_cell_t v[Element::ord];
	static const ord_t log_ord = Element::log_ord;
	static const ord_t ord = Element::ord;
	static const unsigned int log_threads_in_chunk = log_warp_size;
	static const unsigned int log_cells_in_chunk = Element::log_ord;
	static const unsigned int cells_in_chunk = 1<<log_cells_in_chunk;
	static const unsigned int log_elements_in_chunk = 5;
	static const unsigned int elements_in_chunk =  1<<log_elements_in_chunk;
	static const unsigned int log_bits_in_cell = 6;

	static void mul(Chunk (*h_a),Element (*h_e),len_t len, Chunk (*h_res));
	static void chunk_mul(Chunk (* h_a), Chunk (*h_b) , len_t len, Chunk (*h_res), bool copy = true, bool do_xor = false , int shift =0 );
	__CUDA_RUNNABLE__ static void clmul_by_chunk(Chunk& a, Chunk& e, idx_t idx, Chunk* c);
	__CUDA_DEVICE__ static void clmul_by_chunk_bShuffle(const Chunk& a, const Chunk& e, const idx_t idx, Chunk& c);
	__CUDA_DEVICE__ static void clmul_by_chunk_bShuffle_xor(const Chunk& a,const  Chunk& e,const  idx_t in_chunk_idx, Chunk& c,const int shift);
	__CUDA_DEVICE__ static void clmul_by_chunk_bShuffle_xor_mask(const Chunk& a, const Chunk& e, const idx_t in_chunk_idx, Chunk& c,const int shift,const idx_t mask_idx);
	__CUDA_DEVICE__ static void clmul_by_chunk_bShuffle_ixor_mask(const Chunk& a,const  Chunk& e, const idx_t in_chunk_idx, Chunk& c, const int shift, const idx_t mask_idx);
	static void chunkToNormal(Chunk(*h_a), Elements_Chunk(*h_b), len_t len, bool copy = true);
	static void normalToChunk(Elements_Chunk(*h_a), Chunk (*h_b), len_t len,bool copy = true);
//	static void chunk_reduce_xor(Chunk *a, Chunk *c_bottom, Chunk*c_top, idx_t idx);
	static void chunk_xor(Chunk *a, Chunk* b, idx_t idx);
	__CUDA_RUNNABLE__ static void chunk_reduce_xor(Chunk *a, Chunk *c_bottom, idx_t idx, Chunk* to_xor,int shift=0);
	__CUDA_RUNNABLE__ static void chunk_reduce(Chunk *a, Chunk *c_bottom, idx_t idx);
	__CUDA_DEVICE__ static void chunk_reduce_bShuffle_reduce(Chunk *a, chunk_cell_t my_ans[2][2], idx_t idx);
	static void chunkClmul(Chunk (*a),  Element (*e), idx_t idx, Chunk (*c));
	static void add(Chunk (*h_a),Chunk (*h_b),len_t len);
	static void setMod();
	static void aux_k_clmul(Chunk *a, Element* e, len_t len,Chunk* c_shared);
	void print() const;
};
class Elements_Chunk{
public:
	Elements_Chunk(){};
	~Elements_Chunk(){};
	static const len_t log_elements_in_elements_chunk = Chunk::log_elements_in_chunk;
	static const len_t elements_in_elements_chunk = 1<<log_elements_in_elements_chunk;
	Element e[elements_in_elements_chunk];
	void print() const;
};
} /* namespace FFF */

#endif /* CHUNK_CUH_ */

