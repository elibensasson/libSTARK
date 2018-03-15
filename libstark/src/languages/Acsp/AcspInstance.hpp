/**
 *       @file  AcspInstance.hpp
 *      @brief  Header file for Acsp Partial Instance
 *
 *	Contains Acsp Partiial instance definitions
 *
 *     @author  Michael Riabzev, RiabzevMichael@gmail.com
 * =====================================================================================
 */
#pragma once 

#ifndef  __Acsp_Instance_HPP
#define  __Acsp_Instance_HPP

#include "AcspWitness.hpp"
#include "common/langCommon/Sequence.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include <algebraLib/FieldElement.hpp>
#include <algebraLib/UnivariatePolynomialGeneral.hpp>
#include "common/Algebra/FiniteSetInterface.hpp"
#include <vector>
#include <memory>
#include <cassert>
#include <functional>

namespace libstark {

/**
 * @class AcspInstance
 * @brief class for Acsp Instance
 *
 * An Acsp partial instance is a tuple \f$(\mathbb{F},H,\vec{N},P,WitnessDegreeBound,B)\f$ such that: \n
 * \f$\mathbb{F}\f$ is a field
 *
 * \f$H \subset \mathbb{F}\f$ is a subset of \f$ \mathbb{F} \f$
 *
 * \f$\vec{N} \in \mathbb{F}[x]^k\f$ is a vector of univariate polynomials over \f$ \mathbb{F} \f$
 *
 * \f$P \in \mathbb{F}[x_0,x_1,x_2,\dots,x_k]\f$ is a multivariate polynomial over \f$ \mathbb{F} \f$
 *
 * \f$WitnessDegreeBound \in \mathbb{N}\f$ is an apper bound for satisfying witness
 *
 * \f$B \subset \mathbb{F} \times \amthbb{F}\f$ is the boundry constraints set
 *
 *
 * For a polynomial \f$Q \in \mathbb{F}[x]\f$ we define \f$P \circ (x \vert Q \circ \vec{N}) \in \mathbb{F}[x]\f$ by
 * \f$(P \circ (x \vert Q \circ \vec{N}))(x) := P(x,Q(N_1(x)),Q(N_2(x)),\dots,Q(N_k(x)))\f$
 *
 * An Acsp partial instance \f$(\mathbb{F},H,\vec{N},P,\lambda,I)\f$ is satisfiable partial instance if and only if 
 * there exists \f$A \in \mathbb{F}[x]\f$ and \f$ \vec{x} \in \mathbb{F}^n \f$ (for some \f$ n \in \mathbb{N} \f$) such that:
 * \f{eqnarray*}{
 * \forall z \in H: (P \circ (x \vert A \circ \vec{N}))(z) = 0 \\
 * \deg A <= WitnessDegreeBound \\
 * \forall (x,y) \in B : A(x)=y \\
 * \f}
 *
 * In the code we give more descriptive names:
 *  
 * we name \f$ \mathbb{F} \f$ as 'contextField' \n
 * we name \f$H\f$ as 'vanishingSet' \n 
 * we name \f$N\f$ as 'neighborPolys' \n
 * we name \f$P\f$ as 'constraintPoly' \n
 * we name \f$B\f$ as 'boundaryConstraints' \n
 *
 *
 * Methods:\n
 * Instance class contains only getters, 
 * constructor and a destructor.
 */

class AcspInstance {
public:
	typedef Algebra::FiniteSetInterface set;
	typedef Algebra::UnivariatePolynomialInterface uniPoly;
	typedef std::vector<std::unique_ptr<const uniPoly>> polynomialsVec;
	typedef Algebra::PolynomialInterface polynomial;
	typedef Algebra::FieldElement fieldElement;
    typedef std::map<Algebra::FieldElement, Algebra::FieldElement, Algebra::classCompElements> boundaryConstraints_t;
    typedef std::function< std::vector<Algebra::FieldElement>(const AcspInstance&, const AcspWitness&, const std::vector<Algebra::FieldElement>& basis, const Algebra::FieldElement& shift, const bool witnessIsEvaluation)> compAlgorithm_t;
    typedef std::function< std::vector<std::unique_ptr<uniPoly>>(const AcspInstance&, const AcspWitness&)> witnessCheckerHelperAlgorithm_t;

	AcspInstance(
		std::unique_ptr<const set>&& vanishingSet,
		polynomialsVec&& neighborPolys,
		std::unique_ptr<const polynomial>&& constraintPoly,
        const Algebra::PolynomialDegree& witnessDegreeBound, 
		const boundaryConstraints_t& boundaryConstraints,
        const compAlgorithm_t& compositionAlgorithm = naiveComposition_N_division_Alg,
        const witnessCheckerHelperAlgorithm_t& witnessCheckerHelperAlgorithm = naiveWCHelper
		)
		:
			vanishingSet_(std::move(vanishingSet)),
			neighborPolys_(0),
			constraintPoly_(std::move(constraintPoly)),
            witnessDegreeBound_({witnessDegreeBound}),
			boundaryConstraints_({boundaryConstraints}),
            compositionAlgorithm_(compositionAlgorithm),
            witnessCheckerHelperAlgorithm_(witnessCheckerHelperAlgorithm)
			{
                neighborPolys_.push_back(std::move(neighborPolys));
            }
	
	AcspInstance(
		std::unique_ptr<const set>&& vanishingSet,
		std::vector<polynomialsVec>&& neighborPolys,
		std::unique_ptr<const polynomial>&& constraintPoly,
        const std::vector<Algebra::PolynomialDegree>& witnessDegreeBound, 
		const std::vector<boundaryConstraints_t>& boundaryConstraints,
        const compAlgorithm_t& compositionAlgorithm = naiveComposition_N_division_Alg,
        const witnessCheckerHelperAlgorithm_t& witnessCheckerHelperAlgorithm = naiveWCHelper
		)
		:
			vanishingSet_(std::move(vanishingSet)),
			neighborPolys_(std::move(neighborPolys)),
			constraintPoly_(std::move(constraintPoly)),
            witnessDegreeBound_(witnessDegreeBound),
			boundaryConstraints_(boundaryConstraints),
            compositionAlgorithm_(compositionAlgorithm),
            witnessCheckerHelperAlgorithm_(witnessCheckerHelperAlgorithm)
			{};
	
	AcspInstance(AcspInstance&& src) = default;

	AcspInstance() = delete;
	AcspInstance(const AcspInstance& src) = delete;
    
	inline const set& vanishingSet()const {
		return *(vanishingSet_.get());
	}

	inline const std::vector<polynomialsVec>& neighborPolys()const {
		return neighborPolys_;
	}

	inline const polynomial& constraintPoly()const {
		return *(constraintPoly_.get());
	}

    inline const std::vector<boundaryConstraints_t>& boundaryConstraints()const{
        return boundaryConstraints_;
    }
    
    inline const std::vector<Algebra::PolynomialDegree> witnessDegreeBound()const{
        return witnessDegreeBound_;
    }

    inline std::vector<Algebra::FieldElement> composeWithWitness_and_divideByVanishingSpacePoly(const AcspWitness& witness, const std::vector<Algebra::FieldElement>& basis, const Algebra::FieldElement& shift, const bool witnessIsEvaluation = false)const{
        return compositionAlgorithm_(*this,witness,basis,shift,witnessIsEvaluation);
    }
	
    inline std::vector<std::unique_ptr<uniPoly>> prepareForWitnessChecker(const AcspWitness& witness)const{
        return witnessCheckerHelperAlgorithm_(*this,witness);
    }
	
    private:

	std::unique_ptr<const set> vanishingSet_;
	std::vector<polynomialsVec> neighborPolys_;
	std::unique_ptr<const polynomial> constraintPoly_;
	std::vector<Algebra::PolynomialDegree> witnessDegreeBound_;
    std::vector<boundaryConstraints_t> boundaryConstraints_;

    //A hint for fast construction of the composition polynomial
    compAlgorithm_t compositionAlgorithm_; 

    //A hint for the deterministic witness checker for faster check if vanishing on vanishing set
    witnessCheckerHelperAlgorithm_t witnessCheckerHelperAlgorithm_;

    static std::vector<Algebra::FieldElement> naiveComposition_N_division_Alg(const AcspInstance& instance, const AcspWitness& witness, const std::vector<Algebra::FieldElement>& basis, const Algebra::FieldElement& shift, const bool witnessIsEvaluation){
        using std::vector;
        using std::min;
        using std::unique_ptr;
        using Algebra::FieldElement;
        using Algebra::PolynomialDegree;
        using Algebra::zero;
        using Algebra::UnivariatePolynomialGeneral;
       
        //
        //get the composition degree bound
        //
        vector<PolynomialDegree> constraintsInputDegrees;

        // first input is "x" which has degree 1
        constraintsInputDegrees.push_back(PolynomialDegree(1));

        // rest are composition of neighbor with witness
        for(size_t wIndex = 0; wIndex < witness.assignmentPolys().size(); wIndex++){
            const auto witnessDegree = witness.assignmentPolys()[wIndex]->getDegree();
            for (const auto& n : instance.neighborPolys()[wIndex]){
                constraintsInputDegrees.push_back(n->getDegreeBound(witnessDegree));
            }
        }

        // get the composition degree bound
        const PolynomialDegree degBound = instance.constraintPoly().getDegreeBound(constraintsInputDegrees);

        //
        //define interpolation space
        //
        const size_t degForPoly = degBound.isInteger()? ceil(log2(1+PolynomialDegree::integral_t(degBound))) : 0;
        const auto interpolationBasis = Algebra::getStandartOrderedBasis(min(Algebra::ExtensionDegree, int(degForPoly)));

        //construct evaluation
        vector<FieldElement> evaluation(Infrastructure::POW2(interpolationBasis.size()));
        for(size_t i=0; i< evaluation.size(); i++){
            const FieldElement x = getSpaceElementByIndex(interpolationBasis,zero(),i);
            
            //construct the assignment for the constraints poly
            vector<FieldElement> assignment;
            assignment.push_back(x);
            for(size_t wIndex = 0; wIndex < witness.assignmentPolys().size(); wIndex++){
                for(const auto& n : instance.neighborPolys()[wIndex]){
                    assignment.push_back(witness.assignmentPolys()[wIndex]->eval(n->eval(x)));
                }
            }

            //evaluate and return
            evaluation[i] = instance.constraintPoly().eval(assignment);
        }

        UnivariatePolynomialGeneral compositionPoly(evaluation,interpolationBasis,zero());

        //build denominator and divide
        return (UnivariatePolynomialGeneral(instance.vanishingSet().vanishingPoly()).divideByMe(compositionPoly))->eval(basis,shift);
    }
    
    static std::vector<std::unique_ptr<uniPoly>> naiveWCHelper(const AcspInstance& instance, const AcspWitness& witness){
        std::vector<std::unique_ptr<uniPoly>> res;
        for(size_t wIndex = 0; wIndex < witness.assignmentPolys().size(); wIndex++){
            res.push_back(std::unique_ptr<uniPoly>(new Algebra::UnivariatePolynomialGeneral(*(witness.assignmentPolys()[wIndex]))));
        }

        return res;
    }

};

}// namespace libstark

#endif   // __Acsp_Instance_HPP
