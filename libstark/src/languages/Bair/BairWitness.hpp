#ifndef Bair_WITNESS_HPP__
#define Bair_WITNESS_HPP__

#include <algebraLib/FieldElement.hpp>
#include "common/langCommon/Sequence.hpp"

#include <memory>
#include <vector>

namespace libstark{

/**
 * @class BairWitness
 * @brief class for Bair Witness
 *
 * A Bair witness for Bair instance \f$(\mathbb{F},d,\mathcal{V},\mathcal{C}_\mathcal{A}, \mathcal{C}_\pi, B)\f$
 * is a pair \f$ (\mathcal{A},\pi) \f$ such that:
 *
 * \f$ \mathcal{A} : \mathcal{D} \to (\mathcal{V} \to \mathbb{F}) \f$ which is called the assignment
 *
 * \f$ \pi : \mathcal{D} \to \mathcal {D} \f$ which is called the permutation
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
 * It said that a Bair instance \f$(\mathbb{F},d,\mathcal{V},\mathcal{C}_\mathcal{A}, \mathcal{C}_\pi, B)\f$
 * is satisfied by a Bair witness \f$ (\mathcal{A},\pi) \f$ the following constraints hold:
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
 * Boundary constraints:
 * \f[
 * \forall ((x,y),\alpha) \in B : [\mathcal{A}(x)](y) = \alpha
 * \f]
 *
 * In the code we give more descriptive names:
 * 
 * we name \f$ \mathcal{A} \f$ as 'get_assignment' \n
 * we name \f$ \pi \f$ as 'permutation' \n
 *
 *
 * Methods:\n
 * Instance class contains only getters, 
 * constructor and a destructor.
 */
class BairWitness {
public:
	typedef std::vector<Algebra::FieldElement> color_t;
	typedef Sequence<color_t> assignment_t;
	typedef std::unique_ptr<assignment_t> assignment_ptr;

	typedef Sequence<size_t> permutation_t;
	typedef std::unique_ptr<permutation_t> permutation_ptr;
	
	BairWitness(
		assignment_ptr&& assignment,
		permutation_ptr&& permutation)
	:
		assignment_(std::move(assignment)),
		permutation_(std::move(permutation)){};
	
	BairWitness(BairWitness&& src) = default;
	BairWitness(const BairWitness& src) = delete;

	inline color_t get_color(size_t vecIndex)const {
		return assignment_->getElementByIndex(vecIndex);
	}
	
	inline const permutation_t& permutation()const {
		return *permutation_;
	}
	
private:
	assignment_ptr assignment_;
	permutation_ptr permutation_;
};

} // namespace libstark

#endif // Bair_WITNESS_HPP__
