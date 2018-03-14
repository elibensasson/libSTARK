
/******************************************************************************************************/
/************************************ ShiftedSubspacePolynomial Class ***************************************/
/******************************************************************************************************/

#include "ShiftedSubspacePolynomial.hpp"
using std::vector;

namespace Algebra{
	FieldElement square(FieldElement x){
		return x*x;
	}

	/** Class Constructor - Computes the subspace polynomial that vanishes exactly over the span of spanSet*/

	ShiftedSubspacePolynomial::ShiftedSubspacePolynomial(const elementsSet_t& spanSet, const FieldElement& constantFactor):AffinePolynomial(computeVanishingPoly(spanSet), constantFactor){
		
	}


	/** Returns the vector of coefficients of the subspace polynomial vanishing on the span of spanSet={e1,..,ek}
	* For simplicity, let us first describe an alg that would work assuming {e1,..ek} are lin. independent:
	* The algorithm would inductively computes the (coeffs of the) subspace polynomial P_i of the subspace spanned by {e_1,..,e_i}.
	* For i=1, this is the polynomial x^2 + e1*x. Assume we have computed P_{i-1}.
	* it turns out that P_i is P_{i-1} composed from the outside with x^2 + P_{i-1}(e_i)*x
	* this is the formula the code implements.
	All this was assuming {e1,..,ek} were linearly independent.
	Now, to remove this assumption, before constructing the next P_i we check if the current e_i is linearly dependent
	on {e1,..,e_{i-1}} - we do this by simply checking if P_{i-1}(e_i) =0.
	If so, we simply let P_i= P_{i-1}.
	Otherwise, we derive P_i using the formula described above.
	*/
	vector<FieldElement> ShiftedSubspacePolynomial::computeVanishingPoly(const elementsSet_t& spanSet) {
		
		vector<FieldElement> coefficients ;
		//if spanSet is empty we return the zero polynomial
		if (spanSet.size() == 0)
			return coefficients;


		//Initializing as the subspace poly of the space {0} - which is x
		coefficients.push_back(one());
		FieldElement c, eiPower;
		unsigned int i = 0, j;
		elementsSet_t::iterator z = spanSet.begin();
		while (z != spanSet.end()){
			//compute c= P_{i-1}(e_i)
			c = zero();

			eiPower = *z; 	//eiPower starts as ei
			for (j = 0; j < coefficients.size(); j++) {
				c += coefficients[j] * eiPower;
				eiPower = square(eiPower);
			}
			//Now check if c is non-zero - which means e_i is independent of previous ej's.
			if (!(c == zero())){
				//if so, update P_i to be zero on the extended span of {e1,..ei}
				i++;
				coefficients.push_back(one());
				for (j = i - 1; j >= 1; j--) {
					coefficients[j] = c*coefficients[j] + square(coefficients[j - 1]);
				}
				coefficients[0] = c*coefficients[0];

			}
			z++;
		}
			return coefficients;
	}





	/** Testing purposes */
	//void SubspacePolynomial::print() const {
	//	cout << "Vanishing Polynomial: " << endl;
	//
	//	if (coefficients == NULL) {
	//		cout << "Empty object!" << endl;
	//		return;
	//	}
	//
	//	for (unsigned long i = 0; i<size; i++) {
	//		cout << i << ": " << coefficients[i] << endl;
	//	}
	//
	//	if (constantFactor != generateConstant<FieldElement,>(0))
	//		cout << "Constant Factor: " << constantFactor << endl;
	//}



}//namespace Algebra
