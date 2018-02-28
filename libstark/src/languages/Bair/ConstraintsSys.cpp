#include "ConstraintsSys.hpp"

namespace libstark{
using std::vector;
using std::unique_ptr;
using Algebra::FieldElement;
using Algebra::PolynomialInterface;
using Algebra::PolynomialDegree;
using Algebra::zero;

namespace{
class linearCombPoly : public PolynomialInterface{
public:
    linearCombPoly(const ConstraintSys& cs, const vector<FieldElement>& coeffs): cs_(cs.clone()), coeffs_(coeffs) {};

    FieldElement eval(const vector<FieldElement>& x)const{
        const vector<FieldElement> resList = cs_->eval(x);
        FieldElement res = zero();
        for(size_t i=0; i< resList.size(); i++){
            res += coeffs_[i] * resList[i];
        }

        return res;
    }

    size_t numVars()const{
        return cs_->numVars();
    }

    PolynomialDegree getDegreeBound(const vector<PolynomialDegree>& inputDegrees)const{
        PolynomialDegree res = PolynomialDegree::getZeroPolyDegree();
        for( const auto& p : cs_->constraints()){
            res = std::max(res,p->getDegreeBound(inputDegrees));
        }
        return res;
    }

    unique_ptr<PolynomialInterface> clone()const{
        return unique_ptr<PolynomialInterface>(new linearCombPoly(*cs_,coeffs_));
    }

    bool isEffectiveInput(const size_t varId)const{
        return cs_->varUsed(varId);
    }

private:
    const unique_ptr<ConstraintSys> cs_;
    vector<FieldElement> coeffs_;
};
}

bool ConstraintSys::varUsed(const size_t varId) const{
    for(const auto& poly : constraints()){
        if(poly->isEffectiveInput(varId)){
            return true;
        }
    }
    return false;
}

size_t ConstraintSys::numMappings()const{
    return constraints().size();
}

vector<FieldElement> ConstraintSys::eval(const vector<FieldElement>& assignment)const{
    vector<FieldElement> res(numMappings());
    for(unsigned int i=0; i< res.size() ;i++){
        res[i] = constraints()[i]->eval(assignment);
    }

    return res;
}

PolynomialInterface* ConstraintSys::getLinearComb(const vector<FieldElement>& coeffs)const{
    linearCombPoly* res = new linearCombPoly(*this,coeffs);
    return res;
}

bool ConstraintSys::verify(const vector<FieldElement>& assignment) const{
	const vector<FieldElement> resList = eval(assignment);
    for (const FieldElement& r: resList){
		if (r != Algebra::zero()){
			return false;
		}
	}
	return true;
}

Algebra::PolynomialDegree ConstraintSys::getMaximalDegree()const{
    using Algebra::PolynomialDegree;
    using std::vector;
    using std::max;

    const vector<PolynomialDegree> inputDegrees(numVars(),PolynomialDegree(1));
    PolynomialDegree res = PolynomialDegree::getZeroPolyDegree();
    
    for(const auto& poly : constraints()){
        res = max(res,poly->getDegreeBound(inputDegrees));
    }

    return res;
}

} // namespace libstark
