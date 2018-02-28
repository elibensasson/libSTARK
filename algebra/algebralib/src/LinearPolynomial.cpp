#include "algebraLib/LinearPolynomial.hpp"

namespace Algebra{

using std::unique_ptr;
	
unique_ptr<PolynomialInterface> LinearPolynomial::clone()const{
    return unique_ptr<PolynomialInterface>(new LinearPolynomial(*this));
}

FieldElement LinearPolynomial::getCoefficient(const unsigned int index)const{
    switch (index){
        case 0 : return coefficient0_;
        case 1 : return coefficient1_;
        default : return zero();
    }
}

PolynomialDegree LinearPolynomial::getDegree()const{
    if(coefficient1_ != zero()) return PolynomialDegree(1);
    if(coefficient0_ != zero()) return PolynomialDegree(0);
    return PolynomialDegree(-1); 
}

LinearPolynomial LinearPolynomial::compose(const LinearPolynomial& p)const{
    return LinearPolynomial(coefficient0_+coefficient1_*p.coefficient0_, coefficient1_*p.coefficient1_);
}

bool operator==(const LinearPolynomial& p1, const LinearPolynomial& p2){
    return (p1.getCoefficient(0) == p2.getCoefficient(0)) && (p1.getCoefficient(1) == p2.getCoefficient(1));
}

LinearPolynomial operator+(const LinearPolynomial& p, const FieldElement& c){
    return LinearPolynomial(p.getCoefficient(0) + c, p.getCoefficient(1));
}

} // namespace Algebra
