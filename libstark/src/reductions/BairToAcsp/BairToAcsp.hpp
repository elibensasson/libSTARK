/************************************** TinyRAMtoBair.hpp ****************************************/

/**
 * @file BEXtoAcsp.hpp
 *
 * Declares the BairToAcsp reduction class
 */

  /***********************************************************************************************/

#pragma once
#ifndef __Bair_TO_Acsp_HPP
#define __Bair_TO_Acsp_HPP

#include "languages/Bair/BairInstance.hpp"
#include "languages/Bair/BairWitness.hpp"
#include "languages/Acsp/AcspInstance.hpp"
#include "languages/Acsp/AcspWitness.hpp"

#include <memory>

namespace libstark{


class CBairToAcsp{
public:
    /**
    * Reduces an BairInstance to an AcspInstance.
    * @param[in]  partialInstance
    * @return a pointer to an AcspInstance.
    */
    static ::std::unique_ptr<AcspInstance> reduceInstance(const BairInstance&  instance, const std::vector<Algebra::FieldElement>& coeffsPi, const std::vector<Algebra::FieldElement>& coeffsChi);

    /**
    * Reduces a BairExecutedWitness to an AcspWitness.
    * @param[in]  fullInstance
    * @param[in]  witness
    * @return a pointer to an AcspWitness.
    */
    static ::std::unique_ptr<AcspWitness> reduceWitness(const BairInstance& instance, const BairWitness& witness);

}; // class BairToAcsp

}//namespace libstark

#endif // __Bair_TO_Acsp_HPP
