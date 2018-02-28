#include "ConstraintsSys.hpp"
#include "common/langCommon/Sequence.hpp"
#include "common/Infrastructure/Infrastructure.hpp"

#include <vector>
#include <memory>

#ifndef BairINSTANCE_HPP__
#define BairINSTANCE_HPP__

namespace libstark{

/**
 * @class BairInstance
 * @brief class for Bair instance
 *
 * A Bair instance is a tuple \f$(\mathbb{F},d,\mathcal{V},\mathcal{C}_\mathcal{A}, \mathcal{C}_\pi, B, padding_\pi)\f$ such that:
 * 
 * \f$\mathbb{F}\f$ is a field with characteristic 2.
 *
 * \f$ d \in \mathbb{N} \f$ is the domain size indicator, we define the domain \f$ \mathcal{D} = \{0 \dots 2^d-2\} \f$.
 *
 * \f$ \mathcal{V} \times \{0,1\} \f$ is the variables set over which the constraint systems are defined.
 *
 * \f$ \mathcal{C}_\mathcal{A} \f$ is the constraint system (ConstraintSys) for the assignment \f$ \mathcal{A} \f$.
 *
 * \f$ \mathcal{C}_\pi \f$ is the constraint system (ConstraintSys) for the permutation \f$ \pi \f$.
 *
 * \f$ B \in (\mathcal{D} \times \mathcal{V}) \times \mathbb{F} \f$ is the set of boundary constraints
 *
 * \f$ padding_\pi \in \mathbb{F}^\mathcal{V} \f$ is a hint, that is not verified, but will be explained later 
 *
 * For two assignments \f$ \alpha , \beta : \mathcal{V} \to \mathbb{F} \f$ we define \f$ (\alpha,\beta):\mathcal{V} \times\{0,1\} \to \mathbb{F} \f$ by:
 * \f[ 
 * [(\alpha,\beta)](v,i) =
 * \begin{cases}
 * \alpha\left(v\right) & i=0\\
 * \beta\left(v\right) & i=1
 * \end{cases}
 * \f]
 *
 * A Bair instance \f$(\mathbb{F},d,\mathcal{V},\mathcal{C}_\mathcal{A}, \mathcal{C}_\pi, B)\f$
 * is satisfiable if and only if there exists:
 *
 * \f$ \mathcal{A} : \mathcal{D} \to (\mathcal{V} \to \mathbb{F}) \f$ which is called the assignment
 *
 * \f$ \pi : \mathcal{D} \to \mathcal {D} \f$ which is called the permutation
 *
 * such that the following constraints hold:
 *
 * Assignment constraints satisfaction:
 * \f[
 * \forall n \in \mathcal{D} \setminus \{ 2^d - 2 \}, \forall p \in \mathcal{C}_\mathcal{A} : p(\mathcal{A}(n), \mathcal{A}(n+1)) = 0
 * \f]
 *
 * Permutation constraints satisfaction:
 * \f[
 * \begin{align*}
 * \left[\mathcal{V}\times\left\{ 1\right\} \right]\left(\mathcal{C}_\pi\right)\neq\emptyset\Rightarrow & \pi \in S_\mathcal{D}\text{ (meaning it is a permutation)}\\
 * \forall n \in \mathcal{D} \setminus \{2^d -2 \},\forall p \in \mathcal{C}_\pi : & p\left(\mathcal{A} \left(n\right),\mathcal{A} \left( \pi \left(n\right)\right)\right)=0
 * \end{align*}
 * \f]
 *
 * The \f $padding_\pi \f$ hint is a vector such that \f$ \forall \alpha \in \mathbb{F}^\mathcal{V} \f$ it holds that:
 * \f$ \exists p \in \mathcal{C}_\pi : & p\left(\alpha,padding_\pi \right) \ne 0 \f$
 * 
 * Boundary constraints:
 * \f[
 * \forall ((x,y),\alpha) \in B : [\mathcal{A}(x)](y) = \alpha
 * \f]
 *
 * In the code we give more descriptive names:
 * we name \f$ \mathbb{F} \f$ as 'contextField' \n
 * we name \f$ d \f$ as 'domainSizeIndicator' \n
 * we name the cardinality \f$ | \mathcal{D} | \f$ as 'domainSize' \n
 * we define \f$ \mathcal{V} = \{ 0 \dots \text{'vectorsLen'} \}\f$ \n
 * we name \f$ \mathcal{C}_\mathcal{A} \f$ as 'constraintsAssignment' \n
 * we name \f$ \mathcal{C}_\pi \f$ as 'constraintsPermutation' \n
 * we name \f$ B \f$ as 'boundaryConstraints' \n
 *
 *
 * Methods:\n
 * Instance class contains only getters, 
 * constructor and a destructor.
 */

class BairInstance {
public:
	typedef std::unique_ptr<ConstraintSys> constraintsPtr_t;
	typedef std::pair<size_t,size_t> point_t;
    typedef std::map<point_t,Algebra::FieldElement> boundaryConstraints_t;
    typedef Sequence<size_t> permutation_t;

	BairInstance(
		const size_t vectorsLen,
		const short domainSizeIndicator,
		constraintsPtr_t&& constraints_assignment,
		constraintsPtr_t&& constraints_permutation,
		const boundaryConstraints_t& boundaryConstraints,
        const std::vector<Algebra::FieldElement>& paddingPi)
	:
		vectorsLen_(vectorsLen),
		domainSizeIndicator_(domainSizeIndicator),
		constraintsAssignment_(std::move(constraints_assignment)),
		constraintsPermutation_(std::move(constraints_permutation)),
		boundaryConstraints_(boundaryConstraints),
        paddingPi_(paddingPi){};
	
	BairInstance(BairInstance&& src) = default;
	
    BairInstance(const BairInstance& src) = delete;
	
	inline const size_t& vectorsLen()const {
		return vectorsLen_;
	}
	
	inline const short& domainSizeIndicator()const {
		return domainSizeIndicator_;
	}
	
	inline const size_t domainSize()const {
		return Infrastructure::POW2(domainSizeIndicator_)-1;
	}
	
	inline const ConstraintSys& constraintsAssignment()const {
		return *constraintsAssignment_;
	}
	
	inline const ConstraintSys& constraintsPermutation()const {
		return *constraintsPermutation_;
	}
	
    inline const boundaryConstraints_t& boundaryConstraints()const{
        return boundaryConstraints_;
    }

    inline const std::vector<Algebra::FieldElement> paddingPi()const{
        return paddingPi_;
    }

private:
	size_t vectorsLen_;
	short domainSizeIndicator_;

    //Constraints system to verify only the coloring (also known as C_time)
	constraintsPtr_t constraintsAssignment_;

    //Constraints system to verify both coloring and permutation (also known as C_mem)
	constraintsPtr_t constraintsPermutation_;

	boundaryConstraints_t boundaryConstraints_;

    //hint
    std::vector<Algebra::FieldElement> paddingPi_;
	
}; //class BairInstance

} // namespace libstark


#endif // BairINSTANCE_HPP__
