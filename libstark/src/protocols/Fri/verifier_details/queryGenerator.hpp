#ifndef RS_IOPP_BIASED_DETAILS_QUERYGEN_HPP__
#define RS_IOPP_BIASED_DETAILS_QUERYGEN_HPP__

#include "../common/common.hpp"
#include "protocols/common/verifier.hpp"
#include <algebraLib/FieldElement.hpp>
#include <vector>

namespace libstark{
namespace Protocols{
namespace Fri{
namespace Verifier{
namespace details{

class RS_result{
public:
    std::vector<Algebra::FieldElement> results;
    
    void init(const std::vector<Algebra::FieldElement>& basis, const size_t degBound);
    bool verify()const;
private:
    std::vector<Algebra::FieldElement> basis_;
    size_t degBound_;
};

class ConsistencyPath_result{
public:
    std::vector<Algebra::FieldElement> L0Data;
    Algebra::FieldElement columnPointData;
    
    void init(const std::vector<Algebra::FieldElement>& basis, const Algebra::FieldElement& columnId);
    bool verify()const;
private:
    std::vector<Algebra::FieldElement> basis_;
    Algebra::FieldElement columnId_;
    std::vector<Algebra::FieldElement> lagrangeConstants_;
};

class equavalenceQuery_result{
public:
    Algebra::FieldElement res1;
    Algebra::FieldElement res2;
    bool verify()const{return res1 == res2;}
};

typedef std::vector<ConsistencyPath_result> Consistency_path_t;

typedef common::state_t<std::map<size_t,ResultLocation>> RS_queriesTree;

struct RS_queries{
    RS_queriesTree proof;
    std::map<size_t,std::vector<Algebra::FieldElement*>> univariate;
};

void addRandomComitmentPath(const short degBound_logCeil, const std::vector<Algebra::FieldElement>& evaluationBasis, RS_queries& queries, RS_result& results, const size_t depth, const bool L0isMSB);

void addRandomQueryPath(RS_queries& queries, const std::vector<Algebra::FieldElement>& evaluationBasis, Consistency_path_t& results, const bool L0isMSB);

} //namespace details
} //namespace Verifier
} //nmasepace Fri
} //namespace Protocols
} //namespace libstark

#endif //#ifndef RS_IOPP_BIASED_DETAILS_QUERYGEN_HPP__
