#include "FieldElement.hpp"
#include <vector>
#include <memory>

#ifndef MAPPINGS_SYS_HPP__
#define MAPPINGS_SYS_HPP__
namespace Algebra{

class multivarFunc{
public:
    virtual ~multivarFunc(){};
    virtual size_t numVars() const = 0;   
    virtual Algebra::FieldElement eval(const std::vector<FieldElement>& assignment) const = 0;
};

class MappingsSys{
public:
    virtual ~MappingsSys(){};
    virtual MappingsSys* clone() const = 0;

    virtual size_t numVars() const = 0;   
    virtual size_t numMappings() const = 0;

    // assignment length expected is numVars
    virtual std::vector<FieldElement> eval(const std::vector<FieldElement>& assignment) const = 0;
    
    // num coefficients expected should be numMappings
    virtual multivarFunc* getLinearComb(const std::vector<FieldElement>& coeffs)const;
}; // class MappingsSys

} //namespace Algebra
#endif //MAPPINGS_SYS_HPP__
