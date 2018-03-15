/**
 *       @file  AcspWitnessChecker_UTEST.cpp
 *      @brief  Unit test for AcspWitnessChecker
 *
 *     @author  Michael Riabzev, RiabzevMichael@gmail.com
 * =====================================================================================
 */
#include "lightCircLib/lightCircPoly.hpp"
#include <languages/Acsp/AcspWitnessChecker.hpp>
#include <common/Algebra/LinearSpace.hpp>
#include <common/Infrastructure/Infrastructure.hpp>
#include <algebraLib/UnivariatePolynomialGeneral.hpp>
#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <vector>
#include <set>

namespace{

using libstark::AcspInstance;
using libstark::AcspWitness;
using libstark::AcspWitnessChecker;
using libstark::lightCircPoly;
using libstark::Sequence;
using Algebra::FieldElement;
using Algebra::zero;
using Algebra::one;
using Algebra::generateRandom;
using Algebra::elementsSet_t;
using Algebra::PolynomialDegree;
using Algebra::UnivariatePolynomialInterface;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::DivisorPolynomial;
using Algebra::FiniteSetInterface;
using Algebra::FieldElementPredicate;
using Algebra::LinearSpace;
using Algebra::getStandartOrderedBasis;
using Infrastructure::Log2;

using std::pair;
using std::vector;
using std::set;
using std::unique_ptr;
using std::move;

#ifdef __GNUC__
// TODO: work around until gcc adds make_unique, remove on upgrade
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#else	// #ifdef __GNUC__
using std::make_unique;
#endif	// #ifdef __GNUC__

/**
 * @brief   Generates a random univariate polynomial
 * @param   maxDegree maximal degree for polynomial
 * @return  the polynomial
 */
unique_ptr< UnivariatePolynomialGeneral> generateRandomPoly(unsigned int maxDegree) {
	unique_ptr< UnivariatePolynomialGeneral> poly(new UnivariatePolynomialGeneral);	//polynomial for test
	for(unsigned int i=0; i<=maxDegree; i++){
		FieldElement coeff = generateRandom();
		poly->setCoefficient(i,coeff);
	}
	return move(poly);
}

/**
 * @class randomSequence
 * @brief A mapping of \f$\mathbb{N}\f$ into the field
 * such that the first \f$len\f$ integers are mapped into random field elements
 * and the rest are mapped to zeros
 */
class randomSequence : public Sequence<FieldElement> {
public:
	/**
	 * @brief   The constructor
	 * @param   n boundary of indexes that can be mapped to non-zero elements (namely \f$len\f$)
	 */
	randomSequence(Sequence<FieldElement>::index_t n):order(n){
		for (unsigned int i=0; i<order.size(); i++){
			order[i] = generateRandom();
		}
	}

	/**
	 * @brief   The mapping of integers to the field
	 * @param   index some integer
	 * @return  its mapping
	 */
	FieldElement getElementByIndex(Sequence<FieldElement>::index_t index)const {
		if (index < order.size()) return order[index];
		else return zero();
	}

private:

	/**
	 * The mapping is represented using a vector of field elements,
	 * if an integer is in the domain of the vector coordinates
	 * it is mapped to the vectors value in that coordinate,
	 * otherwise it is mapped to zero.
	 */
	vector< FieldElement> order;
};

/**
 * @class specificSet
 * @brief A set of field elements
 * defined by explicit insertion of elements into it
 */
class specificSet : public FiniteSetInterface {
public:
	/**
	 * @brief   Default construction
	 */
	specificSet() {
		vanishingPoly_ =  UnivariatePolynomialGeneral(elements_);
    }

	/**
	 * @brief   Adds an element to the set
	 * @param   element element to add
	 */
	void addElement(const FieldElement& element){
		elements_.insert(element);
		vanishingPoly_ =  UnivariatePolynomialGeneral(elements_);
	}

	/**
	 * @brief   return the set of elements
	 * @return  the elements set
	 */
	const elementsSet_t& getElements()const {
		return elements_;
	}

	/**
	 * @brief   Checks if an elements that satisfies a predicate exist in the set
	 * @param   pred the predicate
	 * @return  True iff such an element exists
	 */
	bool exist(const unique_ptr<const FieldElementPredicate>& pred)const { 
		for(elementsSet_t::iterator z=elements_.begin(); z!=elements_.end(); z++){
			if (pred->test(*z) == true){
				return true;
			}
		}
	return false;
	}

	size_t size()const {return elements_.size();}
	
	virtual const UnivariatePolynomialGeneral& vanishingPoly()const{
        return vanishingPoly_; 
	}
    
    /**
     * Returns whether a field element is a member of the set
     */
    bool contains(const FieldElement& e)const{
        return count(elements_.begin(),elements_.end(),e)>0;
    }

private:
	/**
	 * The set of elements
	 */
	elementsSet_t elements_;
    UnivariatePolynomialGeneral vanishingPoly_;
	
};

namespace Acsp_UTEST {

/**
 * @brief   Generates a random pair of AcspInstance and AcspWitness
 * such that the witness satisfies the instance for vanishing validation check
 * @return  The random pair
 */
static pair<AcspInstance,AcspWitness> vanishing_generate_valid_pair(){
	unsigned int neighbours_amount = 1+ rand() % 10; //neighbours_amount = number of variables of P, cant be 0
	unsigned int vanishing_set_size = rand() % 100;
	unsigned int maxDegree = rand() % 100;

	/**
	 * Construct witness
	 */
	AcspWitness witness(generateRandomPoly(maxDegree));


	/**
	 * Construct instance
	 */

	/**
	 * Construct vanishing set
	 */
	unique_ptr<specificSet> vanishingSet =
		unique_ptr<specificSet>(new specificSet);
	for (unsigned long i=0 ; i<vanishing_set_size; i++){
		FieldElement root = generateRandom();
		vanishingSet->addElement(root);
	}

	/**
	 * Construct neighbours
	 */
	AcspInstance::polynomialsVec neighborPolys;
	for (unsigned int i=0; i<neighbours_amount; i++){
		neighborPolys.push_back(generateRandomPoly(maxDegree));
	}

	/**
	 * Construct constraint polynomial in such a way
	 * that the Witness will satisfy the instance.
	 * We construct the constraints polynomial as
	 * a 'distinct product' of univariate polynomials:
	 * \f$P(x_0,x_1,x_2,\dots,x_n) = 1\cdot q_1(x_1)\cdot q_2(x_2)\cdots q_n(x_n)\f$
	 * so in this case:
	 * \f$(P\circ (x \vert A\circ \vec{N}))(z) = \displaystyle{\prod_{i=0}^{n} (q_i\circ A\circ N_i)(z)}\f$
	 * so we build \f$P\f$ in such a way that for each \f$z \in H\f$ exist an index
	 * \f$i\f$ such that \f$ ( x - A(N_i(z))) \vert q_i(x) \f$.
	 *
	 * This is done by dividing (randomly) the elements of the vanishing set
	 * to \f$n\f$ subset, where \f$n\f$ is the number of variables of \f$P\f$,
	 * name them \f$S_1,S_2,\dots,S_n\f$.
	 * We define \f$q_i (x) = \displaystyle{\prod_{z\in S_i } (x - A(N_i(z)))}\f$
	 */
	
	
	/** Generate roots sets vector */
	elementsSet_t cloningSource;
	vector<elementsSet_t> rootsSets(neighbours_amount,cloningSource);
	const elementsSet_t& vanishingElements = vanishingSet->getElements();
	for (elementsSet_t::iterator z = vanishingElements.begin(); z!=vanishingElements.end(); z++){
		unsigned int index = rand() % neighbours_amount;
		FieldElement neighb_eval = neighborPolys[index]->eval(*z);
		FieldElement root = witness.assignmentPoly().eval(neighb_eval);
		rootsSets[index].insert(root);
	}

	/** construct P */
	unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(UnivariatePolynomialGeneral(one())));
	for (vector<elementsSet_t>::iterator roots=rootsSets.begin();roots!=rootsSets.end();roots++ ) {
		UnivariatePolynomialGeneral q(*roots);
		lightCircPoly qq(q);
		ConstraintsPolynomial->multiplyDistinct(qq);
	}
	
	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(), AcspInstance::boundaryConstraints_t());


	/** Return Acsp pair */
	return move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
}

/**
 * @brief   Generates a random pair of AcspInstance and AcspWitness
 * such that the witness does not satisfy the instance for vanishing validation check
 * @return  The random pair
 */
static pair<AcspInstance,AcspWitness> vanishing_generate_invalid_pair(){
	unsigned int neighbours_amount = 1+ rand() % 10; //neighbours_amount = number of variables of P, cant be 0
	unsigned int vanishing_set_size = 1+ rand() % 100; //if there are no elements that have too be, no restriction checked
	unsigned int maxDegree = rand() % 65;


	/**
	 * Construct witness
	 */
	AcspWitness witness(generateRandomPoly(maxDegree));


	/**
	 * Construct instance
	 */

	/**
	 * Construct vanishing set
	 */
	unique_ptr<specificSet> vanishingSet =
		unique_ptr<specificSet>(new specificSet);
	for (unsigned long i=0 ; i<vanishing_set_size; i++){
		FieldElement root = generateRandom();
		vanishingSet->addElement(root);
	}

	/**
	 * Construct neighbours
	 */
	AcspInstance::polynomialsVec neighborPolys;
	for (unsigned int i=0; i<neighbours_amount; i++){
		neighborPolys.push_back(generateRandomPoly(maxDegree));
	}

	/**
	 * Similarly to the completeness check,
	 * we generate P as a product of univariate polynomials
	 * defined by their roots.
	 * In this case first of all roots are selected randomly, but a
	 * special root (to be explained now).
	 * We choose some \f$ z\f$ from the vanishing set, and for each
	 * \f$ q_i(x)\f$ we set \f$A(N_i(z))\f$ to be a root.
	 * Now we know that for each index \f$ i\f$, \f$ z\f$ is a root
	 * of \f$ q_i(A(N_i(x)))\f$. This tells us that \f$z\f$ is not
	 * a root of \f$ q_i(A(N_i(x))) + 1\f$ for any index \f$i\f$.
	 * We define \f$ P(x) = \displaystyle{\prod_i (q_i(x) + 1)}\f$,
	 * by this we make sure \f$ z\f$ is not a root of \f$ P\circ A\circ\vec{N}\f$,
	 * hence the witness does not satisfy the instance.
	 */


	/** Generate roots sets vector */
	elementsSet_t cloningSource;
	vector< elementsSet_t > rootsSets(neighbours_amount,cloningSource);

	for (vector<elementsSet_t>::iterator roots=rootsSets.begin();roots!=rootsSets.end();roots++ ) {
		for (unsigned int i=0; i<maxDegree; i++){
			FieldElement root = generateRandom();
			roots->insert(root);
		}
	}

	/** choosing the specific element \f$ z\f$ and the needed root to each polynomial */
	FieldElement z = *(vanishingSet->getElements().begin());//There must be at least one element
	for(unsigned int i=0; i< neighbours_amount; i++){
		FieldElement neighb_eval = neighborPolys[i]->eval(z);
		FieldElement root = witness.assignmentPoly().eval(neighb_eval);
		rootsSets[i].insert(root);
	}

	/** construct P */
	unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(UnivariatePolynomialGeneral(one())));
	UnivariatePolynomialGeneral one_P( one());
	for (vector<elementsSet_t>::iterator roots=rootsSets.begin();roots!=rootsSets.end();roots++ ) {
		UnivariatePolynomialGeneral q(*roots);
		q.add(one_P);
		lightCircPoly qq(q);
		ConstraintsPolynomial->multiplyDistinct(qq);
	}

	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(), AcspInstance::boundaryConstraints_t());


	/** Return Acsp pair */
	return move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
}

/**
 * @brief   Generates a random pair of AcspInstance and AcspWitness
 * such that the witness satisfies the instance for witness degree validation check,
 * meaning  deg A <= AcspInstance.witnessDegreeBound
 * @return  The random pair
 */
static pair<AcspInstance,AcspWitness> witness_degree_generate_valid_pair(){
	unsigned int witnessDegree = rand() % 100;
	unsigned int vanishingSetSize = witnessDegree +1 + rand()%100;

	/**
	 * Construct witness
	 */
	AcspWitness witness(generateRandomPoly(witnessDegree));

	/**
	 * Construct instance
	 */
	
	/**
	 * Construct neighbours (we don't care about it, empty)
	 */
	AcspInstance::polynomialsVec neighborPolys;
	
	/**
	 * Construct constraints polynomial (we don't care about it, this is the univariate identity polynomial)
	 */
	unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(UnivariatePolynomialGeneral(one())));
	/**
	 * Construct vanishing set (empty, because we don't care about it)
	 */
	unique_ptr<specificSet> vanishingSet =
		unique_ptr<specificSet>(new specificSet);
	while (vanishingSet->size() < vanishingSetSize){
		FieldElement root = generateRandom();
		vanishingSet->addElement(root);
	}
	
	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(),AcspInstance::boundaryConstraints_t());

	/** Return Acsp pair */
	return move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
}

/**
 * @brief   Generates a random pair of AcspInstance and AcspWitness
 * such that the witness does not satisfy the instance for
 * witness degree validation check,
 * meaning  deg A > AcspInstance.witnessDegreeBound
 * @return  The random pair
 */
static pair<AcspInstance,AcspWitness> witness_degree_generate_invalid_pair(){
	unsigned int witnessDegree = 1 + rand() % 10;
	unsigned int vanishingSetSize =  rand()%witnessDegree;

	/**
	 * Construct witness
	 */
	AcspWitness witness(generateRandomPoly(witnessDegree));

	/**
	 * Construct instance
	 */
	
	/**
	 * Construct neighbours (we don't care about it, empty)
	 */
	AcspInstance::polynomialsVec neighborPolys;
	
	/**
	 * Construct constraints polynomial (we don't care about it, this is the univariate identity polynomial)
	 */
	unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(UnivariatePolynomialGeneral(one())));

	/**
	 * Construct vanishing set 
	 */
	unique_ptr<specificSet> vanishingSet =
		unique_ptr<specificSet>(new specificSet);
	while (vanishingSet->size() < vanishingSetSize){
		FieldElement root = generateRandom();
		vanishingSet->addElement(root);
	}
	
	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),PolynomialDegree(PolynomialDegree::integral_t(witness.assignmentPoly().getDegree())-1), AcspInstance::boundaryConstraints_t());

	/** Return Acsp pair */
	return move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
}

/**
 * @brief   Generates a random pair of AcspInstance and AcspWitness
 * such that the witness satisfies the instance for boundary constraints validation check
 * @return  The random pair
 */
static pair<AcspInstance,AcspWitness> boundary_generate_valid_pair(){
	int inputLen = rand() % 100;

	/**
	 * Construct witness
	 */
	AcspWitness witness(generateRandomPoly(0));

	/**
	 * Construct instance
	 */
	
	/**
	 * Construct neighbours (we don't care about it, empty)
	 */
	AcspInstance::polynomialsVec neighborPolys;
	
	/**
	 * Construct constraints polynomial (we don't care about it, this is the univariate identity polynomial)
	 */
	unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(UnivariatePolynomialGeneral(one())));

	/**
	 * Construct vanishing set (empty, because we don't care about it)
	 */
	unique_ptr<specificSet> vanishingSet =
		unique_ptr<specificSet>(new specificSet);
	
	/** Construct boundary constraints  parameters */	
	AcspInstance::boundaryConstraints_t boundaryConstraints;

	for(int i=0; i<inputLen; i++){
		const FieldElement x = generateRandom();
        boundaryConstraints[x] = witness.assignmentPoly().eval(x);
	}
	
	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(),boundaryConstraints);

	/** Return Acsp pair */
	return move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
}

/**
 * @brief   Generates a pair (AcspInstance,AcspWitness)
 * that does not satisfy the boundary constraints
 * @return  The pair
 */
static pair<AcspInstance,AcspWitness> boundary_generate_invalid_pair(){
	unsigned int expectedInputLen = 1 + rand() % 100;

	/**
	 * Construct witness
	 */
	AcspWitness witness(generateRandomPoly(0));

	/**
	 * Construct instance
	 */
	
	/**
	 * Construct neighbours (we don't care about it, empty)
	 */
	AcspInstance::polynomialsVec neighborPolys;
	
	/**
	 * Construct constraints polynomial (we don't care about it, this is the univariate identity polynomial)
	 */
	unique_ptr<lightCircPoly> ConstraintsPolynomial (new lightCircPoly(UnivariatePolynomialGeneral(one())));

	/**
	 * Construct vanishing set (empty, because we don't care about it)
	 */
	unique_ptr<specificSet> vanishingSet =
		unique_ptr<specificSet>(new specificSet);
	
	/** Construct boundary constraints parameters */	
	AcspInstance::boundaryConstraints_t boundaryConstraints;

	for(unsigned int i=0; i<expectedInputLen; i++){
        const FieldElement x = generateRandom();
		const FieldElement y  = witness.assignmentPoly().eval(x);
	    boundaryConstraints[x] = y + one();
    }
	
	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(),boundaryConstraints);

	/** Return Acsp pair */
	pair<AcspInstance,AcspWitness> AcspPair = move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
	return move(AcspPair);
}

/**
 * @brief   Generates a valid pair (AcspInstance,AcspWitness)
 * such a pair can be proven and verified using PCP
 * First the field and the vanishing set (which is an affine subspace) is defined
 * We define the polynomial vanishing on the set to be \f$Z(x)\f$
 * the witness is defined to be the polynomial \f$ \frac{Z(x)}{x}\f$, the constraint poly is \f$P(x,y) = x \cdot y \f$
 * and \f$ \vec{N} = (x) \f$
 * no input is given
 * @return  The pair
 */
static pair<AcspInstance,AcspWitness> generate_valid_pair_most_in_witness(){

	const size_t vanishingSpaceDegree = 3;

	/**
	 * Construct vanishing set - affine
	 */
	unique_ptr<LinearSpace> vanishingSet(new LinearSpace(Algebra::getStandartOrderedBasis(vanishingSpaceDegree)));

	/**
	 * Construct neighbours
	 */
	AcspInstance::polynomialsVec neighborPolys;
	UnivariatePolynomialGeneral* idPoly = new UnivariatePolynomialGeneral; 
	idPoly->setCoefficient(1, one());
	neighborPolys.push_back(unique_ptr<UnivariatePolynomialInterface>(idPoly));
	
	/**
	 * Construct constraints polynomial
	 */
	unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(*idPoly));
	ConstraintsPolynomial->multiplyDistinct(lightCircPoly(*idPoly));
	
	/**
	 * Construct witness
	 */
	const UnivariatePolynomialInterface& vanishingPoly = vanishingSet->vanishingPoly();
	unique_ptr<UnivariatePolynomialInterface> A(idPoly->divideByMe(vanishingPoly));
	AcspWitness witness(move(A));

	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(), AcspInstance::boundaryConstraints_t());

	/** Return Acsp pair */
	delete idPoly;
	return move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
}

/**
 * @brief   Generates a valid pair (AcspInstance,AcspWitness)
 * such a pair can be proven and verified using PCP
 * First the field and the vanishing set (which is an affine subspace) is defined
 * We define the polynomial vanishing on the set to be \f$Z(x)\f$
 * the witness is defined to be the identity polynomial \f$x\f$, \f$P(x) = Z(x) \f$
 * and \f$ \vec{N} = () \f$
 * no input is given
 * @return  The pair
 */
static pair<AcspInstance,AcspWitness> generate_valid_pair_all_in_constraintsPoly(){
	const size_t vanishingSpaceDegree = 3;

	/**
	 * Construct vanishing set - affine
	 */
    const vector<FieldElement> vanishingSpaceBasis = getStandartOrderedBasis(vanishingSpaceDegree);
	unique_ptr<LinearSpace> vanishingSet(new LinearSpace(vanishingSpaceBasis));

	/**
	 * Construct neighbours
	 */
	AcspInstance::polynomialsVec neighborPolys;
	
	/**
	 * Construct constraints polynomial
	 */
	unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(vanishingSet->vanishingPoly()));
	
	/**
	 * Construct witness
	 */
	unique_ptr<UnivariatePolynomialGeneral> idPoly(new UnivariatePolynomialGeneral); 
	idPoly->setCoefficient(1, one());
	AcspWitness witness(move(idPoly));

	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(), AcspInstance::boundaryConstraints_t());

	/** Return Acsp pair */
	return move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
}

/**
 * @brief   Generates a valid pair (AcspInstance,AcspWitness)
 * such a pair can be proven and verified using PCP
 * First the field and the vanishing set (which is an affine subspace) is defined
 * We define the polynomial vanishing on the set to be \f$Z(x)\f$
 * the witness is defined to be the identity polynomial \f$x\f$, \f$P(x,y) = y \f$
 * and \f$ \vec{N} = (Z(x)) \f$
 * no input is given
 * @return  The pair
 */
static pair<AcspInstance,AcspWitness> generate_valid_pair_all_in_neighbor(){

	const size_t vanishingSpaceDegree = 3;

	/**
	 * Construct vanishing set - affine
	 */
    const vector<FieldElement> vanishingSpaceBasis = getStandartOrderedBasis(vanishingSpaceDegree);
	unique_ptr<LinearSpace> vanishingSet(new LinearSpace(vanishingSpaceBasis));

	/**
	 * Construct neighbours
	 */
	AcspInstance::polynomialsVec neighborPolys;
	neighborPolys.push_back(unique_ptr<UnivariatePolynomialInterface>(new UnivariatePolynomialGeneral(vanishingSet->vanishingPoly())));
	
	/**
	 * Construct constraints polynomial
	 */
	UnivariatePolynomialGeneral* idPoly = new UnivariatePolynomialGeneral; 
	idPoly->setCoefficient(1, one());
    unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(UnivariatePolynomialGeneral(zero())));
	ConstraintsPolynomial->addDistinct(lightCircPoly(*idPoly));
	
	/**
	 * Construct witness
	 */
	AcspWitness witness(move(unique_ptr<UnivariatePolynomialGeneral>(idPoly)));

	/** Construct the instance data */	
	AcspInstance instance(move(vanishingSet),move(neighborPolys),move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(), AcspInstance::boundaryConstraints_t());

	/** Return Acsp pair */
	return move(pair<AcspInstance,AcspWitness>(move(instance),move(witness)));
}

//added by Ariel
//In this test the constraint polynomial is x+y. There is one neighbour that's the identity, and the constraint poly is Z_H, i.e.,
//the vanishing space subspace poly. The witness poly is the identity x
static pair<AcspInstance, AcspWitness> generate_valid_pair_all_in_constraintsPoly2(){
	const size_t vanishingSpaceDegree = 3;

	/**
	* Construct vanishing set - affine
	*/
    const vector<FieldElement> vanishingSpaceBasis = getStandartOrderedBasis(vanishingSpaceDegree);
	unique_ptr<LinearSpace> vanishingSet(new LinearSpace(vanishingSpaceBasis));

	/**
	* Construct neighbours
	*/
	unique_ptr<UnivariatePolynomialGeneral> idPoly(new UnivariatePolynomialGeneral);
	idPoly->setCoefficient(1, one());
	AcspInstance::polynomialsVec neighborPolys;
	neighborPolys.push_back(move(idPoly));

	/**
	* Construct constraints polynomial Z_H + Z_H (the add distinct method creates a new variable)
	*/
	idPoly = unique_ptr<UnivariatePolynomialGeneral>(new UnivariatePolynomialGeneral());
	idPoly->setCoefficient(1, one());

	unique_ptr<lightCircPoly> ConstraintsPolynomial(new lightCircPoly(vanishingSet->vanishingPoly()));
	ConstraintsPolynomial->addDistinct(lightCircPoly(vanishingSet->vanishingPoly()));

	/**
	* Construct witness
	*/
	idPoly = unique_ptr<UnivariatePolynomialGeneral>(new UnivariatePolynomialGeneral());
	idPoly->setCoefficient(1, one());
	AcspWitness witness(move(idPoly));

	/** Construct the instance data */
	AcspInstance instance(move(vanishingSet), move(neighborPolys), move(ConstraintsPolynomial),witness.assignmentPoly().getDegree(), AcspInstance::boundaryConstraints_t());

	/** Return Acsp pair */
	return move(pair<AcspInstance, AcspWitness>(move(instance), move(witness)));
}

} //namespace Acsp_UTEST

/**
 * @brief   GTEST function to test completeness of AcspWitnessChecker::verify for a full witness
 * such that the witness is the identity polynomial, the constraint polynomial is P(x,y)=y
 * and the single neighbor is the vanishing polynomial
 */
TEST(AcspWitnessChecker,verify_all_in_neighbor){
	pair<AcspInstance,AcspWitness> validPair = Acsp_UTEST::generate_valid_pair_all_in_neighbor();
    EXPECT_TRUE(AcspWitnessChecker::verify(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test completeness of AcspWitnessChecker::verify for a full witness
 * such that the witness is the identity polynomial, the constraint polynomial is the vanishing univariate polynomial,
 * and there are no neighbors
 */
TEST(AcspWitnessChecker,verify_all_in_constraintsPoly){
	pair<AcspInstance,AcspWitness> validPair = Acsp_UTEST::generate_valid_pair_all_in_constraintsPoly();
	EXPECT_TRUE(AcspWitnessChecker::verify(validPair.first,validPair.second));
}
/** added by ARIEL
* @brief   GTEST function to test completeness of AcspWitnessChecker::verify for a full witness
* such that the witness is the identity polynomial, the constraint polynomial is Z_H(x)+Z_H(y) where H is the vanishing space,
* there is one neighbor which is the identity
*/
TEST(AcspWitnessChecker, verify_all_in_constraintsPoly2){
	pair<AcspInstance, AcspWitness> validPair = Acsp_UTEST::generate_valid_pair_all_in_constraintsPoly2();
	EXPECT_TRUE(AcspWitnessChecker::verify(validPair.first, validPair.second));
}

/**
 * @brief   GTEST function to test completeness of AcspWitnessChecker::verify for a full witness
 * such that the witness is the vanishing polynomial and the instance is trivial
 */
TEST(AcspWitnessChecker,DISABLED_verify_most_in_witness){
	pair<AcspInstance,AcspWitness> validPair = Acsp_UTEST::generate_valid_pair_most_in_witness();
	EXPECT_TRUE(AcspWitnessChecker::verify(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test completeness of AcspWitnessChecker::verify_vanishing
 */
TEST(AcspWitnessChecker,verify_vanishing_completness){
	pair<AcspInstance,AcspWitness> validPair = Acsp_UTEST::vanishing_generate_valid_pair();
	EXPECT_TRUE(AcspWitnessChecker::verify_vanishing(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test soundness of AcspWitnessChecker::verify_vanishing
 */
TEST(AcspWitnessChecker,verify_vanishing_soundness){
	pair<AcspInstance,AcspWitness> validPair = Acsp_UTEST::vanishing_generate_invalid_pair();
	EXPECT_FALSE(AcspWitnessChecker::verify_vanishing(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test completeness of AcspWitnessChecker::verify_witness_degree
 */
TEST(AcspWitnessChecker,verify_witness_degree_completness){
	pair<AcspInstance,AcspWitness> validPair = Acsp_UTEST::witness_degree_generate_valid_pair();
	EXPECT_TRUE(AcspWitnessChecker::verify_witness_degree(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test soundness of AcspWitnessChecker::verify_witness_degree
 */
TEST(AcspWitnessChecker,verify_witness_degree_soundness){
	pair<AcspInstance,AcspWitness> validPair = Acsp_UTEST::witness_degree_generate_invalid_pair();
	EXPECT_FALSE(AcspWitnessChecker::verify_witness_degree(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test completeness of AcspWitnessChecker::verify_boundary
 */
TEST(AcspWitnessChecker,verify_boundary_complitness){
	pair<AcspInstance,AcspWitness> validPair = Acsp_UTEST::boundary_generate_valid_pair();
	EXPECT_TRUE(AcspWitnessChecker::verify_boundary(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test soundness of AcspWitnessChecker::verify_boundary
 */
TEST(AcspWitnessChecker,verify_boundary_soundness){
	pair<AcspInstance,AcspWitness> AcspPair = Acsp_UTEST::boundary_generate_invalid_pair();
	EXPECT_FALSE(AcspWitnessChecker::verify_boundary(AcspPair.first, AcspPair.second));
}

}
