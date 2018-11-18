/*
 *
 *  Created on: 2 ���� 2014
 *      Author: hamilis
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_
#include <omp.h>

#define __PARALLEL__
//#define __GPU
//#define __MEASURE

#ifdef OLD_OPENMP
typedef long long plooplongtype;
typedef int ploopinttype;
#else
typedef unsigned long long plooplongtype;
typedef unsigned int ploopinttype;
#endif

namespace FFF {

	/**** typedef *****/
#ifdef __GNUC__
	typedef long long unsigned int idx_t;
#else	// #ifdef GNUGC
	typedef long long idx_t;
#endif	// #ifdef GNUGC
	typedef unsigned int chunk_cell_t;
	typedef unsigned int ord_t;
	typedef unsigned int len_t;

	/**** Macros ****/
	#define andMask(k) ((((cell_t)1)<<(k))-1)
	#define TOGGLE_BIT_CELL(e,i) (e^=(((cell_t)1)<<(i)))
	#define TOGGLE_BIT(e,i) (TOGGLE_BIT_CELL((e)[(i)>>Element::log_bits_in_cell],(i) & andMask(Element::log_bits_in_cell)))
	#define EXTRACT_BIT_CELL(e,i) ((e)&(((cell_t)1)<<(i)))
	#define EXTRACT_BIT(e,i) (EXTRACT_BIT_CELL(e[(i)>>log_bits_in_cell],i& andMask(log_bits_in_cell)))
	#define Malloc(type,len) (type*)malloc(sizeof(type)*(len));
	#define MIN(a,b) ((a)>(b) ? (b) : (a))
	#define MAX(a,b) ((a)<(b) ? (b) : (a))

	/**** Global Constants ****/
	const unsigned int max_nonzero_coefs_in_mod = 5;
	const unsigned int bits_in_byte = 8;
	const unsigned int log_warp_size = 5;
	const unsigned int warp_size = 1<<log_warp_size;
	const unsigned int log_max_block_size = 10;
	const unsigned int max_block_size = 1<<log_max_block_size;
	len_t sizeCiel(len_t a, len_t b);
	extern unsigned int omp_max_threads;
	extern chunk_cell_t partition_byte[];
	extern chunk_cell_t taylorExp_masks[];
	extern chunk_cell_t alter_masks[];
	extern unsigned int log_omp_max_threads;
	extern const unsigned int max_threads_machine;
	extern const len_t log[];

	template< typename T>
	void swap(T* a,T* b,len_t l){
		for(idx_t i = 0 ; i < l; ++i){
			a^=b;
			b^=a;
			a^=b;
		}
	}

	void set_omp_state(int omt);

}


#endif /* DEFINITIONS_H_ */
