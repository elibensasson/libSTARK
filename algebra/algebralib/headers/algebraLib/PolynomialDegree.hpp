#ifndef  __PolynomialDegree_HPP
#define  __PolynomialDegree_HPP

namespace Algebra {

/**
 * @class PolynomialDegree
 * @brief The degree of a polynomial can
 * be a natural integer, or \f$ -\infty\f$
 * (only for the zero polynomial).
 * There are some arithmetic expectations
 * from the degree, for example, the degree
 * of a product of polynomials is the summary
 * of their degrees.
 * Such an equality can not be defined using
 * integers only.
 * This class should provide a representation
 * for polynomial degree with implementation
 * for arithmetic operators that provides
 * expected results
 */
class PolynomialDegree {
public:
	typedef long long integral_t;
	/**
	 * @brief   Constructor of degree from an integer
	 * @param   degree, if non-negative construct a
	 * PolynomialDegree with same value, otherwise
	 * constructs the \f$ -\infty \f$ degree
	 */
	explicit PolynomialDegree(const integral_t degree);

    /**
     * Construct the degree for -infinity
     */
    static PolynomialDegree getZeroPolyDegree(){
        return PolynomialDegree(-1);
    }

	/**
	 * @brief   Checks if the degree is an integer (not \f$ -\infty \f$)
	 * @return  True iff the degree is an integer
	 */
	bool isInteger()const;

	/**
	 * @brief   conversion to int
	 * @return  The degree if it is an integer, a negative value otherwise
	 */
	explicit operator integral_t()const;

    /**
     * checks if a polynomial of such degree is constant
     */
    bool isConstantPolyDeg()const{
        return (degree_ < 1);
    }

private:
	integral_t degree_;
};

bool operator<(const PolynomialDegree& d1, const PolynomialDegree& d2);
bool operator==(const PolynomialDegree& d1, const PolynomialDegree& d2);
bool operator!=(const PolynomialDegree& d1, const PolynomialDegree& d2);

PolynomialDegree degreeOfProduct(const PolynomialDegree& d1, const PolynomialDegree& d2);

//Return the degree bound of polyA(polyB)
PolynomialDegree degreeOfComposition(const PolynomialDegree& polyA_deg, const PolynomialDegree& polyB_deg);

} // namespace Algebra


#endif   // __PolynomialDegree_HPP
