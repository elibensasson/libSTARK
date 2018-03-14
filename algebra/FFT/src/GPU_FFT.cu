/*

 * GPU_FFT.cu
 *
 *  Created on: Aug 26, 2014
 *      Author: matan
 */
#include "GPU_FFT.cuh"
#include "Polynomials.h"
#include "Definitions.h"
#include <ctime>
#ifdef __GNUC__
#include <sys/time.h>
#endif	//#ifdef __GNUC__
#include <iostream>
#include <stdio.h>
//#include "Tests.h"
#include "Chunk.cuh"
#include <bitset>
#include <iostream>
#include <cstdio>

using namespace std;
#ifdef __GPU
namespace FFF{
/*------- CHUNK ------*/
/*
 * Chunk constant memory
 */
__constant__ idx_t p_Mod[max_nonzero_coefs_in_mod];
__constant__ idx_t p_ModLen;
__constant__ Element element_mul;
__constant__ Chunk c;

/*
 * FFT Constant Memory
 */
__constant__ chunk_cell_t d_chunk_cell_mask[Chunk::log_elements_in_chunk+2];
__constant__ chunk_cell_t d_partition[1<<bits_in_byte];
__constant__ chunk_cell_t d_alter_mask[Chunk::log_elements_in_chunk+1];
__constant__ Chunk d_linear_mul[1];
__constant__ Chunk d_ilinear_mul[1];

#define DUP_ODD_BITS(cell) cell = ((cell) & 0x55555555) | (((cell)&0x55555555)<<1)
#define DUP_EVEN_BITS(cell) cell = ((cell) & 0xaaaaaaaa) | (((cell)&0xaaaaaaaa)>>1)
__device__ void a_chunkToNormal(Chunk *d_a, Elements_Chunk *d_b, idx_t idx)
{
	cell_t ans = 0;
	idx_t element_idx = idx & andMask(Chunk::log_elements_in_chunk);
	idx_t cell_idx = idx >> Chunk::log_elements_in_chunk;
	for(unsigned int i = cell_idx<<Element::log_bits_in_cell ; i < ((cell_idx+1)<<Element::log_bits_in_cell); ++i)
		ans^=(((cell_t)(((d_a->v[i])>>(element_idx))&1))<<(i-(cell_idx<<Element::log_bits_in_cell)));
	d_b->e[element_idx].c[cell_idx]=ans;
}
__global__ void k_chunkToNormal(Chunk *d_a,Elements_Chunk *d_b , len_t len)
{
	const unsigned int threads_in_chunk = Chunk::elements_in_chunk * Element::element_len;
	 __shared__ Chunk input[max_block_size / threads_in_chunk];
	idx_t idx = threadIdx.x + blockDim.x*blockIdx.x;
	if(idx >= len*threads_in_chunk)
		return;
	idx_t chunkIdx = (idx) / (Element::element_len*Chunk::elements_in_chunk);
	idx_t in_chunkIdx = (idx  & (Element::element_len * Chunk::elements_in_chunk - 1));
	idx_t chunks_in_block = blockDim.x / Chunk::cells_in_chunk;
	idx_t inBlockChunkIdx = chunkIdx & (threads_in_chunk-1);
	for(unsigned int i = 0 ; i < sizeof(cell_t)/sizeof(chunk_cell_t) ; ++i){
		input[inBlockChunkIdx].v[in_chunkIdx + i*threads_in_chunk] = d_a[chunkIdx].v[in_chunkIdx+i*threads_in_chunk];
	}
	a_chunkToNormal(&(input[inBlockChunkIdx]), &(d_b[chunkIdx]),in_chunkIdx);
}
__host__	void Chunk::chunkToNormal(Chunk(*h_a), Elements_Chunk(*h_b), len_t len, bool copy)
{
	//Declare device variables
	Chunk (*d_a);
	Elements_Chunk (*d_b);

	const unsigned int num_element = len*elements_in_chunk;
	const unsigned int threads = Element::element_len * num_element;

	//Define Block  and Grid Size.
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
	if(copy){
	//Allocate Memory on GPU. (global)
		cudaMalloc(&d_a,sizeof(Chunk)*len);
		cudaMalloc(&d_b,sizeof(Elements_Chunk)*len);

	//Copy memory to GPU.
		cudaMemcpy(d_a,h_a,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
	} else {
		d_a = h_a;
		d_b = h_b;
	}

	//Launch Kernel
	k_chunkToNormal<<<gridSize,blockSize>>>(d_a,d_b,len);
	if(copy){
	//Copy results back to memory
		cudaMemcpy(h_b,d_b,sizeof(Elements_Chunk)*len,cudaMemcpyDeviceToHost);

	//Free allocated memory.
		cudaFree(d_a);
		cudaFree(d_b);
	}
}
__device__ void a_normalToChunk(Elements_Chunk *d_a, Chunk *d_b, idx_t idx)
{
	chunk_cell_t ans = 0;
	idx_t cell_idx = idx>>Element::log_bits_in_cell;
	for(unsigned int i = 0 ; i < Chunk::elements_in_chunk ; ++i)
		ans^=((((d_a->e[i].c[cell_idx])>>(idx& andMask(Element::log_bits_in_cell)))&1)<<i);
	d_b->v[idx]=ans;
}
__global__		void k_normalToChunk(Elements_Chunk *d_a,Chunk *d_b , len_t len)
{
	idx_t idx = threadIdx.x + blockDim.x*blockIdx.x;
	if(idx >= (len<<Chunk::log_cells_in_chunk))
		return;
	idx_t chunkIdx = (idx) >> Chunk::log_cells_in_chunk;
	idx_t in_chunkIdx = (idx  & andMask(Chunk::log_cells_in_chunk));
	a_normalToChunk(&(d_a[chunkIdx]),&(d_b[chunkIdx]),in_chunkIdx);
}
__host__	void Chunk::normalToChunk(Elements_Chunk(*h_a), Chunk (*h_b), len_t len,bool copy)
{

	//Declare device variables
	Elements_Chunk (*d_a);
	Chunk (*d_b);

	const unsigned int threads = len<<Chunk::log_cells_in_chunk;

	//Define Block  and Grid Size.
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(threads,max_block_size),1,1);

	//Allocate Memory on GPU. (global)
	if(copy){
		cudaMalloc(&d_a,sizeof(Elements_Chunk)*len);
		cudaMalloc(&d_b,sizeof(Chunk)*len);

	//Copy memory to GPU.
		cudaMemcpy(d_a,h_a,sizeof(Elements_Chunk)*len,cudaMemcpyHostToDevice);
	} else{
		d_a = h_a;
		d_b = h_b;
	}
	//Launch Kernel
	k_normalToChunk<<<gridSize,blockSize>>>(d_a,d_b,len);

	//Copy results back to memory
	if(copy){
		cudaMemcpy(h_b,d_b,sizeof(Chunk)*len,cudaMemcpyDeviceToHost);
		//Free allocated memory.
		cudaFree(d_a);
		cudaFree(d_b);
	}

}
__host__ 	void Chunk::setMod(){
	cudaMemcpyToSymbol(p_Mod,&(Element::irr_poly_index[ord>>log_warp_size]),sizeof(idx_t)*max_nonzero_coefs_in_mod);
	cudaMemcpyToSymbol(p_ModLen,&(Element::mod_len[ord>>log_warp_size]),sizeof(idx_t));
}
//__device__ void Chunk::chunk_reduce_xor(Chunk *a, Chunk *c_bottom, Chunk*c_top, idx_t idx)
//{
//	chunk_cell_t ans=c_bottom->v[idx];
//	unsigned int temp_idx;
//	for(idx_t i = 0 ; i < p_ModLen ; ++i)
//	{
//		for(idx_t j = 0 ; j < p_ModLen ; ++j)
//		{
//			temp_idx = idx+(ord<<1)-p_Mod[i]-p_Mod[j];
//			if(temp_idx >= (ord<<1)-p_Mod[j] && temp_idx < (ord<<1))
//				ans^=c_top->v[temp_idx-ord];
//		}
//	}
//	a->v[idx]^=ans;
//}
__device__ void Chunk::chunk_xor(Chunk *a, Chunk* b, idx_t idx){
	a->v[idx]^=b->v[idx];
}
__device__ void Chunk::chunk_reduce_xor(Chunk *a, Chunk *c_bottom, idx_t idx,Chunk* to_xor ,int shift)
{
	// replaced p_ModLen-1 by 4
	for(unsigned int i = 0 ; i < (ord>>1); i+=warp_size)
#pragma unroll
		for(unsigned int j = 0 ; j < 4 ; ++j)
		{
			c_bottom->v[(ord>>1)+idx+i+p_Mod[j]]^=c_bottom->v[(ord>>1)+ord+idx+i];
		}
	for(unsigned int i = 0 ; i < (ord>>1); i+=warp_size)
#pragma unroll
		for(unsigned int j = 0 ; (j) < 4 ; ++j)
		{
			c_bottom->v[idx+i+p_Mod[j]]^=c_bottom->v[ord+idx+i];
		}
		for(unsigned int i = 0 ; i < ord ; i+=warp_size){
			to_xor->v[idx+i]^=(c_bottom->v[idx+i]>>shift);
		}
}

__device__ void Chunk::chunk_reduce(Chunk *a, Chunk *c_bottom, idx_t idx)
{
	//replaced p_ModLen with 5
	for(unsigned int i = 0 ; i < (ord>>1); i+=warp_size)
		for(unsigned int j = 0 ; j+1 < 5 ; ++j)
		{
			c_bottom->v[(ord>>1)+idx+i+p_Mod[j]]^=c_bottom->v[(ord>>1)+ord+idx+i];
		}
	for(unsigned int i = 0 ; i < (ord>>1); i+=warp_size)
		for(unsigned int j = 0 ; (j+1) < 5 ; ++j)
		{
			c_bottom->v[idx+i+p_Mod[j]]^=c_bottom->v[ord+idx+i];
		}
		for(unsigned int i = 0 ; i < ord ; i+=warp_size){
			a->v[idx+i]=c_bottom->v[idx+i];
		}
}
__device__ void Chunk::chunkClmul(Chunk (*a),  Element (*e), idx_t idx, Chunk (*c))
{
	chunk_cell_t my_ans[2][(ord>>(log_warp_size))]={0};
	for(unsigned int k = 0 ; k < ord ; ++k)
	{
		if(EXTRACT_BIT(e->c,k))
			for(unsigned int t = 0 ; t <  (ord>>log_warp_size); ++t)
			{
				int b = (k>(idx+warp_size*t));
				my_ans[b][t]^=a->v[idx+warp_size*t+(b<<log_ord)-k];
			}
	}
	for(unsigned int i = 0 ; i < (ord>>log_warp_size); ++i)
	{
		c->v[idx+i*warp_size] = my_ans[0][i];
		c->v[ord+idx+i*warp_size] = my_ans[1][i];
	}
}
__device__ void Chunk::aux_k_clmul(Chunk *a, Element* e, len_t len,Chunk* c_shared)
{

	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);
	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size)
		my_shared_chunk->v[in_chunk_idx+i]=a[chunk_idx].v[in_chunk_idx+i];
	Chunk::chunkClmul(my_shared_chunk,e,in_chunk_idx,my_shared_chunk);
	Chunk::chunk_reduce(a+chunk_idx,my_shared_chunk,in_chunk_idx);
}
__global__ void k_clmul(Chunk *a,Element *e,len_t len )
{
	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
	__shared__ Chunk c_shared[shared_len<<1];
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	if(idx>=(len<<Chunk::log_threads_in_chunk))
		return;
	Chunk::aux_k_clmul(a,e,len,c_shared);
}
__host__	void Chunk::mul(Chunk (*h_a),Element (*h_e),len_t len, Chunk (*h_res)){

#ifdef __MEASURE
	cudaEvent_t start,stop;
	float time;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
#endif
	//Declare device variables
	Chunk 	(*d_a);
	Element (*d_e);

	//Define Block  and Grid Size.
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(len<<Chunk::log_threads_in_chunk,max_block_size),1,1);

	//Allocate Memory on GPU. (global)
	cudaMalloc(&d_a,sizeof(Chunk)*len);
	cudaMalloc(&d_e,sizeof(Element));

	//Copy memory to GPU.
	cudaMemcpy(d_a,h_a,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
	cudaMemcpy(d_e,h_e,sizeof(Element),cudaMemcpyHostToDevice);

	//Set Mod
	setMod();
//	setElementMul(h_e);
	//Launch Kernel
#ifdef __MEASURE
	cudaEventRecord(start,0);
#endif
	k_clmul<<<gridSize,blockSize>>>(d_a,d_e,len);
#ifdef __MEASURE
	cudaEventRecord(stop,0);
#endif

	//Copy results to host
	cudaMemcpy(h_res,d_a,sizeof(Chunk)*len,cudaMemcpyDeviceToHost);
	//Free allocated memory.
	cudaFree(d_a);
	cudaFree(d_e);
#ifdef __MEASURE
	cudaEventElapsedTime(&time,start,stop);
	printf("Time for the mul: %f ms on %d chunks \n",time,len);
#endif
}

__global__ void k_add(Chunk (*a), Chunk (*b), len_t l)
{
	unsigned int idx = threadIdx.x+blockIdx.x*blockDim.x;
	if(idx>=l*Chunk::cells_in_chunk)
		return;
	((chunk_cell_t*)a)[idx]^=((chunk_cell_t*)b)[idx];
}
__host__ void Chunk::add(Chunk (*h_a),Chunk (*h_b),len_t len)
{

	//Declare device variables
	Chunk (*d_a);
	Chunk (*d_b);

	//Define Block  and Grid Size.
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(max_block_size,len),1,1);

	//Allocate Memory on GPU. (global)
	cudaMalloc(&d_a,sizeof(Chunk)*len);
	cudaMalloc(&d_b,sizeof(Chunk)*len);

	//Copy memory to GPU.
	cudaMemcpy(d_a,h_a,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
	cudaMemcpy(d_b,h_b,sizeof(Chunk)*len,cudaMemcpyHostToDevice);

	//Launch Kernel
	k_add<<<gridSize,blockSize>>>(d_a,d_b,len);

	//Copy results to CPU memory
	cudaMemcpy(h_a,d_a,sizeof(Chunk)*len,cudaMemcpyDeviceToHost);

	//Free allocated memory.
	cudaFree(d_a);
	cudaFree(d_b);
}
__host__ void Chunk::print() const {
	for(unsigned int i = 0 ; i < cells_in_chunk ; ++i){
		cout << bitset<bits_in_byte*sizeof(chunk_cell_t)>(this->v[i])<<endl;
	}
}
__host__	void Elements_Chunk::print() const{
		for(unsigned int i = 0 ; i < elements_in_elements_chunk ; ++i){
			Element::printElement(this->e[i]);
			cout<<endl;
		}
	}
//Mul chunk by another chunk
__device__ void Chunk::clmul_by_chunk_bShuffle_ixor_mask(const Chunk& a,const  Chunk& e, const idx_t in_chunk_idx, Chunk& c, const int shift, const idx_t mask_idx){
	/*
	 * Carryles multiplication
	 */
	chunk_cell_t a_reg[2];
	chunk_cell_t e_reg;
	a_reg[0] = a.v[in_chunk_idx];
	a_reg[1] = a.v[in_chunk_idx+warp_size];

	a_reg[0]^=(a_reg[0]&d_alter_mask[mask_idx+1])<<(1<<mask_idx);
	a_reg[1]^=(a_reg[1]&d_alter_mask[mask_idx+1])<<(1<<mask_idx);

	chunk_cell_t my_ans[2][2]={0};
	int b;
	for(unsigned k = 0 ; k < warp_size; ++k){
		e_reg=e.v[k];
		b= (in_chunk_idx>= k);
		my_ans[0][0] ^= (b*__shfl_up(a_reg[0],k)) & e_reg;
		my_ans[0][1] ^= ((1-b)*__shfl_down(a_reg[0],warp_size-k))& e_reg;
		my_ans[0][1] ^= (b*__shfl_up(a_reg[1],k)) & e_reg;
		my_ans[1][0] ^= ((1-b)*__shfl_down(a_reg[1],32-k))& e_reg;

		e_reg=e.v[k+warp_size];;
		my_ans[0][1] ^= (b*__shfl_up(a_reg[0],k)) & e_reg;
		my_ans[1][0] ^= ((1-b)*__shfl_down(a_reg[0],32-k))& e_reg;
		my_ans[1][0] ^= (b*__shfl_up(a_reg[1],k)) & e_reg;
		my_ans[1][1] ^= ((1-b)*__shfl_down(a_reg[1],32-k))& e_reg;
	}
	/*
	 * Reduce
	 */
#pragma unroll 4
	for(unsigned int i = 0 ; i < 4 ; ++i){
		b=(in_chunk_idx<p_Mod[i]);
		my_ans[1][0]^=(b*__shfl_down(my_ans[1][1],32-p_Mod[i]));
		my_ans[0][1]^=((1-b)*__shfl_up(my_ans[1][1],p_Mod[i]));
	}
#pragma unroll 4
	for(unsigned int i = 0 ; i < 4 ; ++i){
		b=(in_chunk_idx<p_Mod[i]);
		my_ans[0][1]^=(b * __shfl_down(my_ans[1][0],32-p_Mod[i]));
		my_ans[0][0]^=((1-b) * __shfl_up(my_ans[1][0],p_Mod[i]));
	}

	a_reg[0] ^= (my_ans[0][0] >> shift);
	a_reg[1] ^= (my_ans[0][1] >> shift);

	c.v[in_chunk_idx]  = a_reg[0];
	c.v[in_chunk_idx+warp_size] = a_reg[1];
}
__device__ void Chunk::clmul_by_chunk_bShuffle_xor_mask(const Chunk& a,const  Chunk& e, const idx_t in_chunk_idx, Chunk& c, const int shift, const idx_t mask_idx){
	/*
	 * Carryles multiplication
	 */
	chunk_cell_t a_reg[2];
	chunk_cell_t e_reg;
	a_reg[0] = a.v[in_chunk_idx];
	a_reg[1] = a.v[in_chunk_idx+warp_size];

	chunk_cell_t my_ans[2][2]={0};
	int b;
	for(unsigned k = 0 ; k < warp_size; ++k){
		e_reg=e.v[k];
		b= (in_chunk_idx>= k);
		my_ans[0][0] ^= (b*__shfl_up(a_reg[0],k)) & e_reg;
		my_ans[0][1] ^= ((1-b)*__shfl_down(a_reg[0],warp_size-k))& e_reg;
		my_ans[0][1] ^= (b*__shfl_up(a_reg[1],k)) & e_reg;
		my_ans[1][0] ^= ((1-b)*__shfl_down(a_reg[1],32-k))& e_reg;

		e_reg=e.v[k+warp_size];;
		my_ans[0][1] ^= (b*__shfl_up(a_reg[0],k)) & e_reg;
		my_ans[1][0] ^= ((1-b)*__shfl_down(a_reg[0],32-k))& e_reg;
		my_ans[1][0] ^= (b*__shfl_up(a_reg[1],k)) & e_reg;
		my_ans[1][1] ^= ((1-b)*__shfl_down(a_reg[1],32-k))& e_reg;
	}
	/*
	 * Reduce
	 */
#pragma unroll 4
	for(unsigned int i = 0 ; i < 4 ; ++i){
		b=(in_chunk_idx<p_Mod[i]);
		my_ans[1][0]^=(b*__shfl_down(my_ans[1][1],32-p_Mod[i]));
		my_ans[0][1]^=((1-b)*__shfl_up(my_ans[1][1],p_Mod[i]));
	}
#pragma unroll 4
	for(unsigned int i = 0 ; i < 4 ; ++i){
		b=(in_chunk_idx<p_Mod[i]);
		my_ans[0][1]^=(b * __shfl_down(my_ans[1][0],32-p_Mod[i]));
		my_ans[0][0]^=((1-b) * __shfl_up(my_ans[1][0],p_Mod[i]));
	}

	a_reg[0] ^= (my_ans[0][0] >> shift);
	a_reg[1] ^= (my_ans[0][1] >> shift);

	a_reg[0]^=(a_reg[0]&d_alter_mask[mask_idx+1])<<(1<<mask_idx);
	a_reg[1]^=(a_reg[1]&d_alter_mask[mask_idx+1])<<(1<<mask_idx);

	c.v[in_chunk_idx]  = a_reg[0];
	c.v[in_chunk_idx+warp_size] = a_reg[1];
}
__device__ void Chunk::clmul_by_chunk_bShuffle_xor(const Chunk& a,const  Chunk& e, const idx_t in_chunk_idx, Chunk& c, const int shift){
	/*
	 * Carryles multiplication
	 */
	chunk_cell_t a_reg[2];
	chunk_cell_t e_reg;
	a_reg[0] = a.v[in_chunk_idx];
	a_reg[1] = a.v[in_chunk_idx+warp_size];

	chunk_cell_t my_ans[2][2]={0};
	int b;
	for(unsigned k = 0 ; k < warp_size; ++k){
		e_reg=e.v[k];
		b= (in_chunk_idx>= k);
		my_ans[0][0] ^= (b*__shfl_up(a_reg[0],k)) & e_reg;
		my_ans[0][1] ^= ((1-b)*__shfl_down(a_reg[0],warp_size-k))& e_reg;
		my_ans[0][1] ^= (b*__shfl_up(a_reg[1],k)) & e_reg;
		my_ans[1][0] ^= ((1-b)*__shfl_down(a_reg[1],32-k))& e_reg;

		e_reg=e.v[k+warp_size];;
		my_ans[0][1] ^= (b*__shfl_up(a_reg[0],k)) & e_reg;
		my_ans[1][0] ^= ((1-b)*__shfl_down(a_reg[0],32-k))& e_reg;
		my_ans[1][0] ^= (b*__shfl_up(a_reg[1],k)) & e_reg;
		my_ans[1][1] ^= ((1-b)*__shfl_down(a_reg[1],32-k))& e_reg;
	}
	/*
	 * Reduce
	 */
#pragma unroll 4
	for(unsigned int i = 0 ; i < 4 ; ++i){
		b=(in_chunk_idx<p_Mod[i]);
		my_ans[1][0]^=(b*__shfl_down(my_ans[1][1],32-p_Mod[i]));
		my_ans[0][1]^=((1-b)*__shfl_up(my_ans[1][1],p_Mod[i]));
	}
#pragma unroll 4
	for(unsigned int i = 0 ; i < 4 ; ++i){
		b=(in_chunk_idx<p_Mod[i]);
		my_ans[0][1]^=(b * __shfl_down(my_ans[1][0],32-p_Mod[i]));
		my_ans[0][0]^=((1-b) * __shfl_up(my_ans[1][0],p_Mod[i]));
	}
		c.v[in_chunk_idx] ^= (my_ans[0][0] >> shift);
		c.v[in_chunk_idx+warp_size] ^= (my_ans[0][1] >> shift);
}
__device__ void Chunk::clmul_by_chunk_bShuffle(const Chunk& a,const  Chunk& e, const idx_t in_chunk_idx, Chunk& c){
	/*
	 * Carryles multiplication
	 */
	chunk_cell_t a_reg[2];
	chunk_cell_t e_reg;
	a_reg[0] = a.v[in_chunk_idx];
	a_reg[1] = a.v[in_chunk_idx+warp_size];

	chunk_cell_t my_ans[2][2]={0};
	int b;
	for(unsigned k = 0 ; k < warp_size; ++k){
		e_reg=e.v[k];
		b= (in_chunk_idx>= k);
		my_ans[0][0] ^= (b*__shfl_up(a_reg[0],k)) & e_reg;
		my_ans[0][1] ^= ((1-b)*__shfl_down(a_reg[0],warp_size-k))& e_reg;
		my_ans[0][1] ^= (b*__shfl_up(a_reg[1],k)) & e_reg;
		my_ans[1][0] ^= ((1-b)*__shfl_down(a_reg[1],32-k))& e_reg;

		e_reg=e.v[k+warp_size];
		my_ans[0][1] ^= (b*__shfl_up(a_reg[0],k)) & e_reg;
		my_ans[1][0] ^= ((1-b)*__shfl_down(a_reg[0],32-k))& e_reg;
		my_ans[1][0] ^= (b*__shfl_up(a_reg[1],k)) & e_reg;
		my_ans[1][1] ^= ((1-b)*__shfl_down(a_reg[1],32-k))& e_reg;
	}
	/*
	 * Reduce
	 */
#pragma unroll 4
	for(unsigned int i = 0 ; i < 4 ; ++i){
		b=(in_chunk_idx<p_Mod[i]);
		my_ans[1][0]^=(b*__shfl_down(my_ans[1][1],32-p_Mod[i]));
		my_ans[0][1]^=((1-b)*__shfl_up(my_ans[1][1],p_Mod[i]));
	}
#pragma unroll 4
	for(unsigned int i = 0 ; i < 4 ; ++i){
		b=(in_chunk_idx<p_Mod[i]);
		my_ans[0][1]^=(b * __shfl_down(my_ans[1][0],32-p_Mod[i]));
		my_ans[0][0]^=((1-b) * __shfl_up(my_ans[1][0],p_Mod[i]));
	}
		c.v[in_chunk_idx] = my_ans[0][0];
		c.v[in_chunk_idx+warp_size] = my_ans[0][1];
}
__device__ void Chunk::clmul_by_chunk(Chunk& a, Chunk& e, idx_t idx, Chunk* c){
	chunk_cell_t my_ans[2][(ord>>(log_warp_size))]={0};
	int b;
//	const int l = ord>>log_warp_size;
	for(unsigned int k = 0 ; k < ord ; ++k)
#pragma unroll 2
		for(unsigned int t = 0 ; t <  2; ++t)
		{
			b = (k>(idx+warp_size*t));
			my_ans[b][t]^=a.v[idx+warp_size*t+(b<<log_ord)-k]&e.v[k];
		}
#pragma unroll 2
	for(unsigned int i = 0 ; i < 2; ++i)
	{
		c->v[idx+i*warp_size] = my_ans[0][i];
		c->v[ord+idx+i*warp_size] = my_ans[1][i];
	}
}
__global__ void k_mul_chunk(Chunk* cs, Chunk* c, len_t cs_len)
{
	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
	__shared__ Chunk c_shared[shared_len<<1];
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	if(idx>=(cs_len<<Chunk::log_threads_in_chunk))
		return;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);
	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size){
		my_shared_chunk->v[in_chunk_idx+i]=cs[chunk_idx].v[in_chunk_idx+i];
		my_shared_chunk[1].v[in_chunk_idx+i]=c->v[in_chunk_idx+i];
	}
	Chunk::clmul_by_chunk(my_shared_chunk[0],my_shared_chunk[1],in_chunk_idx,my_shared_chunk);
	Chunk::chunk_reduce(cs+chunk_idx,my_shared_chunk,in_chunk_idx);
}
__global__ void k_mul_chunk_xor(Chunk* cs, Chunk* c, len_t cs_len,Chunk* to_xor, int shift = 0)
{
	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
	__shared__ Chunk c_shared[shared_len<<1];
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	if(idx>=(cs_len<<Chunk::log_threads_in_chunk))
		return;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);
	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size){
		my_shared_chunk->v[in_chunk_idx+i]=cs[chunk_idx].v[in_chunk_idx+i];
		my_shared_chunk[1].v[in_chunk_idx+i]=c->v[in_chunk_idx+i];
	}
	Chunk::clmul_by_chunk(my_shared_chunk[0],my_shared_chunk[1],in_chunk_idx,my_shared_chunk);
	Chunk::chunk_reduce_xor(cs+chunk_idx,my_shared_chunk,in_chunk_idx,to_xor,shift);
}
//Mul a chunk by a chunk
void Chunk::chunk_mul(Chunk (* h_a), Chunk (*h_b) , len_t len, Chunk (*h_res), bool copy, bool do_xor, int shift){
#ifdef __MEASURE
	cudaEvent_t start,stop;
	float time;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
#endif
	//Declare device variables
	Chunk 	(*d_a);
	Chunk   (*d_b);

	//Define Block  and Grid Size.
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(len<<Chunk::log_threads_in_chunk,max_block_size),1,1);

	if(copy){
	//Allocate Memory on GPU. (global)
		cudaMalloc(&d_a,sizeof(Chunk)*len);
		cudaMalloc(&d_b,sizeof(Chunk));
	//Copy memory to GPU.
		cudaMemcpy(d_a,h_a,sizeof(Chunk)*len,cudaMemcpyHostToDevice);
		cudaMemcpy(d_b,h_b,sizeof(Chunk),cudaMemcpyHostToDevice);
	} else {
		d_a = h_a;
		d_b = h_b;
	}


	//Set Mod
	setMod();
//	setElementMul(h_e);
	//Launch Kernel
#ifdef __MEASURE
	cudaEventRecord(start,0);
#endif
	if(do_xor)
		k_mul_chunk_xor<<<gridSize,blockSize>>>(d_a,d_b,len,d_a,shift);
	else
		k_mul_chunk<<<gridSize,blockSize>>>(d_a,d_b,len);
#ifdef __MEASURE
	cudaEventRecord(stop,0);
#endif

	if(copy){
	//Copy results to host
		cudaMemcpy(h_res,d_a,sizeof(Chunk)*len,cudaMemcpyDeviceToHost);
	//Free allocated memory.
		cudaFree(d_a);
		cudaFree(d_b);
	}
#ifdef __MEASURE
	cudaEventElapsedTime(&time,start,stop);
	printf("Time for the mul: %f ms on %d chunks \n",time,len);
#endif
}









/*-------------------------------------*/

/*** GPU FFT ***/
const unsigned int multThreadsInBlock = 1024;

__global__ void k_multiExp_mult_bShuffle(Chunk* d_a, Chunk* d_b , len_t b_len , len_t a_len){
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	if(idx>=(a_len<<Chunk::log_threads_in_chunk))
		return;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
//	const idx_t shared_chunk_idx = ;
	Chunk::clmul_by_chunk_bShuffle(d_a[chunk_idx],d_b[chunk_idx & (b_len-1)],in_chunk_idx,d_a[chunk_idx]);
//	Chunk::chunk_reduce(d_a+chunk_idx,my_shared_chunk,in_chunk_idx);
}
__global__ void k_multiExp_mult(Chunk* d_a, Chunk* d_b , len_t b_len , len_t a_len){
//	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
//	__shared__ Chunk c_shared[shared_len<<1];
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	if(idx>=(a_len<<Chunk::log_threads_in_chunk))
		return;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
//	const idx_t shared_chunk_idx = ;
//	Chunk* my_shared_chunk = c_shared+(((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk))<<1);
//	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size){
//		my_shared_chunk->v[in_chunk_idx+i]=d_a[chunk_idx].v[in_chunk_idx+i];
//		my_shared_chunk[1].v[in_chunk_idx+i]=d_b[chunk_idx & (b_len - 1)].v[in_chunk_idx+i];
//	}
	Chunk::clmul_by_chunk_bShuffle(d_a[chunk_idx],d_b[chunk_idx & (b_len - 1)],in_chunk_idx,d_a[chunk_idx]);
//	Chunk::chunk_reduce(d_a+chunk_idx,my_shared_chunk,in_chunk_idx);
}

/*
 * Gets as input:
 * 	d_a - the polynomial.
 * 	d_b - the multiexponent of current b_m for on strip of elements.
 * 	b_len - length of d_b in chunks.
 *
 * 	Multiplies chunk d_a[i] by chunk d_b[i mod b_len].
 */
void GPU_FFT::multiExp_mult(len_t a_len, Chunk* d_a , Chunk* d_b , len_t b_len){
	unsigned int threads = (a_len<<Chunk::log_elements_in_chunk);
	dim3 blockSize(multThreadsInBlock,1,1);
	dim3 gridSize(sizeCiel(threads,multThreadsInBlock),1,1);

	//Launch Kernel
	k_multiExp_mult_bShuffle<<<gridSize,blockSize>>>(d_a,d_b,b_len, threads>>Chunk::log_elements_in_chunk);
}

void GPU_FFT::multiExponentiate_gpu(const FFT* fft,Chunk* d_chunk_P, len_t p_len, len_t dim, Chunk* d_exp ){
	//Copy exps to memory.
	if(dim<=Chunk::log_elements_in_chunk){
		cudaMemcpy(d_exp,fft->gpu_exp[fft->basis.getSize()-dim],
				sizeof(Chunk),cudaMemcpyHostToDevice);
		multiExp_mult(p_len,d_chunk_P,d_exp,1);
	} else {
		cudaMemcpy(d_exp,fft->gpu_exp[fft->basis.getSize()-dim],
				sizeof(Chunk)*(1<<(dim-Chunk::log_elements_in_chunk)),cudaMemcpyHostToDevice);
		multiExp_mult(p_len,d_chunk_P,d_exp,1<<(dim-Chunk::log_elements_in_chunk));
	}

}

void GPU_FFT::imultiExponentiate_gpu(const FFT* fft,Chunk* d_chunk_P, len_t p_len, len_t dim, Chunk* d_exp ){
	//Copy exps to memory.
	if(dim<=Chunk::log_elements_in_chunk){
		cudaMemcpy(d_exp,fft->gpu_i_exp[fft->basis.getSize()-dim],
				sizeof(Chunk),cudaMemcpyHostToDevice);
		multiExp_mult(p_len,d_chunk_P,d_exp,1);
	} else {
		cudaMemcpy(d_exp,fft->gpu_i_exp[fft->basis.getSize()-dim],
				sizeof(Chunk)*(1<<(dim-Chunk::log_elements_in_chunk)),cudaMemcpyHostToDevice);
		multiExp_mult(p_len,d_chunk_P,d_exp,1<<(dim-Chunk::log_elements_in_chunk));
	}

}

__device__ void taylor_smaller_than_chunk(Chunk* chunk,idx_t in_chunk_idx, len_t t_dim, len_t p_len, idx_t idx){
		if(idx >= ((p_len) << (Chunk::log_cells_in_chunk)))
			return;
		//Performs the rest of the expansion.
		chunk_cell_t cell = chunk->v[in_chunk_idx];
		for(; t_dim >=2 ; --t_dim){
			cell ^= (cell & (d_chunk_cell_mask[t_dim]<<((1<<t_dim)-(1<<(t_dim-2)))))>>(1<<(t_dim-2));
			cell ^= (cell & (d_chunk_cell_mask[t_dim]<<(1<<(t_dim-1))))>>(1<<(t_dim-2));
		}
		chunk->v[in_chunk_idx]= cell;
}
__global__ void k_taylorExpansion_iteration_large(Chunk* d_chunk_P , len_t p_len , len_t t_dim){
	idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	idx_t in_chunk_idx = idx & (Chunk::cells_in_chunk-1);
	/*
	 * flag = true if current sub-polynomial fits in a thread block.
	 */
	bool flag = (t_dim-Chunk::log_elements_in_chunk <= log_max_block_size-Chunk::log_cells_in_chunk + 2);
	if(idx >= ((p_len) << (Chunk::log_cells_in_chunk-2)))
		return;
	do {
		len_t sub_len = (1<<(t_dim-Chunk::log_elements_in_chunk));
		idx_t chunk_idx = idx >> (Chunk::log_cells_in_chunk);
		chunk_idx = (chunk_idx /(sub_len>>2))*(sub_len) + ((chunk_idx) & ((sub_len>>2)-1));
		d_chunk_P[(sub_len>>1) + chunk_idx ].v[in_chunk_idx] ^= d_chunk_P[(3*(sub_len>>2)) + chunk_idx].v[in_chunk_idx];
		d_chunk_P[(sub_len>>2) + chunk_idx ].v[in_chunk_idx] ^= d_chunk_P[(sub_len>>1) + chunk_idx].v[in_chunk_idx];
		if(flag)
			__syncthreads();
		--t_dim;
	} while ( flag && t_dim > Chunk::log_elements_in_chunk + 1);
		//If number of threads needed is less than a thread block - we can just continue!
}
__global__ void k_taylorExpansion_iteration_twoChunks(Chunk* d_chunk_P, len_t p_len, len_t t_dim){
	idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	idx_t in_chunk_idx = idx & (Chunk::cells_in_chunk-1);
	if(idx >= ((p_len) << (Chunk::log_cells_in_chunk-1)))
		return;
	idx_t chunk_idx= (idx >> (Chunk::log_cells_in_chunk) ) << 1;
	d_chunk_P[(chunk_idx)+1].v[in_chunk_idx]^= (d_chunk_P[(chunk_idx)+1].v[in_chunk_idx]>>(Chunk::elements_in_chunk>>1));
	d_chunk_P[(chunk_idx)].v[in_chunk_idx]^= (d_chunk_P[(chunk_idx)+1].v[in_chunk_idx]<<(Chunk::elements_in_chunk>>1));
}
__global__ void k_taylorExpansion_iteration_singleChunk(Chunk* d_chunk_P, len_t p_len, len_t t_dim){

	idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	idx_t in_chunk_idx = idx & (Chunk::cells_in_chunk-1);
	//Performs the rest of the expansion.
	idx_t chunk_idx = (idx >> (Chunk::log_cells_in_chunk));
	taylor_smaller_than_chunk(d_chunk_P+chunk_idx,in_chunk_idx,t_dim,p_len,idx);
}
void taylorExpansion_iteration(const FFT* fft, Chunk * d_chunk_P , len_t p_len , len_t t_dim){
	unsigned int threads;
	dim3 blockSize(max_block_size,1,1);
	if( t_dim >= Chunk::log_elements_in_chunk + 2){
		threads = p_len<<(Chunk::log_cells_in_chunk-2);
		dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
		k_taylorExpansion_iteration_large<<<gridSize,blockSize>>>(d_chunk_P,p_len,t_dim);
	} else if (t_dim == Chunk::log_elements_in_chunk + 1){
		threads = p_len <<(Chunk::log_cells_in_chunk-1);
		dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
		k_taylorExpansion_iteration_twoChunks<<<gridSize,blockSize>>>(d_chunk_P,p_len,t_dim);
	} else {
		threads = p_len << Chunk::log_cells_in_chunk;
		dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
		k_taylorExpansion_iteration_singleChunk<<<gridSize,blockSize>>>(d_chunk_P,p_len,t_dim);
	}
//	k_taylorExpansion_iteration<<<gridSize,blockSize>>>(d_chunk_P,p_len,t_dim);
}
void GPU_FFT::taylorExpansion_gpu(const FFT* fft, Chunk* d_chunk_P, len_t p_len , len_t dim){
	len_t t_dim = dim;
	/*
	 * The sub-polynomial requires more than a single thread block.
	 */
	while(t_dim+Chunk::log_cells_in_chunk > Chunk::log_elements_in_chunk+log_max_block_size + 2){
		taylorExpansion_iteration(fft,d_chunk_P,p_len,t_dim);
		--t_dim;
	}
	/*
	 * The sub-polynomial requires more than 2 chunks BUT less than a single thread block.
	 */
	if(t_dim >= Chunk::log_elements_in_chunk + 2){
		taylorExpansion_iteration(fft,d_chunk_P,p_len,t_dim);
		t_dim = Chunk::log_elements_in_chunk+1;
	}
	/*
	 * Each sub-polynomial takes exactly two chunks.
	 */
	if(t_dim == Chunk::log_elements_in_chunk + 1){
		taylorExpansion_iteration(fft,d_chunk_P,p_len,t_dim);
		--t_dim;
	}
	/*
	 * Each sub-polynomial takes at most one chunk.
	 */
	taylorExpansion_iteration(fft,d_chunk_P,p_len,t_dim);
}

	/*
	 *  Input:
	 *  1) d_chunk_p - The polynomial on device's memory.
	 *  2) p_len - number of chunks in d_chunk_p.
	 *  3) chunk_idx - the chunk current thread has to deal with.
	 *  4) in_chunk_idx - the number of the cell the current thread deals with.
	 *  5) t_dim - The dim of the original (input) subpolynomial.
	 *
	 *  This function performs what a single thread does when performing the partition function, on a single chunk.
	 */
__device__ void partition_in_chunk(Chunk* d_chunk_p, len_t p_len , idx_t in_chunk_idx, len_t t_dim){
	chunk_cell_t ans = d_chunk_p->v[in_chunk_idx];
	for(unsigned int i = 2 ; i <= Chunk::log_elements_in_chunk && i<=t_dim ; ++i){
		ans = 	(ans & d_chunk_cell_mask[i]) |
				(ans & (d_chunk_cell_mask[i]<<((1<<(i))-(1<<(i-2))))) |
				(ans & (d_chunk_cell_mask[i]<<(1<<(i-1))))>>(1<<(i-2)) |
				(ans & (d_chunk_cell_mask[i]<<(1<<(i-2))))<<(1<<(i-2));
	}
	d_chunk_p->v[in_chunk_idx]=ans;
}

__device__ void partition_two_chunks(Chunk* d_chunk_p, idx_t in_chunk_idx){
	chunk_cell_t ans[2];
	chunk_cell_t load[2];
	const chunk_cell_t mask = d_chunk_cell_mask[Chunk::log_elements_in_chunk+1];
	load[0]=d_chunk_p->v[in_chunk_idx];
	load[1]=d_chunk_p[1].v[in_chunk_idx];
	ans[0]=(load[0] & mask) | ((load[1] & mask)<<(Chunk::elements_in_chunk>>1));
	ans[1]=(load[1] & (mask<< (Chunk::elements_in_chunk>>1)) )|
			((load[0] >> (Chunk::elements_in_chunk>>1)) & mask);
	d_chunk_p->v[in_chunk_idx] = ans[0];
	d_chunk_p[1].v[in_chunk_idx] = ans[1];
}
__device__ void partition_general(Chunk* d_chunk_p_src, Chunk* d_chunk_p_dst,  idx_t chunk_idx, idx_t in_chunk_idx, idx_t t_dim){
	if(chunk_idx & 1)
		d_chunk_p_dst[(chunk_idx>>1) + (1<<(t_dim-1))].v[in_chunk_idx] = d_chunk_p_src[chunk_idx].v[in_chunk_idx];
	else
		d_chunk_p_dst[chunk_idx>>1].v[in_chunk_idx] = d_chunk_p_src[chunk_idx].v[in_chunk_idx];
}

__global__ void k_partition_iteration_two_chunks(Chunk* d_chunk_p_src, len_t p_len){
	idx_t idx = threadIdx.x + blockIdx.x * blockDim.x;
	idx_t chunk_idx = (idx >> Chunk::log_cells_in_chunk)<<1;
	idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
	if(chunk_idx >= p_len){
		return;
	}
	partition_two_chunks(d_chunk_p_src+chunk_idx, in_chunk_idx);
}
__global__ void k_partition_iteration_in_chunk(Chunk* d_chunk_p_src, len_t p_len, len_t t_dim){
	idx_t idx = threadIdx.x + blockIdx.x * blockDim.x;
	idx_t chunk_idx = idx >> Chunk::log_cells_in_chunk;
	idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
	if(chunk_idx >= p_len){
		return;
	}
	partition_in_chunk(d_chunk_p_src+chunk_idx,p_len,in_chunk_idx,t_dim);
}
__global__ void k_partition_iteration_general(Chunk* d_chunk_p_src, Chunk* d_chunk_p_dst, len_t p_len, len_t t_dim){
	idx_t idx = threadIdx.x + blockIdx.x * blockDim.x;
	idx_t chunk_idx = idx >> Chunk::log_cells_in_chunk;
	idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
	idx_t base = chunk_idx ^ (chunk_idx & andMask(t_dim-Chunk::log_elements_in_chunk));
	chunk_idx &= andMask(t_dim-Chunk::log_elements_in_chunk);
	if(base+chunk_idx >= p_len)
		return;
	partition_general(d_chunk_p_src+base,d_chunk_p_dst+base, chunk_idx, in_chunk_idx,t_dim-Chunk::log_elements_in_chunk );
}
/*
 * If t_dim > Chunk::log_elements_in_chunk+1 the result is written in dst, otherwise it will be written in src.
 */
bool GPU_FFT::partition(Chunk* d_chunk_p_src, Chunk* d_chunk_p_dst, len_t p_len, len_t t_dim){
	len_t threads = p_len << Chunk::log_cells_in_chunk;
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
	k_partition_iteration_in_chunk<<<gridSize,blockSize>>>(d_chunk_p_src,p_len,t_dim);
	if(t_dim >= Chunk::log_elements_in_chunk +1){
      	dim3 gridSizeTwoChunks(sizeCiel(threads>>1,max_block_size),1,1);
		k_partition_iteration_two_chunks<<<gridSizeTwoChunks,blockSize>>>(d_chunk_p_src,p_len);
	}
	if(t_dim > Chunk::log_elements_in_chunk+1){
		k_partition_iteration_general<<<gridSize,blockSize>>>(d_chunk_p_src,d_chunk_p_dst,p_len,t_dim);
		return true;
	}
	return false;
}

//__global__ void k_xor_chunk_vector_by_single_chuk(Chunk* chunk_vec, Chunk* single_chunk, len_t chunk_vec_len){
//	idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
//	idx_t chunk_idx = idx >> Chunk::log_cells_in_chunk;
//	idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
//	if(chunk_idx >= chunk_vec_len){
//		return;
//	}
//	chunk_vec[chunk_idx].v[in_chunk_idx] ^= single_chunk->v[in_chunk_idx];
//	return;
//}
__global__ void k_copy_and_shift_vec(Chunk* d_chunk_src, Chunk* d_chunk_dst, len_t p_len){

	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
	__shared__ Chunk c_shared[shared_len<<1];

	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);

	if(chunk_idx  >= p_len){
		return;
	}

	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);

	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size){
		my_shared_chunk->v[in_chunk_idx+i]=d_chunk_src[chunk_idx].v[in_chunk_idx+i];
		my_shared_chunk[1].v[in_chunk_idx+i]=d_linear_mul->v[in_chunk_idx+i];
	}

	chunk_cell_t tmp;
	for(unsigned int i = 0 ; i < Element::ord ; i+=warp_size){
		tmp = my_shared_chunk->v[in_chunk_idx+i];
		my_shared_chunk->v[in_chunk_idx+i] = (tmp & 0xaaaaaaaa) ^ ((tmp & 0xaaaaaaaa)>>1);
		d_chunk_dst[chunk_idx].v[in_chunk_idx+i] = (tmp & 0x55555555) ^ ((tmp & 0x55555555)<<1);
	}
	Chunk::clmul_by_chunk(my_shared_chunk[0],my_shared_chunk[1],in_chunk_idx,my_shared_chunk);
	Chunk::chunk_reduce(d_chunk_src+chunk_idx,my_shared_chunk,in_chunk_idx);
	for(unsigned int i = 0 ; i < Element::ord ; i+=warp_size){
		d_chunk_src[chunk_idx].v[in_chunk_idx+i] ^= d_chunk_dst[chunk_idx].v[in_chunk_idx+i];
	}
	return;
}
//__global__ void k_xor_chunk_vectors(Chunk* d_chunk, Chunk* x, len_t p_len){
//	idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
//	idx_t chunk_idx = idx >> Chunk::log_cells_in_chunk;
//	idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
//	if(chunk_idx >= p_len){
//		return;
//	}
//	d_chunk[chunk_idx].v[in_chunk_idx] ^= x[chunk_idx].v[in_chunk_idx];
//}
void GPU_FFT::linearEvaluation(Chunk* d_chunk_p,Chunk* d_chunk_p_cpy, len_t p_len){
	len_t threads = p_len << Chunk::log_threads_in_chunk;
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
	k_copy_and_shift_vec<<<gridSize,blockSize>>>(d_chunk_p,d_chunk_p_cpy,p_len);
}
__global__ void k_subspaceAdd_general(Chunk* d_a, len_t a_len, len_t b_len){
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	idx_t chunk_idx = (idx >> Chunk::log_cells_in_chunk);
	chunk_idx = ((chunk_idx / b_len)*(2*b_len)) +b_len+(chunk_idx & (b_len-1));
	if(chunk_idx  >= a_len){
		return;
	}
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
	d_a[chunk_idx].v[in_chunk_idx]^=d_a[chunk_idx-b_len].v[in_chunk_idx];
}
__global__ void k_subspaceMult_general(Chunk* d_a, Chunk* d_b , len_t b_len , len_t a_len){
	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
	__shared__ Chunk c_shared[shared_len<<1];
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	idx_t chunk_idx = (((idx >> Chunk::log_threads_in_chunk))/b_len)*(2*b_len)+b_len;
	const idx_t b_chunk_idx = (idx>>Chunk::log_threads_in_chunk) & (b_len - 1);
	chunk_idx+=b_chunk_idx;
	if(chunk_idx  >= a_len){
		return;
	}
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);
	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size){
		my_shared_chunk->v[in_chunk_idx+i]=d_a[chunk_idx].v[in_chunk_idx+i];
		my_shared_chunk[1].v[in_chunk_idx+i]=d_b[b_chunk_idx].v[in_chunk_idx+i];
	}
	Chunk::clmul_by_chunk_bShuffle_xor(d_a[chunk_idx],d_b[b_chunk_idx],in_chunk_idx,d_a[chunk_idx-b_len],0);
//	Chunk::clmul_by_chunk(my_shared_chunk[0],my_shared_chunk[1],in_chunk_idx,my_shared_chunk);
//	Chunk::chunk_reduce_xor(d_a+chunk_idx,my_shared_chunk,in_chunk_idx,d_a+chunk_idx-b_len,0);
}
__global__ void k_subspaceMult_chunk_and_add(Chunk* d_a, Chunk* d_b , len_t log_elements_in_b ,len_t a_len ){
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
	if(chunk_idx  >= a_len){
		return;
	}
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
	Chunk::clmul_by_chunk_bShuffle_xor_mask(d_a[chunk_idx],*d_b,in_chunk_idx,d_a[chunk_idx],1<<log_elements_in_b,log_elements_in_b);
}
/*
 * Dim = Size of U.
 */
void GPU_FFT::WFromUV(Chunk* d_chunk_p, len_t p_len, Chunk* subspace, len_t dim){
	if(dim>=Chunk::log_elements_in_chunk){
		len_t threadsMul = p_len << (Chunk::log_threads_in_chunk-1);
		len_t threadsAdd = p_len << (Chunk::log_cells_in_chunk-1);
		dim3 block(multThreadsInBlock,1,1);
		dim3 gridMul(sizeCiel(threadsMul,multThreadsInBlock),1,1);
		dim3 gridAdd(sizeCiel(threadsAdd,multThreadsInBlock),1,1);
		k_subspaceMult_general<<<gridMul,block>>>(d_chunk_p, subspace, (1<<(dim-Chunk::log_elements_in_chunk)),p_len);
		k_subspaceAdd_general<<<gridAdd,block>>>(d_chunk_p,p_len,1<<(dim-Chunk::log_elements_in_chunk));
	} else {
		len_t threadsMul = p_len << Chunk::log_threads_in_chunk;
		dim3 block(multThreadsInBlock,1,1);
		dim3 gridMul(sizeCiel(threadsMul,multThreadsInBlock),1,1);
		k_subspaceMult_chunk_and_add<<<gridMul,block>>>(d_chunk_p,subspace, dim , p_len);
	}
}
void GPU_FFT::setUpConstantMemory(const FFT* fft){
	/*
	 * Masks for taylor expansion on small dimensions.
	 */
	cudaMemcpyToSymbol(d_chunk_cell_mask,taylorExp_masks,sizeof(chunk_cell_t)*(Chunk::log_elements_in_chunk+2));
	/*
	 * Masks for WFromUV
	 */
	cudaMemcpyToSymbol(d_alter_mask,alter_masks,sizeof(chunk_cell_t)*(Chunk::log_elements_in_chunk+1));
	/*
	 * Table for partition operation on small dimension.
	 */
	cudaMemcpyToSymbol(d_partition,partition_byte,sizeof(chunk_cell_t)*(1<<bits_in_byte));
	/*
	 * Chunks to multiply and add when calculating linear functions.
	 */
	cudaMemcpyToSymbol(d_linear_mul,&fft->linear_mul,sizeof(Chunk));
	cudaMemcpyToSymbol(d_ilinear_mul,&fft->ilinear_mul,sizeof(Chunk));
}
/*
 * p is a pointer to a chunk array on gpu, l is its length.
 * prints that array.
 */
void printChunkOnGPU(Chunk* p , len_t l){
// TODO: No real need for printing and this break the building of FFT as separate library
/*
	Chunk* h_p = Tests::copyFromGPU(p,l);
	for(unsigned int i = 0 ; i < l ; ++i){
		h_p[i].print();
		std::cout << std::endl << std::endl;

	}
	free(h_p);
	std::cout << std::endl << std::endl;
*/
}

/*
 * This is the gpu implementation of the FFT when it fits into a single thread block.
 * In that case, the whole FFT can be calculated using a single kernel invocation and some additional
 * synchronization primitives.
 *
 * In this implementation, each WARP is responsible for a single chunk, therefore if 32 threads compose a single warp and
 * we use 1024 threads in a thread block, then each thread block is responsible for 32 chunks.
 *
 * 0) First we load ALL subspaces and exponents into global memory in a special pre-allocated array.
 * We also allocate 64 Chunks long array in shared memory.
 *
 * 1) We load the shared memory with the relevant polynomial (depends on the index).
 * 2) Series of Multiexp -> taylorExp -> partition, all inside local memory.
 * 	2.1) Before Each Multiexp we load 32 chunks of exps to shared memory.
 * 3) Linear evaluation.
 * 4) Series of WFromUV, while loading subspaces from global memory.
 *
 * This whole implementation assumes that:
 * 		Chunk::log_cells_in_chunk = 1+ Chunk::log_elements_in_chunk
 */

__device__ void InTB_preMulExpand(	Chunk* const d_a,
									const Chunk* const d_b,
									const len_t b_len ,
									const idx_t idx ,
									const idx_t chunkIdx ,
									const idx_t in_chunk_idx){
	chunk_cell_t load[2];
	load[0]=d_a[chunkIdx].v[in_chunk_idx];
	load[1]=d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk];
	__syncthreads();
	d_a[chunkIdx<<1].v[in_chunk_idx] = load[0];
	d_a[chunkIdx<<1].v[in_chunk_idx + Chunk::elements_in_chunk] = load[1];
	d_a[(chunkIdx<<1)+1].v[in_chunk_idx]
	                       = d_b[chunkIdx & (b_len-1)].v[in_chunk_idx];
	d_a[(chunkIdx<<1)+1].v[in_chunk_idx + Chunk::elements_in_chunk]
	                       = d_b[chunkIdx & (b_len-1)].v[in_chunk_idx + Chunk::elements_in_chunk];
	__syncthreads();
}
/*
 * Takes every second chunk and puts all of them in the first half.
 * Second half output is unknown.
 * 1) d_a - Pointer for shmem array of the polynomial.
 * 2) idx - index of the thread.
 * 3) chunkIdx - (idx / Chunk::elemetns_in_chunk).
 * 4) in_chunk_idx - (idx % Chunk::elements_in_chunk).
 */
__device__ void InTB_postMulShrink(	Chunk * const d_a,
									const idx_t idx ,
									const idx_t chunkIdx ,
									const idx_t in_chunk_idx){
	chunk_cell_t load[2];
	load[0]=d_a[chunkIdx<<1].v[in_chunk_idx];
	load[1]=d_a[chunkIdx<<1].v[in_chunk_idx+Chunk::elements_in_chunk];
	__syncthreads();
	d_a[chunkIdx].v[in_chunk_idx]=load[0];
	d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk]=load[1];
	__syncthreads();
}
/*
 * Multiplies d_a[0] by d_a[1] and writes the answer at d_a[0].
 * The state of d_a[1] is unknown at the end.
 *
 * 1)	d_a - the chunk the mult.
 * 2) 	in_chunk_idx - Is thread index % Chunk::elements_in_chunk
 */
__device__ void InTB_Mult(Chunk* const d_a, const idx_t in_chunk_idx){
	Chunk::clmul_by_chunk(*d_a,d_a[1],in_chunk_idx,d_a);
	Chunk::chunk_reduce(d_a,d_a,in_chunk_idx);
	__syncthreads();
}
/*
 * This is the multi exponentiation.
 * 1) d_a - pointer to the shmem.
 * 2) exp - pointer to the exponents to multiply (global mem).
 * 3) dim - dimension of multiexponentiation.
 * 4) idx - thread index.
 * 5) chunkIdx - idx/Chunk::elements_in_chunk.
 * 6) in_chunk_idx - idx%Chunk::elements_in_chunk.
 */
 __device__ void InTB_MultiExp(	Chunk* const d_a,
		 	 	 	 	 	 	const Chunk* const exp,
		 	 	 	 	 	 	const len_t exp_len,
		 	 	 	 	 	 	const idx_t idx,
		 	 	 	 	 	 	const idx_t chunkIdx,
		 	 	 	 	 	 	const idx_t in_chunk_idx,
		 	 	 	 	 	 	const len_t half_d_a_len){
	chunk_cell_t load[2];
	chunk_cell_t swap;
	load[0] = d_a[half_d_a_len + chunkIdx].v[in_chunk_idx];
	load[1] = d_a[half_d_a_len + chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk];

	InTB_preMulExpand(d_a,exp,exp_len,idx,chunkIdx,in_chunk_idx);
	InTB_Mult(d_a+(chunkIdx<<1),in_chunk_idx);
	InTB_postMulShrink(d_a,idx,chunkIdx,in_chunk_idx);

	swap = load[0];
	load[0]=d_a[chunkIdx].v[in_chunk_idx];
	d_a[chunkIdx].v[in_chunk_idx] = swap;
	swap = load[1];
	load[1]=d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk];
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk] = swap;

	InTB_preMulExpand(d_a,exp+((half_d_a_len) & (exp_len>>1)),exp_len,idx,chunkIdx,in_chunk_idx);
	InTB_Mult(d_a+(chunkIdx<<1),in_chunk_idx);
	InTB_postMulShrink(d_a,idx,chunkIdx,in_chunk_idx);

	d_a[half_d_a_len + chunkIdx].v[in_chunk_idx] = d_a[chunkIdx].v[in_chunk_idx];
	d_a[half_d_a_len + chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk] =
			d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk];
	d_a[chunkIdx].v[in_chunk_idx] = load[0];
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk] = load[1];
	__syncthreads();

 }
/*
 * Taylor Expansion
 *
 * 1) d_a is the pointer to the beginning of the polynomial in shmem.
 * 2) dim>=2. Also dim<=10.
 * 3) idx - the idx of the thread.
 * 4) in_chunk_idx - (idx % (Chunk::chunk_cells_in_chunk)).
 * 5) chunkIdx -  index of chunk INSIDE sub-polynomial.
 */
__device__ void InTB_TaylorExp(	Chunk* const d_a,
								idx_t dim,
								const idx_t idx,
								const idx_t in_chunk_idx,
								const idx_t chunkIdx,
								const idx_t half_d_a_len){
	len_t l,s;
	/*
	 * If more than a single chunk is needed for a subpolynomial
	 */
	if(dim> Chunk::log_elements_in_chunk){
		for(; dim > Chunk::log_elements_in_chunk+1 ; --dim){
				l=dim-Chunk::log_elements_in_chunk;
				s=1<<(l-2);
				l=((chunkIdx>>(l-2))<<l)+(chunkIdx&andMask(l-2));
				d_a[l+(2*s)].v[in_chunk_idx]^=d_a[l+(3*s)].v[in_chunk_idx];
				d_a[l+(s)].v[in_chunk_idx]^=d_a[l+(2*s)].v[in_chunk_idx];
			__syncthreads();
		}
		/*
		 * Subpolynomial of size 2 chunks
		 */
		s=Chunk::cells_in_chunk>>1;
		l=chunkIdx*2;
		for(unsigned int i = 0 ; i < 2 ; ++i){
			d_a[l+1+i*half_d_a_len].v[in_chunk_idx]^=
					(d_a[l+1+i*half_d_a_len].v[in_chunk_idx]>>(Chunk::elements_in_chunk>>1));
			d_a[l+i*half_d_a_len].v[in_chunk_idx]^=
					(d_a[l+1+i*half_d_a_len].v[in_chunk_idx]<<(Chunk::elements_in_chunk>>1));
		}
		--dim;
		__syncthreads();
	}
	unsigned int i = dim;
	/*
	 * Code duplicated to prevent additional register usage for loop counter, assuming that:
	 * 		Chunk::log_cells_in_chunk = Chunk::log_elements_in_chunk+1.
	 */
	for(unsigned int j = 0 ; j < 4 ; ++j ) {
		chunk_cell_t cell = d_a[chunkIdx+j*(half_d_a_len>>1)].v[in_chunk_idx];
		for(; dim >=2 ; --dim){
			cell ^= (cell &
					(d_chunk_cell_mask[dim]<<((1<<dim)-(1<<(dim-2)))))>>(1<<(dim-2));
			cell ^= (cell &
					(d_chunk_cell_mask[dim]<<(1<<(dim-1))))>>(1<<(dim-2));
		}
		d_a[chunkIdx+j*(half_d_a_len>>1)].v[in_chunk_idx] = cell ;
		dim = i;
	}
	__syncthreads();
}
/*
 * This is the partition operation
 * 1)d_a - the shmem ptr for the polynomial.
 * 2) dim - the dimension of partition.
 * 3) idx - the thread index.
 * 4) chunk_idx - (idx >> Chunk::log_elements_in_chunk).
 * 5) in_chunk_idx - (idx % Chunk::elements_in_chunk).
 * 6) d_a_len - log  length of subpoly in chunks.
 */
__device__ void InTB_partition(	Chunk* const d_a,
								const len_t dim,
								const idx_t idx,
								const idx_t chunk_idx,
								const idx_t in_chunk_idx,
								const len_t d_a_len,
								const len_t half_d_a_len){
	/*
	 * Assumes:
	 *     Chunk::log_cells_in_chunk = 1+ Chunk::log_elements_in_chunk
	 */
	idx_t i=2;
	chunk_cell_t ans[2];
	for(unsigned int j = 0 ; j < 2 ; ++j){
		ans[0]= d_a[chunk_idx + j*half_d_a_len].v[in_chunk_idx];
		ans[1]=d_a[chunk_idx + j*half_d_a_len].v[in_chunk_idx + (Chunk::cells_in_chunk>>1)];
		for(i=2; i <= Chunk::log_elements_in_chunk && i<=dim ; ++i){
			ans[0] = 	(ans[0] & d_chunk_cell_mask[i]) |
					(ans[0] & (d_chunk_cell_mask[i]<<((1<<(i))-(1<<(i-2))))) |
					(ans[0] & (d_chunk_cell_mask[i]<<(1<<(i-1))))>>(1<<(i-2)) |
					(ans[0] & (d_chunk_cell_mask[i]<<(1<<(i-2))))<<(1<<(i-2));
			ans[1] = 	(ans[1] & d_chunk_cell_mask[i]) |
					(ans[1] & (d_chunk_cell_mask[i]<<((1<<(i))-(1<<(i-2))))) |
					(ans[1] & (d_chunk_cell_mask[i]<<(1<<(i-1))))>>(1<<(i-2)) |
					(ans[1] & (d_chunk_cell_mask[i]<<(1<<(i-2))))<<(1<<(i-2));
		}
		d_a[chunk_idx + j*half_d_a_len].v[in_chunk_idx]=ans[0];
		d_a[chunk_idx + j*half_d_a_len].v[in_chunk_idx+(Chunk::cells_in_chunk>>1)] = ans[1];
	}
	__syncthreads();

	if(dim<=Chunk::log_elements_in_chunk){
		return;
	}
	idx_t s = (idx& andMask(log_max_block_size))>> Chunk::log_cells_in_chunk;
	s<<=1;
	idx_t t = idx & andMask(Chunk::log_cells_in_chunk);

	for(unsigned int j = 0 ; j < 2 ; ++j){
		ans[0]=d_a[s + j*half_d_a_len].v[t];
		ans[1]=d_a[s+1 + j*half_d_a_len].v[t];
		d_a[s+j*half_d_a_len].v[t] = (ans[0] & d_chunk_cell_mask[Chunk::log_elements_in_chunk+1]) | ((ans[1] & d_chunk_cell_mask[Chunk::log_elements_in_chunk+1])<<(Chunk::elements_in_chunk>>1));
		d_a[s+1 + j*half_d_a_len].v[t] = (ans[1] & (d_chunk_cell_mask[Chunk::log_elements_in_chunk+1]<< (Chunk::elements_in_chunk>>1)) )|
				((ans[0] >> (Chunk::elements_in_chunk>>1)) & d_chunk_cell_mask[Chunk::log_elements_in_chunk+1]);
	}
	__syncthreads();

	if(dim<=Chunk::log_elements_in_chunk+1){
		return;
	}
	ans[0] = d_a[chunk_idx].v[in_chunk_idx];
	ans[1] = d_a[chunk_idx].v[in_chunk_idx+Chunk::elements_in_chunk];
	chunk_cell_t ans_u[2];
	ans_u[0] = d_a[chunk_idx+half_d_a_len].v[in_chunk_idx];
	ans_u[1] = d_a[chunk_idx+ half_d_a_len].v[in_chunk_idx+Chunk::elements_in_chunk];
	__syncthreads();
	s = (chunk_idx >> d_a_len)<<(d_a_len);
	t = chunk_idx & andMask(d_a_len);
	if(t & 1){
		d_a[s+(t>>1) + (1<<(d_a_len-1))].v[in_chunk_idx] = ans[0];
		d_a[s+(t>>1) + (1<<(d_a_len-1))].v[in_chunk_idx+Chunk::elements_in_chunk] = ans[1];
	} else {
		d_a[s+(t>>1)].v[in_chunk_idx] = ans[0];
		d_a[s+(t>>1)].v[in_chunk_idx+Chunk::elements_in_chunk] = ans[1];
	}

	s=((chunk_idx+half_d_a_len) >> d_a_len)<<d_a_len;
	t=(chunk_idx+half_d_a_len)&andMask(d_a_len);
	if(t & 1){
		d_a[s+(t>>1) + (1<<(d_a_len-1))].v[in_chunk_idx] = ans_u[0];
		d_a[s+(t>>1) + (1<<(d_a_len-1))].v[in_chunk_idx+Chunk::elements_in_chunk] = ans_u[1];
	} else {
		d_a[s+(t>>1)].v[in_chunk_idx] = ans_u[0];
		d_a[s+(t>>1)].v[in_chunk_idx+Chunk::elements_in_chunk] = ans_u[1];
	}
	__syncthreads();

}

/*
 * This is the linear evaluation phase
 */
__device__ void InTB_LinearEvaluation(	Chunk* const d_a,
										const idx_t idx ,
										const idx_t chunkIdx ,
										const idx_t in_chunk_idx,
										const len_t half_d_a_len){
	chunk_cell_t load[4];
	load[0] = d_a[chunkIdx].v[in_chunk_idx];
	load[1] = d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk];
	load[2] = d_a[half_d_a_len+chunkIdx].v[in_chunk_idx];
	load[3] = d_a[half_d_a_len+chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk];

	DUP_ODD_BITS(load[0]);
	DUP_ODD_BITS(load[1]);

	DUP_EVEN_BITS(d_a[chunkIdx].v[in_chunk_idx]);
	DUP_EVEN_BITS(d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk]);
	__syncthreads();
	InTB_preMulExpand(d_a,d_linear_mul,1,idx,chunkIdx,in_chunk_idx);
	InTB_Mult(d_a+(chunkIdx<<1),in_chunk_idx);
	InTB_postMulShrink(d_a,idx,chunkIdx,in_chunk_idx);

	d_a[chunkIdx].v[in_chunk_idx] ^= load[0];
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk]  ^= load[1];
	__syncthreads();

	load[0]=load[2];
	load[2]=d_a[chunkIdx].v[in_chunk_idx];
	d_a[chunkIdx].v[in_chunk_idx] = load[0];

	load[1]=load[3];
	load[3]=d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk];
	d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk] = load[1];

	DUP_ODD_BITS(load[0]);
	DUP_ODD_BITS(load[1]);

	DUP_EVEN_BITS(d_a[chunkIdx].v[in_chunk_idx]);
	DUP_EVEN_BITS(d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk]);
	__syncthreads();
	InTB_preMulExpand(d_a,d_linear_mul,1,idx,chunkIdx,in_chunk_idx);
	InTB_Mult(d_a+(chunkIdx<<1),in_chunk_idx);
	InTB_postMulShrink(d_a,idx,chunkIdx,in_chunk_idx);

	d_a[chunkIdx].v[in_chunk_idx] ^= load[0];
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk]  ^= load[1];
	__syncthreads();

	d_a[chunkIdx + half_d_a_len].v[in_chunk_idx] = d_a[chunkIdx].v[in_chunk_idx];
	d_a[chunkIdx + half_d_a_len].v[in_chunk_idx + Chunk::elements_in_chunk] =
			d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk];

	d_a[chunkIdx].v[in_chunk_idx]=load[2];
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk]=load[3];

	__syncthreads();
}

/*
 * This is the WFromUV operation in chunk (i.e. dim<5)
 * dim = dim of subspace.
 */
__device__ void InTB_WFromUV_inChunk(	Chunk* const d_a,
										const Chunk * const subspace,
										const len_t dim,
										const idx_t idx,
										const idx_t chunkIdx,
										const idx_t in_chunk_idx,
										const len_t half_d_a_len){
	/*
	 * 1)Backup the WHOLE cell.
	 * 2)Multiply by subspace chunk (will nullify lower top).
	 * 3)Xor by itself when shifting right subspace-times.
	 * 4)Xor the load.
	 */
	chunk_cell_t load[4];
	/*
	 * Just backing up upper half
	 */
	load[2] = d_a[chunkIdx + half_d_a_len].v[in_chunk_idx];
	load[3] = d_a[chunkIdx + half_d_a_len].v[in_chunk_idx + Chunk::elements_in_chunk];

	load[0] = d_a[chunkIdx].v[in_chunk_idx] ;
	load[1] = d_a[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk] ;
	load[0] ^= ((load[0]&(d_alter_mask[dim+1]))<<(1<<dim));
	load[1] ^= ((load[1]&(d_alter_mask[dim+1]))<<(1<<dim));

	__syncthreads();

	InTB_preMulExpand(d_a,subspace,1,idx,chunkIdx,in_chunk_idx);
	InTB_Mult(d_a+(chunkIdx<<1),in_chunk_idx);
	InTB_postMulShrink(d_a,idx,chunkIdx,in_chunk_idx);
	d_a[chunkIdx].v[in_chunk_idx] ^= (load[0]^(d_a[chunkIdx].v[in_chunk_idx]>>(1<<dim)));
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk] ^= (load[1]^(d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk]>>(1<<dim)));

	__syncthreads();

	/*
	 * Switching from calculating lower half , into upper half.
	 */
	load[0] = load[2];
	load[2] = d_a[chunkIdx].v[in_chunk_idx];
	d_a[chunkIdx].v[in_chunk_idx] = load[0];

	load[1] = load[3];
	load[3] = d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk];
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk] = load[1];

	load[0] ^= ((load[0]&(d_alter_mask[dim+1]))<<(1<<dim));
	load[1] ^= ((load[1]&(d_alter_mask[dim+1]))<<(1<<dim));

	__syncthreads();

	InTB_preMulExpand(d_a,subspace,1,idx,chunkIdx,in_chunk_idx);
	InTB_Mult(d_a+(chunkIdx<<1),in_chunk_idx);
	InTB_postMulShrink(d_a,idx,chunkIdx,in_chunk_idx);
	d_a[chunkIdx].v[in_chunk_idx] ^= (load[0]^(d_a[chunkIdx].v[in_chunk_idx]>>(1<<dim)));
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk] ^= (load[1]^(d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk]>>(1<<dim)));

	__syncthreads();

	d_a[chunkIdx + half_d_a_len].v[in_chunk_idx] = d_a[chunkIdx].v[in_chunk_idx];
	d_a[chunkIdx + half_d_a_len].v[in_chunk_idx + Chunk::elements_in_chunk] =
			d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk];

	d_a[chunkIdx].v[in_chunk_idx] = load[2];
	d_a[chunkIdx].v[in_chunk_idx + Chunk::elements_in_chunk] = load[3];

	__syncthreads();
}
/*
 * This is when the SUBSPACE dim it >=5.
 * 1) d_a - shmem ptr.
 * 2) subspace - global mem ptr of length (1<<(dim-1-Chunk::log_elements_in_chunk)) to a subspace of dim (dim-1).
 * 3) dim - dimension of subspace.
 * 4) idx - threadIdx.
 */
__device__ void InTB_WFromUV_outChunk(	Chunk* const d_a,
										const Chunk* const subspace,
										const len_t dim,
										const idx_t idx,
										const len_t half_d_a_len){
	/*
	 * 1) Backup everything on register.
	 * 2) Multiply by subspace only bottom halves of the sub-polynomials.
	 */
	idx_t chunkIdx64[2];
	idx_t original_idx[2];
	original_idx[0] = ((idx& andMask(log_max_block_size)) >> Chunk::log_cells_in_chunk);
	original_idx[1] = original_idx[0]+(max_block_size>>Chunk::log_cells_in_chunk);
	chunkIdx64[0] = (((original_idx[0])>>(dim-Chunk::log_elements_in_chunk))<<(dim+1-Chunk::log_elements_in_chunk)) +
			(original_idx[0] & andMask(dim-Chunk::log_elements_in_chunk));
	chunkIdx64[1] = (((original_idx[1])>>(dim-Chunk::log_elements_in_chunk))<<(dim+1-Chunk::log_elements_in_chunk)) +
			(original_idx[1] & andMask(dim-Chunk::log_elements_in_chunk));
	idx_t in_chunk_idx64 = (idx) & andMask(Chunk::log_cells_in_chunk);
	chunk_cell_t load[4];
	//Backing up everything on local memory.
	load[0]= d_a[chunkIdx64[0]].v[in_chunk_idx64];
	load[1]= d_a[chunkIdx64[0] + (1<<(dim-Chunk::log_elements_in_chunk))].v[in_chunk_idx64];
	load[2] = d_a[chunkIdx64[1]].v[in_chunk_idx64];
	load[3] = d_a[chunkIdx64[1]+ (1<<(dim-Chunk::log_elements_in_chunk))].v[in_chunk_idx64];

	__syncthreads();

	//Preparing data for multiplication.
	d_a[(original_idx[0])<<1].v[in_chunk_idx64]=load[1];
	d_a[(original_idx[1])<<1].v[in_chunk_idx64]=load[3];

	//If my chunk should be multiplied, I load the relevant subspace chunk to the next chunk, so they will be multiplied.
	//My chunk should be multiplied if my chunkIdx/(sizeOfSubspace in Chunk) is odd.
	//chunkIdx is the same for all threads in the warp - so no divergence is possible.
	d_a[(original_idx[0]<<1)+1].v[in_chunk_idx64]=
		subspace[original_idx[0] & andMask(dim-Chunk::log_elements_in_chunk)].v[in_chunk_idx64];
	d_a[(original_idx[1]<<1)+1].v[in_chunk_idx64]=
		subspace[original_idx[1] & andMask(dim-Chunk::log_elements_in_chunk)].v[in_chunk_idx64];
	__syncthreads();
	InTB_Mult(d_a+(((idx&andMask(log_max_block_size))>>Chunk::log_threads_in_chunk)<<1),idx&andMask(Chunk::log_threads_in_chunk));

	chunk_cell_t t[2];
	t[0]= d_a[original_idx[0]<<1].v[in_chunk_idx64];
	t[1]= d_a[original_idx[1]<<1].v[in_chunk_idx64];
	__syncthreads();
	load[1]^=load[0];
	load[3]^=load[2];
	d_a[chunkIdx64[0]+(1<<(dim-Chunk::log_elements_in_chunk))].v[in_chunk_idx64] = t[0] ^ load[1];
	d_a[chunkIdx64[0]].v[in_chunk_idx64] = t[0] ^ load[0];
	d_a[chunkIdx64[1]+(1<<(dim-Chunk::log_elements_in_chunk))].v[in_chunk_idx64] = t[1] ^ load[3];
	d_a[chunkIdx64[1]].v[in_chunk_idx64] = t[1] ^ load[2];
	__syncthreads();
}

__global__ void
//__launch_bounds__(1<<log_max_block_size)
k_gpuFFT_InTB(Chunk* const d_a, Chunk** subspaces, Chunk** exps, len_t dim, const len_t poly_len){
	const len_t half_d_a_len = (max_block_size>>Chunk::log_threads_in_chunk);
	__shared__ Chunk s[half_d_a_len<<1];
	const idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	const idx_t poly_idx = (idx >> log_max_block_size)<<(log_max_block_size+1-Chunk::log_elements_in_chunk);
	const idx_t chunkIdx = (idx  & andMask(log_max_block_size))>> Chunk::log_elements_in_chunk;
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_elements_in_chunk);
	if(poly_idx+chunkIdx < poly_len){
		s[chunkIdx].v[in_chunk_idx] = d_a[(poly_idx) + chunkIdx].v[in_chunk_idx];
		s[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk] = d_a[(poly_idx) + chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk];
	}
	if(poly_idx+chunkIdx+half_d_a_len < poly_len){
		s[chunkIdx+half_d_a_len].v[in_chunk_idx] = d_a[(poly_idx) + chunkIdx + half_d_a_len].v[in_chunk_idx];
		s[chunkIdx+half_d_a_len].v[in_chunk_idx+Chunk::elements_in_chunk] =
				d_a[(poly_idx) + chunkIdx + half_d_a_len].v[in_chunk_idx+Chunk::elements_in_chunk];
	}

	__syncthreads();
	idx_t i = dim;
	for(; i > Chunk::log_elements_in_chunk ; --i){
		InTB_MultiExp(s,exps[i],1<<(i-Chunk::log_elements_in_chunk),idx,chunkIdx,in_chunk_idx,half_d_a_len);
		//The problem is in TaylorExp - s is nullified!
		InTB_TaylorExp(s,i,idx,idx & andMask(Chunk::log_cells_in_chunk),(idx&andMask(log_max_block_size)) >> Chunk::log_cells_in_chunk,half_d_a_len);
		InTB_partition(s,i,idx,chunkIdx,in_chunk_idx,(i-Chunk::log_elements_in_chunk),half_d_a_len);
	}
	for(; i >= 2; --i){
		InTB_MultiExp(s,exps[i],1,idx,chunkIdx,in_chunk_idx,half_d_a_len);
		InTB_TaylorExp(s,i,idx,idx & andMask(Chunk::log_cells_in_chunk),(idx&andMask(log_max_block_size)) >> Chunk::log_cells_in_chunk,half_d_a_len);
		InTB_partition(s,i,idx,chunkIdx,in_chunk_idx,0,half_d_a_len);
	}
	InTB_LinearEvaluation(s,idx,chunkIdx,in_chunk_idx,half_d_a_len);
	for(; i < Chunk::log_elements_in_chunk && i<dim ; ++i){
		InTB_WFromUV_inChunk(s,subspaces[i],i,idx,chunkIdx,in_chunk_idx,half_d_a_len);
	}
	for( ; i < dim ; ++i){
		InTB_WFromUV_outChunk(s,subspaces[i],i,idx,half_d_a_len);
	}
	if(poly_idx+chunkIdx < poly_len){
		d_a[(poly_idx) + chunkIdx].v[in_chunk_idx] = s[chunkIdx].v[in_chunk_idx];
		d_a[(poly_idx) + chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk] = s[chunkIdx].v[in_chunk_idx+Chunk::elements_in_chunk];
	}
	if(poly_idx+chunkIdx+half_d_a_len < poly_len){
		d_a[(poly_idx) + chunkIdx + half_d_a_len].v[in_chunk_idx] = s[chunkIdx+half_d_a_len].v[in_chunk_idx];
		d_a[(poly_idx) + chunkIdx + half_d_a_len].v[in_chunk_idx+Chunk::elements_in_chunk] =
				s[chunkIdx+half_d_a_len].v[in_chunk_idx+Chunk::elements_in_chunk];
	}
}
 void GPU_FFT::fft_gpu_InTB(len_t dim, const FFT* const fft, Chunk* const p, len_t p_len){
	 len_t threads = p_len<<Chunk::log_elements_in_chunk;
	 dim3 block(max_block_size,1,1);
	 dim3 grid(sizeCiel(threads,max_block_size),1,1);
	 Chunk* d_subspaces[log_max_block_size+2];
	 Chunk* d_exps[log_max_block_size+2];
	 Chunk** d_subspaces_allocated;
	 Chunk** d_exps_allocated;

	 cudaMalloc(&d_subspaces_allocated,sizeof(Chunk*)*(log_max_block_size+2));
	 cudaMalloc(&d_exps_allocated,sizeof(Chunk*)*(log_max_block_size+2));

	 len_t fftSize = fft->basis.getSize();
	 for(unsigned int i = 2 ; i <= dim && i<=Chunk::log_elements_in_chunk ; ++i){
		 cudaMalloc(&(d_subspaces[i-1]),sizeof(Chunk));
		 cudaMalloc(&(d_exps[i]),sizeof(Chunk));
		 cudaMemcpy(d_subspaces[i-1],fft->gpu_subspace[fftSize-i],sizeof(Chunk),cudaMemcpyHostToDevice);
		 cudaMemcpy(d_exps[i],fft->gpu_exp[fftSize-i],sizeof(Chunk),cudaMemcpyHostToDevice);
	 }
	 for(unsigned int i = Chunk::log_elements_in_chunk+1 ; i <= dim ; ++i){
		 cudaMalloc(&(d_subspaces[i-1]),sizeof(Chunk)*(1<<(i-1-Chunk::log_elements_in_chunk)));
		 cudaMalloc(&(d_exps[i]),sizeof(Chunk)*(1<<(i-Chunk::log_elements_in_chunk)));
		 cudaMemcpy(d_subspaces[i-1],fft->gpu_subspace[fftSize-i],sizeof(Chunk)*(1<<(i-1-Chunk::log_elements_in_chunk)),cudaMemcpyHostToDevice);
		 cudaMemcpy(d_exps[i],fft->gpu_exp[fftSize-i],sizeof(Chunk)*(1<<(i-Chunk::log_elements_in_chunk)),cudaMemcpyHostToDevice);
	 }

	 cudaMemcpy(d_subspaces_allocated,d_subspaces,sizeof(Chunk*)*(log_max_block_size+2),cudaMemcpyHostToDevice);
	 cudaMemcpy(d_exps_allocated,d_exps,sizeof(Chunk*)*(log_max_block_size+2),cudaMemcpyHostToDevice);

	 k_gpuFFT_InTB<<<grid,block>>>(p,d_subspaces_allocated,d_exps_allocated,dim,p_len);
	 for(unsigned int i = 2 ; i<= dim ; ++i){
		 cudaFree(d_subspaces[i-1]);
		 cudaFree(d_exps[i]);
	 }

	 cudaFree(d_subspaces_allocated);
	 cudaFree(d_exps_allocated);
 }
 void GPU_FFT::fft_gpu(const FFT* const fft,Polynomial* P){
	 	 Chunk::setMod();
		setUpConstantMemory(fft);
		Element* normalized_P = *P;
		len_t p_len = 1<<fft->basis.getSize();
		len_t fixed_len = MAX((p_len),warp_size);
		if(p_len < fixed_len) {
			normalized_P = (Element*)malloc(sizeof(Element)*fixed_len);
			memset(normalized_P,0,sizeof(Element)*fixed_len);
			memcpy(normalized_P,*P,sizeof(Element)*p_len);
		}
		fixed_len >>= Chunk::log_elements_in_chunk;
		p_len = fixed_len;
		Elements_Chunk* d_p;
		Chunk* d_chunk_P;
		Chunk* d_chunk_P_cpy;
		Chunk* d_swap;
		cudaMalloc(&d_p,sizeof(Elements_Chunk)*(fixed_len));
		cudaMalloc(&d_chunk_P,sizeof(Chunk)*(fixed_len));
		cudaMemcpy(d_p,normalized_P,sizeof(Elements_Chunk)*(fixed_len),cudaMemcpyHostToDevice);
		Chunk::normalToChunk((Elements_Chunk*)d_p,d_chunk_P,fixed_len,false);
		cudaFree(d_p);
		cudaMalloc(&d_chunk_P_cpy,sizeof(Chunk)*(fixed_len));
		len_t dim = fft->basis.getSize();
#ifdef __MEASURE
#ifdef __GNUC__
		timespec start,end;
		clock_gettime(CLOCK_REALTIME,&start);
#endif	//#ifdef __GNUC__
#endif	//#ifdef __MEASURE
		/*
		 * The Algorithm
		 */
		unsigned int i = dim;
		for(; i > 1 ; --i){
			multiExponentiate_gpu(fft,d_chunk_P,p_len,i,d_chunk_P_cpy);
			taylorExpansion_gpu(fft,d_chunk_P,p_len,i);
			if(partition(d_chunk_P,d_chunk_P_cpy,p_len,i)){
				d_swap = d_chunk_P;
				d_chunk_P=d_chunk_P_cpy;
				d_chunk_P_cpy = d_swap;
			}
		}
//		fft_gpu_InTB(i,fft,d_chunk_P,p_len);
		linearEvaluation(d_chunk_P,d_chunk_P_cpy,p_len);
		for(; i < dim ; ++i ){
			cudaMemcpy(d_chunk_P_cpy,fft->gpu_subspace[dim-1-i],sizeof(Chunk)<<(MAX((int)i-(int)Chunk::log_elements_in_chunk,0)),cudaMemcpyHostToDevice);
			WFromUV(d_chunk_P,p_len,d_chunk_P_cpy,i);
		}
		/*
		 * End of algorithm
		 */
#ifdef __MEASURE
#ifdef __GNUC__
		clock_gettime(CLOCK_REALTIME,&end);
		double elapsed_secs = end.tv_sec - start.tv_sec
				+ ((double) (end.tv_nsec - start.tv_nsec)) / ((double) 1000000000);
		std::cout << elapsed_secs << "GpU!!" <<std::endl;
#endif	//#ifdef __GNUC__
#endif	//#ifdef __MEASURE

		cudaFree(d_chunk_P_cpy);
		cudaMalloc(&d_p,sizeof(Elements_Chunk)*(fixed_len));
		Chunk::chunkToNormal(d_chunk_P,(Elements_Chunk*)d_p,fixed_len,false);
		cudaMemcpy(normalized_P,d_p,sizeof(Elements_Chunk)*(fixed_len),cudaMemcpyDeviceToHost);
		cudaFree(d_chunk_P);
		cudaFree(d_p);
		if(normalized_P != *P){
			free(*P);
			*P=normalized_P;
		}
	 }
/*
 * This is the inverse FFT implementation
 */

 void GPU_FFT::ifft_gpu(const FFT* const fft,Polynomial* P){
	 	 Chunk::setMod();
		setUpConstantMemory(fft);
		Element* normalized_P = *P;
		len_t p_len = 1<<fft->basis.getSize();
		len_t fixed_len = MAX((p_len),warp_size);
		if(p_len < fixed_len) {
			normalized_P = (Element*)malloc(sizeof(Element)*fixed_len);
			memset(normalized_P,0,sizeof(Element)*fixed_len);
			memcpy(normalized_P,*P,sizeof(Element)*p_len);
		}
		fixed_len >>= Chunk::log_elements_in_chunk;
		p_len = fixed_len;
		Elements_Chunk* d_p;
		Chunk* d_chunk_P;
		Chunk* d_chunk_P_cpy;
		Chunk* d_swap;
		cudaMalloc(&d_p,sizeof(Elements_Chunk)*(fixed_len));
		cudaMalloc(&d_chunk_P,sizeof(Chunk)*(fixed_len));
		cudaMemcpy(d_p,normalized_P,sizeof(Elements_Chunk)*(fixed_len),cudaMemcpyHostToDevice);
		Chunk::normalToChunk((Elements_Chunk*)d_p,d_chunk_P,fixed_len,false);
		cudaFree(d_p);
		cudaMalloc(&d_chunk_P_cpy,sizeof(Chunk)*(fixed_len));
		len_t dim = fft->basis.getSize();

#ifdef __MEASURE
#ifdef __GNUC__
		timespec start,end;
		clock_gettime(CLOCK_REALTIME,&start);
#endif	//#ifdef __GNUC__
#endif  //#ifdef __MEASURE
		/*
		 * The Algorithm
		 */
		unsigned int i = dim-1;
		for(; i >= 1 ; --i ){
			cudaMemcpy(d_chunk_P_cpy,fft->gpu_subspace[dim-1-i],sizeof(Chunk)<<(MAX((int)i-(int)Chunk::log_elements_in_chunk,0)),cudaMemcpyHostToDevice);
			UVFromW(d_chunk_P,p_len,d_chunk_P_cpy,i);
		}
		ilinearEvaluation(d_chunk_P,d_chunk_P_cpy,p_len);
		for(i=2; i <= dim ; ++i){
			if(ipartition(d_chunk_P,d_chunk_P_cpy,p_len,i)){
				d_swap = d_chunk_P;
				d_chunk_P=d_chunk_P_cpy;
				d_chunk_P_cpy = d_swap;
			}
			itaylorExpansion_gpu(fft,d_chunk_P,p_len,i);
			imultiExponentiate_gpu(fft,d_chunk_P,p_len,i,d_chunk_P_cpy);
		}
		/*
		 * End of algorithm
		 */
#ifdef __MEASURE
#ifdef __GNUC__
		clock_gettime(CLOCK_REALTIME,&end);
		double elapsed_secs = end.tv_sec - start.tv_sec
				+ ((double) (end.tv_nsec - start.tv_nsec)) / ((double) 1000000000);
		std::cout << elapsed_secs << "GpU!!" <<std::endl;
#endif	//#ifdef __GNUC__
#endif  //#ifdef __MEASURE

		cudaFree(d_chunk_P_cpy);
		cudaMalloc(&d_p,sizeof(Elements_Chunk)*(fixed_len));
		Chunk::chunkToNormal(d_chunk_P,(Elements_Chunk*)d_p,fixed_len,false);
		cudaMemcpy(normalized_P,d_p,sizeof(Elements_Chunk)*(fixed_len),cudaMemcpyDeviceToHost);
		cudaFree(d_chunk_P);
		cudaFree(d_p);
		if(normalized_P != *P){
			free(*P);
			*P=normalized_P;
		}
	 }
/*
 * No need for multiexponents multiplication, it's the same as the FFT only with the multiexponents of another element.
 */
/*
 * inverse-Taylor expansion
 */

__device__ void itaylor_smaller_than_chunk(Chunk* chunk,idx_t in_chunk_idx, len_t t_dim, len_t p_len, idx_t idx){
		if(idx >= ((p_len) << (Chunk::log_cells_in_chunk)))
			return;
		//Performs the rest of the expansion.
		chunk_cell_t cell = chunk->v[in_chunk_idx];
		for(len_t i = 2; i<=t_dim && i <=Chunk::log_elements_in_chunk ; ++i){
			cell ^= (cell & (d_chunk_cell_mask[i]<<(1<<(i-1))))>>(1<<(i-2));
			cell ^= (cell & (d_chunk_cell_mask[i]<<((1<<i)-(1<<(i-2)))))>>(1<<(i-2));
		}
		chunk->v[in_chunk_idx]= cell;
}
__global__ void k_itaylorExpansion_iteration_large(Chunk* d_chunk_P , len_t p_len , len_t t_dim,len_t i){
	idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	idx_t in_chunk_idx = idx & (Chunk::cells_in_chunk-1);
	/*
	 * flag = true if current sub-polynomial fits in a thread block.
	 */
	bool flag = (i-Chunk::log_elements_in_chunk <= log_max_block_size-Chunk::log_cells_in_chunk + 2);
	if(idx >= ((p_len) << (Chunk::log_cells_in_chunk-2)))
		return;
	do {
		len_t sub_len = (1<<(i-Chunk::log_elements_in_chunk));
		idx_t chunk_idx = idx >> (Chunk::log_cells_in_chunk);
		chunk_idx = (chunk_idx /(sub_len>>2))*(sub_len) + ((chunk_idx) & ((sub_len>>2)-1));
		d_chunk_P[(sub_len>>2) + chunk_idx ].v[in_chunk_idx] ^= d_chunk_P[(sub_len>>1) + chunk_idx].v[in_chunk_idx];
		d_chunk_P[(sub_len>>1) + chunk_idx ].v[in_chunk_idx] ^= d_chunk_P[(3*(sub_len>>2)) + chunk_idx].v[in_chunk_idx];
		if(flag)
			__syncthreads();
		++i;
	} while (i-Chunk::log_elements_in_chunk <= log_max_block_size-Chunk::log_cells_in_chunk + 2 && i<=t_dim);
		//If number of threads needed is less than a thread block - we can just continue!
}
__global__ void k_itaylorExpansion_iteration_twoChunks(Chunk* d_chunk_P, len_t p_len, len_t t_dim){
	idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	idx_t in_chunk_idx = idx & (Chunk::cells_in_chunk-1);
	if(idx >= ((p_len) << (Chunk::log_cells_in_chunk-1)))
		return;
	idx_t chunk_idx= (idx >> (Chunk::log_cells_in_chunk) ) << 1;
	d_chunk_P[(chunk_idx)].v[in_chunk_idx]^= (d_chunk_P[(chunk_idx)+1].v[in_chunk_idx]<<(Chunk::elements_in_chunk>>1));
	d_chunk_P[(chunk_idx)+1].v[in_chunk_idx]^= (d_chunk_P[(chunk_idx)+1].v[in_chunk_idx]>>(Chunk::elements_in_chunk>>1));

}
__global__ void k_itaylorExpansion_iteration_singleChunk(Chunk* d_chunk_P, len_t p_len, len_t t_dim){

	idx_t idx = threadIdx.x + blockIdx.x*blockDim.x;
	idx_t in_chunk_idx = idx & (Chunk::cells_in_chunk-1);
	//Performs the rest of the expansion.
	idx_t chunk_idx = (idx >> (Chunk::log_cells_in_chunk));
	itaylor_smaller_than_chunk(d_chunk_P+chunk_idx,in_chunk_idx,t_dim,p_len,idx);
}
void itaylorExpansion_iteration(const FFT* fft, Chunk * d_chunk_P , len_t p_len , len_t t_dim,len_t i){
	unsigned int threads;
	dim3 blockSize(max_block_size,1,1);
	if( i >= Chunk::log_elements_in_chunk + 2){
		threads = p_len<<(Chunk::log_cells_in_chunk-2);
		dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
		k_itaylorExpansion_iteration_large<<<gridSize,blockSize>>>(d_chunk_P,p_len,t_dim,i);
	} else if (i == Chunk::log_elements_in_chunk + 1){
		threads = p_len <<(Chunk::log_cells_in_chunk-1);
		dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
		k_itaylorExpansion_iteration_twoChunks<<<gridSize,blockSize>>>(d_chunk_P,p_len,t_dim);
	} else {
		threads = p_len << Chunk::log_cells_in_chunk;
		dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
		k_itaylorExpansion_iteration_singleChunk<<<gridSize,blockSize>>>(d_chunk_P,p_len,t_dim);
	}
//	k_taylorExpansion_iteration<<<gridSize,blockSize>>>(d_chunk_P,p_len,t_dim);
}
void GPU_FFT::itaylorExpansion_gpu(const FFT* fft, Chunk* d_chunk_P, len_t p_len , len_t t_dim){
	/*
	 * The sub-polynomial requires more than a single thread block.
	 */
	itaylorExpansion_iteration(fft,d_chunk_P,p_len,t_dim,2);
	len_t i = Chunk::log_elements_in_chunk+1;
	if(t_dim>=i){
		itaylorExpansion_iteration(fft,d_chunk_P,p_len,i,i);
		++i;
	}
	if(t_dim>=i){
		itaylorExpansion_iteration(fft,d_chunk_P,p_len,t_dim,i);
		i = log_max_block_size+Chunk::log_elements_in_chunk+3-Chunk::log_cells_in_chunk;
	}

	while(t_dim>=i){
		itaylorExpansion_iteration(fft,d_chunk_P,p_len,t_dim,i);
		++i;
	}
}
/*
 * inverse-partition
 */

	/*
	 *  Input:
	 *  1) d_chunk_p - The polynomial on device's memory.
	 *  2) p_len - number of chunks in d_chunk_p.
	 *  3) chunk_idx - the chunk current thread has to deal with.
	 *  4) in_chunk_idx - the number of the cell the current thread deals with.
	 *  5) t_dim - The dim of the original (input) subpolynomial.
	 *
	 *  This function performs what a single thread does when performing the partition function, on a single chunk.
	 */
__device__ void ipartition_in_chunk(Chunk* d_chunk_p, len_t p_len , idx_t in_chunk_idx, len_t t_dim){
	chunk_cell_t ans = d_chunk_p->v[in_chunk_idx];
	for(unsigned int i = MIN(t_dim,Chunk::log_elements_in_chunk) ; i >= 2 ; --i){
		ans = 	(ans & d_chunk_cell_mask[i]) |
				(ans & (d_chunk_cell_mask[i]<<((1<<(i))-(1<<(i-2))))) |
				(ans & (d_chunk_cell_mask[i]<<(1<<(i-1))))>>(1<<(i-2)) |
				(ans & (d_chunk_cell_mask[i]<<(1<<(i-2))))<<(1<<(i-2));
	}
	d_chunk_p->v[in_chunk_idx]=ans;
}

//__device__ void ipartition_two_chunks(Chunk* d_chunk_p, idx_t in_chunk_idx){
//	chunk_cell_t ans[2];
//	chunk_cell_t load[2];
//	const chunk_cell_t mask = d_chunk_cell_mask[Chunk::log_elements_in_chunk+1];
//	load[0]=d_chunk_p->v[in_chunk_idx];
//	load[1]=d_chunk_p[1].v[in_chunk_idx];
//	ans[0]=(load[0] & mask) | ((load[1] & mask)<<(Chunk::elements_in_chunk>>1));
//	ans[1]=(load[1] & (mask<< (Chunk::elements_in_chunk>>1)) )|
//			((load[0] >> (Chunk::elements_in_chunk>>1)) & mask);
//	d_chunk_p->v[in_chunk_idx] = ans[0];
//	d_chunk_p[1].v[in_chunk_idx] = ans[1];
//}

__device__ void ipartition_general(Chunk* d_chunk_p_src, Chunk* d_chunk_p_dst,  idx_t chunk_idx, idx_t in_chunk_idx, idx_t t_dim){
	if(chunk_idx & 1)
		d_chunk_p_dst[chunk_idx].v[in_chunk_idx] = d_chunk_p_src[(chunk_idx>>1) + (1<<(t_dim-1))].v[in_chunk_idx];
	else
		d_chunk_p_dst[chunk_idx].v[in_chunk_idx] = d_chunk_p_src[chunk_idx>>1].v[in_chunk_idx];
}

__global__ void k_ipartition_iteration_two_chunks(Chunk* d_chunk_p_src, len_t p_len){
	idx_t idx = threadIdx.x + blockIdx.x * blockDim.x;
	idx_t chunk_idx = (idx >> Chunk::log_cells_in_chunk)<<1;
	idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
	if(chunk_idx >= p_len){
		return;
	}
	partition_two_chunks(d_chunk_p_src+chunk_idx, in_chunk_idx);
}
__global__ void k_ipartition_iteration_in_chunk(Chunk* d_chunk_p_src, len_t p_len, len_t t_dim){
	idx_t idx = threadIdx.x + blockIdx.x * blockDim.x;
	idx_t chunk_idx = idx >> Chunk::log_cells_in_chunk;
	idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
	if(chunk_idx >= p_len){
		return;
	}
	ipartition_in_chunk(d_chunk_p_src+chunk_idx,p_len,in_chunk_idx,t_dim);
}
__global__ void k_ipartition_iteration_general(Chunk* d_chunk_p_src, Chunk* d_chunk_p_dst, len_t p_len, len_t t_dim){
	idx_t idx = threadIdx.x + blockIdx.x * blockDim.x;
	idx_t chunk_idx = idx >> Chunk::log_cells_in_chunk;
	idx_t in_chunk_idx = idx & andMask(Chunk::log_cells_in_chunk);
	idx_t base = chunk_idx ^ (chunk_idx & andMask(t_dim-Chunk::log_elements_in_chunk));
	chunk_idx &= andMask(t_dim-Chunk::log_elements_in_chunk);
	if(base+chunk_idx >= p_len)
		return;
	ipartition_general(d_chunk_p_src+base,d_chunk_p_dst+base, chunk_idx, in_chunk_idx,t_dim-Chunk::log_elements_in_chunk );
}
/*
 * If t_dim > Chunk::log_elements_in_chunk+1 the result is written in dst, otherwise it will be written in src.
 */
bool GPU_FFT::ipartition(Chunk* d_chunk_p_src, Chunk* d_chunk_p_dst, len_t p_len, len_t t_dim){
	len_t threads = p_len << Chunk::log_cells_in_chunk;
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
	bool flag = false;
	if(t_dim > Chunk::log_elements_in_chunk+1){
		k_ipartition_iteration_general<<<gridSize,blockSize>>>(d_chunk_p_src,d_chunk_p_dst,p_len,t_dim);
		Chunk* swap;
		swap = d_chunk_p_src;
		d_chunk_p_src = d_chunk_p_dst;
		d_chunk_p_dst = swap;
		flag= true;
	}
	if(t_dim >= Chunk::log_elements_in_chunk +1){
      	dim3 gridSizeTwoChunks(sizeCiel(threads>>1,max_block_size),1,1);
		k_ipartition_iteration_two_chunks<<<gridSizeTwoChunks,blockSize>>>(d_chunk_p_src,p_len);
	}
	k_ipartition_iteration_in_chunk<<<gridSize,blockSize>>>(d_chunk_p_src,p_len,t_dim);
	return flag;
}
/*
 * inverse linear evaluation
 */
__global__ void k_icopy_and_shift_vec(Chunk* d_chunk_src, Chunk* d_chunk_dst, len_t p_len)
{
	const idx_t shared_len = max_block_size>>Chunk::log_threads_in_chunk;
	__shared__ Chunk c_shared[shared_len<<1];
	chunk_cell_t t;
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);

	if(chunk_idx  >= p_len){
		return;
	}

	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
	const idx_t shared_chunk_idx = ((idx & andMask(log_max_block_size)) >> (Chunk::log_threads_in_chunk));
	Chunk* my_shared_chunk = c_shared+(shared_chunk_idx<<1);

	for(unsigned int i =0 ; i < Chunk::ord ;i+=warp_size){
		my_shared_chunk->v[in_chunk_idx+i]=d_chunk_src[chunk_idx].v[in_chunk_idx+i];
		my_shared_chunk[1].v[in_chunk_idx+i]=d_ilinear_mul->v[in_chunk_idx+i];
	}

	chunk_cell_t tmp;
#pragma unroll
	for(unsigned int i = 0 ; i < Element::ord ; i+=warp_size){
		tmp = my_shared_chunk->v[in_chunk_idx+i];
		t = (tmp& 0x55555555) ^ ((tmp & 0xaaaaaaaa)>>1);
		my_shared_chunk->v[in_chunk_idx+i] = t^(t<<1);
		d_chunk_dst[chunk_idx].v[in_chunk_idx+i] = (tmp & 0x55555555) ;
	}
	Chunk::clmul_by_chunk(my_shared_chunk[0],my_shared_chunk[1],in_chunk_idx,my_shared_chunk);
	Chunk::chunk_reduce(d_chunk_src+chunk_idx,my_shared_chunk,in_chunk_idx);
	for(unsigned int i = 0 ; i < Element::ord ; i+=warp_size){
		d_chunk_src[chunk_idx].v[in_chunk_idx+i] ^= d_chunk_dst[chunk_idx].v[in_chunk_idx+i];
	}
	return;
}
void GPU_FFT::ilinearEvaluation(Chunk* d_chunk_p,Chunk* d_chunk_p_cpy, len_t p_len)
{
	len_t threads = p_len << Chunk::log_threads_in_chunk;
	dim3 blockSize(max_block_size,1,1);
	dim3 gridSize(sizeCiel(threads,max_block_size),1,1);
	k_icopy_and_shift_vec<<<gridSize,blockSize>>>(d_chunk_p,d_chunk_p_cpy,p_len);
}
/*
 * UVFromW
 */

__global__ void k_isubspaceMult_chunk_and_add(Chunk* d_a, Chunk* d_b , len_t log_elements_in_b ,len_t a_len ){
	idx_t idx = blockDim.x*blockIdx.x+threadIdx.x;
	const idx_t chunk_idx = (idx >> Chunk::log_threads_in_chunk);
	if(chunk_idx  >= a_len){
		return;
	}
	const idx_t in_chunk_idx = idx & andMask(Chunk::log_threads_in_chunk);
	Chunk::clmul_by_chunk_bShuffle_ixor_mask(d_a[chunk_idx],*d_b,in_chunk_idx,d_a[chunk_idx],1<<log_elements_in_b,log_elements_in_b);

}
/*
 * Dim = Size of U.
 */
void GPU_FFT::UVFromW(Chunk* d_chunk_p, len_t p_len, Chunk* subspace, len_t dim){
	if(dim>=Chunk::log_elements_in_chunk){
		len_t threadsMul = p_len << (Chunk::log_threads_in_chunk-1);
		len_t threadsAdd = p_len << (Chunk::log_cells_in_chunk-1);
		dim3 block(multThreadsInBlock,1,1);
		dim3 gridMul(sizeCiel(threadsMul,multThreadsInBlock),1,1);
		dim3 gridAdd(sizeCiel(threadsAdd,multThreadsInBlock),1,1);
		k_subspaceAdd_general<<<gridAdd,block>>>(d_chunk_p,p_len,1<<(dim-Chunk::log_elements_in_chunk));
		k_subspaceMult_general<<<gridMul,block>>>(d_chunk_p, subspace, (1<<(dim-Chunk::log_elements_in_chunk)),p_len);
	} else {
		len_t threadsMul = p_len << Chunk::log_threads_in_chunk;
		dim3 block(multThreadsInBlock,1,1);
		dim3 gridMul(sizeCiel(threadsMul,multThreadsInBlock),1,1);
		k_isubspaceMult_chunk_and_add<<<gridMul,block>>>(d_chunk_p,subspace, dim , p_len);
	}
}

}

#endif
