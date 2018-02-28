//class constraintPoly : public PolynomialInterface{
//public:
//
//	constraintPoly(const Polynomial& poly, const vector<Variable>& varsTranslation){
//		const FieldElement ZERO = generateConstant<FieldElement, 0>();
//		if (poly.getConstant() != ZERO){
//			product_t constantSummand;
//			constantSummand.coeff = poly.getConstant();
//			poly_.push_back(constantSummand);
//		}
//
//		for (const auto& monomial : poly.getMonomials()){
//			if (monomial.getCoefficient() == ZERO)continue;
//
//			product_t newProd;
//			newProd.coeff = monomial.getCoefficient();
//
//			for (size_t varId = 0; varId < varsTranslation.size(); varId++){
//				for (size_t i = 0; i< monomial.getVariableDegree(varsTranslation[varId]); i++){
//					newProd.vars.push_back(varId);
//				}
//			}
//
//			poly_.push_back(newProd);
//		}
//	}
//
//	FieldElement eval(const vector<FieldElement>& x)const{
//		const FieldElement ZERO = generateConstant<FieldElement, 0>();
//		const FieldElement ONE = generateConstant<FieldElement, 1>();
//
//		FieldElement res = ZERO;
//
//		for (const auto& prod : poly_){
//			FieldElement prod_res = prod.coeff;
//			for (const auto& varId : prod.vars){
//				prod_res *= x[varId];
//			}
//
//			res += prod_res;
//		}
//
//		return res;
//	}
//
//	FieldElement eval(const sparceAssignment_t& x)const{
//		const FieldElement ZERO = generateConstant<FieldElement, 0>();
//		const FieldElement ONE = generateConstant<FieldElement, 1>();
//
//		FieldElement res = ZERO;
//
//		for (const auto& prod : poly_){
//			FieldElement prod_res = prod.coeff;
//			for (const auto& varId : prod.vars){
//				prod_res *= x.at(varId);
//			}
//
//			res += prod_res;
//		}
//
//		return res;
//	}
//
//	PolynomialDegree getDegreeBound(const vector<PolynomialDegree>& inputDegrees)const{
//		PolynomialDegree deg(-1);
//
//		for (const auto& prod : poly_){
//			PolynomialDegree prodDeg(0);
//			for (const auto& varId : prod.vars){
//				prodDeg = prodDeg + inputDegrees[varId];//ARIEL - fixed + to =
//			}
//
//			deg = max(deg, prodDeg);
//		}
//
//		return deg;
//	}
//
//	unique_ptr<PolynomialInterface> clone()const{
//		return unique_ptr<PolynomialInterface>(new constraintPoly(*this));
//	}
//
//	bool isEffectiveInput(const size_t varId)const{
//
//		for (const auto& prod : poly_){
//			for (const auto& varId_ : prod.vars){
//				if (varId_ == varId)return true;
//			}
//		}
//		return false;
//	}
//
//private:
//	typedef size_t varId_t;
//	typedef struct product_t{
//		vector<varId_t> vars;
//		FieldElement coeff;
//	};
//	vector<product_t> poly_;
//};
