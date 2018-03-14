/**MultiVarPoly.hpp - simple implementation of Multivariate Polynomial as a vector of monomials*/
#include "FieldElement.hpp"
#include "PolynomialDegree.hpp"
#include "PolynomialInterface.hpp"
using namespace std;
namespace Algebra{
	struct MultiVarMonomial
	{
		FieldElement coeff;
		vector<pair<long, long>> vars;
	};

	class MultiVarPoly : public PolynomialInterface{
	public:

		//adds monomial to polynomial. If monomial exists with different coefficient adds the new coefficient to existing monomial
		//Warning:Assumes monomial always contains variables in same order.
		void AddMonomial(MultiVarMonomial M){
			for (auto it = monomials.begin(); it != monomials.end(); it++){
				if ((*it).vars == M.vars){
					(*it).coeff += M.coeff;
					return;
				}
			}
			monomials.push_back(M);
		}

		//adds monomial to polynomial with coeff 1, assuming this monomial does not yet appear. If monomial already exists does nothing.
		void AddMonomial(vector<pair<long, long>> M){
			for (auto it = monomials.begin(); it != monomials.end(); it++){
				if ((*it).vars == M)
					return;

			}
			MultiVarMonomial A = { one(), M };
			monomials.push_back(A);
		}
		FieldElement eval(const vector<FieldElement>& x)const{
			const FieldElement ZERO = generateConstant<FieldElement, 0>();
			const FieldElement ONE = generateConstant<FieldElement, 1>();

			FieldElement res = ZERO;

			for (const auto& prod : monomials){
				FieldElement prod_res = prod.coeff;
				for (const auto& varId : prod.vars){
					for (long i = 0; i<varId.second; ++i)
						prod_res *= x[varId.first];
				}

				res += prod_res;
			}

			return res;
		}

		//FieldElement eval(const sparceAssignment_t& x)const{
		//	const FieldElement ZERO = generateConstant<FieldElement, 0>();
		//	const FieldElement ONE = generateConstant<FieldElement, 1>();

		//	FieldElement res = ZERO;

		//	for (const auto& prod : monomials){
		//		FieldElement prod_res = prod.coeff;
		//		for (const auto& varId : prod.vars){
		//			prod_res *= x.at(varId);
		//		}

		//		res += prod_res;
		//	}

		//	return res;
		//}

		PolynomialDegree getDegreeBound(const vector<PolynomialDegree>& inputDegrees)const{
			PolynomialDegree deg(-1);

			for (const auto& prod : monomials){
				PolynomialDegree prodDeg(0);
				for (const auto& varId : prod.vars){
					prodDeg = prodDeg + PolynomialDegree(varId.second)*inputDegrees[varId.first];
				}

				deg = max(deg, prodDeg);
			}

			return deg;
		}

		int getNonNegativeDegree()const{
			int deg;
			for (const auto& prod : monomials){
				int prodDeg = 0;
				for (const auto& varId : prod.vars){
					prodDeg = prodDeg + varId.second;
				}

				deg = max(deg, prodDeg);
			}

			return deg;
		}

		unique_ptr<PolynomialInterface> clone()const{
			return unique_ptr<PolynomialInterface>(new MultiVarPoly(*this));
		}

		bool isEffectiveInput(const size_t varId)const{

			for (const auto& prod : monomials){
				for (const auto& varId_ : prod.vars){
					if (varId_.first == varId)return true;
				}
			}
			return false;
		}

	private:
		vector<MultiVarMonomial> monomials;
	};
}
