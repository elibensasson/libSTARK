#include "queryGenerator.hpp"
#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Utils/ErrorHandling.hpp"
#include "common/Utils/TaskReporting.hpp"
#include <algebraLib/UnivariatePolynomialGeneral.hpp>
#include <algebraLib/SubspacePolynomial.hpp>

namespace libstark{
namespace Protocols{
namespace Fri{
namespace Verifier{
namespace details{

//using common::dimOfLBeta;
using common::getL1Basis;
using common::getL0Basis;
using common::getColumnBasis;
using common::getBasisLIndex_byL0L1indices;

using Infrastructure::POW2;
using Algebra::FieldElement;
using Algebra::zero;
using std::vector;
using std::pair;
using std::max;

void RS_result::init(const vector<FieldElement>& basis, const size_t degBound){
    results.resize(POW2(basis.size()));
    basis_ = basis;
    degBound_ = degBound;
}

//Special cases for degrees
namespace{
bool verifyDeg0(const vector<FieldElement>& results){
    bool res = true;
    for(const FieldElement& y : results){
        if(y != Algebra::zero()){
            res = false;
        }
    }
    return res;
}

bool verifyDeg1(const vector<FieldElement>& results){
    bool res = true;
    const FieldElement& c = results[0];
    for(const FieldElement& y : results){
        if(y != c){
            res = false;
        }
    }
    return res;
}

bool verifyDeg2(const vector<FieldElement>& results, const unsigned int dim){
    bool res = true;
    
    //the function should be of the form y=ax+b
    const FieldElement& b = results[0];
    int mask = 2;
    for(unsigned int i=1; i< dim; i++){
        for(int j=0; j<mask; j++){
            if(results[j] != results[j^mask] + results[mask] + b){
                res = false;
            }
        }
    }
    
    return res;
}
}

bool RS_result::verify()const{
    TASK("Deterministic test of degree " + std::to_string(degBound_) + " over evaluation space of size " + std::to_string(POW2(basis_.size())));
    
    bool res = false;
    //
    // Special cases optimization
    //
    switch(degBound_)
    {
        case 0: res=verifyDeg0(results); break;
        case 1: res=verifyDeg1(results); break;
        case 2: res=verifyDeg2(results, basis_.size()); break;
        default:
                const Algebra::UnivariatePolynomialGeneral P_Poly(results,basis_,Algebra::zero());
                res = (Algebra::PolynomialDegree::integral_t(P_Poly.getDegree()) < long(degBound_));

    }

    if(!res){
        //std::cout<<"RS PCPP verifier rejected, degree is "<<Algebra::PolynomialDegree::integral_t(P_Poly.getDegree())<<" while expected at most "<<degBound_<<std::endl;
    }
    return res;
}

void ConsistencyPath_result::init(const vector<FieldElement>& basis, const FieldElement& columnId){
    L0Data.resize(POW2(basis.size()));
    basis_ = basis;
    columnId_ = columnId;

    lagrangeConstants_.resize(POW2(basis.size()));
    //Init Lagrange constants
    {
        vector<FieldElement> spaceVals(POW2(basis_.size()));
        spaceVals[0]=Algebra::zero();
        for(unsigned int i=0; i<basis_.size(); i++){
            const int mask = 1<<i;
            spaceVals[mask] = basis_[i];
            for(int j=1; j<mask; j++){
                spaceVals[j^mask] = spaceVals[j] + spaceVals[mask];
            }
        }

        vector<FieldElement> enumerator(spaceVals.size(),Algebra::one());
        vector<FieldElement> denumerator(spaceVals.size(),Algebra::one());
        for(unsigned int i=0; i< L0Data.size(); i++){
            for(unsigned int j=0; j<L0Data.size(); j++){
                if(i!=j){
                    enumerator[i] *= (columnId_ - spaceVals[j]);
                    denumerator[i] *= (spaceVals[i] - spaceVals[j]);
                }
            }
        }

        denumerator = Algebra::invertPointwise(denumerator);
        for(unsigned int i=0; i< enumerator.size();i++){
            lagrangeConstants_[i] = (enumerator[i]*denumerator[i]);
        }
    }
}

bool ConsistencyPath_result::verify()const{
    
    //
    //Verify the polynomial with values L0Data[.]
    //over the space spanned by basis_[.]
    //when evaluated over columnId_
    //its value is columnPointData
    //
    //This is done using Lagrange interpolation
    //


    FieldElement sum = columnPointData;
    for(unsigned int i=0; i< L0Data.size();i++){
        sum += (lagrangeConstants_[i]*L0Data[i]);
    }
    
    return sum == Algebra::zero();
}

namespace{

void addRandomComitmentPath(const short degBound_logCeil, const vector<FieldElement>& evaluationBasis, RS_queriesTree& queries, RS_result& results, const size_t depth, const bool L0isMSB){

    //
    //Global constants
    //
    const size_t MIN_DEPTH = 0;

    //
    // If in minimal depth, query the polynomial
    //
    if(depth <= MIN_DEPTH){
        results.init(evaluationBasis,POW2(degBound_logCeil));
        for(size_t i=0; i< POW2(evaluationBasis.size()); i++){
            auto query_iter = queries.localState.insert(pair<size_t,ResultLocation>(i,ResultLocation())).first;
            query_iter->second.addAnswerPtr(&(results.results[i]));
        }
        return;
    }

    //
    // recursive call
    //
    const vector<FieldElement> basisForColumnsProof = getColumnBasis(evaluationBasis, L0isMSB);
    const short curr_degree_log=max(0,short(basisForColumnsProof.size())-short(evaluationBasis.size()-degBound_logCeil));
    const FieldElement columnId = Algebra::generateRandom();
    addRandomComitmentPath(curr_degree_log,basisForColumnsProof,queries.subproofs[columnId],results,depth-1, L0isMSB);
}

FieldElement getSubproofKeyByIndex(const RS_queriesTree& t, const size_t idx){
    size_t i =0;
    for(const auto& e : t.subproofs){
        if(i++ == idx){
            return e.first;
        }
    }
    
    _COMMON_FATAL("Index to large");
}

void addRandomQueryPath(RS_queriesTree& commitedTree,
    std::map<size_t,std::vector<Algebra::FieldElement*>>& univariate,
    const vector<FieldElement>& evaluationBasis,
    const size_t sigmentIdx,
    Consistency_path_t& results, const size_t depth, const bool L0isMSB){
    
    // the base case, got to deepest point
    if(commitedTree.subproofs.empty()){
       results.resize(depth);
       return; 
    }

    // draw a random committed column
    const size_t columnIndex = std::rand() % commitedTree.subproofs.size();
    const FieldElement columnId = getSubproofKeyByIndex(commitedTree, columnIndex);

    // calculate curr sigment data
    const auto currSigmentBasis = getL0Basis(evaluationBasis, L0isMSB);
    const auto L1Basis = getL1Basis(evaluationBasis, L0isMSB);
    const FieldElement currSigmentOffset = getSpaceElementByIndex(L1Basis, zero(), sigmentIdx);

    // call recursively
    // do it before initializing results to keep their address in memory unchanged
    // (as the size of the results vector is yet unknown)
    {
        const auto nextColumnBasis = getColumnBasis(evaluationBasis, L0isMSB);
        const auto nextSigmentDim = getL0Basis(nextColumnBasis, L0isMSB).size();
        const size_t nextSigmentIdx = sigmentIdx >> nextSigmentDim;

        addRandomQueryPath(commitedTree.subproofs[columnId], univariate, nextColumnBasis, nextSigmentIdx, results, depth+1, L0isMSB);
    }

    // initialize queries and results
    {
        auto& currRes = results[depth];
        currRes.init(currSigmentBasis, columnId - currSigmentOffset);

        commitedTree.subproofs[columnId].localState[sigmentIdx].addAnswerPtr(&currRes.columnPointData);

        const size_t currSigmentSize = POW2(currSigmentBasis.size());
        for(size_t i=0; i< currSigmentSize; i++){
            const size_t currGlobalIdx = getBasisLIndex_byL0L1indices(evaluationBasis,i,sigmentIdx, L0isMSB);
            if(depth > 0){
                commitedTree.localState[currGlobalIdx].addAnswerPtr(&currRes.L0Data[i]);
            }
            else{
                univariate[currGlobalIdx].push_back(&currRes.L0Data[i]);
            }
        }
    }
}

}

void addRandomComitmentPath(const short degBound_logCeil, const vector<FieldElement>& evaluationBasis, RS_queries& queries, RS_result& results, const size_t depth, const bool L0isMSB){
    addRandomComitmentPath(degBound_logCeil, evaluationBasis, queries.proof, results, depth, L0isMSB);
}

void addRandomQueryPath(RS_queries& queries, const vector<FieldElement>& evaluationBasis, Consistency_path_t& results, const bool L0isMSB){
    const size_t numOfSigments = POW2(getL1Basis(evaluationBasis, L0isMSB).size());
    const size_t sigmentIdx = rand() % numOfSigments;
    addRandomQueryPath(queries.proof,queries.univariate,evaluationBasis, sigmentIdx, results, 0, L0isMSB);
}

} //namespace details
} //namespace Verifier
} //nmasepace Fri
} //namespace Protocols
} //namespace libstark
