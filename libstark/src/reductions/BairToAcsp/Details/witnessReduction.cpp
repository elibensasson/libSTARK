#include "witnessReduction.hpp"
#include "../Routing/LongSymmetricDeBruijnNetwork.hpp"
#include "common/Utils/TaskReporting.hpp"

#include <vector>

namespace libstark{
namespace BairToAcsp{

using std::vector;
using std::unique_ptr;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::FieldElement;
using Algebra::zero;
using Algebra::one;

typedef LongSymmetricDeBruijnNetwork permNetwork_t;

unique_ptr<AcspWitness> witnessReduction::reduceWitness( const BairInstance& instance, const BairWitness& witness){
    
    TASK("Reducing Bair witness to Acsp");
    
    //get common information
    common commonDef(instance, vector<FieldElement>(instance.constraintsPermutation().numMappings(),one()), vector<FieldElement>(instance.constraintsAssignment().numMappings(),one()));
    witnessMappings witnessMapping(commonDef);

    //get the embedding
    vector<evaluation_t> mappings = getEmbeddingMapping(instance, witness, commonDef,witnessMapping);
    
    //ordered basis for witness space
    const auto& basis = witnessMapping.getImageSpaceOrderedBasis();
//#define ZERO_WITNESS  //sometimes for debugging it's convenient to use an identically zero witness
#ifdef ZERO_WITNESS 
	for (auto& p : mapping)
		p = zero();
#endif
    //construct witness
    //and return it
    std::vector<unique_ptr<const AcspWitness::polynomial>> assignments;
    for(const auto& mapping : mappings){
        assignments.push_back(unique_ptr<const AcspWitness::polynomial>(new UnivariatePolynomialGeneral(mapping,basis,zero())));
    }
    unique_ptr<AcspWitness> witness_ptr(new AcspWitness(move(assignments)));
    return move(witness_ptr);
}

class exponentsPermutation : public Sequence<size_t>{
public:
    exponentsPermutation(const BairInstance& instance):instance_(instance),commonDef_(instance, vector<FieldElement>(instance.constraintsPermutation().numMappings(),one()), vector<FieldElement>(instance.constraintsAssignment().numMappings(),one())){};

	size_t getElementByIndex(index_t index)const{
        const size_t singeltonIndex = instance_.domainSize();
           
        //singleton case 
        if(index == singeltonIndex) return 0;

        //general case
        return expModulu(index);
    }

private:
    const BairInstance& instance_;
    const common commonDef_;

    size_t expModulu(const size_t exp)const{
        
        const int64_t modulus = commonDef_.rowsModulus();
        const size_t modulus_deg = floor(Infrastructure::Log2(modulus));
        const size_t overflow_mask = 1UL<<modulus_deg;

        size_t res = 1;
        for(size_t i=1; i<= exp ; i++){
            res <<= 1;
            if( res & overflow_mask){
                res ^= modulus;
            }
        }

        return res;
    }
};

class inversePermutation : public Sequence<size_t>{
    public :
        inversePermutation(const Sequence<size_t>& src, const size_t numElements): seq_(numElements){
            for(size_t val=0; val< numElements; val++){
                const size_t index = src.getElementByIndex(val);
                seq_[index] = val;
            }
        }
		size_t getElementByIndex(index_t index)const{
            if (index < seq_.size()) return seq_[index];
            else {
                _COMMON_FATAL("Access to such index is unexpected");
            }
        }
    private:
        vector<size_t> seq_;
};

//represents the permutation \f$ g \circ h \circ g^{-1} \f$
class conjugatePermutation : public Sequence<size_t>{
    public:
        conjugatePermutation(const Sequence<size_t>& g, const Sequence<size_t>& h, const size_t numElements):
            g_(g), h_(h), g_inv_(g,numElements){};
        
		size_t getElementByIndex(index_t index)const{
            const size_t v1 = g_inv_.getElementByIndex(index);
            const size_t v2 = h_.getElementByIndex(v1);
            const size_t v3 = g_.getElementByIndex(v2);

            return v3;
        } 

    private:
        const Sequence<size_t>& g_;
        const Sequence<size_t>& h_;
        const inversePermutation g_inv_;
};

class addSingeltonToPermutation : public Sequence<size_t>{
    public:
        addSingeltonToPermutation(const Sequence<size_t>& orig, const size_t singletoneIndex): origPerm_(orig) ,singletoneIndex_(singletoneIndex){};
        size_t getElementByIndex(index_t index)const{
            if (index == singletoneIndex_) return singletoneIndex_;
            return origPerm_.getElementByIndex(index);
        }
    
    private:
        const Sequence<size_t>& origPerm_;
        const size_t singletoneIndex_;
};

void fillWithRandomVals(vector<vector<FieldElement>>& src){
    for (auto& v : src){
        for (auto& e : v){
            e = Algebra::generateRandom();
        }
    }
}

//Mapping
vector<witnessReduction::evaluation_t> witnessReduction::getEmbeddingMapping( const BairInstance& instance, const BairWitness& witness, const common& commonDef, const witnessMappings& witnessMapping){

    //define the mapping on which the assignment polynomial will be interpolated on
    //This mapping is the arithmetization of the witness
    const size_t mappingSize = Infrastructure::POW2(witnessMapping.getImageSpaceOrderedBasis().size());
    vector<evaluation_t> mappings(commonDef.witnessLayersAmount(), evaluation_t(mappingSize,Algebra::zero()));

    //fill with random values for ZK
    fillWithRandomVals(mappings);
    
    //Map the coloring (assignment)
    mapChi(instance,witness,mappings,commonDef,witnessMapping);

    //Map the routing network (Pi) if needed
    if(commonDef.hasRoutingNetwork()){
        mapNetwork(instance,witness,mappings,commonDef,witnessMapping);
    }
    
    //return the mapping
    return mappings;
}

void witnessReduction::mapChi(const BairInstance& instance, const BairWitness& witness, vector<evaluation_t>& mappings, const common& commonDef, const witnessMappings& witnessMapping){
    const size_t cyclicDomainSize = instance.domainSize();
    
    const vector<size_t> unroutedVars = commonDef.variablesNonPerm();

    //ordered basis for witness space
    const auto& basis = witnessMapping.getImageSpaceOrderedBasis();

    //Map the coloring of the circle
    {
    auto currRow_spaceIndex = witnessMapping.getFirstRow_spaceIndex();
    for( size_t vecId =0; vecId < cyclicDomainSize; vecId++){
        const auto& assignment = witness.get_color(vecId);
        for (const size_t& varId :  unroutedVars){
            const size_t varIndex = commonDef.getVarLocation(varId).index;
            const commonMappings::witnessIndex_t indicator = witnessMapping.mapIndexOfNonPermutationVariable_witnessIndex(currRow_spaceIndex,varIndex);
            mappings[indicator.first][indicator.second] = assignment[varId];
        }
        currRow_spaceIndex = witnessMapping.getNextRow_spaceIndex(currRow_spaceIndex);
    }

    }

}

void witnessReduction::mapNetwork(const BairInstance& instance, const BairWitness& witness, vector<evaluation_t>& mappings, const common& commonDef, const witnessMappings& witnessMapping){
    
    /// We want the "log" permutation, the one that maps:
    /// \f$ g^i \mapsto i \f$, and for the special case: \f$ 0 \mapsto singletoneIndex \f$
    /// The log permutation defines the mapping from the label in the first column
    /// of the DeBruijn to the vectorId it should hold,
    /// thus it defines the mapping from the values in the network to vectors ids
    const exponentsPermutation expPerm(instance);
    const inversePermutation logPerm(expPerm, instance.domainSize()+1);

    //Add another element to permutation as a singleton, so we can rout it using DeBruijn
    const addSingeltonToPermutation expendedPerm(witness.permutation(),instance.domainSize());
    
    /// Because we mess with the indexes, we have to change the permutation as well
    /// If the good permutation was \f$ i \mapsto \pi(i) \f$
    /// after changing the numbering, using some permutation \f$ \sigma \f$
    /// if the routing network will (originally) rout \f$ \pi \f$ we
    /// would get the permutation \f$ \sigma(i) \mapsto \sigma \circ \pi(i) \f$
    /// But what we want is \f$ \sigma(i) \mapsto \pi \circ \sigma(i) \f$,
    /// So instead we rout the conjugate permutation \f$ \sigma^{-1} \circ \pi \circ \sigma \f$
    /// This way we get: 
    /// \f$ \sigma(i) \mapsto \sigma \circ \sigma^{-1} \circ \pi \circ \sigma(i) = \pi \circ \sigma(i) \f$
    const conjugatePermutation permToRout(expPerm,expendedPerm, instance.domainSize()+1);

    //map routing network
    permNetwork_t net(instance.domainSizeIndicator());
    net.rout(permToRout);
   
    const vector<size_t> routedIndexes = commonDef.variablesPerm();

    //define the index for the additional vector
    const size_t addedVecId = instance.domainSize();
    
    //ordered basis for witness space
    const auto& basis = witnessMapping.getImageSpaceOrderedBasis();

    //map the main routing network
    {
    
    for (size_t rowId=0; rowId < net.height(); rowId++){
        for(size_t columnId = 0; columnId < net.getWingWidth() ; columnId++){
            for(short netPartId = 0; netPartId < 2; netPartId++){//loop over the two halves

                const RoutingNetwork::dataID_t dataId = net.getDataID(netPartId, columnId, rowId);
                const size_t vecId = logPerm.getElementByIndex(dataId);
                const auto& coloring =  (vecId == addedVecId? instance.paddingPi() : witness.get_color(vecId));
                
                //map routing network
                for (size_t packetId = 0; packetId < routedIndexes.size(); packetId++){
                    const auto currPacketIndex = routedIndexes[packetId];
                    const FieldElement val = coloring[currPacketIndex];
                    const auto indicator_index = witnessMapping.mapNetworkElement_witnessIndex(rowId,columnId,2*packetId + netPartId);
                    mappings[indicator_index.first][indicator_index.second] = val;
                }

                //map routing bits
                if( columnId < net.getWingWidth()-1){
                    const short bitVal = net.routingBit(netPartId,columnId,rowId);
                    const auto indicator_index = witnessMapping.mapNetworkRoutingBit_witnessIndex(rowId,columnId,netPartId);
                    
                    switch(bitVal){
                        case 0: mappings[indicator_index.first][indicator_index.second] = zero(); break;
                        case 1: mappings[indicator_index.first][indicator_index.second] = one(); break;
                        default : _COMMON_FATAL("Bad value of bit");
                    }
                }
            }
        }
    }
    
    }
}
    
} //namespace BairToAcsp
} //namespace libstark
