/*
 * Element.h
 *
 *  Created on: 2 ���� 2014
 *      Author: hamilis
 */

/*
 * This is the header file of the elements as they are represented on CPU.
 * Each element is just an array of cell_t, the name of the attribute it c, so e.c will return that array that represents element c.
 * As for now, cell_t is long unsigned int, therefore it is 8 bytes so elements in GF(2^64) will be exactly in one.
 * Given Element e, denote by e_i the i-th bit (starting from LSB, starting from entry 0 in the array) in e.c.
 * So the polynomial that e represents in the finite field of GF(2^64) is:
 * 				Sum(i=0 to 63) { e_i * x^i }.
 */
#ifndef ELEMENT_H_
#define ELEMENT_H_
#include "Definitions.h"
namespace FFF{

// When changing be sure to update "log_bits_in_cell".
typedef long long unsigned int cell_t;

class Element {
	//Attributes
public:
	static const unsigned int log_bits_in_byte = 3;
	static const unsigned int bits_in_byte = 1<<log_bits_in_byte;
	static const unsigned int log_ord = 6;
	static const unsigned int ord = 1<<log_ord;
	static const unsigned int log_bits_in_cell =6;
	static const unsigned int bits_in_cell = 1<<log_bits_in_cell;
	static const unsigned int element_len = (ord>>log_bits_in_byte)/sizeof(cell_t);
/*
 * http://www.hpl.hp.com/techreports/98/HPL-98-135.pdf - List of irreducible binary
 * polynomials.
 * All of these polynomials are either trinomials or pentanomials.
 * Moreover they are of the form x^d+x^j+...+1 where:
 *
 * 		j*(j-1)*(j-2)/6 <= d
 */
static const idx_t irr_poly_index[][max_nonzero_coefs_in_mod];
static const cell_t irr_poly_e[][element_len];

/*
 * mod_len[i] is the number of non-zero coefficients in the i-th irreducible polynomial
 */
static const len_t mod_len[];

private:
/*
 * Input:
 * 1) a - pointer to array of cell_t represents an element.
 * 2) i - Will contain the degree of the polynomial represented by a.
 * 3) len - length of array of a.
 */
	static void deg(cell_t * a, int & i, len_t len);
	/*
	 * Left shifts array pointed by v of length len by k bits to the left (from LSB to MSB).
	 * k - can be larger than the number of bits in a single cell.
	 */
	static void left_shift(cell_t * v, unsigned int k , unsigned int len);
	/*
	 * Given to binary polynomials a,b writes in g the egcd polynomial of a and b, using the extended euclidian algorithm.
	 */
	static void egcd(cell_t a[element_len+1], cell_t b[element_len+1], cell_t g[element_len+1]);
public:

	cell_t c[element_len];

	/*
	 * The c'tor does nothing.
	 */
	Element(){};
	/*
	 * Copy c'tor.
	 */
	Element(const Element& e);
	/*
	 * Assignment operator.
	 */
	void operator=(const Element& e);
	/*
	 * D'tor, there is nothing to deallocate.
	 */
	~Element(){};
	/*
	 * Denote by r(x) the irreducible polynomial that defines the field GF(2^64) therefore r(x) = x^64 + s(x).
	 * This function assigns s(x) to e.
	 */
	static void generateOrdElement(Element* e);
	/*
	 *	This function performs a carryless multiplication of a and b, and XORs thee result to res.
	 */
	static void clmulXor(const cell_t* a,const  cell_t* b, cell_t* res);
	static void reduce(cell_t* clmul_res, cell_t* res);
	static void clmul(const cell_t* a,const  cell_t* b, cell_t* res);

/***** Constant Memory Settings *****/
	/*
	 * This function is used for the GPU implementation and sets the relevant irreducible polynomials at the constant memory.
	 */
	static void setMod();
/***** Chunk/Vector Operations*****/
	/*
	 * return true iff elements a and b are equal.
	 */
	static bool equals(Element* a, Element* b);
	static bool equals(Element& a, Element& b);

/***** CPU Operations *****/
	/*
	 * a[i] = a[i]*e.
	 */
	static void setElementMul(Element* e);

	/*
	 * writes in c the addition of elements a and b.
	 * c = a + b.
	 */
	static void c_add(const Element* const a,const  Element* const b, Element* const c);
	static void c_add(const Element& a, const Element& b, Element& c);
	/*
	 * writes in c the multiplication of elements a and b.
	 * c = a * b.
	 */
	static void c_mul(const Element* a,const Element* b,Element* c);
	static void c_mul(const Element& a,const Element& b, Element& c);
	/*
	 * XORs to c the multiplication of a and b.
	 * c ^= (a*b)
	 */
	static void c_mulXor(const Element& a,const Element& b,Element& c);

    //computes:
    //c[i] = b[i] + factor * a[i]
    //b[i] = c[i]
    //a[i] = a[i] + c[i]
    //THIS CODE IS IRREDUCIBLE SPECIFIC!!!
    static void do_FFT_step(const Element& factor,Element* a, Element* b, const int len);

	/*
	 * squares elements a in c.
	 * c = (a^2).
	 */
	static void c_sqr(Element* a, Element* c);
	static void c_sqr(Element& a, Element& c);
	/*
	 * Exponentiates element a by exp and writes the result into res.
	 * res = (a^exp).
	 */
	static void c_exp(Element a, unsigned long long exp, Element& res);
	/*
	 * Inverts a and writes the result in c.
	 * c = a ^ (-1).
	 */
	static void c_inv(Element* a, Element* c);
	static void c_inv(Element& a, Element& res);
	/*
	 * a = 0.
	 */
	static void c_setZero(Element* a);
	static void c_setZero(Element& a);
	/*
	 * Returns true iff a is zero element.
	 */
	static bool c_isZero(Element& a);
	/*
	 * a = 1.
	 */
	static void c_setUnit(Element* a);
	static void c_setUnit(Element& a);
	/*
	 * Returns true iff a is the unit element of the field.
	 */
	static bool c_isUnit(Element* a);
	static bool c_isUnit(Element& a);
	/*
	 * a,b,c - arrays of elements of length l.
	 * c [i] = a[i] * b[i].
	 */
	static void c_vecMul(Element* a, Element* b, Element* c, len_t l);
	/*
	 * a, b - arrays of length l.
	 * a[i] ^= b[i].
	 */
	static void vecXor(cell_t * a, cell_t * b, len_t len);
	/*
	 * a,b,c - arrays of length l.
	 * c[i] = a[i] ^ b[i].
	 */
	static void vecXor(cell_t * a, cell_t * b, cell_t* c, len_t len);
	/*
	 * a,b - arrays of length len.
	 * a[i] = b[i].
	 */
 	static void assign_cells(cell_t* a, cell_t* b,len_t len);
 	/*
 	 * Element pointed by a is set to b.
 	 */
	static void assign(Element (*a),const Element (*b));
	static void assign(Element& a,const Element &b);
	/**** Testing and Representation ****/
	/*
	 * prints cells pointed by x of length l.
	 */
	static void printCells(const cell_t * x, len_t l);
	/*
	 * Prints the element x.
	 */
	static void printElement(const Element& x);
	/*
	 * This is the implementation of the naive multiplication method over finite fields, just for comparison.
	 */
	static void naiveMul(const cell_t* a, const cell_t* b, cell_t* c);
	static void naiveMul(const Element& a, const Element& b, Element& c);


	Element operator+(const Element& e) const
	{
		Element res;
		Element::c_add(Element(*this),e,res);
		return res;
	}

	Element operator*(const Element& e) const
	{
		Element res;
		Element::c_mul(Element(*this),e,res);
		return res;
	}
};
};

#endif /* ELEMENT_H_ */
