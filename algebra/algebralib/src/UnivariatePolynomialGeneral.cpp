#include "algebraLib/UnivariatePolynomialGeneral.hpp"
#include "algebraLib/FFT.hpp"
#include "algebraLib/ErrorHandling.hpp"

#include <Polynomials.h>

#include <memory>
#include <math.h>

using std::unique_ptr;
using std::vector;
using std::max;

namespace Algebra{
			
UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(const UnivariatePolynomialInterface& src):polynomial_(src.getCoefficients()){ }

unique_ptr<PolynomialInterface> UnivariatePolynomialGeneral::clone()const{
    return unique_ptr<PolynomialInterface>(new UnivariatePolynomialGeneral(*this));
}

UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(const FieldElement& constant):polynomial_(1) {
	setCoefficient(0,constant);
}

UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(const vector<FieldElement>& coeffs):polynomial_(coeffs) {};
UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(vector<FieldElement>&& coeffs):polynomial_(std::move(coeffs)) {};

namespace{
//multiply poly by $x-r$
void multiplyByRootPoly(vector<FieldElement>& poly,const FieldElement& r){
    poly.resize(poly.size()+1,zero());
    for(int i=poly.size()-1; i>0; i--){
        poly[i] = poly[i-1] + r*poly[i];
    }
    poly[0] *= r;
}
}

UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(const elementsSet_t& roots):polynomial_(1) {
	polynomial_[0] = one();

	/**
	 * For each root \f$r\f$ generate
	 * the affine polynomial \f$q(x) = x - r\f$
	 * and multiply current polynomial 
	 * by it
	 */
	for(const auto& element : roots){
        multiplyByRootPoly(polynomial_,element);
	}
}

UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(const vector<FieldElement>& evaluation, const vector<FieldElement>& orderedBasis, const FieldElement& spaceShift): polynomial_(IFFT(evaluation,orderedBasis,spaceShift)){};

UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(const vector<FieldElement>& evaluation, const AffineSpace& evaluationSpace): polynomial_(IFFT(evaluation,evaluationSpace.getBasis(),evaluationSpace.getAffineShift())){};

UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(const evaluation_t& evaluationTabel, const elementsSet_t& spaceBasis){
    
    const short basisSize = spaceBasis.size();
    const size_t spaceSize = (1<<basisSize);
    vector<FieldElement> orderedBasis;
    for(const auto& b : spaceBasis) orderedBasis.push_back(b);

    //construct the evaluation
    vector<FieldElement> vals(spaceSize);
    for(size_t i=0; (i>>basisSize)==0; i++){
        const auto iter = evaluationTabel.find(getSpaceElementByIndex(orderedBasis, zero(), i));
        ALGEBRALIB_ASSERT(iter != evaluationTabel.end(),"Bad evaluation table, impossible interpolation");
        vals[i] = iter->second;
    }

    //compute the IFFT
    polynomial_ = IFFT(vals,orderedBasis,zero());
}

UnivariatePolynomialGeneral::UnivariatePolynomialGeneral(const evaluation_t& evaluationTabel){

    //use Lagrange interpolation
    for(const auto& point : evaluationTabel){
        
        //construct current lagrange poly
        UnivariatePolynomialGeneral lagrangePoly(one());
        FieldElement denumenator = one();
        for(const auto& other_point : evaluationTabel){
            if(other_point.first != point.first){
            denumenator *= (point.first - other_point.first);
            multiplyByRootPoly(lagrangePoly.polynomial_,other_point.first);
            }
        }

        lagrangePoly.multiply(point.second/denumenator);

        //add to curr poly
        add(lagrangePoly);
    }
}
                        

FieldElement UnivariatePolynomialGeneral::eval(const FieldElement& x)const {
	//use horner evaluation
    FieldElement res = zero();
    for(int i=polynomial_.size()-1; i>=0; i--){
        res = res*x + polynomial_[i];
    }
    
    return res;
}

std::vector<FieldElement> UnivariatePolynomialGeneral::eval(const std::vector<FieldElement>& orderedBasis, const FieldElement& shift)const{
    return FFT(polynomial_,orderedBasis,shift);
}

const std::vector<FieldElement> UnivariatePolynomialGeneral::getCoefficients()const{
    return polynomial_;
}

namespace{
    ///Returns log in base 2
    double Log2( double n )  {
        return log(n) / log((double)2);  // log(n)/log(2) is ::Infrastructure::Log2.
    }
}

UnivariatePolynomialInterface* UnivariatePolynomialGeneral::eval(const UnivariatePolynomialInterface& p)const{
	const PolynomialDegree::integral_t maxDeg(degreeOfComposition(getDegree(), p.getDegree()));

    const size_t evalSpaceDim = ceil(Log2(maxDeg+1));
    const elementsSet_t basisSet = getStandartBasis(evalSpaceDim);
    const vector<FieldElement> evalSpaceBasis(basisSet.begin(),basisSet.end());
    vector<FieldElement> evaluation(1L<<evalSpaceDim);
    for(unsigned int i=0; i< evaluation.size(); i++){
        const FieldElement x = getSpaceElementByIndex(evalSpaceBasis,zero(),i);
        evaluation[i] = eval(p.eval(x));
    }

    return new UnivariatePolynomialGeneral(evaluation,evalSpaceBasis,zero());
}

void UnivariatePolynomialGeneral::setCoefficient(const unsigned int index,const FieldElement& coefficient) {
    if (index >= polynomial_.size()){
        polynomial_.resize(index+1,zero());
    }
    
    polynomial_[index] = coefficient;
}

FieldElement UnivariatePolynomialGeneral::getCoefficient(const unsigned int index)const {
    if(index >= polynomial_.size()){
        return zero();
    }
    return polynomial_[index];
}

PolynomialDegree UnivariatePolynomialGeneral::getDegree()const {
    
    //if there are no coefficients this is the ZERO polynomial
    if(polynomial_.size() == 0){
        return PolynomialDegree::getZeroPolyDegree();
    }
    
    //find the highest non zero coefficient index
    for(int i=polynomial_.size()-1; i>=0; i--){
        if (polynomial_[i] != zero()){
            return PolynomialDegree(i);
        }
    }

    //no non-zero coefficients found
    return PolynomialDegree::getZeroPolyDegree();
}
	
unique_ptr<UnivariatePolynomialInterface> UnivariatePolynomialGeneral::divideByMe(const UnivariatePolynomialInterface& dividend)const {
   /*
    * Division implemented using the algorithm from Wikipedia:
    * https://en.wikipedia.org/wiki/Polynomial_long_division
    *
    * function n / d:
    * require d ≠ 0
    * q ← 0
    * r ← n       # At each step n = d × q + r
    * while r ≠ 0 AND degree(r) ≥ degree(d):
    *   t ← lead(r)/lead(d)     # Divide the leading terms
    *   q ← q + t
    *   r ← r − t * d
    * return (q, r)
    */

    const PolynomialDegree enumDeg = dividend.getDegree();
    const PolynomialDegree denumDeg = getDegree();

    ALGEBRALIB_ASSERT(denumDeg.isInteger(), "Divisor polynomial can not be the ZERO polynomial");
    
    //if the enumerator has lower degree then denumerator, return the ZERO polynomial
    if(enumDeg < denumDeg){
        return unique_ptr<UnivariatePolynomialInterface>(new UnivariatePolynomialGeneral());
    }

    const long long enumDegInt = PolynomialDegree::integral_t(enumDeg);
    const long long denumDegInt = PolynomialDegree::integral_t(denumDeg);
    const FieldElement denumLeadInv = one()/polynomial_[denumDegInt];

    vector<FieldElement> q((enumDegInt - denumDegInt) + 1, zero());
    vector<FieldElement> r = dividend.getCoefficients();
    long long r_deg = enumDegInt;

    while (r_deg >= denumDegInt){
        const FieldElement t_val = r[r_deg];
        const int t_shift = r_deg-denumDegInt;

        q[t_shift] += t_val;

        for(int i=0; i< denumDegInt; i++){
            r[i+t_shift] += t_val*polynomial_[i];
        }

        r_deg = -1;
        for(int i=denumDegInt+t_shift-1; i>=0; i--){
            if(r[i] != zero()){
                r_deg = i;
                break;
            }
        }
    }

    return unique_ptr<UnivariatePolynomialInterface>(new UnivariatePolynomialGeneral(q));
}

void UnivariatePolynomialGeneral::multiply(const UnivariatePolynomialGeneral& other){
    //use FFT for multiplication:
    //first evaluate both polynomials on a big enough space,
    //multiplicate the evaluations,
    //and interpolate it back using IFFT
	
    const PolynomialDegree degreeBound = degreeOfProduct(getDegree(),other.getDegree());

    //if expected degree is -infinity, this is the zero polynomial
    if(!degreeBound.isInteger()){
        polynomial_.resize(0);
        return;
    }
    
    //else find a big enough evaluation space
    const size_t space_dim = ceil(log2(1+ PolynomialDegree::integral_t(degreeBound)));
    const auto basis = getStandartBasis(space_dim);
    vector<FieldElement> orderedBasis(basis.begin(),basis.end());

    //evaluate both polys
    vector<FieldElement> eval1 = FFT(polynomial_,orderedBasis,zero());
    const vector<FieldElement> eval2 = FFT(other.polynomial_,orderedBasis,zero());


    //compute the product evaluation
#pragma omp parallel for
    for(unsigned long long i=0; i< eval2.size(); i++){
        eval1[i] *= eval2[i];
    }

    //interpolate to get the coefficients
    polynomial_ = IFFT(eval1,orderedBasis,zero());
}

void UnivariatePolynomialGeneral::multiply(const FieldElement& factor){

    //multiply the coefficients pointwise
#pragma omp parallel for
    for(unsigned long long i=0; i< polynomial_.size() ; i++){
        polynomial_[i] *= factor;
    }
}

void UnivariatePolynomialGeneral::multiply_by_monomial(const FieldElement& factor, const size_t deg){
    multiply(factor);
    const vector<FieldElement> padding(deg,zero());
    polynomial_.insert(polynomial_.begin(),padding.begin(),padding.end());
}

void UnivariatePolynomialGeneral::add(const UnivariatePolynomialGeneral& other){
    const PolynomialDegree degOfOther = other.getDegree();

    //if the other is the zero poly, nothing should be done
    if(!degOfOther.isInteger())return;

    //otherwise, the other polynomial has effective coefficients.
    //extend current poly len if needed
    const size_t otherNumCoeffs = 1+PolynomialDegree::integral_t(degOfOther);
    if(polynomial_.size() < otherNumCoeffs){
        polynomial_.resize(otherNumCoeffs,zero());
    }

    //add the coefficients pointwise
#pragma omp parallel for
    for(unsigned long long i=0; i< otherNumCoeffs ; i++){
        polynomial_[i] += other.polynomial_[i];
    }
}

UnivariatePolynomialGeneral operator+(const UnivariatePolynomialGeneral& a, const UnivariatePolynomialGeneral& b) { 
    UnivariatePolynomialGeneral res(a);
    res.add(b);
    return res;
}

UnivariatePolynomialGeneral& operator+=(UnivariatePolynomialGeneral& x, const UnivariatePolynomialGeneral& b) {
    x.add(b);
    return x;
}

UnivariatePolynomialGeneral operator-(const UnivariatePolynomialGeneral& a, const UnivariatePolynomialGeneral& b)
{ return a+b; }

UnivariatePolynomialGeneral operator*(const UnivariatePolynomialGeneral& a, const UnivariatePolynomialGeneral& b) {
    UnivariatePolynomialGeneral res(a);
    res.multiply(b);
    return res;
}

UnivariatePolynomialGeneral& operator*=(UnivariatePolynomialGeneral& x, const UnivariatePolynomialGeneral& b) { 
    x.multiply(b);
    return x;
}

UnivariatePolynomialGeneral operator*(const UnivariatePolynomialGeneral& poly, const FieldElement& factor) {
    UnivariatePolynomialGeneral res(poly);
    res.multiply(factor);
    return res;
}

UnivariatePolynomialGeneral operator*(const FieldElement& factor, const UnivariatePolynomialGeneral& poly) {
    return poly*factor;
}

UnivariatePolynomialGeneral& operator*=(UnivariatePolynomialGeneral& poly, const FieldElement& factor) { 
    poly.multiply(factor);
    return poly;
}

bool operator==(const UnivariatePolynomialGeneral& a, const UnivariatePolynomialGeneral& b) { 
    //if the degrees differ, they can't be the same
    const PolynomialDegree aDeg = a.getDegree();
    const PolynomialDegree bDeg = b.getDegree();
    if(aDeg != bDeg)return false;

    const size_t numCoeffs = max(PolynomialDegree::integral_t(0),1+PolynomialDegree::integral_t(aDeg));
    for(size_t i=0; i< numCoeffs; i++){
        if(a.getCoefficient(i) != b.getCoefficient(i))return false;
    }

    //if difference not found
    return true;
}

bool operator!=(const UnivariatePolynomialGeneral& a, const UnivariatePolynomialGeneral& b)
{ return !(a == b); }

std::ostream& operator<<(std::ostream& s, const UnivariatePolynomialGeneral& a) {
    for(int i=0; i<= PolynomialDegree::integral_t(a.getDegree()); i++){
            s << a.getCoefficient(i); 
    }
    return s;
}

} //namespace Algebra
