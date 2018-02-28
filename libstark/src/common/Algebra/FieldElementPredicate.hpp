#include <algebraLib/FieldElement.hpp>

#ifndef  __FieldElementPredicate_HPP
#define  __FieldElementPredicate_HPP

namespace Algebra {
class FieldElementPredicate {
public:
	virtual bool test(const FieldElement&)const =0;
    virtual ~FieldElementPredicate(){};
};
} // namespace Algebra 

#endif   // __FieldElementPredicate_HPP
