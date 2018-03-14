#include "constraints.hpp"
#include "AcspSummandsPolynomial.hpp"

#include <memory>
#include <map>
#include <vector>
#include <algorithm>

namespace libstark{
namespace BairToAcsp{

using Algebra::FieldElement;
using Algebra::zero;
using Algebra::one;
using Algebra::mapIntegerToFieldElement;
using Algebra::PolynomialInterface;
using Algebra::PolynomialDegree;

using std::vector;
using std::map;
using std::max;
using std::unique_ptr;
   
namespace{ 
    void addPermutationCS_checks(
AcspSummandsPolynomial& bigPoly, 
const vector<unique_ptr<PolynomialInterface>>& constraints,
const AcspSummandsPolynomial::polyID&  columnVanishingPoly,
const AcspSummandsPolynomial::polyID&  idPoly,
const AcspNeighbors& neighbors,
const common& commonDef,
const instanceMappings& instanceMapping, 
const spaces& spacesGenerator,
const CS_testLocations& testLocations);

    void addAssignmentCS_checks(
AcspSummandsPolynomial& bigPoly, 
const vector<unique_ptr<PolynomialInterface>>& constraints,
const AcspSummandsPolynomial::polyID&  idPoly,
const AcspNeighbors& neighbors,
const common& commonDef,
const instanceMappings& instanceMapping, 
const spaces& spacesGenerator,
const CS_testLocations& testLocations);
    
    void addRoutingNetwork_checks(
AcspSummandsPolynomial& bigPoly, 
const AcspSummandsPolynomial::polyID&  columnVanishingPoly,
const AcspSummandsPolynomial::polyID&  idPoly,
const AcspNeighbors& neighbors,
const common& commonDef,
const instanceMappings& instanceMapping,
const spaces& spacesGenerator,
const vector<FieldElement>& paddingPi);
}

/**
 * Construct the constraints polynomial from a Bair instance.
 * This function does the actual work of wiring the constraints-polynomial arithmetic circuit, using some helper functions for sub-circuits.
 */
unique_ptr<Algebra::PolynomialInterface> constraints::getConstraintsPoly(
const BairInstance& instance,
const AcspNeighbors& neighbors,
const common& commonDef,
const instanceMappings& instanceMapping,
const spaces& spacesGenerator,
const CS_testLocations& testLocations){
    
    // initialize result polynomial
    AcspSummandsPolynomial result(spacesGenerator.getVanishingSpaceBasis());

    // add commonly used subspace polynomials
    const auto columnVanishingPoly = result.addSubspacePoly(spacesGenerator.getNetworkColumnBasis());
    const auto idPoly = result.addSubspacePoly(Algebra::elementsSet_t()); //vanishing on span( empty set )

    //embed the Permutation constraint system checks
    addPermutationCS_checks(
            result, 
            commonDef.getConstraintsPi(),
            columnVanishingPoly,
            idPoly,
            neighbors,
            commonDef,
            instanceMapping,
            spacesGenerator, 
            testLocations);

    //embed the Assignment constraint system checks
    addAssignmentCS_checks(
            result, 
            commonDef.getConstraintsChi(),
            idPoly,
            neighbors,
            commonDef,
            instanceMapping, 
            spacesGenerator, 
            testLocations);
    
    
    /******************************************************************
     *        embed the Routing Network checks (if exists)
     ******************************************************************/
     if(commonDef.hasRoutingNetwork()){
        addRoutingNetwork_checks(
            result, 
            columnVanishingPoly,
            idPoly,
            neighbors,
            commonDef,
            instanceMapping,
            spacesGenerator,
            instance.paddingPi());

     }
    
    return result.clone();

}

namespace{
class sparceInputsPoly : public PolynomialInterface{
public:
    sparceInputsPoly(const PolynomialInterface& poly, const map<size_t,size_t> varsTranslation, const size_t inputsNum):
        poly_(poly.clone()),inputsNum_(inputsNum){
            
            size_t maxUsed = 0;
            for(const auto& translation : varsTranslation){
                varsTranslation_.push_back(translation);
                maxUsed = std::max(maxUsed,translation.first);
            }

            assignemntLen_ = maxUsed+1;
        };

    FieldElement eval(const vector<FieldElement>& x)const{
        vector<FieldElement> assignment(assignemntLen_);
        for(const auto& translation : varsTranslation_){
            assignment[translation.first] = x[translation.second];
        }
        return poly_->eval(assignment);
    }

    size_t numVars()const{
        return inputsNum_;
    }
    
    bool isRoot(const vector<FieldElement>& x)const{
        vector<FieldElement> miniAssignment(inputsNum_, zero());
        for(const auto& translation : varsTranslation_){
            miniAssignment[translation.first] = x[translation.second];
        }
        return poly_->isRoot(miniAssignment);
    }

    PolynomialDegree getDegreeBound(const vector<PolynomialDegree>& inputDegrees)const{
        vector<PolynomialDegree> miniInputDegrees(inputsNum_,PolynomialDegree(0));
        for(const auto& translation : varsTranslation_){
            miniInputDegrees[translation.first] = inputDegrees[translation.second];
        }
        return poly_->getDegreeBound(miniInputDegrees);
    }

    unique_ptr<PolynomialInterface> clone()const{
        map<size_t,size_t> varsTransMap;
        for(const auto& translation : varsTranslation_){
            varsTransMap[translation.first] = translation.second;
        }
        return unique_ptr<PolynomialInterface>(new sparceInputsPoly(*poly_,varsTransMap,inputsNum_));
    }

    bool isEffectiveInput(const size_t varId)const{
        for(const auto& translation : varsTranslation_){
            if ((translation.second == varId) && (poly_->isEffectiveInput(translation.first))){
                return true;
            }
        }
        return false;
    }

private:
    const unique_ptr<PolynomialInterface> poly_;
    vector<std::pair<size_t,size_t>> varsTranslation_;
    const size_t inputsNum_;
    size_t assignemntLen_;
};

class booleanityTestPoly : public PolynomialInterface{
public:
    booleanityTestPoly(const size_t varToTest):varToTest_(varToTest){};
   
    FieldElement eval(const vector<FieldElement>& x)const{
        const FieldElement& val = x[varToTest_];
        return val*(val-one());
    }

    size_t numVars()const{
        return varToTest_+1;
    }
    
    bool isRoot(const vector<FieldElement>& x)const{
        const FieldElement& val = x[varToTest_];
        return (val==one()) || (val==zero());
    }

    PolynomialDegree getDegreeBound(const vector<PolynomialDegree>& inputDegrees)const{
        return degreeOfProduct(inputDegrees[varToTest_],inputDegrees[varToTest_]);
    }

    unique_ptr<PolynomialInterface> clone()const{
        return unique_ptr<PolynomialInterface>(new booleanityTestPoly(varToTest_));
    }

    bool isEffectiveInput(const size_t varId)const{
        return varId == varToTest_;
    }

private:
    const size_t varToTest_;
};

class equivalenceTestPoly : public PolynomialInterface{
public:
    equivalenceTestPoly(const size_t var1, const size_t var2):var1_(var1),var2_(var2){};
   
    FieldElement eval(const vector<FieldElement>& x)const{
        return x[var1_] - x[var2_];
    }

    size_t numVars()const{
        return max(var1_,var2_)+1;
    }
    
    bool isRoot(const vector<FieldElement>& x)const{
        return x[var1_] == x[var2_];
    }

    PolynomialDegree getDegreeBound(const vector<PolynomialDegree>& inputDegrees)const{
        return max(inputDegrees[var1_],inputDegrees[var2_]);
    }

    unique_ptr<PolynomialInterface> clone()const{
        return unique_ptr<PolynomialInterface>(new equivalenceTestPoly(var1_,var2_));
    }

    bool isEffectiveInput(const size_t varId)const{
        return (varId == var1_) || (varId == var2_);
    }

private:
    const size_t var1_;
    const size_t var2_;
};

class constValTestPoly : public PolynomialInterface{
public:
    constValTestPoly(const size_t var, const FieldElement& val):var_(var),val_(val){};
   
    FieldElement eval(const vector<FieldElement>& x)const{
        return x[var_] - val_;
    }

    size_t numVars()const{
        return var_+1;
    }
    
    bool isRoot(const vector<FieldElement>& x)const{
        return x[var_] == val_;
    }

    PolynomialDegree getDegreeBound(const vector<PolynomialDegree>& inputDegrees)const{
        return inputDegrees[var_];
    }

    unique_ptr<PolynomialInterface> clone()const{
        return unique_ptr<PolynomialInterface>(new constValTestPoly(var_,val_));
    }

    bool isEffectiveInput(const size_t varId)const{
        return varId == var_;
    }

private:
    const size_t var_;
    const FieldElement val_;
};

class DeBruijnRoutingTestPoly : public PolynomialInterface{
public:
    DeBruijnRoutingTestPoly(const size_t currVal_var, const size_t neighbor0_var, const size_t neighbor1_var, const size_t routingBit_var):
        currVal_var_(currVal_var),
        neighbor0_var_(neighbor0_var),
        neighbor1_var_(neighbor1_var),
        routingBit_var_(routingBit_var){};
   
    FieldElement eval(const vector<FieldElement>& x)const{
        const FieldElement& v  = x[currVal_var_];
        const FieldElement& n0 = x[neighbor0_var_];
        const FieldElement& n1 = x[neighbor1_var_];
        const FieldElement& b  = x[routingBit_var_];

        /********************************************************************
         * Construct the test
         *
         * The test is just the polynomial:
         * Let \f$ b \f$ be the value of the routing bit,
         * \f$ v \f$ be the value of in current location,
         * and \f$ n_0, n_1 \f$ be the values in the both neighbours.
         * The test should verify that if \f$ b = 0 \f$ than \f$ v = n_0 \f$
         * and otherwise if \f$ b = 1 \f$ than \f$ v=n_1 \f$.
         * (It is verified using the booleanity test that \f$ b \in \{0,1\} \f$)
         * 
         * It does it using the polynomial (over a field of characteristic 2 only):
         * \f$ b \cdot (v+n_1) + (1+b) \cdot (v+n_0) = 
         * b \cdot ( v+n_1 + v+n_0 ) + (v+n_0) =
         * b \cdot ( n_0 + n_1) + v + n_0 \f$
         * *****************************************************************/
        return b*(n0 + n1) + v + n0;
    }

    size_t numVars()const{
        return max(max(currVal_var_, routingBit_var_), max(neighbor0_var_,neighbor1_var_))+1;
    }
    
    PolynomialDegree getDegreeBound(const vector<PolynomialDegree>& inputDegrees)const{
        const PolynomialDegree& v  = inputDegrees[currVal_var_];
        const PolynomialDegree& n0 = inputDegrees[neighbor0_var_];
        const PolynomialDegree& n1 = inputDegrees[neighbor1_var_];
        const PolynomialDegree& b  = inputDegrees[routingBit_var_];
        
        /// degree of \f$ b \cdot (v - n_0) \f$
        const PolynomialDegree d0 = degreeOfProduct(b,max(v,n0));
        
        /// degree of \f$ b \cdot (v - n_1) \f$
        const PolynomialDegree d1 = degreeOfProduct(b,max(v,n1));

        return max(d0,d1);
    }

    unique_ptr<PolynomialInterface> clone()const{
        return unique_ptr<PolynomialInterface>(new DeBruijnRoutingTestPoly(*this));
    }

    bool isEffectiveInput(const size_t varId)const{
        return  (varId == currVal_var_) 
                || 
                (varId == neighbor0_var_)
                || 
                (varId == neighbor1_var_)
                || 
                (varId == routingBit_var_);
    }

private:
    const size_t currVal_var_; 
    const size_t neighbor0_var_;
    const size_t neighbor1_var_;
    const size_t routingBit_var_;
};

void addAssignmentCS_checks(
AcspSummandsPolynomial& bigPoly, 
const vector<unique_ptr<PolynomialInterface>>& constraints,
const AcspSummandsPolynomial::polyID&  idPoly,
const AcspNeighbors& neighbors,
const common& commonDef,
const instanceMappings& instanceMapping, 
const spaces& spacesGenerator,
const CS_testLocations& testLocations){
    
    const auto columnVanishingPoly_noCarry = bigPoly.addSubspacePoly(spacesGenerator.getNetworkColumnAssignmentNoCarryBasis());
    const size_t chiCarryBitLocation =  commonDef.heightSpaceDimension() - 1;
    const FieldElement offsetForWithCarrySpace = mapIntegerToFieldElement(chiCarryBitLocation,1,1);
    
    const size_t assignmentSize = 2*(commonDef.variablesPerm().size() + commonDef.variablesNonPerm().size());
   
    // each polynomial in the constraint system find right assignment for it,
    //and multiply the result by a selector for current location
    for(size_t poly_indx=0; poly_indx<constraints.size(); poly_indx++){
        const PolynomialInterface* poly = constraints[poly_indx].get();

        const size_t testIndex = testLocations.indexOfConstraint_Assignment(poly_indx);

        // the test location shift (a field element in the tested column)
        const FieldElement testShift = instanceMapping.mapNonPermutationElement(testIndex);

        // construct the auxiliary polynomials
        // There are two such polynomials :
        // 
        // One vanishing on the zero row in current column
        // because the constraint system consistency tests should not be done there
        //
        // The second vanishes on the last row (aka \f$ g^{-1} \f$) in current column
        // because the definition of Bair does not require consistency
        // between the last assignment and the first assignment
        const FieldElement zeroRowOffset = instanceMapping.mapNonPermutation_zeroRow(testIndex);
        const FieldElement lastRowOffset = instanceMapping.mapNonPermutation_lastRow(testIndex);
        const AcspSummandsPolynomial::AuxPoly zeroRowAuxPoly(idPoly,zeroRowOffset);
        const AcspSummandsPolynomial::AuxPoly lastRowAuxPoly(idPoly,lastRowOffset);
        const AcspSummandsPolynomial::AuxPolyVec auxPolys_noCarry = {zeroRowAuxPoly}; //when the MSB cleared, multiplying by $x$ could not produce $1$
        const AcspSummandsPolynomial::AuxPolyVec auxPolys_withCarry = {lastRowAuxPoly}; //when the MSB set, it can not be the zero index

        //
        // construct the constraint polynomial for the current summand
        //
        
        // construct input translation
        map<size_t,size_t> inputTranslation_noCarry;
        map<size_t,size_t> inputTranslation_withCarry;
        bool usesVarFromNextLine = false;
        for(size_t varId=0; varId< assignmentSize; varId++){

            //if the variable is used, get next input from the global inputs
            if(neighbors.existsAssignmentCS(poly_indx,varId)){
                inputTranslation_noCarry  [varId] = 1 + neighbors.locationOfAssignmentCS(poly_indx,varId,0);
                inputTranslation_withCarry[varId] = 1 + neighbors.locationOfAssignmentCS(poly_indx,varId,1);
                if (varId > assignmentSize/2){
                    usesVarFromNextLine = true;
                }
            }
        }

        // construct the constraint polynomial
        const sparceInputsPoly constraintPoly_noCarry(*poly,  inputTranslation_noCarry, assignmentSize);
        const sparceInputsPoly constraintPoly_withCarry(*poly,  inputTranslation_withCarry, assignmentSize);

        //
        // Add the summand
        //

        bigPoly.addSummand(columnVanishingPoly_noCarry, testShift, constraintPoly_noCarry, auxPolys_noCarry);
        if(usesVarFromNextLine){
            bigPoly.addSummand(columnVanishingPoly_noCarry, offsetForWithCarrySpace + testShift, constraintPoly_withCarry, auxPolys_withCarry);
        }
    }
}

void addPermutationCS_checks(
AcspSummandsPolynomial& bigPoly, 
const vector<unique_ptr<PolynomialInterface>>& constraints,
const AcspSummandsPolynomial::polyID&  columnVanishingPoly,
const AcspSummandsPolynomial::polyID&  idPoly,
const AcspNeighbors& neighbors,
const common& commonDef,
const instanceMappings& instanceMapping, 
const spaces& spacesGenerator,
const CS_testLocations& testLocations){
     
    const size_t assignmentSize = 2*(commonDef.variablesPerm().size() + commonDef.variablesNonPerm().size());
   
    // each polynomial in the constraint system find right assignment for it,
    //and multiply the result by a selector for current location
    for(size_t poly_indx=0; poly_indx<constraints.size(); poly_indx++){
        const PolynomialInterface* poly = constraints[poly_indx].get();

        const size_t testIndex = testLocations.indexOfConstraint_Permuation(poly_indx);

        // the test location shift (a field element in the tested column)
        const FieldElement testShift = instanceMapping.mapNonPermutationElement(testIndex);

        // construct the auxiliary polynomials
        // There are two such polynomials :
        // 
        // One vanishing on the zero row in current column
        // because the constraint system consistency tests should not be done there
        //
        // The second vanishes on the last row (aka \f$ g^{-1} \f$) in current column
        // because the definition of Bair does not require consistency
        // between the last assignment and the first assignment
        const FieldElement zeroRowOffset = instanceMapping.mapNonPermutation_zeroRow(testIndex);
        const FieldElement lastRowOffset = instanceMapping.mapNonPermutation_lastRow(testIndex);
        const AcspSummandsPolynomial::AuxPoly zeroRowAuxPoly(idPoly,zeroRowOffset);
        const AcspSummandsPolynomial::AuxPoly lastRowAuxPoly(idPoly,lastRowOffset);
        const AcspSummandsPolynomial::AuxPolyVec auxPolys = {zeroRowAuxPoly, lastRowAuxPoly};

        //
        // construct the constraint polynomial for the current summand
        //
        
        // construct input translation
        map<size_t,size_t> inputTranslation;
        for(size_t varId=0; varId< assignmentSize; varId++){

            //if the variable is used, get next input from the global inputs
            if(neighbors.existsPermCS(poly_indx,varId)){
                inputTranslation[varId] = 1 + neighbors.locationOfPermCS(poly_indx,varId);
            }
        }

        // construct the constraint polynomial
        const sparceInputsPoly constraintPoly(*poly,  inputTranslation, assignmentSize);

        //
        // Add the summand
        //

        bigPoly.addSummand(columnVanishingPoly, testShift, constraintPoly, auxPolys);
    }
}

void addRoutingNetwork_checks(
AcspSummandsPolynomial& bigPoly,
const AcspSummandsPolynomial::polyID&  columnVanishingPoly,
const AcspSummandsPolynomial::polyID&  idPoly,
const AcspNeighbors& neighbors,
const common& commonDef,
const instanceMappings& instanceMapping,
const spaces& spacesGenerator,
const vector<FieldElement>& paddingPi){
    
    // prepare subspace polynomials
    const auto layersVanishingPoly = bigPoly.addSubspacePoly(spacesGenerator.getNetworkLayersBasis());

    // calculate the shift for last network column
    const size_t lastColIndex = commonDef.imageWidth() - 2;
    const FieldElement lastColShift = instanceMapping.mapPermutationColumnId_spaceElement(lastColIndex);
    
    
    /******************************************************************
     *  add the routing bits checks (verify they are all boolean)
     ******************************************************************/
    {
        const size_t routingBitsLayerIndex0 = 2*commonDef.variablesPerm().size();
        const size_t routingBitsLayerIndex1 = routingBitsLayerIndex0 + 1;
        const FieldElement routingBitsLayerOffset0 = instanceMapping.mapPermutationLayerId_spaceElement(routingBitsLayerIndex0);
        const FieldElement routingBitsLayerOffset1 = instanceMapping.mapPermutationLayerId_spaceElement(routingBitsLayerIndex1);

        // construct the auxiliary polynomials,
        // those the polynomials that vanishes on both routing bits layers,
        // on the zero column (no network there)
        // and on the last column (no routing bits there)
        const FieldElement layer0_zeroColShift = instanceMapping.mapPermutationLayerId_spaceElement(routingBitsLayerIndex0);
        const FieldElement layer1_zeroColShift = instanceMapping.mapPermutationLayerId_spaceElement(routingBitsLayerIndex1);
        const FieldElement layer0_lastColShift = lastColShift + layer0_zeroColShift;
        const FieldElement layer1_lastColShift = lastColShift + layer1_zeroColShift;
        
        const AcspSummandsPolynomial::AuxPoly layer0_zeroCol_poly(columnVanishingPoly,layer0_zeroColShift);
        const AcspSummandsPolynomial::AuxPoly layer1_zeroCol_poly(columnVanishingPoly,layer1_zeroColShift);
        const AcspSummandsPolynomial::AuxPoly layer0_lastCol_poly(columnVanishingPoly,layer0_lastColShift);
        const AcspSummandsPolynomial::AuxPoly layer1_lastCol_poly(columnVanishingPoly,layer1_lastColShift);

        const AcspSummandsPolynomial::AuxPolyVec auxPolys0 = {layer0_zeroCol_poly, layer0_lastCol_poly};
        const AcspSummandsPolynomial::AuxPolyVec auxPolys1 = {layer1_zeroCol_poly, layer1_lastCol_poly};

        //construct the booleanity test polynomial
        const booleanityTestPoly test0(neighbors.locationOfId(routingBitsLayerIndex0) + 1);
        const booleanityTestPoly test1(neighbors.locationOfId(routingBitsLayerIndex1) + 1);

        // add the summand
        bigPoly.addSummand(layersVanishingPoly, routingBitsLayerOffset0, test0, auxPolys0); 
        bigPoly.addSummand(layersVanishingPoly, routingBitsLayerOffset1, test1, auxPolys1); 
    }

    /******************************************************************
     *      add the last DeBruijn column identical check
     ******************************************************************/
    for(size_t currVar=0; currVar < commonDef.variablesPerm().size(); currVar++){
        const size_t currLayer = 2*currVar;
        
        // get the test shift
        const FieldElement testShift = lastColShift + instanceMapping.mapPermutationLayerId_spaceElement(currLayer);

        // get the constraint polynomial
        const equivalenceTestPoly test(1+neighbors.locationOfId(currLayer), 1+neighbors.locationOfTwinLayer(currLayer));

        // add the test
        bigPoly.addSummand(columnVanishingPoly, testShift, test, {});
    }
   
    /******************************************************************
     *      add the zero element in routing table
     *      is routed to itself
     *      and its value is the padding value check
     ******************************************************************/
    for(size_t currVar=0; currVar < 2*commonDef.variablesPerm().size(); currVar++){
        const size_t currLayer = currVar;
        
        // get the test shift
        const FieldElement testShift = instanceMapping.mapPermutationElement_spaceElement(0,currLayer);

        // get the expected value
        const FieldElement expectedVal = paddingPi[commonDef.variablesPerm()[currVar>>1]];

        // get the constraint polynomial
        const constValTestPoly test(1+neighbors.locationOfId(currVar), expectedVal);

        // add the test
        bigPoly.addSummand(idPoly, testShift, test, {});
    }

    /******************************************************************
     *          embed the DeBruijn routing checks
     ******************************************************************/
    {
    //a polynomial that vanishes on the vector space where DeBruijn vertexes
    //have only 2 neighbors each, namely the most significant bits of
    //both the column ID and the row ID are zeros.
    //
    //Notice this is a subspace of the layer space, and it contains as
    //well the last column of the DeBruijn, which has no outgoing
    //edges from it vertexes at all
    const auto vanishingOnDeBruijnTwoNeighborsSpace = bigPoly.addSubspacePoly(spacesGenerator.getDeBruijnNeighborsPairSpaceBasis());

    
    //calculate shifts for DeBruijn routing test
    const vector<FieldElement> DeBruijnShifts = spacesGenerator.getDeBruijnLayerCosets();

    //each routed variable uses two DeBruijn layers
    const size_t amountOfLayersToCheck = commonDef.variablesPerm().size()*2;
    
    for(size_t currLayer=0; currLayer < amountOfLayersToCheck; currLayer++){
       
        // Construct the auxiliary polynomials
        const FieldElement currLayerOffset = instanceMapping.mapPermutationLayerId_spaceElement(currLayer);
        const AcspSummandsPolynomial::AuxPoly vanishingOnZeroColumn(columnVanishingPoly,currLayerOffset);
        const AcspSummandsPolynomial::AuxPoly vanishingOnLastColumn(columnVanishingPoly,currLayerOffset + lastColShift);
        const AcspSummandsPolynomial::AuxPolyVec auxPolys = {vanishingOnZeroColumn, vanishingOnLastColumn};

        for(size_t cosetId=0; cosetId < DeBruijnShifts.size(); cosetId++){

            // Construct the test polynomial
            const size_t currVal_var = neighbors.locationOfId(currLayer) + 1;
            const size_t N0_var = neighbors.locationOfDeBruijn(0,cosetId,currLayer) + 1;
            const size_t N1_var = neighbors.locationOfDeBruijn(1,cosetId,currLayer) + 1;
            const size_t bit_var = neighbors.locationOfRoutingBit(currLayer) + 1;
            const DeBruijnRoutingTestPoly test(currVal_var, N0_var, N1_var, bit_var);

            // Add the test
            bigPoly.addSummand(vanishingOnDeBruijnTwoNeighborsSpace, DeBruijnShifts[cosetId] + currLayerOffset, test, auxPolys);
        }
    }
    }
}

}

} //namespace BairToAcsp
} //namespace libstark
