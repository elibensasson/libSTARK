/*
 * Polynomials.cpp
 *
 *  Created on: Jul 2, 2014
 *      Author: matan
 */

#include "Polynomials.h"
#include <iostream>

namespace FFF {
	void Polynomials::taylorExpansionIteration(Polynomial p, len_t l){
		Element::vecXor((cell_t*)&p[l>>1],(cell_t*)&p[3*(l>>2)],Element::element_len*(l>>2));
		Element::vecXor((cell_t*)&p[(l>>2)],(cell_t*)&p[(l>>1)],Element::element_len*(l>>2));
	}
	void Polynomials::i_taylorExpansionIteration(Polynomial p, len_t l){
		Element::vecXor((cell_t*)&p[(l>>2)],(cell_t*)&p[(l>>1)],Element::element_len*(l>>2));
		Element::vecXor((cell_t*)&p[l>>1],(cell_t*)&p[3*(l>>2)],Element::element_len*(l>>2));
	}
	void Polynomials::taylorExpansionIterationOMP(Polynomial p, len_t l){
#pragma omp parallel for schedule(guided)
		for(idx_t i = 0 ; i < (l>>2) ; ++i){
			Element::c_add(p[(l>>1)+i],p[3*(l>>2)+i],p[(l>>1)+i]);
			Element::c_add(p[(l>>2)+i],p[(l>>1)+i],p[(l>>2)+i]);
		}
	}
	void Polynomials::i_taylorExpansionIterationOMP(Polynomial p, len_t l){
#pragma omp parallel for
		for(idx_t i = 0 ; i < (l>>2) ; ++i){
			Element::c_add(p[(l>>2)+i],p[(l>>1)+i],p[(l>>2)+i]);
			Element::c_add(p[(l>>1)+i],p[3*(l>>2)+i],p[(l>>1)+i]);
		}
	}
	void Polynomials::taylorExpansion(Polynomial p, len_t log_len){
		len_t len=1<<log_len;
		for(idx_t i = len ; i > 2 ; i>>=1 ){
			for(int i_p = len-i; i_p>=0 ;i_p-=i)
				taylorExpansionIteration((Element*)&p[i_p],i);
		}
	}
	void Polynomials::i_taylorExpansion(Polynomial p, len_t log_len){
		len_t len=1<<log_len;
		for(idx_t i = 4 ; i <=len ; i<<=1 ){
			for(unsigned int i_p = 0; i_p<= len-i ;i_p+=i)
				i_taylorExpansionIteration((Element*)&p[i_p],i);
		}
	}
	void Polynomials::taylorExpansionOMP(Polynomial p,len_t log_len){
		len_t len=1<<log_len;
		idx_t i;
		for(i = len ; i > len/omp_max_threads ; i>>=1 ){
			for(int i_p = len-i; i_p>=0 ;i_p-=i)
				taylorExpansionIterationOMP((Element*)&p[i_p],i);
		}
		for(;i>2;i>>=1){
#pragma omp parallel for
			for(int i_p = len-i ; i_p >=0 ; i_p-=i)
				taylorExpansionIteration((Element*)&p[i_p],i);
		}
	}
	void Polynomials::i_taylorExpansionOMP(Polynomial p,len_t log_len){
		len_t len=1<<log_len;
		idx_t i;
		for(i=4;i<=len/omp_max_threads;i<<=1){
#pragma omp parallel for
			for(int i_p = len-i ; i_p >=0 ; i_p-=i)
				i_taylorExpansionIteration((Element*)&p[i_p],i);
		}
		for(; i <= len ; i<<=1 ){
			for(int i_p = len-i; i_p>=0 ;i_p-=i)
				i_taylorExpansionIterationOMP((Element*)&p[i_p],i);
		}
	}

	void Polynomials::printPolynomial(Polynomial p, len_t p_len){
		bool printed = false;
		for(unsigned int i = 0 ; i < p_len ; ++i){
			if(!Element::c_isZero(p[i])){
				Element::printElement(p[i]);
				std::cout<<"x^"<<i<<std::endl;
				printed = true;
			}
		}
		if(!printed)
			std::cout << 0 <<std::endl;
	}
	void Polynomials::evaluate(Polynomial p, len_t p_len, Element& e,Element& res){
		Element a;
		Element::c_setUnit(a);
		Element::c_setZero(res);
		for(unsigned int i =0  ; i < p_len ; ++i){
			Element::c_mulXor(a,p[i],res);
			Element::c_mul(a,e,a);
		}
	}
} /* namespace FFF */
