#ifndef ALGEBRALIB_VARIABLE_OPERATORS_HPP_
#define ALGEBRALIB_VARIABLE_OPERATORS_HPP_

#include <algebraLib/variable.hpp>
#include <algebraLib/CircuitPolynomial.hpp>

#include <memory>

namespace Algebra {

// LinearTerm - operator*
inline LinearTerm        operator*(const LinearTerm& first, const FElem& second)					{ auto retval = first; return retval *= second; }
inline LinearTerm        operator*(const FElem& first, const LinearTerm& second)					{ return second * first; }
inline LinearTerm        operator*(const Variable& first, const FElem& second)						{ return LinearTerm(first) * second; }
inline LinearTerm        operator*(const FElem& first, const Variable& second)						{ return second * first; }

// LinearCombination - operator+
inline LinearCombination operator+(const LinearCombination& first, const LinearCombination& second) { auto retval = first; return retval += second; }
inline LinearCombination operator+(const LinearCombination& first, const LinearTerm& second)        { auto retval = first; return retval += LinearCombination(second); }
inline LinearCombination operator+(const LinearTerm& first, const LinearCombination& second)		{ return second + first; }
inline LinearCombination operator+(const LinearCombination& first, const Variable& second)          { auto retval = first; return retval += LinearCombination(second); }
inline LinearCombination operator+(const Variable& first, const LinearCombination& second)			{ return second + first; }
inline LinearCombination operator+(const LinearCombination& first, const FElem& second)             { auto retval = first; return retval += LinearCombination(second); }
inline LinearCombination operator+(const FElem& first, const LinearCombination& second)				{ return second + first; }

inline LinearCombination operator+(const LinearTerm& first, const LinearTerm& second)		        { return LinearCombination(first) + LinearCombination(second); }
inline LinearCombination operator+(const LinearTerm& first, const Variable& second)					{ return LinearCombination(first) + LinearCombination(second); }
inline LinearCombination operator+(const Variable& first, const LinearTerm& second)					{ return second + first; }
inline LinearCombination operator+(const LinearTerm& first, const FElem& second)					{ return LinearCombination(first) + LinearCombination(second); }
inline LinearCombination operator+(const FElem& first, const LinearTerm& second)					{ return second + first; }

inline LinearCombination operator+(const Variable& first, const Variable& second)					{ return LinearCombination(first) + LinearCombination(second); }
inline LinearCombination operator+(const Variable& first, const FElem& second)						{ return LinearCombination(first) + LinearCombination(second); }
inline LinearCombination operator+(const FElem& first, const Variable& second)						{ return second + first; }

// LinearCombination - operator*
inline LinearCombination operator*(const LinearCombination& first, const FElem& second)             { auto retval = first; return retval *= second; }
inline LinearCombination operator*(const FElem& first, const LinearCombination& second)				{ return second * first; }

// CircuitPolynomial - operator+
inline CircuitPolynomial operator+(const CircuitPolynomial& first, const CircuitPolynomial& second)	{ auto retval = first; return retval += second; }
inline CircuitPolynomial operator+(const CircuitPolynomial& first, const LinearCombination& second)	{ return first + CircuitPolynomial(second); }
inline CircuitPolynomial operator+(const LinearCombination& first, const CircuitPolynomial& second)	{ return second + first; }
inline CircuitPolynomial operator+(const CircuitPolynomial& first, const LinearTerm& second)		{ return first + CircuitPolynomial(second); }
inline CircuitPolynomial operator+(const LinearTerm& first, const CircuitPolynomial& second)		{ return second + first; }
inline CircuitPolynomial operator+(const CircuitPolynomial& first, const Variable& second)			{ return first + CircuitPolynomial(second); }
inline CircuitPolynomial operator+(const Variable& first, const CircuitPolynomial& second)			{ return second + first; }
inline CircuitPolynomial operator+(const CircuitPolynomial& first, const FElem& second)				{ return first + CircuitPolynomial(second); }
inline CircuitPolynomial operator+(const FElem& first, const CircuitPolynomial& second)				{ return second + first; }

// CircuitPolynomial - operator*
inline CircuitPolynomial operator*(const CircuitPolynomial& first, const CircuitPolynomial& second)	{ auto retval = first; return retval *= second; }
inline CircuitPolynomial operator*(const CircuitPolynomial& first, const LinearCombination& second)	{ return first * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const LinearCombination& first, const CircuitPolynomial& second)	{ return second * first; }
inline CircuitPolynomial operator*(const CircuitPolynomial& first, const LinearTerm& second)		{ return first * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const LinearTerm& first, const CircuitPolynomial& second)		{ return second * first; }
inline CircuitPolynomial operator*(const CircuitPolynomial& first, const Variable& second)			{ return first * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const Variable& first, const CircuitPolynomial& second)			{ return second * first; }
inline CircuitPolynomial operator*(const CircuitPolynomial& first, const FElem& second)				{ return first * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const FElem& first, const CircuitPolynomial& second)				{ return second * first; }

inline CircuitPolynomial operator*(const LinearCombination& first, const LinearCombination& second)	{ return CircuitPolynomial(first) * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const LinearCombination& first, const LinearTerm& second)		{ return CircuitPolynomial(first) * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const LinearTerm& first, const LinearCombination& second)		{ return second * first; }
inline CircuitPolynomial operator*(const LinearCombination& first, const Variable& second)			{ return CircuitPolynomial(first) * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const Variable& first, const LinearCombination& second)			{ return second * first; }


inline CircuitPolynomial operator*(const LinearTerm& first, const LinearTerm& second)				{ return CircuitPolynomial(first) * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const LinearTerm& first, const Variable& second)					{ return CircuitPolynomial(first) * CircuitPolynomial(second); }
inline CircuitPolynomial operator*(const Variable& first, const LinearTerm& second)					{ return second * first; }

inline CircuitPolynomial operator*(const Variable& first, const Variable& second)					{ return CircuitPolynomial(first) * CircuitPolynomial(second); }

} //namespace

#endif //ALGEBRALIB_VARIABLE_OPERATORS_HPP