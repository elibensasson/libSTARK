/*
 * Polynomials.h
 *
 *  Created on: Jul 2, 2014
 *      Author: matan
 */

#ifndef POLYNOMIALS_H_
#define POLYNOMIALS_H_
#include "Definitions.h"
#include "Element.h"
namespace FFF {
typedef Element* Polynomial;
class Polynomials {
	static void taylorExpansionIteration(Polynomial p, len_t l);
	static void i_taylorExpansionIteration(Polynomial p, len_t l);
	static void taylorExpansionIterationOMP(Polynomial p, len_t l);
	static void i_taylorExpansionIterationOMP(Polynomial p, len_t l);
public:
	/*
	 * Calculates the Taylor Expansion of p at (x^2-x).
	 * Given a polynomial f(x) of deg 2^(k+1) calculating the taylor expansion of f(x)
	 * at (x^2-x) is finding m=2^k polynomials h_0(x),...,h_(m-1)(x), each of degree <= 2.
	 * Such that:
	 * 			f(x)= h_0(x)*(x^2-x)^0+ h_1(x)*(x^2-x)^1+....+h_(m-1)(x)*(x^2-x)^(m-1).
	 * 	Full algorithm can be found in Gao & Mateer's Paper.
	 * 	After the function invocation 2q+r coefficient (r is either 0 or 1) in p will represent the
	 * 	r-th coefficient in h_q(x).
	 */
	static void taylorExpansion(Polynomial p, len_t log_len );
	static void i_taylorExpansion(Polynomial p, len_t log_len );
	static void taylorExpansionOMP(Polynomial p,len_t log_len);
	static void i_taylorExpansionOMP(Polynomial p,len_t log_len);
	static void printPolynomial(Polynomial p, len_t p_len);
	/*
	 * res <- p(e) .
	 * deg(p)=p_len-1.
	 */
	static void evaluate(Polynomial p, len_t p_len, Element& e,Element& res);
};
} /* namespace FFF */
#endif /* POLYNOMIALS_H_ */
