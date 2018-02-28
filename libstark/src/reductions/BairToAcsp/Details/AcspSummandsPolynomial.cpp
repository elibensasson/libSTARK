/** AcspSummandsPolynomial.cpp       **/
/*  This class is designed to hold a multivariate-polynomial precisely of the form that arises in the BS/BGHSP PCP.
For more details see hpp file
*/

#include "AcspSummandsPolynomial.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Utils/ErrorHandling.hpp"
#include <exception>

using Algebra::FieldElement;
using Algebra::SubspacePolynomial;
using Algebra::elementsSet_t;
using Algebra::PolynomialInterface;
using Algebra::PolynomialDegree;

using std::vector;
using std::unique_ptr;
using std::max;
using std::string;
using std::exception;

namespace{
class not_a_polynomial_exeption : public exception{};
}

namespace libstark{
namespace BairToAcsp{

typedef AcspSummandsPolynomial::polyID polyID;

    /// Creates a new AcspSummandsPolynomial, and sets \f$H\f$ to be \f$ span( spanSetH ) \f$
	AcspSummandsPolynomial::AcspSummandsPolynomial(const Algebra::elementsSet_t& spanSetH): Z_H(spanSetH) {};

	//Copy constructor
	AcspSummandsPolynomial::AcspSummandsPolynomial(const AcspSummandsPolynomial& that):Z_H(that.Z_H),subspacePolys(that.subspacePolys),denomsVec_(that.denomsVec_),summands(that.summands){
		

	}

    
	/// adds a subspace polynomial vanishing on T into subspacePolys and returns its id, i.e, its index in subspacePolys
	polyID  AcspSummandsPolynomial::addSubspacePoly(const Algebra::elementsSet_t& T){
        ///verify \f$ span(T) \subset H \f$ by checking \f$ \forall t \in T : Z_H(t) = 0 \f$
        {
            const Algebra::FieldElement ZERO = Algebra::zero();
            for(const auto& t : T){
                if (Z_H.eval(t) != ZERO) _COMMON_FATAL("T must be a subset of H");
            }
        }
        
        subspacePolys.push_back(SubspacePolynomial(T));
		return subspacePolys.size() -1;
		
	}

	/// evaluates this polynomial on \f$ a \f$
	FieldElement AcspSummandsPolynomial::eval(const EvaluationPoint& a)const {
            return evalOnSet({a})[0];
	}

    size_t AcspSummandsPolynomial::numVars()const {
        _COMMON_FATAL("Not implemented");
    }
    
    vector<FieldElement> AcspSummandsPolynomial::evalOnSet(const vector<vector<FieldElement>>& x_set)const{
		
        try{
            vector<FieldElement> res = evalDividedByZH(x_set,true);
            for(size_t i=0; i<x_set.size(); i++){
                res[i] *= Z_H.eval(x_set[i][0]);
            }
            return res;
        }
        catch(not_a_polynomial_exeption){
            _COMMON_FATAL("AcspSummand is not polynomial");
        }
    
    }

    /// check if the assignment is a root
    bool AcspSummandsPolynomial::isRoot(const EvaluationPoint& a)const{
        const Algebra::FieldElement ZERO = Algebra::zero();
        try{
            if(evalDividedByZH({a},true)[0] == ZERO)return true;
        }
        catch(not_a_polynomial_exeption){
            return false;
        }

        return Z_H.eval(a[0]) == ZERO;
    }

	/// evaluated this polynomial divided by \f$ Z_H \f$ on \f$ a \f$ for every \f$a \in a_set \f$
	vector<FieldElement>  AcspSummandsPolynomial::evalDividedByZH(const vector<EvaluationPoint>& a_set,const bool allowEvalIn_H)const{
	    
        //constants
        static const FieldElement ZERO = Algebra::zero();
        static const FieldElement ONE = Algebra::one();
         
		//it pays to store in advance the values of the different subspace polys on a
		//as each such value appears in many summands
		vector<vector<FieldElement>> subspaceValues(a_set.size());

        for (size_t j =0; j< a_set.size(); j++){
            subspaceValues[j].resize(subspacePolys.size());
            for (polyID i = 0; i < subspacePolys.size(); i++){
                subspaceValues[j][i] = subspacePolys[i].eval(a_set[j][0]);
            }
        }

        //it pays of to do all the needed inversions together
        vector<FieldElement> valsToInverse(denomsVec_.size()*a_set.size());
        
        for (size_t j =0; j< a_set.size(); j++){
            for (polyID i=0; i<denomsVec_.size(); i++){
                const size_t valToInv_index = j*denomsVec_.size()+i;
                valsToInverse[valToInv_index] = subspaceValues[j][denomsVec_[i].first] - denomsVec_[i].second;
                
                if (allowEvalIn_H && valsToInverse[valToInv_index] == ZERO){
                    valsToInverse[valToInv_index] = ONE;
                }
                
                _COMMON_ASSERT(valsToInverse[valToInv_index] != ZERO,"can't invert zero");
            }
        }
        
        const vector<FieldElement> invRes = Algebra::invertPointwise(valsToInverse);
        vector<vector<FieldElement>> denomsVals(a_set.size());

        for (size_t j =0; j< a_set.size(); j++){
            denomsVals[j].resize(denomsVec_.size());
            for (polyID i=0; i<denomsVec_.size(); i++){
                const size_t valToInv_index = j*denomsVec_.size()+i;
                denomsVals[j][i] = invRes[valToInv_index];
                
                if (allowEvalIn_H && subspaceValues[j][denomsVec_[i].first] == denomsVec_[i].second){
                    denomsVals[j][i] = ZERO;
                }
            }
        }


		vector<FieldElement> res(a_set.size(),ZERO);

		for (unsigned int i = 0;i<  summands.size(); i++){
            for( size_t j=0; j< a_set.size(); j++){
                res[j] += summands[i].eval(a_set[j], subspaceValues[j], denomsVals[j]);
            }
        }
		
        return res;
	}



	/// Next two methods commented out, see explanation above
	/// construct the summand 1/(Z_S+c) * P_i, where c=Z_S(shift)
    /// It is enforced that \f$ shift \in H \f$ (otherwise _COMMON_FATAL is called)
	void AcspSummandsPolynomial::addSummand(const polyID Z_S, const FieldElement& shift, const Constraint& P_i){
        ///verify \f$ shift \in H \f$ by checking \f$ Z_H(shift) = 0 \f$
        {
            const Algebra::FieldElement ZERO = Algebra::zero();
            if (Z_H.eval(shift) != ZERO) _COMMON_FATAL("shift must be an element of H");
        }
		const FieldElement c = subspacePolys[Z_S].eval(shift);
		summands.push_back(AcspConstraintSummand(denomsVec_.size(), P_i));
        denomsVec_.push_back(DenominatorType(Z_S,c));
	}

	/// construct the summand 1/(Z_S+c) * P_i * W, where  c=Z_S(shift) and W is the product of polynomials in auxPolys
    /// It is enforced that \f$ shift \in H \f$ and all the shifts are in \f$ H \f$ as well (otherwise _COMMON_FATAL is called)
	void AcspSummandsPolynomial::addSummand(const polyID Z_S, const FieldElement& shift, const Constraint& P_i, const AuxPolyVec& auxVec){
        const FieldElement ZERO = Algebra::zero();
        
		///verify \f$ shift \in H \f$ by checking \f$ Z_H(shift) = 0 \f$
        {
            if (Z_H.eval(shift) != ZERO) _COMMON_FATAL("shift must be an element of H");
        }
        
		vector<FieldElement> shifts;
		vector<polyID> auxPolys;
        ///verify all shifts are in \f$ H \f$, and, on the way, insert their images into the shifts vector
        {

            for(const auto& s : auxVec){
                if (Z_H.eval(s.second) != ZERO) _COMMON_FATAL("all shifts must be in H");
				const FieldElement polyShift = subspacePolys[s.first].eval(s.second);
				shifts.push_back(polyShift);
		
				auxPolys.push_back(s.first);
            }
        }
        const FieldElement c = subspacePolys[Z_S].eval(shift);
		summands.push_back(AcspConstraintSummand(denomsVec_.size(), P_i, auxPolys,shifts));
        denomsVec_.push_back(DenominatorType(Z_S,c));
	}
        
    /**
     * @brief return a clone of the current polynomial
     * @return a unique_ptr of PolynomialInterface,
     * representing a polynomial equivalent to current
     */
    unique_ptr<PolynomialInterface> AcspSummandsPolynomial::clone()const{
        return unique_ptr<PolynomialInterface>(new AcspSummandsPolynomial(*this));
    }

    /**
     * @brief   Returns an upper bound for degree of this polynomial
     * when composed with univariate polynomials (as its variables)
     * @param   degrees vector of univariate polynomial, name it \f$\vec{d}\f$
     * @return  a degree \f$D\f$ such that
     * \f$ D \ge \deg P\circ\vec{Q} \f$ when \f$P\f$ is the current polynomial,
     * each \f$Q_i\f$ is a univariate polynomial of degree \f$d_i\f$ at most,
     * and \f$(P\circ\vec{Q})(x) = P(Q_1(x),Q(2),\dots,Q_n(x))\f$
     */
    PolynomialDegree AcspSummandsPolynomial::getDegreeBound(const vector<PolynomialDegree>& inputDegreesBound)const{
        PolynomialDegree maxSummandDeg(-1);

        for(const auto& summand : summands){
            const PolynomialDegree currDeg = summand.getDegreeBound(inputDegreesBound);
            maxSummandDeg = max(currDeg,maxSummandDeg);
        }
        
        return degreeOfProduct(Z_H.getDegree(),maxSummandDeg);
    }
    
    /**
     * @brief   Returns an upper bound for degree of this polynomial divided by Z_H
     * when composed with univariate polynomials (as its variables)
     * @param   degrees vector of univariate polynomial, name it \f$\vec{d}\f$
     * @return  a degree \f$D\f$ such that
     * \f$ D \ge \deg P\circ\vec{Q} \f$ when \f$P\f$ is the current polynomial,
     * each \f$Q_i\f$ is a univariate polynomial of degree \f$d_i\f$ at most,
     * and \f$(P\circ\vec{Q})(x) = P(Q_1(x),Q(2),\dots,Q_n(x))\f$
     */
    PolynomialDegree AcspSummandsPolynomial::getDegreeBound_DividedByZH(const vector<PolynomialDegree>& inputDegreesBound)const{
        PolynomialDegree maxSummandDeg(-1);

        for(const auto& summand : summands){
            const PolynomialDegree currDeg = summand.getDegreeBound(inputDegreesBound);
            maxSummandDeg = max(currDeg,maxSummandDeg);
        }
        
        return maxSummandDeg;
    }
	


	/***AcspConstraintSummand class  methods ****/

	//move constructor
	AcspSummandsPolynomial::AcspConstraintSummand::AcspConstraintSummand(AcspConstraintSummand&& rhs) : 
        denominatorIndex_(std::move(rhs.denominatorIndex_)),
        P_i(std::move(rhs.P_i)),
        auxPolys(std::move(rhs.auxPolys)),
        shifts(std::move(rhs.shifts)){
		}
		

	//construct the summand 1/(Z_S+c) * P_i. Note that P_i is without const - cause it's a unique_ptr, so it needs to be moved and destroyed.
	AcspSummandsPolynomial::AcspConstraintSummand::AcspConstraintSummand(const polyID denomIndex ,  const Constraint& P_i):
        denominatorIndex_(denomIndex),P_i(P_i.clone()){};

	//construct the summand 1/(Z_S+c) * P_i * W, where W is the product of polynomials in auxPolys
	AcspSummandsPolynomial::AcspConstraintSummand::AcspConstraintSummand(const polyID denomIndex, const Constraint& P_i, const vector<polyID>& auxPolys, const vector<FieldElement>& shifts):
        denominatorIndex_(denomIndex),
        P_i(P_i.clone()),
        auxPolys(auxPolys),
        shifts(shifts){};
   
    //copy constructor
    AcspSummandsPolynomial::AcspConstraintSummand::AcspConstraintSummand(const AcspConstraintSummand& src):
        denominatorIndex_(src.denominatorIndex_),
        P_i(src.P_i->clone()),
        auxPolys(src.auxPolys),
        shifts(src.shifts){};


	/// evaluates the summand on a given point a - important point evaluates without the \f$ Z_H \f$ part!
	/// it receives as input the array subspaceValues containing the evaluations on a of the subspace polys
	/// associated with the AcspConstraintPolynomial object
	
    FieldElement AcspSummandsPolynomial::AcspConstraintSummand::eval(const EvaluationPoint& a, const vector<FieldElement>& subspaceValues, const vector<FieldElement>& denomsValues) const{
        static const FieldElement ZERO = Algebra::zero();

		FieldElement res = P_i->eval(a);
		
        for (unsigned int i = 0; i < auxPolys.size(); i++){
            res *= subspaceValues[auxPolys[i]]+shifts[i];
        }
		 
         FieldElement d = denomsValues[denominatorIndex_];
		 
		 if ((d == ZERO) && (res != ZERO)){
            throw not_a_polynomial_exeption();
         }
		 return res*d;

	}

    /**
     * @brief   Returns an upper bound for degree of this polynomial
     * when composed with univariate polynomials (as its variables)
     * @param   degrees vector of univariate polynomial, name it \f$\vec{d}\f$
     * @return  a degree \f$D\f$ such that
     * \f$ D \ge \deg P\circ\vec{Q} \f$ when \f$P\f$ is the current polynomial,
     * each \f$Q_i\f$ is a univariate polynomial of degree \f$d_i\f$ at most,
     * and \f$(P\circ\vec{Q})(x) = P(Q_1(x),Q(2),\dots,Q_n(x))\f$
     */
    PolynomialDegree AcspSummandsPolynomial::AcspConstraintSummand::getDegreeBound(const vector<PolynomialDegree>& inputDegreesBound)const{
        //we return only the degree of the constraint P_i, it uses all inputs but the first.
        //it is assumed the degree of the product of the auxPolys is lower than the degree of Z_S
        vector<PolynomialDegree> P_inputsDeg;

        //return the degree bound
		//if (P_i->getDegreeBound(inputDegreesBound)>0)//ARIEL
		//std::cout << inputDegreesBound[0] << inputDegreesBound[1] << inputDegreesBound[2];//ARIEL
		//std::cout << "P_i-deg:" << P_i->getDegreeBound(inputDegreesBound) << std::endl; //ARIEL
        return P_i->getDegreeBound(inputDegreesBound);
    }


} //namespace BairToAcsp
} //namespace libstark
