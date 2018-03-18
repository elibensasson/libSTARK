/**
 * @file BEXtoAcsp_commonDeffinitions.hpp
 *
 * Declares the BairToAcsp reduction common definitions,
 * as descried at Acsp Spec document.
 */

#ifndef _COMMON_BairToACCSP_COMMONDEFINITIONS_HPP_
#define _COMMON_BairToACCSP_COMMONDEFINITIONS_HPP_

#include "languages/Bair/BairInstance.hpp"
#include <algebraLib/UnivariatePolynomialGeneral.hpp>

namespace libstark{
namespace BairToAcsp{
    
const bool SUPPORT_ZK = true;

/**
 * @class commonDeffinitions
 * @brief represents known values commonly used in the reduction, and depend only on the partial instance
 *
 * We use the BairInstance : \f$(\mathbb{F},d,\mathcal{V},\mathcal{C}_\mathcal{A}, \mathcal{C}_\pi, B)\f$
 */
class commonDeffinitions{

public:

    struct varLocation{
        bool inPerm;
        size_t index;
    };

    //constructor
    commonDeffinitions(const BairInstance& instance, const std::vector<Algebra::FieldElement>& coeffsPi, const std::vector<Algebra::FieldElement>& coeffsChi);

    /*
     * variables partition getters
     */

    /// return the vector of ids of variables that must be passed through the routing network
    /// formally , this is the vector of variable \f$ v \in \mathcal{V} \f$
    /// such that \f$ (v,1) \in [\mathcal{V} \times \{1\}](\mathcal{C}_\pi) \f$
    inline const std::vector<size_t>& variablesPerm()const{return variablesPerm_;}

    /// returns the vector of ids of variable that not need to be passed through the routing network
    /// formally , this is the vector of variable \f$ v \in \mathcal{V} \f$
    /// such that \f$ (v,1) \notin [\mathcal{V} \times \{1\}](\mathcal{C}_\pi) \f$
    inline const std::vector<size_t>& variablesNonPerm()const{return variablesNonPerm_;}

    /// returns the location of the variable with given id,
    /// the location says in which variables vector it is found (permutation variable or not)
    /// and what is its index there.
    ///
    /// for example, if getVarLocation(v) = {inPerm = true, index = i}, than
    /// variablesPerm()[i] = v
    ///
    /// and if getVarLocation(u) = {inPerm = false, index = j}, than
    /// variablesNonPerm()[j] = u
    inline const varLocation& getVarLocation(const size_t varId)const{return bairVarLocation_[varId];}

    /*
     * dimensions and sizes getters
     */
    inline const size_t& heightSpaceDimension()const{return heightSpaceDimension_;}
    inline const size_t imageHeight()const{return Infrastructure::POW2(heightSpaceDimension_);}
    inline const size_t& widthSpaceDimension()const{return widthSpaceDimension_;}
    inline const size_t imageWidth()const{return Infrastructure::POW2(widthSpaceDimension_);}
    inline const size_t& amountOfPermutationLayers()const{return amountOfPermutationLayers_;}
    inline const size_t& vanishingLayersSpaceDimension()const{return vanishingLayersSpaceDimension_;}
    inline const size_t vanishingLayersAmount()const{return Infrastructure::POW2(vanishingLayersSpaceDimension_);}
    inline const size_t witnessLayersAmount()const{return numOfWitnessLayers_;}
    inline size_t vanishingSpaceDimension()const{return heightSpaceDimension()+widthSpaceDimension()+vanishingLayersSpaceDimension();}
    inline bool hasRoutingNetwork()const{return variablesPerm_.size()>0;}
    inline const std::vector<Algebra::PolynomialDegree>& witnessDegreeBound()const{return witnessDegreeBound_;}

    //constraints
    const std::vector<std::unique_ptr<Algebra::PolynomialInterface>>& getConstraintsChi()const{return constraintsChi_;}
    const std::vector<std::unique_ptr<Algebra::PolynomialInterface>>& getConstraintsPi()const{return constraintsPi_;}

    //deleted functions
    commonDeffinitions(const commonDeffinitions&) = delete;
private:
    
    //variables partition
    std::vector<size_t> variablesPerm_;
    std::vector<size_t> variablesNonPerm_;
    std::vector<struct varLocation> bairVarLocation_;

    //the constraints for CHI ordered
    std::vector<std::unique_ptr<Algebra::PolynomialInterface>> constraintsChi_;
    std::vector<std::unique_ptr<Algebra::PolynomialInterface>> constraintsPi_;

    //dimensions and sizes
    size_t heightSpaceDimension_;
    size_t widthSpaceDimension_;
    size_t amountOfPermutationLayers_;
    size_t vanishingLayersSpaceDimension_;
    size_t vanishingSpaceDimension_;
    size_t numOfWitnessLayers_;
    std::vector<Algebra::PolynomialDegree> witnessDegreeBound_;

    //private methods
    static std::vector<size_t> getRoutedIndexes(const BairInstance& instance);
    static std::pair<std::vector<size_t>,std::vector<std::unique_ptr<Algebra::PolynomialInterface>>> getUnroutedIndexes_and_ConstraintsChi(const BairInstance& instance, const std::vector<Algebra::FieldElement>& coeffsChi);
    static Algebra::PolynomialDegree getMaxTestDegree(const BairInstance& instance, const bool hasRoutingNetwork, const Algebra::PolynomialDegree& ascpWitnessDegreeBound);
    static std::pair<Algebra::UnivariatePolynomialGeneral,std::vector<std::pair<Algebra::PolynomialInterface*,size_t> > > getCommonUnivariateConstraint(const BairInstance& instance);
};

} //namespace BairToAcsp
} // namespace libstark

#endif //_COMMON_BairToACCSP_COMMONDEFINITIONS_HPP_
