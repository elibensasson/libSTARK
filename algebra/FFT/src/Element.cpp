/*
 * Element.cpp
 *
 *  Created on: May 31, 2014
 *      Author: matan
 */
/*
 * SSE SUPPORT
 */

#ifdef WIN32
#include <wmmintrin.h>
//#include <emmintrin.h>
#endif	// #ifdef WIN32
#ifdef __GNUC__
#include <x86intrin.h>
#endif	// #ifdef __GNUC__
#include <stdint.h>
#include <stdint.h>
#include <bitset>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Element.h"
#include "Definitions.h"
#include <bitset>
#include <iostream>
FFF::Element c_mulXorAux;
namespace FFF {
#ifdef WIN32 //TODO
#define __attribute__(x)
#define GetChar(value,index) (((char*)&value)[index])
#define AS_8CHARS(a) \
    GetChar((a), 0), GetChar((a), 1), \
	 GetChar((a), 2), GetChar((a), 3), \
	 GetChar((a), 4), GetChar((a), 5), \
    GetChar((a), 6), GetChar((a), 7)
#define _mm_set1_epi64(x) {AS_8CHARS(x), AS_8CHARS(x)}
#endif
#ifdef WIN32
 __m128i mask = {-1,-1,-1,-1,-1,-1,-1,-1};
#endif // #ifdef WIN32
#ifdef __GNUC__
       __m128i mask = { (long long int)0b1111111111111111111111111111111111111111111111111111111111111111, 0 };
#endif // #ifdef __GNUC__
const idx_t Element::irr_poly_index[][max_nonzero_coefs_in_mod]={
			{0,(long long unsigned int)-1,(long long unsigned int)-1},
			{0,2,3,7,32},
			{0,1,3,4,64}
		};

const cell_t Element::irr_poly_e[][Element::element_len]={
		{0},
		{0},
		{27}
};
const len_t Element::mod_len[]= {3, 5, 5};

//--------------------------------------------------------------//
	Element::Element(const Element& e){
		memcpy(this->c,e.c,sizeof(cell_t)*element_len);
	}
	void Element::operator=(const Element& e){
		memcpy(this->c,e.c,sizeof(cell_t)*element_len);
	}

void Element::deg(cell_t * a, int & i, len_t len){
	for(i=len-1;a[i]==0;--i){};
	if(i<0){
		i=0;
		return;
	}
	cell_t t = a[i];
	idx_t offset=0;
	i*=bits_in_cell;
	for(idx_t j = bits_in_cell/2 ; j >0 ; j>>=1)
		if((t>>(offset+j))!=0){
			offset+=j;
		}
	i+=offset;
}
void setZero(cell_t * a,len_t len){
	for(unsigned int i = 0 ; i < len ; ++i)
		a[i]=0;
}
void setUnit(cell_t * a, len_t len){
	setZero(a,len);
	a[0]=1;
}
bool isUnit(cell_t * a, len_t len){
	if(a[0]!=1)
		return false;
	for(idx_t i = 1; i  < len ; ++i)
		if(a[i]!=0)
			return false;
	return true;
}

void swap(cell_t * a , cell_t * b,len_t len){
	for(unsigned int i = 0 ; i <  len ; ++i)
	{
		a[i]^=b[i];
		b[i]^=a[i];
		a[i]^=b[i];
	}
}
void Element::assign_cells(cell_t* a, cell_t* b,len_t len){
	memcpy(a,b,sizeof(cell_t)*len);
}



void Element::generateOrdElement(Element* e){
	memset(e->c,0,sizeof(cell_t)*element_len);
	for(unsigned int i = 0 ; i < mod_len[ord>>5]-1;++i)
		TOGGLE_BIT(e->c,irr_poly_index[ord>>5][i]);
}

inline void Element::clmulXor(const cell_t* a,const  cell_t* b, cell_t* res)
{
	_mm_storeu_si128((__m128i*)res,
			_mm_xor_si128(
					*((__m128i*)a),
					_mm_clmulepi64_si128(
							_mm_loadu_si128((__m128i*)a),
							_mm_loadu_si128((__m128i*)b),
							0
							)
			)
	);
//	register __m128i v1,v2;
//	ui64_m128i(v1,(const unsigned long*)a);
//	ui64_m128i(v2,(const unsigned long*)b);
//	m128i_ui64((unsigned long*)res,_mm_clmulepi64_si128(v1,v2,0));
}
inline void Element::clmul(const cell_t* a,const  cell_t* b, cell_t* res)
{
			_mm_clmulepi64_si128(
					_mm_loadu_si128((__m128i*)a),
					_mm_loadu_si128((__m128i*)b),
					0
			);
}
/*
 * CPU Operations
 */
	void Element::c_add(const Element* const a,const Element* const b, Element* const c)
	{
		for(unsigned int i = 0 ; i < element_len ; ++i){
			c->c[i]=(a->c[i]^b->c[i]);
		}
	}
	void Element::c_add(const Element& a,const Element& b, Element& c)
	{
		for(unsigned int i = 0 ; i < element_len ; ++i){
			c.c[i]=(a.c[i]^b.c[i]);
		}
	}
	bool Element::equals(Element& a, Element& b){
		return !memcmp(a.c,b.c,sizeof(cell_t)*element_len);
	}
	void Element::c_mulXor(const Element& a,const Element& b,Element& c){
		cell_t clmul_res[element_len<<1];
		c_mul(&a,&b,(Element*)clmul_res);
		vecXor(clmul_res,c.c,c.c,element_len);
	}

//specific for the irreducible 1 + x + x^3 + x^4 + x^64
const cell_t lookupTable[] = {0x0,0x1b,0x36,0x2d,0x6c,0x77,0x5a,0x41};
const char lookupTable1[] = {0x0,0x1b,0x2d,0x36,0x5a,0x41,0x77,0x6c};

//computes:
//c[i] = b[i] + factor * a[i]
//b[i] = c[i]
//a[i] = a[i] + c[i]
//THIS CODE IS IRREDUCIBLE SPECIFIC!!!
void Element::do_FFT_step(const Element& factor,Element* a, Element* b, const int len){
		register __m128i factor_reg = _mm_set1_epi64(*((__m64*)(&factor)));
		register __m128i lookup_reg = _mm_loadl_epi64((__m128i*)lookupTable1);
        


        const size_t numIterations = len/2;
        const bool hasTail = len % 2;
        
        for(size_t i=0; i< numIterations; i++){
            const size_t currIdx = i<<1;
            
            /*
             * The following code loads two words of 64 bit each,
             * name them a[0] and a[1] (without loss of generality),
             * and compute in parallel (with vectorization) the values
             * c[i] = b[i] + factor * a[i]
             * b[i] = c[i]
             * a[i] = a[i] + c[i]
             * and stores them back to memory
             */

            // Load a[0] and a[1]
            __m128i a_vec = _mm_loadu_si128((__m128i*)(&a[currIdx]));
            
            // Compute factor * a[i] for both i=0,1
            // This multiplication is done as binary polynomials,
            // and results in products of degree at most 126.
            // After this phase those products must be reduced modulu
            // the field defining polynomial irr(x).
            // This code is hard-coded for:
            // irr(x) = x^64 + x^4 + x^3 + x + 1
            //
            // After this operation the vectors t0,t1 contain those products where:
            // t0[ 64: 0] = (factor * a[0]) mod x^64
            // t0[128:64] = (factor * a[0]) div x^64
            // t1[ 64: 0] = (factor * a[1]) mod x^64
            // t1[128:64] = (factor * a[1]) div x^64
            __m128i t0, t1;
            t0=_mm_clmulepi64_si128(a_vec, factor_reg, 0);
            t1=_mm_clmulepi64_si128(a_vec, factor_reg, 1);

            // Split the products to modular reminders and offsets:
            // xmmRes   [ 64: 0] = (factor * a[0]) mod x^64
            // xmmRes   [128:64] = (factor * a[1]) mod x^64
            // xmmCarry0[ 64: 0] = (factor * a[0]) div x^64
            // xmmCarry0[128:64] = (factor * a[1]) div x^64
            //
            // Important note : notice the degree of xmmCarry0[ 64: 0] and xmmCarry0[128:64]
            // is at most (63+63)-64 = 62
            __m128i xmmRes, xmmCarry0, xmmCarry00, xmmCarry01, xmmCarry1;
            xmmRes = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(t0),_mm_castsi128_pd(t1),0));
            xmmCarry0 = _mm_castpd_si128(_mm_shuffle_pd(_mm_castsi128_pd(t0),_mm_castsi128_pd(t1),1|2));

            // xmmCarry00[ 64: 0] = ( ( (factor * a[0]) div x^64 ) * ( 1 + x ) ) (mod x^64)
            // xmmCarry00[128:64] = ( ( (factor * a[1]) div x^64 ) * ( 1 + x ) ) (mod x^64)
            xmmCarry00 = _mm_xor_si128(xmmCarry0,_mm_slli_epi64(xmmCarry0,1));
            
            // xmmCarry01[ 64: 0] = ( ( (factor * a[0]) div x^64 ) * ( x^3 + x^4 ) ) (mod x^64)
            // xmmCarry01[128:64] = ( ( (factor * a[1]) div x^64 ) * ( x^3 + x^4 ) ) (mod x^64)
            xmmCarry01 = _mm_xor_si128(_mm_slli_epi64(xmmCarry0,3),_mm_slli_epi64(xmmCarry0,4));
            
            // xmmCarry1[ 64: 0] = ( ( (factor * a[0]) div x^64 ) * ( 1 + x + x^3 + x^4 ) ) (div x^64)
            // xmmCarry1[128:64] = ( ( (factor * a[1]) div x^64 ) * ( 1 + x + x^3 + x^4 ) ) (div x^64)
            //
            // Important note : notice the degree of xmmCarry1[ 64: 0] and xmmCarry1[128:64]
            // is at most (62+4)-64 = 2.
            // In particular, their support is only in their 3 least significant bits.
            xmmCarry1 = _mm_srli_epi64(xmmCarry0,60);
           
            // We use the fact the degree of either xmmCarr1[ 64: 0] and xmmCarry1[128:64] is at most 2,
            // thus there are only 8 possibilities for each polynomial.
            // Moreover, the product of any such option by 1 + x + x^3 + x^4 is a polynomial
            // of degree oat most 2+4=6,
            // and in particular, defined by its first byte.
            // We use a precomputed lookup table to compute the required reductions:
            //
            // xmmCarry1[ 64: 0] = ( ( ( (factor * a[0]) div x^64 ) * ( 1 + x + x^3 + x^4 ) (div x^64) ) * x^64 ) (mod Irr(x))
            // xmmCarry1[128:64] = ( ( ( (factor * a[1]) div x^64 ) * ( 1 + x + x^3 + x^4 ) (div x^64) ) * x^64 ) (mod Irr(x))
            xmmCarry1 = _mm_shuffle_epi8(lookup_reg,xmmCarry1);
           
            // Computing the reduced polynomial:
            //
            // xmmFinal[ 64: 0] = 
            //  ( (factor * a[0]) mod x^64 ) +
            //  ( ( ( (factor * a[0]) div x^64 ) * ( 1 + x ) ) (mod x^64) ) +
            //  ( ( ( (factor * a[0]) div x^64 ) * ( x^3 + x^4 ) ) (mod x^64) ) +
            //  ( ( ( ( (factor * a[0]) div x^64 ) * ( 1 + x + x^3 + x^4 ) (div x^64) ) * x^64 ) (mod Irr(x)))
            //  =
            //  ( (factor * a[0]) mod x^64 ) +
            //  ( ( ( (factor * a[0]) div x^64 ) * ( 1 + x + x^3 + x^4 ) ) (mod x^64) ) +
            //  ( ( ( ( (factor * a[0]) div x^64 ) * ( 1 + x + x^3 + x^4 ) (div x^64) ) * x^64 ) (mod Irr(x)))
            //  =
            //  ( (factor * a[0]) mod x^64 ) +
            //  ( ( (factor * a[0]) div x^64 ) * ( 1 + x + x^3 + x^4 ) (mod Irr(x)))
            //  =
            //  (factor * a[0]) (mod Irr(x))
            //
            //  And similarly:
            //  xmmFinal[128:64] = (factor * a[1]) (mod Irr(x))
            __m128i tmp1 = _mm_xor_si128(xmmCarry00,xmmCarry01);
            __m128i tmp2 = _mm_xor_si128(xmmCarry1,xmmRes);
            __m128i xmmFinal = _mm_xor_si128(tmp1,tmp2);

            // Computing: b[i] = c[i] = b[i] + factor*a[i]
            __m128i b_vec = _mm_loadu_si128((__m128i*)(&b[currIdx]));
            b_vec = _mm_xor_si128(b_vec,xmmFinal);
            //store b (case 17)
            _mm_storeu_si128((__m128i*)(&b[currIdx]),b_vec);

            // Computing : a[i] = a[i] + c[i] = a[i] + b[i]
            //store a (case 18)
            a_vec = _mm_xor_si128(a_vec,b_vec);
            _mm_storeu_si128((__m128i*)(&a[currIdx]),a_vec);
        }

        if(hasTail){
            b[len-1] = b[len-1] + (factor * a[len-1]);
            a[len-1] = a[len-1] + b[len-1];
        }
	}

void Element::c_mul(const Element* a, const Element* b, Element* c){
		//register __m128i l = _mm_loadu_si128((__m128i*)irr_poly_e[ord>>5]);
		register __m128i t;
        {
        //copy a and b to a location where
        //they can be copied from without accesing
        //forbidden locations
        Element arr[3];
        arr[0] = *a;
        arr[1] = *b;

		t=_mm_clmulepi64_si128(
				_mm_loadu_si128((__m128i*)&(arr[0])),
				_mm_loadu_si128((__m128i*)&(arr[1])),
				0
		);
        }
        
        /*
         *Matans original generic code
         */
		//t=_mm_xor_si128(_mm_clmulepi64_si128(t,l,1),_mm_and_si128(t,mask));
        //t=_mm_xor_si128(_mm_clmulepi64_si128(t,l,1),t);
		//memcpy(c,&t,sizeof(cell_t)*element_len);
        
        /*
         * irreducible specific optimized code
         */
        {
        cell_t res0[2] __attribute__ ((aligned (16)));;
        _mm_store_si128((__m128i*)res0,t);
        const cell_t& lowDeg = res0[0];
        const cell_t& carry0 = res0[1];
        const cell_t carry1 = (carry0>>60);
        const cell_t reducedCarry1 = lookupTable1[carry1];
        
        const cell_t reducedCarry0 = carry0 ^ (carry0<<1) ^ (carry0<<3) ^ (carry0<<4);
		
        *(cell_t*)(c) = lowDeg ^ reducedCarry0 ^ reducedCarry1;
        }
	}


void Element::c_mul(const Element& a,const Element& b, Element& c){
		//__m128i l = _mm_loadu_si128((__m128i*)irr_poly_e[ord>>5]);
		__m128i t;
        
        {
        //copy a and b to a location where
        //they can be copied from without accesing
        //forbidden locations
        Element arr[3];
        arr[0] = a;
        arr[1] = b;

		t=_mm_clmulepi64_si128(
				_mm_loadu_si128((__m128i*)&(arr[0])),
				_mm_loadu_si128((__m128i*)&(arr[1])),
				0
		);
        }
        
        /*
         *Matans original generic code
         */
        //t=_mm_xor_si128(_mm_clmulepi64_si128(t,l,1),_mm_and_si128(t,mask));
        //t=_mm_xor_si128(_mm_clmulepi64_si128(t,l,1),t);
        //memcpy(&c,&t,sizeof(cell_t)*element_len);
		
        /*
         * irreducible specific optimized code
         */
        {
        cell_t res0[2] __attribute__ ((aligned (16)));;
        _mm_store_si128((__m128i*)res0,t);
        const cell_t& lowDeg = res0[0];
        const cell_t& carry0 = res0[1];
        const cell_t carry1 = (carry0>>60);
        const cell_t reducedCarry1 = lookupTable1[carry1];
        
        const cell_t reducedCarry0 = carry0 ^ (carry0<<1) ^ (carry0<<3) ^ (carry0<<4);
		
        *(cell_t*)(&c) = lowDeg ^ reducedCarry0 ^ reducedCarry1;
        }
	}
	void Element::c_sqr(Element* a, Element* c){
		c_mul(a,a,c);
	}
	void Element::c_sqr(Element& a, Element& c){
		c_mul(a,a,c);
	}
	void Element::left_shift(cell_t * v, unsigned int k , unsigned int len){
		 if(k>=(len<<log_bits_in_cell)){
				 setZero(v,len);
				 return;
			 }
			 unsigned int i;// = uint_e_regs[0];
			 unsigned int shift_cells;// = uint_e_regs[1];
			 shift_cells = k>>log_bits_in_cell;
			 unsigned int par;// = uint_e_regs[2];
			 par= k & andMask(log_bits_in_cell);
			 unsigned int rest;// = uint_e_regs[3];
			 rest = bits_in_cell-par;
			 for(i = len - 1 ; i >= shift_cells+1 ; --i){
				 v[i]=v[i-shift_cells]<<par | (v[i-shift_cells-1]>>(rest-1))>>1;
			 }
			 v[shift_cells]=v[0]<<par;
			 for(i=shift_cells;i>0;--i)
				 v[shift_cells-1]=0;
	}
	void Element::egcd(cell_t a[element_len+1], cell_t b[element_len+1], cell_t g[element_len+1]){
		 cell_t u[element_len+1];
		 Element::assign_cells(u,a,element_len+1);//e_regs[0]
		 cell_t v[element_len+1];
		 Element::assign_cells(v,b,element_len+1);//e_regs[1]
		 cell_t g1[element_len+1];//e_regs[2]
		 cell_t g2[element_len+1];//e_regs[3]
		 setZero(g2,element_len+1);
		 setUnit(g1,element_len+1);//g1
		 while(!isUnit(u,element_len+1)){
			 int i; //uint_e_regs[20]
			 int j; //uint_e_regs[21]
			 deg(u,i,element_len+1);
			 deg(v,j,element_len+1);
			 cell_t t[element_len+1];//e_regs[6]
			 if(i<j){
				 swap(u,v,element_len+1);
				 swap(g1,g2,element_len+1);
				 i=j-i;//j
			 } else {
				 i-=j;//j
			 }
			 Element::assign_cells(t,v,element_len+1);//v*z^j
			 left_shift(t,i,element_len+1);
			 Element::vecXor(u,t,element_len+1);//u=u+v*z^j

			 Element::assign_cells(t,g2,element_len+1);//g2*z^j
			 left_shift(t,i,element_len+1);
			 Element::vecXor(g1,t,element_len+1);//g1=g1+g2*z^j

		 }
		Element::assign_cells(g,g1,element_len);
	}

	void Element::c_inv(Element& a, Element& res){
		cell_t c[element_len+1];
		cell_t e[element_len+1];
		assign_cells(e,a.c,element_len);//=e_regs[8]
		e[element_len]=0;
		Element e_ord;
		Element::generateOrdElement(&e_ord);
		assign_cells(c,e_ord.c,element_len);
		c[element_len]=1;
		egcd(e,c,res.c);
	}
	void Element::c_inv(Element* a, Element* res){
		cell_t c[element_len+1];
		cell_t e[element_len+1];
		assign_cells(e,a->c,element_len);//=e_regs[8]
		e[element_len]=0;
		Element e_ord;
		Element::generateOrdElement(&e_ord);
		assign_cells(c,e_ord.c,element_len);
		c[element_len]=1;
		egcd(e,c,res->c);
	}
	void Element::c_setZero(Element* a){
		memset(a->c,0,sizeof(Element));
	}
	void Element::c_setZero(Element& a){
		memset(a.c,0,sizeof(Element));
	}
	void Element::c_setUnit(Element* a){
		c_setZero(a);
		a->c[0]=1;
	}
	void Element::c_setUnit(Element& a){
		c_setZero(a);
		a.c[0]=1;
	}
	bool Element::c_isUnit(Element* a){
		if(a->c[0]!=1)
			return false;
		for(unsigned int i = 1 ; i < element_len ; ++i)
			if(a->c[i]!=0)
				return false;
		return true;
	}
	bool Element::c_isUnit(Element& a){
		if(a.c[0]!=1)
			return false;
		for(unsigned int i = 1 ; i < element_len ; ++i)
			if(a.c[i]!=0)
				return false;
		return true;
	}

void Element::c_exp(Element a, unsigned long long exp, Element& res){
	Element::c_setUnit(res);
	for(; exp != 0 ; exp>>=1){
		if(1&exp)
			Element::c_mul(res,a,res);
		Element::c_sqr(a,a);
	}
}
void Element::vecXor(cell_t * a, cell_t * b, len_t len){
		for(unsigned int i = 0 ; i <  len ; ++i)
			a[i]^=b[i];
	}
void Element::vecXor(cell_t * a, cell_t * b, cell_t* c, len_t len){
		for(unsigned int i = 0 ; i <  len ; ++i)
			c[i]=(a[i]^b[i]);
	}
void Element::printCells(const cell_t * x, len_t l){
	for(unsigned int i = l-1 ; i < (~0UL) ; --i){
		std::cout <<std::bitset<bits_in_cell>(x[i]);
		if(i!=0)
			std::cout << ",";
	}

}
void Element::printElement(const Element& x){
	printCells(x.c,element_len);
}
void Element::assign(Element (*a),const Element (*b)){
	memcpy(a,b,sizeof(cell_t)*element_len);
}
void Element::assign(Element& a,const Element &b){
	memcpy(&a,&b,sizeof(cell_t)*element_len);
}

void Element::c_vecMul(Element* a, Element* b, Element* c, len_t l){
	for(unsigned int i = 0  ; i < l ; ++i){
		Element::c_mul(a[i],b[i],c[i]);
	}
}

bool Element::c_isZero(Element& a){
	for(unsigned int i = 0 ; i  < element_len ; ++i)
		if(a.c[i]!=0)
			return false;
	return true;
}
void Element::naiveMul(const cell_t* a, const cell_t* b, cell_t* c){
	cell_t clmul[element_len<<1];
	for(unsigned int i = 0 ; i < (element_len<<1) ; ++i)
		clmul[i]=0;
	for(unsigned int i = 0 ; i < ord ; ++i)
		for(unsigned int j = 0 ; j < ord ; ++j){
			if(EXTRACT_BIT(a,i) &&
					EXTRACT_BIT(b,j) )
				clmul[(i+j)>>log_bits_in_cell]^=(((cell_t)1)<<((i+j) & andMask(log_bits_in_cell)));
		}
	for(unsigned int i = (ord<<1)-1 ; i>=ord ; --i){
		if(EXTRACT_BIT(clmul,i)){
			for(unsigned int j = 0 ; j <  mod_len[ord>>5];++j)
				TOGGLE_BIT(clmul,i-ord + irr_poly_index[ord>>5][j]);
		}
	}
	unsigned int i;
	for(i = 0 ; i < element_len ; ++i)
		c[i]=clmul[i];
}

void Element::naiveMul(const Element& a, const Element& b, Element& c){
	naiveMul(a.c,b.c,c.c);
}
} /* namespace FFF */
