#ifndef Bair_WITNESS_CHECKER_HPP__
#define Bair_WITNESS_CHECKER_HPP__

#include "BairInstance.hpp"
#include "BairWitness.hpp"

namespace libstark {

/**
 * @class BairWitnessChecker
 * @brief class for deterministic Bair Witness Checker
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
 * is satisfied be a Bair witness \f$ (\mathcal{A},\pi) \f$ the following constraints hold:
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
 */
class BairWitnessChecker {
public:
    /// verify the witness satisfies the instance
	static bool verify(const BairInstance& instance, const BairWitness& witness);
	
    /// verify that \f$ \pi \f$ is a permutation, if required to be
    static bool verify_permutation(const BairInstance& instance, const BairWitness& witness);
	
    /// verifies assignment constraints satisfaction
	static bool verify_constraintsAssignment(const BairInstance& instance, const BairWitness& witness);
	
    /// verifies permutation constraints satisfaction
    static bool verify_constraintsPermutation(const BairInstance& instance, const BairWitness& witness);
    
    /// verifies boundary constraints
    static bool verify_boundary(const BairInstance& instance, const BairWitness& witness);
};


} // namespace libstark

#endif //Bair_WITNESS_CHECKER_HPP__
