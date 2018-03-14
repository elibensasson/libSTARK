#include <random>
#include <set>
#include <vector>
#include <Element.h>

#ifndef  __Field_Element_HPP__
#define  __Field_Element_HPP__

namespace Algebra {

const int ExtensionDegree = 64;

/* c++11 pseudorandom number generator */
extern std::mt19937_64 rng;

/**
* @class FieldElement
*/

class FieldElement {
public:

	/**
	* @brief   Default constructor
	*/
	FieldElement() :element_(){};

	/**
	*  @brief   conversion construction from FFF element
	*/
	explicit FieldElement(const FFF::Element& e) : element_(e){};

    /**
	* @brief   conversion to FFF::Element
	*/
	explicit operator const FFF::Element()const{ return element_; }

	/**
	* @brief   copy constructor
	*/
	FieldElement(const FieldElement& src) : element_(src.element_){};


	/**
	* inverse
	*/
	
	inline FieldElement inverse()const{ 
        FieldElement res;
        FFF::Element dummy = element_;
        FFF::Element::c_inv(dummy,res.element_);
        return res; 
    }

	//added by Ariel
	inline static FieldElement sqr(const FieldElement& x){
		return x*x;
	}

	/** Bunch of operators */
	friend FieldElement operator+(const FieldElement&, const FieldElement&);
	friend FieldElement& operator+=(FieldElement&, const FieldElement&);
	friend FieldElement operator-(const FieldElement&, const FieldElement&);
	friend FieldElement operator/(const FieldElement&, const FieldElement&);
	friend FieldElement operator*(const FieldElement&, const FieldElement&);
	friend FieldElement& operator*=(FieldElement&, const FieldElement&);
	friend bool operator==(const FieldElement&, const FieldElement&);
	friend bool operator!=(const FieldElement&, const FieldElement&);
	//friend bool operator<(const FieldElement&, const FieldElement&);
	friend FieldElement& operator-=(FieldElement&, const FieldElement&);
	friend FieldElement& operator/=(FieldElement&, const FieldElement&);
    friend const FieldElement zero();
    friend const FieldElement one();
    friend const FieldElement xFE();

	friend FieldElement power(const FieldElement&, long exponent);
	friend std::ostream& operator<<(std::ostream&, const FieldElement&);
    friend bool compareFieldElements(const FieldElement& e1, const FieldElement& e2);
	friend FieldElement mapIntegerToFieldElement(const size_t shift, const size_t numBits, size_t numToWrite);
	friend size_t mapFieldElementToInteger(const size_t shift, const size_t numBits, const FieldElement& e);

	/**
	* element_ as string
	*/
	::std::string asString() const;

private:

    const static short numBits = 64;
	FFF::Element element_; /**< @brief instance state */
	
};

//Arithmetic operators
inline FieldElement operator/(const FieldElement& a, const FieldElement& b){
    return a*b.inverse();
}

inline FieldElement& operator/=(FieldElement& x, const FieldElement& b)
{
    x = x /b; return x;
}

inline FieldElement operator+(const FieldElement& a, const FieldElement& b)
{
    FieldElement res;
    FFF::Element::c_add(a.element_,b.element_,res.element_);
    return res;
}

inline FieldElement& operator+=(FieldElement& x, const FieldElement& b)
{
    x = x+b; return x;
}

inline FieldElement operator-(const FieldElement& a, const FieldElement& b)
{
    return a+b;
}

inline FieldElement& operator-=(FieldElement& x, const FieldElement& b)
{
    x = x - b; return x;
}

inline FieldElement operator*(const FieldElement& a, const FieldElement& b)
{
    FieldElement res;
    FFF::Element::c_mul(a.element_,b.element_,res.element_);
    return res;
}

inline FieldElement& operator*=(FieldElement& x, const FieldElement& b)
{
    x= x*b; return x;
}

inline bool operator==(const FieldElement& a, const FieldElement& b)
{
    return a.element_.c[0] == b.element_.c[0];
}

inline bool operator!=(const FieldElement& a, const FieldElement& b)
{
    return a.element_.c[0] != b.element_.c[0];
}

inline FieldElement power(const FieldElement& a, long e)
{
    FieldElement res;
    FFF::Element::c_exp(a.element_,e,res.element_);
    return res;
}

/**
 * @brief   An order on field elements
 * This exists only in order to be able to keep
 * field elements in an STL set, hence the order
 * is arbitrary, as long as it really is an order.
 * In this case lexicographically, representing each
 * FieldElement as vector of bytes
 */
inline bool compareFieldElements(const FieldElement& e1, const FieldElement& e2){
    return e1.element_.c[0] > e2.element_.c[0];
}

struct classCompElements{
    bool operator()(const FieldElement& e1, const FieldElement& e2)const{
        return compareFieldElements(e1,e2);
    }
};

/**
 * Inverts a vector of field elements point-wise
 * Does it in a more efficient way than the trivial solution.
 * As division is known to be time consuming, in the case of n elements,
 * instead of doing n division, only a single division operation is executed,
 * and less than 3n multiplications.
 */
std::vector<FieldElement> invertPointwise(const std::vector<FieldElement>& elems);

/**
 * ElementsSet : just an STL set of field elements
 * @note it is defined specifically here because
 * there is no natural order on field elements,
 * but an STL set requires an order on its values.
 * the generator generates an empty set of elements
 * with some arbitrary order
 */
typedef std::set<FieldElement,classCompElements> elementsSet_t;
elementsSet_t getStandartBasis(const unsigned short basisSize);
std::vector<FieldElement> getStandartOrderedBasis(const unsigned short basisSize);
/** Functions that generate field elements from integers **/

/**
 * Let "numToWrite" = \f$ 2^0  b_0 + 2^1  b_1 + \dots + 2^k  b_k \f$
 * Lets name "shift" as S , and "numBits" as N
 *
 * These functions generated the field element that
 * as a polynomial over GF(2) might be represented as:
 * \f$ x^{s} b_0 + x^{s+1} b_1 + \dots x^{s+n-1} b_{n-1} \f$
 **/
inline FieldElement mapIntegerToFieldElement(const size_t shift, const size_t numBits, size_t numToWrite){
	FieldElement res;
	size_t numToWrite_masked = numToWrite;
	if (numBits < 64){
		const size_t mask = (size_t(1) << numBits) - 1;
		numToWrite_masked = numToWrite & mask;
	}
	res.element_.c[0] = (numToWrite_masked) << shift;

	return res;
}

/**
 * Let "e" = \f$ x^{0} b_0 + x^{1} b_1 + x^{2} b_{2} \dots \f$
 * (as a polynomial over GF(2) and powers of "x" as the standard basis)
 * Lets name "shift" as S , and "numBits" as N
 *
 *  The returned value is the integer \f$ 2^0  b_{S} + 2^1  b_{S+1} + \dots + 2^k  b_{S+N-1} \f$

 **/

inline size_t mapFieldElementToInteger(const size_t shift, const size_t numBits, const FieldElement& e){
    size_t result = (e.element_.c[0]>>shift);
	if (numBits < 64){
		const size_t mask = (size_t(1) << numBits) - 1;
		result &=  mask;
	}

    return result;
 }

/** Constants for 0 and 1 */

inline const FieldElement zero(){
    const static FieldElement ZERO = mapIntegerToFieldElement(0,0,0);
    return ZERO;
} 

inline const FieldElement one(){
    const static FieldElement ONE = mapIntegerToFieldElement(0,1,1);
    return ONE;
}

inline const FieldElement xFE() {
    const static FieldElement X = mapIntegerToFieldElement(1,1,1);
    return X;
}

/** Functions that return random elements */
FieldElement generateRandom();

/**
 * Let \f$ B=(B_0, B_1, \dots , B_n) \f$ be the ordered basis
 * and let \f$ S \f$ be the affine shift.
 * This mapping maps the integer \f$ \sum_{i=0}^n b_i \cdot 2^i \f$ where \f$ \forall i : b_i \in \{0,1\} \f$
 * to the element \f$ S + \sum_{i=0}^n b_i \cdot B_i \f$
 */
FieldElement getSpaceElementByIndex(const std::vector<FieldElement>& orderedBasis, const FieldElement& affineShift, const size_t index);

/**
 * Let \f$ B=(B_0, B_1, \dots , B_n) \f$ be the ordered basis
 * and let \f$ S \f$ be the affine shift.
 * This mapping maps the element \f$ S + \sum_{i=0}^n b_i \cdot B_i \f$ where \f$ \forall i : b_i \in \{0,1\} \f$
 * to the integer \f$ \sum_{i=0}^n b_i \cdot 2^i \f$
 */
size_t getSpaceIndexOfElement(const std::vector<FieldElement>& orderedBasis, const FieldElement& affineShift, const FieldElement& e);

} //namespace Algebra

#endif   // __Field_Element_HPP__
