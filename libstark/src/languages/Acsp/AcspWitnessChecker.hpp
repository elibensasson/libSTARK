#ifndef  __AcspWitnessChecker_HPP
#define  __AcspWitnessChecker_HPP

#include "AcspInstance.hpp"
#include "AcspWitness.hpp"

namespace libstark {

/**
 * @class AcspWitnessChecker
 * @brief class for Acsp (instance, witness) pairs checking
 */
class AcspWitnessChecker {
public:
	/**
	 * @brief   verifies all conditions
	 * @param   instance a tuple \f$(\mathbb{F},H,\vec{N},P,WitnessDegreeBound,B)\f$
	 * @param   witness a polynomial \f$ A \in \mathbb{F}[x]\f$
	 * @return  return true if and only if
	 * \f{eqnarray*}{
	 * \forall z \in H: (P \circ (x \vert A \circ \vec{N}))(z) = 0 \\
	 * \deg A <= witnessDegreeBound \\
	 * \lambda \cdot \deg(P \circ (x \vert A \circ \vec{N})) \le |\mathbb{F}| \\
	 * \forall i \in [n] : x_i = A(I(i)) \\
	 * \f}
	 */
	static bool verify(const AcspInstance& instance, const AcspWitness& witness);
	
	/**
	 * @brief   verifies the vanishing subset condition
	 * @param   instance a tuple \f$(\mathbb{F},H,\vec{N},P,WitnessDegreeBound,B)\f$
	 * @param   witness a polynomial \f$ A \in \mathbb{F}[x]\f$
	 * @return  return true if and only if
	 * \f$
	 * \forall z \in H: (P \circ (x \vert A \circ \vec{N}))(z) = 0
	 * \f$
	 */
	static bool verify_vanishing(const AcspInstance& instance, const AcspWitness& witness);
	
	/**
	 * @brief   verifies the witness degree condition
	 * @param   instance a tuple \f$(\mathbb{F},H,\vec{N},P,WitnessDegreeBound,B)\f$
	 * @param   witness a polynomial \f$ A \in \mathbb{F}[x]\f$
	 * @return  return true if and only if
	 * \f$
	 * \deg A <= witnessDegreeBound
	 * \f$
	 */
	static bool verify_witness_degree(const AcspInstance& instance, const AcspWitness& witness);
	
	/**
	 * @brief   verifies the boundary constraints condition
	 * @param   instance a tuple \f$(\mathbb{F},H,\vec{N},P,WitnessDegreeBound,B)\f$
	 * @param   witness a polynomial \f$ A \in \mathbb{F}[x]\f$
	 * @return  return true if and only if
	 * \f$
	 * \forall (x,y) \in B : A(x)=y$
	 * \f$
	 */
	static bool verify_boundary(const AcspInstance& instance, const AcspWitness& witness);
private:
};

} // namespace libstark
#endif   // __AcspWitnessChecker_HPP
