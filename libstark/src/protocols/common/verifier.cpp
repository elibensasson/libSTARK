#include "verifier.hpp"
#include <set>

namespace libstark{
namespace Protocols{

using Algebra::FieldElement;
using std::set;
    
uniEvalView_t::uniEvalView_t(const short logSizeBytes_) :
    view(logSizeBytes_),logSizeBytes(logSizeBytes_){};
uniEvalView_t::uniEvalView_t() : view(10), logSizeBytes(1){};

void uniEvalView_t::fetchResults()const{
    for(const auto& q : queries){
        const size_t blockIdx = q.first;
        const auto& blockData = view.readData(blockIdx);
        const FieldElement* dataAsVec = (FieldElement*)(&blockData);
        
        for(const auto& qq : q.second){
            const short offsetInBlock = qq.first;
            const FieldElement& res = dataAsVec[offsetInBlock];

            //write value
            for(auto& answer : qq.second){
                answer(res);
            }
        }
    }
}

rawQuery_t uniEvalView_t::getRawQuery()const{
    rawQuery_t res;
    for(const auto& q : queries){
        const size_t blockIndex = q.first;
        const size_t blockPairIndex = blockIndex>>1;
        res.insert(blockPairIndex);
    }

    return res;
}

void uniEvalView_t::fillResultsAndCommitmentRandomly(){
    //fill commitment
    {
        //char* h_bytes = (char*)(&commitment);
        //for(int i=0; i< sizeof(commitment); i++){
        //    h_bytes[i] = rand();
        //}
    }
    
    //fill results
    rawResult_t results(expectedResultsLenght());
    
    //for(auto& hash_val : results){
    //    char* h_bytes = (char*)(&hash_val);
    //    for(int i=0; i< sizeof(hash_val); i++){
    //        h_bytes[i] = rand();
    //    }
    //}

    digestResults(results);
}

void uniEvalView_t::digestResults(const rawResult_t& results){
    //If there are no queries to this buffer it is skipped
    if(queries.size() == 0){
        return;
    }
    
    set<size_t> queriesIndices;
    for(const auto& p : queries){
        queriesIndices.insert(p.first);
    }

    view.DeSerialize(queriesIndices , results);
}

size_t uniEvalView_t::expectedResultsLenght()const{
   
    //If there are no queries to this buffer it is skipped
    if(queries.size() == 0){
        return 0;
    }
    
    set<size_t> queriesIndices;
    for(const auto& p : queries){
        queriesIndices.insert(p.first);
    }

    return view.getSerializationMapping(queriesIndices).size();
}

size_t uniEvalView_t::expectedQueriedFieldElementsNum()const{
   size_t res = 0;
   for(const auto& blockQueries : queries){
    res+= blockQueries.second.size();
   }
   return res;
}

bool uniEvalView_t::verifyComitment()const{
    //If there are no queries to this buffer it is skipped
    if(queries.size() == 0){
        return true;
    }
    
    const bool res = (commitment == view.calculateRoot());
    if(!res){
        return false;
    }
    return true;
}

void ResultLocation::addAnswerPtr(FieldElement* ptr){
    answerLocations_.push_back(ptr);
}

void ResultLocation::answer(const FieldElement& res)const{
    for (const auto& e : answerLocations_){
        *e = res;
    }
}

} //namespace Protocols
} //namespace libstark
