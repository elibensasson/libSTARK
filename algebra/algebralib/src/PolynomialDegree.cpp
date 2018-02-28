#include "algebraLib/PolynomialDegree.hpp"
#include <algorithm>
#include <math.h>

using std::max;

namespace Algebra {
	
PolynomialDegree::PolynomialDegree(const integral_t degree):degree_(degree) {
	if (degree_ < 0) degree_ = -1;
};

bool PolynomialDegree::isInteger()const{
	return degree_ >= 0;
}

PolynomialDegree::operator integral_t()const {
	return degree_;
}

PolynomialDegree degreeOfProduct(const PolynomialDegree& d1, const PolynomialDegree& d2){
    //if at least one of the degrees is -infinity
    //return -infinity
    if(!d1.isInteger() || !d2.isInteger())return PolynomialDegree::getZeroPolyDegree();

    //else, both degrees are integers, and the degree of the product
    //is their sum
    return PolynomialDegree(PolynomialDegree::integral_t(d1) +  PolynomialDegree::integral_t(d2));
}

//Return the degree bound of polyA(polyB)
PolynomialDegree degreeOfComposition(const PolynomialDegree& polyA_deg, const PolynomialDegree& polyB_deg){
	//if A is the Zero polynomial (it has -infinity degree)
	//return -infinity
	if (!polyA_deg.isInteger()) return PolynomialDegree::getZeroPolyDegree();

	//else if B is a constant polynomial (of degree at most 0)
	//the composition is constant as well, so return 0
	if (PolynomialDegree::integral_t(polyB_deg) <= 0) return PolynomialDegree(0);

	//else, both degrees are non-negative, the degree of the composition
	//is their product
	return PolynomialDegree(PolynomialDegree::integral_t(polyA_deg)*PolynomialDegree::integral_t(polyB_deg));
}

bool operator<(const PolynomialDegree& d1, const PolynomialDegree& d2){
	return PolynomialDegree::integral_t(d1) < PolynomialDegree::integral_t(d2);
}

bool operator==(const PolynomialDegree& d1, const PolynomialDegree& d2){
    if(d1.isInteger() && d2.isInteger()){
		return PolynomialDegree::integral_t(d1) == PolynomialDegree::integral_t(d2);
    }
    else{
        return !(d1.isInteger() || d2.isInteger());
    }
}

bool operator!=(const PolynomialDegree& d1, const PolynomialDegree& d2){
    return !(d1 == d2);
}

} // namespace Algebra 
