#include "commonDeffinitions.hpp"
#include <algorithm>

using std::vector;
using std::ceil;
using std::max;
using std::pair;
using std::count;
using std::move;
using std::unique_ptr;
using Infrastructure::Log2;
using Infrastructure::POW2;
using Algebra::FieldElement;
using Algebra::PolynomialDegree;
using Algebra::PolynomialInterface;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::getStandartBasis;
using Algebra::zero;

namespace libstark{
namespace BairToAcsp{

	commonDeffinitions::commonDeffinitions(const BairInstance& instance, const vector<FieldElement>& coeffsPi, const vector<FieldElement>& coeffsChi) {
    
    //getting variables partition
    {
    variablesPerm_ = getRoutedIndexes(instance);
    auto vars_n_constraints = getUnroutedIndexes_and_ConstraintsChi(instance,coeffsChi);
    variablesNonPerm_ = vars_n_constraints.first;
    constraintsChi_ = move(vars_n_constraints.second);
    
    //initialize constraints Pi
    {    
        constraintsPi_.push_back(unique_ptr<PolynomialInterface>(instance.constraintsPermutation().getLinearComb(coeffsPi)));
    }
    }

    //get the variables location translation
    {
    bairVarLocation_.resize(instance.vectorsLen());

    for(size_t i=0; i<variablesPerm_.size() ; i++){
        bairVarLocation_[variablesPerm_[i]] = {true,i};
    }

    for(size_t i=0; i<variablesNonPerm_.size() ; i++){
        bairVarLocation_[variablesNonPerm_[i]] = {false,i};
    }
    }

    //getting dimensions and sizes
    heightSpaceDimension_ = instance.domainSizeIndicator();
    
    widthSpaceDimension_ = 0;
    if(variablesPerm_.size() > 0){
        widthSpaceDimension_ = ceil(Log2(instance.domainSizeIndicator()+2));
    }
    
    amountOfPermutationLayers_ = 0;
    if(variablesPerm_.size() > 0){
        amountOfPermutationLayers_ = 2*(1+variablesPerm_.size());
    }
    
    const double spaceWidth = POW2(widthSpaceDimension_);
    {
    const double totalConstraintsAmount = 1 + 1;//instance.constraintsAssignment().constraints().size() + instance.constraintsPermutation().constraints().size();
    const size_t vanishingLayersConstraints = ceil(totalConstraintsAmount/spaceWidth);
    vanishingLayersSpaceDimension_ = ceil(Log2(amountOfPermutationLayers_ + vanishingLayersConstraints));
    }

    {
        const size_t numOfVariablesLayers = ceil(double(variablesNonPerm_.size())/double(spaceWidth));
        numOfWitnessLayers_ = amountOfPermutationLayers_ + numOfVariablesLayers;
    }

    {
    const size_t spaceWidth = POW2(widthSpaceDimension_);
    const size_t witnessDegreeFactor = (SUPPORT_ZK?2:1);
    witnessDegreeBound_ = vector<PolynomialDegree>(numOfWitnessLayers_,PolynomialDegree(witnessDegreeFactor*POW2(heightSpaceDimension_)*spaceWidth-1));
    }
}

PolynomialDegree commonDeffinitions::getMaxTestDegree(const BairInstance& instance, const bool hasRoutingNetwork, const PolynomialDegree& ascpWitnessDegreeBound){
    
    // initialize a vector of zeros as input degrees,
    // for checking the maximal degree of a polynomial
    // in each constraint system
    const size_t assignmetnSize = 2*instance.vectorsLen();
    vector<PolynomialDegree> inputDegrees(assignmetnSize,ascpWitnessDegreeBound);
   
    // find maximal degree in constraint system tests
    PolynomialDegree maxDeg = max(instance.constraintsAssignment().getMaximalDegree(),instance.constraintsPermutation().getMaximalDegree());

    // find maximal in case there is a routing network
    // the maximal degree test for routing is 2 (specifically booleanity testing)
    if(hasRoutingNetwork){
        maxDeg = max(maxDeg,PolynomialDegree(2));
    }

    // return maximal degree
    return maxDeg;
}


// Routing
vector<size_t> commonDeffinitions::getRoutedIndexes(const BairInstance& instance){
    const auto vecLen = instance.vectorsLen();
    const auto& CSystemPermutation = instance.constraintsPermutation();

    vector<size_t> usedIndexes;
    //Push only the indexes that must be routed
    //meaning, only those that the constraints system uses
    //from the second vector
    for(size_t i=vecLen; i< 2*vecLen; i++){
        if(CSystemPermutation.varUsed(i)){
            usedIndexes.push_back(i-vecLen);
        }
    }

    return usedIndexes;
}

pair<vector<size_t>,vector<unique_ptr<PolynomialInterface>>> commonDeffinitions::getUnroutedIndexes_and_ConstraintsChi(const BairInstance& instance, const vector<FieldElement>& coeffs){
    const auto vecLen = instance.vectorsLen();
    const auto& CSystemPermutation = instance.constraintsPermutation();

    vector<size_t> unusedIndexes;
    //Push only the indexes that are not routed
    //meaning, only those that the constraints system does not use
    //from the second vector
    for(size_t i=vecLen; i< 2*vecLen; i++){
        if(!CSystemPermutation.varUsed(i)){
            unusedIndexes.push_back(i-vecLen);
        }
    }

    //if there are any univariate constraints,
    //reorder the variable, so those used by the most
    //common univariate constraints are all at the beginning
    vector<size_t> reorderedUnusedIndexes;
    vector<unique_ptr<PolynomialInterface>> reorderedConstraints;
    reorderedUnusedIndexes = unusedIndexes;
    {
        reorderedConstraints.push_back(unique_ptr<PolynomialInterface>(instance.constraintsAssignment().getLinearComb(coeffs)));
    }

    return pair<vector<size_t>,vector<unique_ptr<PolynomialInterface>>>(reorderedUnusedIndexes,move(reorderedConstraints));
}

} //namespace BairToAcsp
} //namespace libstark
