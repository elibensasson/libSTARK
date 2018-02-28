/*
 * Basis.cpp
 *
 *  Created on: Jul 2, 2014
 *      Author: matan
 */
#include <iostream>
#include <cstring>
#include "Basis.h"
#include <cstdlib>
namespace FFF {

void Basis::getElement(Basis& b, Element& e,idx_t idx){
	b.getShift(e);
	for(unsigned int i = 0 ; i < b.size ; ++i){
		if(idx & 1)
			Element::c_add(e,b.b[i],e);
		idx>>=1;
	}
}
Basis::Basis() :
		b((Element*)malloc(sizeof(Element))), shift() , size(1){};
Basis::Basis(Basis& that) :
shift(that.shift), size(that.size)
{
	b = ((Element*)malloc(sizeof(Element)*that.size));
	memcpy(this->b, that.b, sizeof(Element)*that.size);
}
Basis::Basis(Element* e, len_t l, Element& s):
			b((Element*)malloc(sizeof(Element)*l)), shift(s),size(l)
{
	memcpy(b,e,sizeof(Element)*l);
}
//Ariel:buggy method, but not actually used - so commented out
//void Basis::setBasis(Element* e, len_t l, Element& s){
//	this->~Basis();
//	this->b=(Element*)malloc(sizeof(Element)*l);
//	this->shift = s;
//	this-> size = l;
//}
void Basis::operator=(const Basis& b){
		this->~Basis();
		this->b= (Element*)malloc(sizeof(Element)*b.size);
		memcpy(this->b,b.b,sizeof(Element)*b.size);
		size = b.size;
		shift=b.shift;
	}
unsigned int Basis::getSize() const {
	return this->size;
}
const Element& Basis::getShift() const{
	return this->shift;
}
void Basis::getBasis(Element* D) const
{
	memcpy(D,this->b,sizeof(Element)*this->size);
}
void Basis::getShift(Element& e) const
{
	Element::assign(e,this->shift);
}

Element* Basis::getBasis() const{
	return this->b;
}
void Basis::printBasis() const{
	std::cout << "Elements:" << std::endl;
	for(unsigned int i = 0 ; i < this->size ; ++i){
		Element::printElement(this->b[i]);
		std::cout << std::endl;
	}
	std:: cout << "Shift:" << std::endl;
	Element::printElement(this->shift);
	std::cout << std::endl << std::endl;

}
Basis::~Basis() {
	free(b);
}

} /* namespace FFF */
