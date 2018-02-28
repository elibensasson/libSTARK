#include "FieldElementPredicate.hpp"
#include <algebraLib/PolynomialInterface.hpp>

#include <memory>

#ifndef  __SET_INTERFACE_H
#define  __SET_INTERFACE_H

namespace Algebra{

class FiniteSetInterface {
public:
	/**
	 * @brief   Returns true iff there exists an element in the set that satisfies some predicate
	 * @param   pred the predicate
	 * @return  True iff an element that satisfies the predicate exists in the set
	 */
	virtual bool exist(const std::unique_ptr<const FieldElementPredicate>& pred)const = 0;

	/**
 	* Returns the size of the set
 	*/ 
	virtual size_t size()const = 0;

	/**
 	* Returns the minimal polynomial that vanishes over the set
 	*/
	virtual const UnivariatePolynomialInterface& vanishingPoly()const = 0;

    /**
     * Returns whether a field element is a member of the set
     */
    virtual bool contains(const FieldElement& e)const = 0;

	virtual ~FiniteSetInterface(){};
};

} //namespace Algebra

#endif   // __SET_INTERFACE_H
