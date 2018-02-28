/*
 * Basis.h
 *
 *  Created on: Jul 2, 2014
 *      Author: matan
 */

#ifndef BASIS_H_
#define BASIS_H_
#include "Definitions.h"
#include "Element.h"
namespace FFF {

class Basis {
	Element* b;
	Element shift;
	len_t size;
public:
	Basis();
	Basis(Basis& b);
	void operator=(const Basis& b);
	Basis(Element* e, len_t l,Element& s);
	void setBasis(Element* e, len_t l, Element& s);
	static void getElement(Basis& b, Element& e,idx_t idx);
	unsigned int getSize() const ;

	/*
	 * Copies into D the elements of the basis.
	 * D has to be preallocated.
	 */
	void getBasis(Element* D)const;
	Element* getBasis() const;
	void getShift(Element& e) const;
	const Element& getShift() const;
	void printBasis() const;
	~Basis();
};

} /* namespace FFF */
#endif /* BASIS_H_ */
