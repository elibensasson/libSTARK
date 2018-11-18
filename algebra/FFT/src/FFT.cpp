/*
 * FFT.cpp
 *
 *  Created on: Jul 2, 2014
 *      Author: matan
 */
#include "FFT.h"
#include "GPU_FFT.cuh"
#include <cstdlib>
#include <omp.h>
#include <cstring>
#include <iostream>
namespace FFF {
/*
 * Internal Function Definitions
 */
/*
 * This function generates basis G from basis B according to the Gao & Mateer's FFT algorithm.
 */
void GFromB(Element* G, Element* B, len_t g_l, Element& g_shift, Element& b_shift)
{
	Element::c_inv(B[g_l],B[g_l]);
	for(unsigned int i = 0 ; i < g_l ; ++i){
		Element::c_mul(B[i],B[g_l],G[i]);
	}
	Element::c_mul(b_shift,B[g_l],g_shift);
}
/*
 * This function generates basis D from basis G according to the Gao & Mateer's FFT algorithm.
 */
void DFromG(Element* D, Element* G, len_t d_l, Element& d_shift, Element& g_shift){
	for(unsigned int i = 0 ; i < d_l ; ++i){
		Element::c_sqr(G[i],D[i]);
		Element::c_add(G[i],D[i],D[i]);
	}
	Element::c_sqr(g_shift,d_shift);
	Element::c_add(g_shift,d_shift,d_shift);
}

void FFT::changeShift(const Element& newShift){
	Element* D = (Element*)malloc(sizeof(Element)*basis.getSize());
	Element* G = (Element*)malloc(sizeof(Element)*basis.getSize());
	Element g_shift;
	Element d_shift;
	int b_size = basis.getSize();
	d_shift = newShift;
	basis.getBasis(D);

	for(unsigned int j = b_size-1 ; j > 0 ; --j )
	{
		GFromB(G,D,j,g_shift,d_shift);
		updateShiftSubspaceElements_cpu(g_shift,j,subspaces[b_size-1-j]);
		DFromG(D,G,j,d_shift,g_shift);
	}
	this->lastD = D[0];
	Element::c_inv(lastD,i_lastD);
	this->lastShift = d_shift;
	free(D);
	free(G);

    basis.getShift(d_shift);
}

FFT::FFT(Basis& b, const fft_operation_t operation) :
		basis(b), bases(new Basis[b.getSize()-1]),
		bms((Element*)malloc(sizeof(Element)*(b.getSize()-1))),
		i_bms((Element*)malloc(sizeof(Element)*(b.getSize()-1))),
		exps((Element**)malloc(sizeof(Element*)*(b.getSize()-1))),
		subspaces((Element**)malloc(sizeof(Element*)*(b.getSize()-1)))
#ifdef __GPU
		,gpu_exp((Chunk**)malloc(sizeof(Chunk*)*(b.getSize()-1)))
		,gpu_i_exp((Chunk**)malloc(sizeof(Chunk*)*(b.getSize()-1)))
		,gpu_subspace((Chunk**)malloc(sizeof(Chunk*)*(b.getSize()-1)))
#endif	// #ifdef __GPU
{
	omp_set_num_threads(omp_max_threads);
	Element* D = (Element*)malloc(sizeof(Element)*b.getSize());
	Element* G = (Element*)malloc(sizeof(Element)*b.getSize());
#ifdef __GPU
	Elements_Chunk E;
#endif	// #ifdef __GPU
	Element g_shift;
	Element d_shift ;
	int b_size = b.getSize();
	for (int i = 0; i < b_size - 1; ++i){
		exps[i] = (Element*)malloc(sizeof(Element)*(1ULL << (b_size - i)));
		subspaces[i] = (Element*)malloc(sizeof(Element)*(1ULL << (b_size - i - 1)));
#ifdef __GPU
		gpu_exp[i] = (Chunk*)malloc(sizeof(Chunk)*(1UL<<(MAX((int)b_size-i-(int)Chunk::log_elements_in_chunk,0))));
		gpu_i_exp[i] = (Chunk*)malloc(sizeof(Chunk)*(1UL<<(MAX((int)b_size-i-(int)Chunk::log_elements_in_chunk,0))));
		gpu_subspace[i] = (Chunk*)malloc(sizeof(Chunk)*(1UL<<(MAX((int)b_size-i-1-(int)Chunk::log_elements_in_chunk,0))));
#endif	// #ifdef __GPU
	}
	b.getShift(d_shift);
	b.getBasis(D);

	for(unsigned int j = b_size-1 ; j > 0 ; --j )
	{
		bms[b_size-1-j]=D[j];
		Element::c_inv(D[j],i_bms[b_size-1-j]);
		if(operation == FFT_OP){
            multiExponentiate_cpu(bms[b_size-1-j],1UL<<(j+1),exps[b_size-1-j]);
        }
        else{ //operation == IFFT_OP
            multiExponentiate_cpu(i_bms[b_size-1-j],1UL<<(j+1),exps[b_size-1-j]);
        }
#ifdef __GPU
		if(j+1 <= Chunk::log_elements_in_chunk){
			for(size_t i = 0 ; i < (1UL<<(Chunk::log_elements_in_chunk-(j+1))); ++i){
				memcpy(&(E.e[i<<(j+1)]),exps[b_size-j-1],sizeof(Element)*(1UL<<(j+1)));
			}
			Chunk::normalToChunk(&E,gpu_exp[b_size-j-1],1,true);
		} else {
			Chunk::normalToChunk((Elements_Chunk*)(exps[b_size-1-j]),gpu_exp[b_size-j-1],1UL<<(j+1-Chunk::log_elements_in_chunk),true);
		}
#endif	// #ifdef __GPU
#ifdef __GPU
		if(j+1 <= Chunk::log_elements_in_chunk){
			for(size_t i = 0 ; i < (1UL<<(Chunk::log_elements_in_chunk-(j+1))); ++i){
				memcpy(&(E.e[i<<(j+1)]),i_exps[b_size-j-1],sizeof(Element)*(1UL<<(j+1)));
			}
			Chunk::normalToChunk(&E,gpu_i_exp[b_size-j-1],1,true);
		} else {
			Chunk::normalToChunk((Elements_Chunk*)i_exps[b_size-1-j],gpu_i_exp[b_size-j-1],1UL<<(j+1-Chunk::log_elements_in_chunk),true);
		}
#endif	// #ifdef __GPU
		GFromB(G,D,j,g_shift,d_shift);
		bases[b_size-1-j]=Basis(G,j,g_shift);
		generateSubspaceElements_cpu(G,g_shift,j,subspaces[b_size-1-j]);
#ifdef __GPU
		if(j>=Chunk::log_elements_in_chunk)
			Chunk::normalToChunk((Elements_Chunk*)subspaces[b_size-1-j],gpu_subspace[b_size-1-j],1UL<<(j-Chunk::log_elements_in_chunk),true);
		else
		{
			for(size_t i =0  ; i < 1UL<<(j) ; ++i)
				for(size_t k = 0 ; k < (Chunk::elements_in_chunk>>(j));++k){
					if(!(k&1))
						Element::c_setZero(E.e[(k<<(j))+i]);
					else
					    E.e[(k<<(j))+i] = subspaces[b_size-1-j][i];
				}

			Chunk::normalToChunk(&E,gpu_subspace[b_size-1-j],1,true);
		}
#endif	// #ifdef __GPU
		DFromG(D,G,j,d_shift,g_shift);
	}
	this->lastD = D[0];
	Element::c_inv(lastD,i_lastD);
	this->lastShift = d_shift;
#ifdef __GPU
	for(size_t i = 0 ; i < (Chunk::elements_in_chunk) ; i+=2){
		E.e[i] = lastShift;
		Element::c_add(lastShift,lastD,E.e[i+1]);
	}
	Chunk::normalToChunk(&E,&linear_mul,1,true);
	E.e[1]=i_lastD;
	Element::c_mul(E.e[1],lastShift,E.e[0]);
	for(size_t i = 2 ; i < (Chunk::elements_in_chunk) ; i+=2){
		E.e[i]=E.e[0];
		E.e[i+1]=E.e[1];
	}
	Chunk::normalToChunk(&E,&ilinear_mul,1,true);
#endif	// #ifdef __GPU
	free(D);
	free(G);
}

FFT::~FFT() {
	delete[] bases;
	free(bms);
	free(i_bms);
	for(unsigned int i = 0 ; i < this->basis.getSize()-1 ; ++i){
		free(exps[i]);
		free(subspaces[i]);
#ifdef __GPU
		free(gpu_exp[i]);
		free(gpu_i_exp[i]);
		free(gpu_subspace[i]);
#endif	// #ifdef __GPU
	}
#ifdef __GPU
	free(gpu_exp);
	free(gpu_i_exp);
	free(gpu_subspace);
#endif	// #ifdef __GPU
	free(exps);
	free(subspaces);
}

	/*
	 * Non-allocating version of the FFT Algorithm. (Ariel:does allocate if basis size larger than p_len)
	 *
	 */
void FFT::AlgFFT(Polynomial* P, len_t p_len)const{
		Element* c = *P;
		len_t eval_len = 1UL<<this->basis.getSize();
		if(p_len > eval_len)
			return;
		else if(p_len < eval_len){
			c =(Element*) malloc(sizeof(Element)*(eval_len));
			memset(c,0,sizeof(Element)*eval_len);
			memcpy(c,*P,sizeof(Element)*p_len);
			free(*P);
			*P = (Polynomial)c;
		}
#ifdef __GPU
		GPU_FFT::fft_gpu(this,(Element**)P);
#else	// #ifdef __GPU
        this->fft_cpu(*P);
#endif	// #ifdef __GPU
		// Use this for debug, print out of the polynomial
//		std::cout << "here in Matan's ALGFFT:" << std::endl;
//		Polynomials::printPolynomial(*P,8);//temp added for debugging ARIEL
	}

void FFT::AlgIFFT(Element* P){
#ifdef __GPU
		this->ifft_cpu(P);
//		this->fft_gpu(P);
#else	// #ifdef __GPU
		this->ifft_cpu(P);
#endif	// #ifdef __GPU
		
	}
	/*
	 * Allocating Version of the FFT Algorithm
	 */
	void FFT::AlgFFT(const Polynomial* P, len_t p_len,  Element** res){
		if(p_len > (1UL<<this->basis.getSize()))
			return;
		else if(p_len < (1UL<<this->basis.getSize())){
			free(*res);
			*res = (Element*) malloc(sizeof(Element)*(1UL<<this->basis.getSize()));
		}
		memset(res,0,sizeof(Element)*(1UL<<this->basis.getSize()));
		memcpy(res,*P,sizeof(Element)*p_len);
		AlgFFT(res,(1UL<<this->basis.getSize()));
	}

	void FFT::AlgIFFT(Element* P,  Polynomial* res){
		len_t p_len = 1UL<<basis.getSize();
		memcpy(*res,P,sizeof(Element)*p_len);
		AlgIFFT(*res);
	}
	void FFT::generateSubspaceElements_cpu(const Element* b,const Element& s,const len_t b_len,Element* const res){
		Element::assign(res[0],s);
		for(int i = b_len-1 ; i >=0 ; --i){
#pragma omp parallel for
			for(idx_t j=0 ; j < (1UL<<b_len) ; j+= 1UL<<(i+1)){
				Element::c_add(res[j],b[i],res[j+(1UL<<i)]);
			}
		}
	}
	void FFT::updateShiftSubspaceElements_cpu(const Element& s,const len_t b_len,Element* const res){
		Element delta;
        Element::c_add(res[0],s,delta);
#pragma omp parallel for
        for(idx_t j=0 ; j < (1UL<<b_len) ; j++){
            Element::c_add(res[j],delta,res[j]);
        }
	}
	void FFT::multiExponentiate_cpu(Element& e, len_t t,Element* res){
		if((t)< omp_max_threads)
			omp_set_num_threads(t);

#pragma omp parallel
		{
		idx_t idx = (unsigned int)omp_get_thread_num();
		idx_t low = (t*idx)/omp_get_num_threads();
		idx_t high = (t*(idx+1))/omp_get_num_threads();
		high = high < t ? high : t;
		Element::c_exp(e,low,res[low]);
		for(idx_t i = low ; i < high-1 ; ++i){
			Element::c_mul(res[i],e,res[i+1]);
		}
	}
		if((t)< omp_max_threads)
			omp_set_num_threads(omp_max_threads);
	}
	void FFT::iGPartition_cpu(Element* G, len_t log_g_len,Element* res){
		len_t g_len = 1<<log_g_len;
		for(idx_t i = 0; i < g_len ; i+=2){
			G[i+1]=res[(g_len>>1)+(i>>1)];
			G[i]=res[i>>1];
		}
	}
	void FFT::GPartition_cpu(Element* G, len_t log_g_len, Element* res)const{
		len_t g_len = 1<<log_g_len;
		for(idx_t i =  0; i < g_len ; i+=2){
			res[i>>1]=G[i];
			res[(g_len>>1)+(i>>1)]=G[i+1];
		}
	}
	void FFT::iGPartition_cpuOMP(Element* G, len_t log_g_len,Element* res){
		len_t g_len = 1<<log_g_len;
#pragma omp parallel for
		for(idx_t i =0  ; i < g_len ; i+=2){
			G[i]=res[i>>1];
			G[i+1]=res[(g_len>>1)+(i>>1)];
		}
	}
	void FFT::GPartition_cpuOMP(Element* G, len_t log_g_len,Element* res)const{
		len_t g_len = 1<<log_g_len;
#pragma omp parallel for schedule(static)
		for(idx_t i =  0; i < g_len ; i+=2){
			res[i>>1]=G[i];
			res[(g_len>>1)+(i>>1)]=G[i+1];
		}
	}
	void FFT::UVFromW_cpu_serial(Polynomial P, len_t log_g_len, len_t size){
		Polynomial G;
		Element* t = subspaces[this->basis.getSize()-log_g_len];
		len_t half_g_len = 1UL<<(log_g_len-1);
		for(idx_t j = 0; j<(1UL<<size) ; j+=(1UL<<log_g_len)){
			G=&P[j];
			for(size_t i = 0 ; i < half_g_len ; ++i)
			{
				Element::c_add(G[i],G[i+half_g_len],G[i+half_g_len]);
				Element::c_mulXor(G[i+half_g_len],t[i],G[i]);
			}
		}
	}
	void FFT::UVFromW_cpu(Polynomial P, len_t log_g_len,  len_t size){
		Polynomial G;
		idx_t tid,low,high;
		len_t half_g_len = 1UL<<(log_g_len-1);
		if(1UL<<(size-log_g_len)>= omp_max_threads)
		{
#pragma omp parallel for private(G,tid)
			for(idx_t j = 0; j<(1UL<<size) ; j+=(1UL<<log_g_len)){
				tid = omp_get_thread_num();
				G=&P[j];
				for(size_t i = 0 ; i < half_g_len ; ++i)
				{
					Element::c_add(G[i],G[i+half_g_len],G[i+half_g_len]);
					Element::c_mulXor(G[i+half_g_len],subspaces[size-log_g_len][i],G[i]);
				}
			}
		}
		else
		{
			for(idx_t j = 0; j<(1UL<<size) ; j+=(1UL<<log_g_len))
			{
				G=&P[j];
				if(half_g_len < omp_max_threads)
					omp_set_num_threads(half_g_len);
#pragma omp parallel private(tid,low,high)
				{
					tid = omp_get_thread_num();
					low = (half_g_len*tid) /omp_get_num_threads();
					high = (half_g_len*(tid+1))/omp_get_num_threads();
					for(size_t i = low ; i < high ; ++i)
					{
						Element::c_add(P[j+i],P[j+i+half_g_len],P[j+i+half_g_len]);
						Element::c_mulXor(P[j+i+half_g_len],subspaces[size-log_g_len][i],P[j+i]);
					}
				}
				if(half_g_len < omp_max_threads)
					omp_set_num_threads(omp_max_threads);
			}
		}
	}
	void FFT::WFromUV_cpu_serial(Polynomial P, len_t log_g_len, len_t size)const{
		Polynomial G;
		Element* t = subspaces[this->basis.getSize()-log_g_len];
		len_t half_g_len = 1UL<<(log_g_len-1);
		for(idx_t j = 0; j<(1UL<<size) ; j+=(1UL<<log_g_len)){
			G=&P[j];
			for(size_t i = 0 ; i < half_g_len ; ++i)
			{
				Element::c_mulXor(G[i+half_g_len],t[i],G[i]);
				Element::c_add(G[i],G[i+half_g_len],G[i+half_g_len]);
			}
		}
	}
	void FFT::WFromUV_cpu(Polynomial P, len_t log_g_len, len_t size)const{
		Polynomial G;
		idx_t tid,low,high;
		len_t half_g_len = 1UL<<(log_g_len-1);
		if(1UL<<(size-log_g_len)>= omp_max_threads)
		{
#pragma omp parallel for private(G,tid) schedule(static,1)
			for(idx_t j = 0; j<(1UL<<size) ; j+=(1UL<<log_g_len)){
				tid = omp_get_thread_num();
				G=&P[j];
				for(size_t i = 0 ; i < half_g_len ; ++i)
				{
					Element::c_mulXor(G[i+half_g_len],subspaces[size-log_g_len][i],G[i]);
					Element::c_add(G[i],G[i+half_g_len],G[i+half_g_len]);
				}
			}
		}
		else
		{
			for(idx_t j = 0; j<(1UL<<size) ; j+=(1UL<<log_g_len))
			{
				G=&P[j];
				if(half_g_len < omp_max_threads)
					omp_set_num_threads(half_g_len);
#pragma omp parallel private(tid,low,high)
				{
					tid = omp_get_thread_num();
					low = (half_g_len*tid) /omp_get_num_threads();
					high = (half_g_len*(tid+1))/omp_get_num_threads();
					for(unsigned int i = low ; i < high ; ++i)
					{
						Element::c_mulXor(P[j+i+half_g_len],subspaces[size-log_g_len][i],P[j+i]);
						Element::c_add(P[j+i],P[j+i+half_g_len],P[j+i+half_g_len]);
					}
				}
				if(half_g_len < omp_max_threads)
					omp_set_num_threads(omp_max_threads);
			}
		}
	}
	void FFT::ifft_serial(Polynomial P,len_t log_size, Element* c_p_cpy){
		const len_t size = log_size;
		const len_t orig_size = this->basis.getSize();
		Element* p_cpy = NULL;
		Element* it;
		Element* private_it;
		for(idx_t i = size ; i>1 ; --i){
			UVFromW_cpu_serial(P,i,size);
		}
		for(idx_t i  = 1 ; i < (1UL<<size); i+=2){
			Element::c_add(P[i],P[i-1],P[i]);
			Element::c_mul(P[i],i_lastD,P[i]);
			Element::c_mulXor(P[i],lastShift,P[i-1]);
		}
		for(idx_t i =2; i<=size ; ++i){
			if(i&1){
				it=P;
				p_cpy=c_p_cpy;
			} else{
				it=c_p_cpy;
				p_cpy=P;
			}
			for( size_t j = 0; j < (1UL<<(size-i)) ; ++j)
			{
				private_it = it+ j*(1UL<<i);
				iGPartition_cpu(private_it,i,p_cpy+(j<<i));
				Polynomials::i_taylorExpansion(private_it,i);
				Element::c_vecMul(exps[orig_size-i],private_it,private_it,1UL<<i);
			}
		}
		if(p_cpy==P) {
			memcpy(P,c_p_cpy,sizeof(Element)*(1<<size));
		}
	}
	void FFT::ifft_cpu(Polynomial P){
		const len_t size = basis.getSize();
		Element* c_p_cpy_local((Element*)malloc(sizeof(Element)*(1UL<<size)));
		Element* p_cpy = NULL;
		Element* it;
		const unsigned int min_log_general_level = MIN(log_omp_max_threads,size-1);
		omp_set_num_threads(omp_max_threads);
		for(idx_t i = size ; i>= size - min_log_general_level+1 ; --i){
			UVFromW_cpu(P,i,size);
		}
		if(min_log_general_level > size-1){ // Can be deleted
#pragma omp parallel for schedule(static,1) //MAX
			for(idx_t i  = 1 ; i < (1UL<<size); i+=2){
				Element::c_add(P[i],P[i-1],P[i]);
				Element::c_mul(P[i],i_lastD,P[i]);
				Element::c_mulXor(P[i],lastShift,P[i-1]);
			}
		}
		if(min_log_general_level <= size-1){
            const size_t step = 1UL << (size - min_log_general_level);
            const size_t bound = 1UL<<size;
#pragma omp parallel for
			for (plooplongtype i = 0; i < bound; i+=step){
				FFT::ifft_serial(P+i,size-min_log_general_level,c_p_cpy_local+i);
			}
		}
		for(idx_t i = size - min_log_general_level + 1; i<=size ; ++i){
			if((i+min_log_general_level-size-1)&1UL){
				it=P;
				p_cpy=c_p_cpy_local;
			} else{
				it=c_p_cpy_local;
				p_cpy=P;
			}
			idx_t mask = andMask(i);
			for (unsigned long long j = 0; j < (1UL << (size - i)); ++j, it += (1UL << i))
			{
				iGPartition_cpuOMP(it,i,p_cpy+(j<<i));
				Polynomials::i_taylorExpansionOMP(it,i);
			}
			it-=(1UL<<size);
#pragma omp parallel for schedule(guided)
			for(idx_t k = 0 ; k < (1UL<<size) ; ++k){
				Element::c_mul(exps[size-i][k & mask],it[k],it[k]);
			}
		}
		if(p_cpy==P)
		{
			if(omp_max_threads > (1UL<<size))
				omp_set_num_threads(1UL<<size);
#pragma omp parallel
			{
				unsigned int tid = omp_get_thread_num();
				unsigned int low = (tid*(1UL<<size))/omp_get_num_threads();
				unsigned int high = ((tid+1)*(1UL<<size))/omp_get_num_threads();
                memcpy(&(P[low]),&(c_p_cpy_local[low]),sizeof(Element)*(high-low));
			}
			if(omp_max_threads > (1UL<<size))
				omp_set_num_threads(omp_max_threads);
		}
        
        free(c_p_cpy_local);
	}
	void FFT::fft_serial(Polynomial P,len_t log_size, Element* c_p_cpy)const{
		const len_t size = log_size;
		const len_t orig_size = this->basis.getSize();
		Element* p_cpy = P;
		Element* it;
		Element* private_it;
		for(idx_t i =size; i>1 ; --i){
			if((size-i)&1UL){
				it=c_p_cpy;
				p_cpy=P;
			} else{
				it=P;
				p_cpy=c_p_cpy;
			}
			for( size_t j = 0; j < (1UL<<(size-i)) ; ++j)
			{
				private_it = it+ j*(1UL<<i);
				Element::c_vecMul(exps[orig_size-i],private_it,private_it,1UL<<i);
				Polynomials::taylorExpansion(private_it,i);
				GPartition_cpu(private_it,i,p_cpy+(j<<i));
			}
		}
		if(p_cpy!=P) {
			memcpy(P,p_cpy,sizeof(Element)*(1UL<<size));
		}
		for(size_t i = 1 ; i <= (1UL<<size); i+=2){
			Element::c_mulXor(P[i],lastShift,P[i-1]);
			Element::c_mul(P[i],lastD,P[i]);
			Element::c_add(P[i],P[i-1],P[i]);
		}
		for(idx_t i = 2 ; i<= size ; ++i){
			WFromUV_cpu_serial(P,i,size);
		}
	}
	void FFT::fft_cpu(Polynomial P)const{
		const len_t size = basis.getSize();
		Element* c_p_cpy_local((Element*)malloc(sizeof(Element)*(1UL<<size)));
		Element* p_cpy = P;
		Element* it = c_p_cpy_local;
		const unsigned int min_log_general_levels = MIN(log_omp_max_threads,size-1);
		omp_set_num_threads(omp_max_threads);
        for(idx_t i =size; i>size-min_log_general_levels ; --i){
			if((size-i)&1){
				it=c_p_cpy_local;
				p_cpy=P;
			} else{
				it=P;
				p_cpy=c_p_cpy_local;
			}
			idx_t mask = andMask(i);
#pragma omp parallel for schedule(guided)
				for(idx_t k = 0 ; k < (1UL<<size) ; ++k){
					Element::c_mul(exps[size-i][k & mask],it[k],it[k]);
				}
				for (unsigned long long j = 0; j < (1UL << (size - i)); ++j, it += (1UL << i))
				{
					Polynomials::taylorExpansionOMP(it,i);
					GPartition_cpuOMP(it,i,p_cpy+(j<<i));
				}
		it-=(1UL<<size);
		}
		if(min_log_general_levels <= size-1){
            const size_t step = 1UL << (size - min_log_general_levels);
            const size_t bound = 1UL<<size;
#pragma omp parallel for
			for (plooplongtype i = 0; i < bound; i += step){
				FFT::fft_serial(p_cpy+i,size-min_log_general_levels,it+i);
			}
		}
		if(p_cpy!=P)
		{
			if(1UL<<size < omp_max_threads)
				omp_set_num_threads(1UL<<size);
#pragma omp parallel
			{
				unsigned int tid = omp_get_thread_num();
				long long low = (tid*(1UL << size)) / omp_get_num_threads();
				long long high = ((tid + 1)*(1UL << size)) / omp_get_num_threads();
			memcpy(&(P[low]),&(p_cpy[low]),sizeof(Element)*(high-low));
			}
			if(1UL<<size < omp_max_threads)
				omp_set_num_threads(omp_max_threads);
		}
        free(c_p_cpy_local);
		if(min_log_general_levels > size-1){
#pragma omp parallel for schedule(static,(1UL<<(size-1))/omp_max_threads)//MAX
			for (plooplongtype i = 1; i <= (1UL << size); i += 2){
				Element::c_mulXor(P[i],lastShift,P[i-1]);
				Element::c_mul(P[i],lastD,P[i]);
				Element::c_add(P[i],P[i-1],P[i]);
			}
		}
		for(idx_t i = size-min_log_general_levels+1; i<= size ; ++i){
			WFromUV_cpu(P,i,size);
		}
	}
} /* namespace FFF */
