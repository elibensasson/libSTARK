/**
 * @file BEXtoAcsp_common.hpp
 *
 * Declares the BairToAcsp reduction common,
 * as descried at Acsp Spec document.
 */

#ifndef _COMMON_BairToAcsp_COMMON_HPP_
#define _COMMON_BairToAcsp_COMMON_HPP_

#include "commonDeffinitions.hpp"
#include "commonInformation.hpp"
//#define ONE_CONSTRAINT //ARIEL-For debugging purposes to get to pcp part faster 
namespace libstark{
namespace BairToAcsp{

/*
 * @class common
 * @brief common values, both well defined and those that have some freedom, united in one class
 */
class common : public commonDeffinitions, public commonInformation{
    public:
        common(const BairInstance& instance, const std::vector<Algebra::FieldElement>& coeffsPi, const std::vector<Algebra::FieldElement>& coeffsChi):
            commonDeffinitions(instance, coeffsPi, coeffsChi),
            commonInformation(*((commonDeffinitions*)this))
            {};
        
        common(const common&) = delete;
};
    
} //namespace BairToAcsp
} //namespace libstark

#endif //_COMMON_BairToAcsp_COMMONINFORMATION_HPP_
