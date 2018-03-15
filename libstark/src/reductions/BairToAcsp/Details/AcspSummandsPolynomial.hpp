
#include <algebraLib/FieldElement.hpp>
#include "algebraLib/SubspacePolynomial.hpp"
#include "common/Algebra/ShiftedSubspacePolynomial.hpp"

#include <algebraLib/PolynomialInterface.hpp>
#ifndef REFACTORING_Acsp_AcspSUMMANDSPOLYNOMIAL_HPP__
#define REFACTORING_Acsp_AcspSUMMANDSPOLYNOMIAL_HPP__

namespace libstark{
namespace BairToAcsp{


/**
 * \class AcspSummandsPolynomial
 * 
 * This class is designed to hold a multivariate-polynomial represented as
 * \f$ \sum_{i=1}^{n} Sel_i \cdot P_i \f$ where for each \f$i\f$, \f$Sel_i \in \mathbb{F}[x] \f$
 * and \f$ P_i \in \mathbb{F}[\mathcal{V}] \f$ for some variables set \f$ \mathcal{V} \f$
 * (this form arises in the BS/BGHSP PCP).
 * Additionally there is another parameter of the polynomial, \f$ H \subset \mathbb{F} \f$ which is a \f$ \mathbb{F}_2 \f$-linear subspace.
 * \f$ H \f$ defines a restriction on each \f$ Sel_i \f$ that we will explain later.
 *
 * We interpret the factors of each summand in the following way:
 *
 * \f$P_i\f$ - the multivariate polynomial representing the constraint , is a rather general polynomial, but usually very low-degree - typically 2 or 3 (but might be arbitrary).
 * 
 * \f$Sel_i\f$ - the 'Selector polynomial', is a polynomial of relatively high degree\f$^*\f$ but with a lot of structure.
 * Its purpose is to vanish on \f$ H \setminus U_i \f$, where \f$U_i\f$ is the set of points on which we wish to verify the constraint \f$P_i\f$.
 * \f$U_i\f$ is usually a subspace of \f$H\f$, but might be an arbitrary subset as well.
 * More details about the structure of \f$Sel_i\f$ are given below - in the description of the private class AcspConstraintSummand.
 * The purpose of this private class is to contain such a summand.
 * Then, we can represent the polynomial as a vector of AcspConstraintSummand (see details below on this private class, defined in this file).
 *
 * Creating a class for this special type of polynomial allows for faster evaluation- especially when
 * the evaluation is not just on one point, but on a large space - as required for the PCP proof.
 * To be more precise, the polynomial we wish to evaluate quickly is a composition of the polynomial represented here
 * with univariate polynomials.
 * See AcspComposedPolynomial for details on this composition.
 * 
 * 
 * \note There is a nuance here (better to ignore it at a first read). Usually in the PCP description \f$Sel_i\f$ is a ratio of two affine polynomials,
 * the numerator being the subspace polynomial \f$Z_H\f$ (which is the *same for all summands*);
 * but in the actual code - we have \f$Z_H\f$ multiply the whole polynomial,
 * and the summand contains only the denumerator part. (There are cases where there are other things in the numerator that come from a parameter called auxPolys)
 * 
 * 
 * \author Ariel Gabizon - ariel.gabizon@gmail.com, and Michael Riabzev - riabzevmichael@gmail.com
 */

	class AcspSummandsPolynomial : public Algebra::PolynomialInterface{
		class AcspConstraintSummand;
    public:
        typedef Algebra::PolynomialInterface Constraint;  //currently we just have the constraints be general polynomials, but perhaps will change later on
        typedef std::vector<Algebra::FieldElement>  EvaluationPoint;
        typedef unsigned int polyID;
		typedef std::vector<Algebra::SubspacePolynomial> SubspacePolyVec;
		typedef std::pair<polyID, Algebra::FieldElement>  AuxPoly;   //the auxillary polynomials are represented by (an ID of a) subspace polynomial, and its shift
		typedef std::vector<AuxPoly> AuxPolyVec;
		typedef std::vector<AcspConstraintSummand> SummandsVector;

        //A denominator is a pair of a linearized polynomial $L$ (represented by its ID)
        //and a constant shift $c$ which is a field element.
        //This is used because each AcspConstraintSummand has such pair, which while it is
        //evaluated on some assignment x,y_1...y_n it should calculate the value of 1/(L(x)-c).
        //Inversion is time consuming, but inversion of a bunch of FieldElements together is cheaper,
        //so we do all the inversions together, and pass the results of the AcspConstraintSummand polynomials.
        typedef std::pair<polyID,Algebra::FieldElement> DenominatorType;
        typedef std::vector<DenominatorType> DenominatorsVecType;
		
        /// Creates a new AcspSummandsPolynomial, and sets \f$H\f$ to be \f$ span( spanSetH ) \f$
		AcspSummandsPolynomial(const Algebra::elementsSet_t& spanSetH);

		//Copy constructor
		AcspSummandsPolynomial(const AcspSummandsPolynomial& that);
		/// Adds a subspace polynomial vanishing on \f$span(T)\f$ into subspacePolys and returns its id, i.e, its index in subspacePolys
        /// It is enforced that \f$ span(T) \subset H \f$ (otherwise _COMMON_FATAL is called)
		polyID addSubspacePoly(const Algebra::elementsSet_t& T);

		/// evaluates this polynomial on \f$ a \f$
		Algebra::FieldElement eval(const EvaluationPoint& a)const;
	
        size_t numVars()const;
        
        //evaluates a polynomial on a given set (represented as vector) of assignments
        std::vector<Algebra::FieldElement> evalOnSet(const std::vector<std::vector<Algebra::FieldElement>>& x_set)const;
    
        // check if the \f$ a \f$ is a root of the polynomial
        bool isRoot(const EvaluationPoint& x)const;

		/// Construct the summand \f$ \frac{1}{Z_S(x-shift)} \cdot P_i \f$.
        /// Technically what we construct is the equivalent summand \f$ \frac{1}{Z_S(x) - c} \cdot P_i \f$, where \f$ c=Z_S(shift) \f$
        /// It is enforced that \f$ shift \in H \f$ (otherwise _COMMON_FATAL is called)
		void addSummand(const polyID Z_S, const Algebra::FieldElement& shift, const Constraint& P_i);

		/// construct the summand \f$ \frac{1}{Z_S+c} \cdot P_i \cdor W \f$, where  \f$c=Z_S(shift)\f$ and \f$W\f$
        /// is the product of polynomials in auxPolys shifted according to the shifts vector
        /// It is enforced that \f$ shift \in H \f$ and all the shifts are in \f$ H \f$ as well (otherwise _COMMON_FATAL is called)
		void addSummand(const polyID Z_S, const Algebra::FieldElement& shift, const Constraint& P_i, const AuxPolyVec& auxVec);

        /**
         * @brief return a clone of the current polynomial
         * @return a unique_ptr of PolynomialInterface,
         * representing a polynomial equivalent to current
         */
        std::unique_ptr<Algebra::PolynomialInterface> clone()const;
        
        /**
         * @brief   Returns an upper bound for degree of this polynomial
         * when composed with univariate polynomials (as its variables)
         * @param   degrees vector of univariate polynomial, name it \f$\vec{d}\f$
         * @return  a degree \f$D\f$ such that
         * \f$ D \ge \deg P\circ\vec{Q} \f$ when \f$P\f$ is the current polynomial,
         * each \f$Q_i\f$ is a univariate polynomial of degree \f$d_i\f$ at most,
         * and \f$(P\circ\vec{Q})(x) = P(Q_1(x),Q(2),\dots,Q_n(x))\f$
         */
        Algebra::PolynomialDegree getDegreeBound(const std::vector<Algebra::PolynomialDegree>& inputDegreesBound)const;
        
        /**
         * @brief   Returns an upper bound for degree of this polynomial divided by ZH
         * when composed with univariate polynomials (as its variables)
         * @param   degrees vector of univariate polynomial, name it \f$\vec{d}\f$
         * @return  a degree \f$D\f$ such that
         * \f$ D \ge \deg P\circ\vec{Q} \f$ when \f$P\f$ is the current polynomial,
         * each \f$Q_i\f$ is a univariate polynomial of degree \f$d_i\f$ at most,
         * and \f$(P\circ\vec{Q})(x) = P(Q_1(x),Q(2),\dots,Q_n(x))\f$
         */
        Algebra::PolynomialDegree getDegreeBound_DividedByZH(const std::vector<Algebra::PolynomialDegree>& inputDegreesBound)const;
		
        /// evaluated this polynomial divided by \f$ Z_H \f$ on \f$ a \f$
        /// \note if \f$ a_0 \in H \f$ the result is not necessarily right, 
        /// but because we multiply it be the result of \f$ Z_H \f$, it should vanish there any way.
		std::vector<Algebra::FieldElement> evalDividedByZH(const std::vector<EvaluationPoint>& a, const bool allowEvalIn_H=false)const;
	
    private:

		///The subspace polynomial that vanishes over the subspace \f$H\f$ - that the Acsp composed (univariate) polynomial is supposed to vanish on
		Algebra::SubspacePolynomial Z_H;
		
		///Stores the different possibilities for the polynomials \f$Z_S\f$ or the auxiliary polynomials described below. The point is there are only a few
		///different such polynomials, each appearing in many summands.
		SubspacePolyVec subspacePolys;

        //the denominators vector, for a bunch inversion (cheaper than one at a time)
        DenominatorsVecType denomsVec_;
		
		///stores the summands in this polynomial
		SummandsVector summands;


		/**
         * \class The AcspConstraintSummand class
         * 
         * This class holds a single summand of the Acsp constraints polynomial - implemented by the class AcspSummandsPolynomial.
         * As described in AcspSummandsPolynomial, the summand is of the form \f$ Sel*P_i \f$ (there we used \f$ Sel_i \f$ rather than \f$ Sel \f$).
         * \f$ Sel \f$ is *usually* of the form \f$ Sel = \frac{Z_H}{Z_S+c} \f$ for some constant \f$ c \in H \f$.
         * where \f$ S \f$ is (supposed to be) a subspace contained in \f$ H \f$, and \f$ Z_S \f$ is the subspace polynomial vanishing on \f$ S \f$.
         * That is, \f$ Sel \f$ vanishes exactly on \f$ H \setminus S+c \f$.
         * We note that \f$ Z_H \f$ is not stored in this class, but only in the AcspSummandsPolynomial class.
         * As \f$ Z_H \f$ and \f$ Z_S \f$ are subspace polynomials,
         * \f$ Sel \f$ can be computed much more efficiently when represented explicitly as this quotient
         * (rather than explicitly computing \f$ Sel \f$'s coefficients), especially in our case, when the enumerator is common for a lot of selectors.
         * In fact, in the PCP we are interested in computing \f$ Sel \cdot \frac{P_i}{Z_H} = \frac{P_i}{Z_S+c} \f$ composed with univariate polynomials.
         * This is another reason it makes sense to explicitly store \f$ Z_S \f$ and \f$ c \f$, rather than just \f$ Sel \f$'s coefficients.
         * Occasionally, \f$ Sel \f$ will be of the form described above
         * but multiplied by a few more auxiliary shifted subspace polynomials.
         * This case is addressed by the member variable auxPolys
         *
         * We stress again, that though in PCP descriptions \f$ Sel = \frac{Z_H}{Z_S+c} \f$
         * what is actually stored here is the summand \frac{1}{Z_S+c} \cdot P_i \f$ - which is not even a poly but a rational function!
         * (Though it is supposed to turn into a poly after the univariate substitution derived from a legal assignment)
         * and this is what the eval function of the object computes
         *
         **/

		class AcspConstraintSummand{
		public:
			
            /// move constructor
			AcspConstraintSummand(AcspConstraintSummand&& rhs);
			
            /// construct the summand \f$ \frac{1}{Z_S+c} \cdot P_i \f$.
            AcspConstraintSummand(const polyID denomIndex,  const Constraint& P_i);

			//// construct the summand \f$ \frac{1}{Z_S+c} \cdot P_i \cdot W \f$ , where \f$ W \f$ is the product of polynomials in auxPolys
			AcspConstraintSummand(const polyID denomIndex, const Constraint& P_i, const std::vector<polyID>& auxPolys, const std::vector<Algebra::FieldElement>& shifts);

            /// copy constructor
            AcspConstraintSummand(const AcspConstraintSummand& src);

            ///	evaluates the summand on a given point \f$ a \f$
            /// \note important point evaluates without the \f$ Z_H \f$ part!
            ///	
            /// it receives as input the array subspaceValues containing the evaluations on \f$ a \f$ of the subspace polys
            ///	associated with the AcspSummandsPolynomial object
            ///
            /// \warning This method does not always evaluate the summand exactly, the summand is represented
            /// as the rational function \f$ \frac{P_i \cdot W}{Z_S + c} \f$.
			/// In the case both the enumerator and the denumerator vanish on the evaluation point \f$ a \f$
            /// the method return \f$ 0 \f$, which is not allways correct, but good enough for our implementation,  
            Algebra::FieldElement eval(const EvaluationPoint& a, const std::vector<Algebra::FieldElement>& subspaceValues, const std::vector<Algebra::FieldElement>& denomsVals) const;
            
            /**
             * @brief   Returns an upper bound for degree of this polynomial
             * when composed with univariate polynomials (as its variables)
             * @param   degrees vector of univariate polynomial, name it \f$\vec{d}\f$
             * @return  a degree \f$D\f$ such that
             * \f$ D \ge \deg P\circ\vec{Q} \f$ when \f$P\f$ is the current polynomial,
             * each \f$Q_i\f$ is a univariate polynomial of degree \f$d_i\f$ at most,
             * and \f$(P\circ\vec{Q})(x) = P(Q_1(x),Q(2),\dots,Q_n(x))\f$
             */
            Algebra::PolynomialDegree getDegreeBound(const std::vector<Algebra::PolynomialDegree>& inputDegreesBound)const;
        
        private:
            polyID denominatorIndex_;
            std::unique_ptr<Constraint> P_i;
            std::vector<polyID> auxPolys;//stores the auxiliary polynomials		
			std::vector<Algebra::FieldElement> shifts;//stores the shifts of the auxiliary polynomials


		};//end of AcspConstraintSummand


	};//end of AcspSummandsPolynomial

} //namespace BairToAcsp
} //namespace libstark

#endif //REFACTORING_Acsp_AcspSUMMANDSPOLYNOMIAL_HPP__
