#include "BairWitnessChecker_UTEST.hpp"
#include "lightCircLib/lightCircPoly.hpp"
#include <languages/Bair/BairWitnessChecker.hpp>

#include <gtest/gtest.h>

namespace PCP_UTESTS{

using libstark::BairWitnessChecker;
using libstark::BairWitness;
using libstark::BairInstance;
using libstark::ConstraintSys;
using libstark::Sequence;
using libstark::lightCircPoly;
using Algebra::FieldElement;
using Algebra::PolynomialInterface;
using Algebra::UnivariatePolynomialInterface;
using Algebra::UnivariatePolynomialGeneral;
using Algebra::PolynomialDegree;
using Algebra::one;
using Algebra::generateRandom;
using Algebra::elementsSet_t;
using Infrastructure::POW2;
using std::pair;
using std::rand;
using std::vector;
using std::unique_ptr;
using std::move;

/****************************************************************
 *                Auxiliary classes templates
 ****************************************************************/
template<class element_t>
class generateElement {
public:
    virtual element_t operator()() const = 0;
};

/**
 * @class specificSequence
 * @brief A mapping of \f$\mathbb{N}\f$ into the <image_t>
 * such that the first \f$len\f$ integers are mapped to some given
 * constants, and the rest are mapped to a given constant as well
 */
template <class image_t>
class expliciteSequence : public Sequence<image_t> {
public:
    /**
     * @brief   The constructor
     * @param   n boundary of indexes that can be mapped to different constant elements (namely \f$len\f$)
     */
    expliciteSequence(size_t n, const generateElement<image_t>& gen,const image_t& rest):order_(n),rest_(rest){
        for (size_t i=0; i<order_.size(); i++){
            order_[i] = gen();
        }
    }

    /**
     * @brief   The mapping of integers to <image_t>
     * @param   index some integer
     * @return  its mapping
     */
    image_t getElementByIndex(typename Sequence<image_t>::index_t index)const {
        if (index < order_.size()) return order_[index];
        else return rest_;
    }

private:

    /**
     * The mapping is represented using a vector of <image_t> elements,
     * if an integer is in the domain of the vector coordinates
     * it is mapped to the vectors value in that coordinate,
     * otherwise it is mapped to the return value of rest_.
     */
    vector< image_t> order_;
    image_t rest_;
};

/****************************************************************
 *                Coloring auxiliary classes
 ****************************************************************/

/********************
 * Random assignment
 *******************/

class randomColorGen : public generateElement<BairWitness::color_t> {
public:
    randomColorGen(size_t len):len_(len){};
    BairWitness::color_t operator()() const{
        BairWitness::color_t color;
        for (unsigned int i=0; i< len_; i++){
            color.push_back(generateRandom());
        }
        return color;
    }
private:
    size_t len_;
};

class randomColoring : public expliciteSequence<BairWitness::color_t> {
public:
    randomColoring(size_t n, size_t vec_len):
        expliciteSequence<BairWitness::color_t>(n,randomColorGen(vec_len), randomColorGen(vec_len)()){};
};

/****************************************************************
 *               Permutations auxiliary classes
 ****************************************************************/

/********************
 * Random permutation
 *******************/

class randOrderGen : public generateElement<size_t> {
    public:
        randOrderGen(size_t numElements): available(numElements){
            for(size_t i=0; i < numElements; i++) available[i] = i;
        }
        size_t operator()() const{
            assert(!available.empty());
            size_t elementIndex = rand() % available.size();
            size_t retElement = available[elementIndex];
            available.erase( available.begin() + elementIndex );
            return retElement;
        }
    private:
        mutable vector<size_t> available;
};

class randPermutation : public expliciteSequence<size_t> {
public:
    randPermutation(size_t numElements):
        expliciteSequence<size_t>(numElements,randOrderGen(numElements),0){};
};

class plusOneSequence : public Sequence<size_t>{
    public:
    
		size_t getElementByIndex(Sequence<size_t>::index_t index)const {
        return index+1;
    }
};

/****************************************************************
 *               Random padding for permutation
 ****************************************************************/
vector<FieldElement> getRandomPadding(const size_t& vectorLen){
    vector<FieldElement> res(vectorLen);
    for(auto& x : res) x = generateRandom();

    return res;
}

/****************************************************************
 *           Constraints System auxiliary classes
 ****************************************************************/

/********************
 * Always satisfied
 *******************/
class allwaysSatisfiedSystem : public ConstraintSys{
public:
    allwaysSatisfiedSystem(size_t numVars):
        numVars_(numVars), 
        firstUsed_(numVars==2){};
    size_t numVars() const {return numVars_;}
    allwaysSatisfiedSystem* clone()const{
        return new allwaysSatisfiedSystem(numVars_);
    }
    const polySet_t& constraints() const {return noPolys_;}
    bool varUsed(const size_t varId) const { 
        //verify the case where first var is
        //not routed, is handled
        if((varId ==0) && !firstUsed_) return false;
        
        //non trivial function
        //this is implemented this way 
        //so the embedding itself will be non trivial
        //This should simulate the expected case
        //where the first half of the variables belongs the
        //previous configuration + auxiliary variables used when 
        //"previous configuration is next configuration"
        //and the second half is the next configuration
        //and the auxiliary variables relevant to current check.
        //so the constraint system would probably say that it uses
        //only previous configuration,next configuration, and
        //auxiliary variables relevant to current check.
        return (varId <= (numVars_*0.75));
    }
private:
    size_t numVars_;
    const polySet_t noPolys_; //empty set
    const bool firstUsed_;
};

class settingSaticfyingSystem : public ConstraintSys {
public:
    settingSaticfyingSystem(
        size_t numVars,
        size_t domainSize,
        const BairWitness& witness,
        const BairWitness::permutation_t& permutation,
        bool makeIncomplete = false)
        : numVars_(numVars) {
            for (unsigned int i=0; i < domainSize; i++){
                size_t perm_img = permutation.getElementByIndex(i);
                BairWitness::color_t c1 = witness.get_color(i);
                BairWitness::color_t c2 = witness.get_color(perm_img);
                vector<FieldElement> assignment(c1);
                for(auto elem : c2){ assignment.push_back(elem);}
                addRootToTrie(assignment);
            }
            if (makeIncomplete){
            //remove one root, so the system
            //won't be satisfied by current setting
                size_t i = rand() % (domainSize-1);
                size_t perm_img = permutation.getElementByIndex(i);
                BairWitness::color_t c1 = witness.get_color(i);
                BairWitness::color_t c2 = witness.get_color(perm_img);
                vector<FieldElement> assignment(c1);
                for(auto elem : c2){ assignment.push_back(elem);}
                removeRootFromTrie(assignment);
            }
           generatePolysFromTrie();
        }
    size_t numVars() const {return numVars_;}
    settingSaticfyingSystem* clone()const{
        return new settingSaticfyingSystem(*this);
    }
    const polySet_t& constraints() const {return polys_;}
    ~settingSaticfyingSystem(){
        for(auto node : rootsTrie_) delete node;
    }
private:
    size_t numVars_;
    polySet_t polys_;

    settingSaticfyingSystem(const settingSaticfyingSystem& ref){
        numVars_ = ref.numVars_;
        for(const auto& p: ref.polys_){
            polys_.push_back(p->clone());
        }
    }

    struct trieNode{
        FieldElement val;
        vector<struct trieNode*> next;
        trieNode(FieldElement newVal):val(newVal){;}
        ~ trieNode(){
            for(struct trieNode* node : next){
                delete node;
            }
        }
    };

    vector<struct trieNode*> rootsTrie_;

    void addRootToTrie_rec(vector<FieldElement>::const_iterator start,vector<FieldElement>::const_iterator end, vector<struct trieNode*>& currLevel){
        if(start == end) return;

        //find if prefix is already in trie
        //if so, just add the needed suffix
        for(struct trieNode* node : currLevel){
            assert(node != NULL);
            if(node->val == *start) return addRootToTrie_rec(start+1,end,node->next);
        }

        //if prefix is not in trie, add it
        struct trieNode* newNode = new struct trieNode(*start);
        currLevel.push_back(newNode);
        return addRootToTrie_rec(start+1,end,newNode->next);

    }

    void addRootToTrie(const vector<FieldElement>& assignment){
        vector<struct trieNode*>& firstList = rootsTrie_;
        addRootToTrie_rec(assignment.begin(), assignment.end(), firstList);
    }

    bool removeRootFromTrie_rec(vector<FieldElement>::const_iterator start,vector<FieldElement>::const_iterator end, vector<struct trieNode*>& currLevel){
        //if finished reading the assignment, then is is found in the trie
        if(start == end) return true;

        //find if prefix is in trie
        //if found, remove it
        for(size_t i=0; i< currLevel.size(); i++){
            assert(currLevel[i] != NULL);
            //if the suffix is found, remove it from the trie
            if((currLevel[i]->val == *start) && removeRootFromTrie_rec(start+1,end,currLevel[i]->next)){
                //if we are currently in the last node,
                //or there is only one way to continue 
                //(it must be the way only to the assignment, otherwise false is returned)
                //for sure it should be removed
                if(currLevel[i]->next.size() <= 1){
                    delete currLevel[i];
                    currLevel.erase(currLevel.begin() + i);
                    return true;
                }

                //we are on the way to more than only the assignment, so return false
                return false;
            }
        }

        //if prefix is not in trie, just return false (not found)
        return false;
    }

    void removeRootFromTrie(const vector<FieldElement>& assignment){
        vector<struct trieNode*>& firstList = rootsTrie_;
        removeRootFromTrie_rec(assignment.begin(), assignment.end(), firstList);
    }

    void generatePolysFromTrie_rec(const lightCircPoly& selector, const vector<struct trieNode*>& currLevel, const size_t currLevelIndex){
        //recursion end
        if(currLevelIndex >= numVars_) return;
        
        //build polynomial
        //that vanishes if and only if the value of
        //the element in the current index of an assignment correlates
        //with some root
        {
        elementsSet_t roots;
        for(const struct trieNode* node : currLevel){
            assert(node != NULL);
            roots.insert(node->val);
        }
        const UnivariatePolynomialGeneral uniPoly(roots);
        const lightCircPoly vanishesOnCurrElem(uniPoly);
        lightCircPoly selectorForCurrNode(selector);
        selectorForCurrNode.multiplyDistinct(vanishesOnCurrElem);

        //extend to fit the amount of variable
        vector<size_t> originalVarsLocations;
        for(unsigned int i=0; i<= currLevelIndex; i++){
            originalVarsLocations.push_back(i);
        }
        polys_.push_back(polyPtr_t(new lightCircPoly(selectorForCurrNode,numVars_,originalVarsLocations)));
        }
     
        //For each possible value for the current variable,
        //build a selector polynomial that vanishes on every other
        //option prefix, but that value.
        //This polynomial would be a factor of a polynomial
        //that would vanish only on possible values for the next variable
        for(size_t currIndex = 0; currIndex < currLevel.size(); currIndex++){
            
            //gather roots
            elementsSet_t roots;
            for(const struct trieNode* node : currLevel){
                assert(node != NULL);
                if(node->val != currLevel[currIndex]->val){
                    roots.insert(node->val);
                }
            }

            //build polynomial
            const UnivariatePolynomialGeneral currLevelSelectorUniPoly(roots);
            const lightCircPoly currLevelSelector(currLevelSelectorUniPoly);
            lightCircPoly indexSelector(selector);
            indexSelector.multiplyDistinct(currLevelSelector);
            
            //call recursion
            generatePolysFromTrie_rec(indexSelector, currLevel[currIndex]->next, currLevelIndex+1);
        }
        
    }

    void generatePolysFromTrie(){
        vector<struct trieNode*>& firstList = rootsTrie_;

        //build first constrain polynomial
        //it vanishes if and only if the value of
        //the first element of an assignment correlates
        //with some root
        {
        elementsSet_t roots;
        for(const struct trieNode* node : firstList){
            assert(node != NULL);
            roots.insert(node->val);
        }
        const UnivariatePolynomialGeneral uniPoly(roots);
        const lightCircPoly vanishesOnFirstElem(uniPoly);

        //extend to fit the amount of variable
        vector<size_t> originalVarsLocations;
        originalVarsLocations.push_back(0);
        polys_.push_back(polyPtr_t(new lightCircPoly(vanishesOnFirstElem,numVars_,originalVarsLocations)));
        }

        //For each possible value for the first variable,
        //build a polynomial that vanishes on every other
        //option, but that value.
        //This polynomial would be a factor of a polynomial
        //that would vanish only on possible values for the next variable
        for(size_t currIndex = 0; currIndex < firstList.size(); currIndex++){
            
            //gather roots
            elementsSet_t roots;
            for(const struct trieNode* node : firstList){
                assert(node != NULL);
                if(node->val != firstList[currIndex]->val){
                    roots.insert(node->val);
                }
            }

            //build polynomial
            const UnivariatePolynomialGeneral selector(roots);
            
            //call recursion
            generatePolysFromTrie_rec(lightCircPoly(selector), firstList[currIndex]->next, 1);
        }
        
    }
};

/***************************************************
 *
 * This generates a valid Bair pair,
 * with no constraints and no permutations.
 * the only relevant test here is boundary constraints.
 *
 ***************************************************/
pair<BairInstance,BairWitness> generate_valid_boundary(){

    /** constants **/
    const size_t vectorLen = 3;
    const short domainSizeIndicator = 3;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;
    const size_t boundary_len = rand() % domainSize;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize));
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/
    
    //empty constraints
    BairInstance::constraintsPtr_t constraintsAssignment(new allwaysSatisfiedSystem(vectorLen*2));
    BairInstance::constraintsPtr_t constraintsPermutation(new allwaysSatisfiedSystem(vectorLen*2));
    
    //random boundary constraints
    BairInstance::boundaryConstraints_t boundaryConstraints;
    for(size_t i=0; i<boundary_len ; i++){
        const BairInstance::point_t location(rand()%domainSize , rand()% vectorLen);
        const FieldElement val = witness.get_color(location.first)[location.second];
        boundaryConstraints[location] = val;
    }

    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment), move(constraintsPermutation), boundaryConstraints, getRandomPadding(vectorLen));	

    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with no constraints and no permutations.
 * the only relevant test here is boundary constraints.
 *
 ***************************************************/
pair<BairInstance,BairWitness> generate_invalid_boundary(){

    /** constants **/
    const size_t vectorLen = 3;
    const short domainSizeIndicator = 3;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;
    const size_t boundary_len = 10 + rand() % domainSize;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize));
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/
    
    //empty constraints
    BairInstance::constraintsPtr_t constraintsAssignment(new allwaysSatisfiedSystem(vectorLen*2));
    BairInstance::constraintsPtr_t constraintsPermutation(new allwaysSatisfiedSystem(vectorLen*2));
    
    //random boundary constraints
    BairInstance::boundaryConstraints_t boundaryConstraints;
    for(size_t i=0; i<boundary_len ; i++){
        const BairInstance::point_t location(rand()%domainSize , rand()% vectorLen);
        const FieldElement val = witness.get_color(location.first)[location.second];
        boundaryConstraints[location] = val + one();
    }

    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment), move(constraintsPermutation), boundaryConstraints, getRandomPadding(vectorLen));	

    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}


/***************************************************
 *
 * This generates a valid Bair pair,
 * with some empty constraints and valid permutations.
 * The boundary is empty.
 * The assignment is random.
 * 
 ***************************************************/
pair<BairInstance,BairWitness> generate_valid_permutations(){

    /** constants **/
    const size_t vectorLen = 3;
    const short domainSizeIndicator = 7;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    
    //construct permutations
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize));
    
    //construct witness
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/
    
    //empty constraints vector
    BairInstance::constraintsPtr_t constraintsAssignment(new allwaysSatisfiedSystem(vectorLen*2));
    BairInstance::constraintsPtr_t constraintsPermutation(new allwaysSatisfiedSystem(vectorLen*2));
    
    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment),move(constraintsPermutation), BairInstance::boundaryConstraints_t(), getRandomPadding(vectorLen));	

    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}

/***************************************************
 *
 * This generates some valid Bair pair
 * 
 ***************************************************/
pair<BairInstance,BairWitness> generate_valid_pair(){

    /** constants **/
    const size_t vectorLen = 2+rand()%10;
    const short domainSizeIndicator = 3;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    
    //construct permutation
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize));
    
    //construct witness
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/
    
    //empty constraints
    BairInstance::constraintsPtr_t constraintsAssignment(new allwaysSatisfiedSystem(vectorLen*2));
    BairInstance::constraintsPtr_t constraintsPermutation(new allwaysSatisfiedSystem(vectorLen*2));
    
    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment),move(constraintsPermutation), BairInstance::boundaryConstraints_t(), getRandomPadding(vectorLen));	

    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with some empty constraints and valid permutations,
 * except of one, which is not a permutation.
 * The boundary is empty.
 * The assignment is random.
 * 
 ***************************************************/
pair<BairInstance,BairWitness> generate_invalid_permutations(){

    /** constants **/
    const size_t vectorLen = 3;
    const short domainSizeIndicator = 7;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    
    //construct (non)permutation
    //with some non-bijective mapping
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize-1));
    
    //construct witness
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/
    
    //empty constraints vector
    BairInstance::constraintsPtr_t constraintsAssignment(new allwaysSatisfiedSystem(vectorLen*2));
    BairInstance::constraintsPtr_t constraintsPermutation(new allwaysSatisfiedSystem(vectorLen*2));

    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment), move(constraintsPermutation), BairInstance::boundaryConstraints_t(), getRandomPadding(vectorLen));	

    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}

/***************************************************
 *
 * This generates a valid Bair pair,
 * with some constraints and valid permutations.
 * The boundary is empty.
 * The assignment is random.
 * 
 * Generation method:
 * generates all parameters but the
 * constraint systems.
 * And defines each constraint system to be satisfied
 * exactly from the parameters given.
 * 
 ***************************************************/
pair<BairInstance,BairWitness> generate_valid_constraints(){

    /** constants **/
    const size_t vectorLen = 3;
    const short domainSizeIndicator = 3;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    
    //construct permutations
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize));
    
    //construct witness
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/

    //construct constraints
    BairInstance::constraintsPtr_t constraintsAssignment(new settingSaticfyingSystem(vectorLen*2,domainSize-1,witness, plusOneSequence()));
    BairInstance::constraintsPtr_t constraintsPermutation(new settingSaticfyingSystem(vectorLen*2,domainSize,witness,witness.permutation()));
    
    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment), move(constraintsPermutation), BairInstance::boundaryConstraints_t(), getRandomPadding(vectorLen));	

    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with constraints and valid permutations.
 * The boundary is empty.
 * The assignment is random.
 * 
 * Generation method:
 * generates all parameters but the
 * constraint systems.
 * And defines each constraint system to be satisfied
 * exactly from the parameters given,
 * except of one victim constraints system that is
 * chosen randomly, one victim index 'i',
 * such that the test of that index 
 * (color(i),color(perm(i))) will fail.
 *
 * In this case the victim is constraintsAssignment
 * 
 ***************************************************/
pair<BairInstance,BairWitness> generate_invalid_constraints_Assignment(){

    /** constants **/
    const size_t vectorLen = 3;
    const short domainSizeIndicator = 4;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    
    //construct permutations
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize));
    
    //construct witness
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/

    //construct constraints
    BairInstance::constraintsPtr_t constraintsAssignment(new settingSaticfyingSystem(vectorLen*2,domainSize-1,witness, plusOneSequence(),true));
    BairInstance::constraintsPtr_t constraintsPermutation(new settingSaticfyingSystem(vectorLen*2,domainSize,witness,witness.permutation()));
    
    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment), move(constraintsPermutation), BairInstance::boundaryConstraints_t(), getRandomPadding(vectorLen));	

    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with constraints and valid permutations.
 * The boundary is empty.
 * The assignment is random.
 * 
 * Generation method:
 * generates all parameters but the
 * constraint systems.
 * And defines each constraint system to be satisfied
 * exactly from the parameters given,
 * except of one victim constraints system that is
 * chosen randomly, one victim index 'i',
 * such that the test of that index 
 * (color(i),color(perm(i))) will fail.
 *
 * In this case the victim is constraintsPermutation
 * 
 ***************************************************/
pair<BairInstance,BairWitness> generate_invalid_constraints_Permutation(){

    /** constants **/
    const size_t vectorLen = 3;
    const short domainSizeIndicator = 4;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    
    //construct permutations
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize));
    
    //construct witness
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/

    //construct constraints
    BairInstance::constraintsPtr_t constraintsAssignment(new settingSaticfyingSystem(vectorLen*2,domainSize-1,witness, plusOneSequence()));
    BairInstance::constraintsPtr_t constraintsPermutation(new settingSaticfyingSystem(vectorLen*2,domainSize,witness,witness.permutation(),true));
    
    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment), move(constraintsPermutation), BairInstance::boundaryConstraints_t(), getRandomPadding(vectorLen));	

    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with constraints and valid permutations.
 * The boundary is empty.
 * The assignment is random.
 * 
 * Generation method:
 * generates all parameters but the
 * constraint systems.
 * And defines each constraint system to be satisfied
 * exactly from the parameters given,
 * except of one victim constraints system that is
 * chosen randomly, one victim index 'i',
 * such that the test of that index 
 * (color(i),color(perm(i))) will fail.
 *
 * In this case the victims are both constraintsAssignment & constraintsPermutation
 * 
 ***************************************************/
pair<BairInstance,BairWitness> generate_invalid_constraints_both(){

    /** constants **/
    const size_t vectorLen = 3;
    const short domainSizeIndicator = 4;
    const size_t domainSize = POW2(domainSizeIndicator) - 1;

    /** construct witness **/
    BairWitness::assignment_ptr assignment(new randomColoring(domainSize,vectorLen));
    
    //construct permutations
    BairWitness::permutation_ptr permutation(new randPermutation(domainSize));
    
    //construct witness
    BairWitness witness(move(assignment), move(permutation));

    /** construct instance **/

    //construct constraints
    BairInstance::constraintsPtr_t constraintsAssignment(new settingSaticfyingSystem(vectorLen*2,domainSize-1,witness, plusOneSequence(),true));
    BairInstance::constraintsPtr_t constraintsPermutation(new settingSaticfyingSystem(vectorLen*2,domainSize,witness,witness.permutation(),true));
    
    //construct the instance
    BairInstance instance(vectorLen,domainSizeIndicator,move(constraintsAssignment), move(constraintsPermutation), BairInstance::boundaryConstraints_t(), getRandomPadding(vectorLen));	
    
    /** Return the result **/
    return pair<BairInstance,BairWitness>(move(instance),move(witness));
}

/***************************************************************************
 *
 *                              GTEST tests
 *
 ***************************************************************************/

/**
 * @brief   GTEST function to test completeness of BairWitnessChecker::verify_constraints
 */
TEST(BairWitnessChecker,verify_constraints_completeness){
    pair<BairInstance,BairWitness> validPair = generate_valid_constraints();
    EXPECT_TRUE(BairWitnessChecker::verify_constraintsAssignment(validPair.first,validPair.second));
    EXPECT_TRUE(BairWitnessChecker::verify_constraintsPermutation(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test soundness of BairWitnessChecker::verify_constraints
 * the Assignment constraints should not feet
 */
TEST(BairWitnessChecker,verify_constraints_Assignment_soundness){
    pair<BairInstance,BairWitness> validPair = generate_invalid_constraints_Assignment();
    EXPECT_FALSE(BairWitnessChecker::verify_constraintsAssignment(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test soundness of BairWitnessChecker::verify_constraints
 * the Permutation constraints should not feet
 */
TEST(BairWitnessChecker,verify_constraints_Permutation_soundness){
    pair<BairInstance,BairWitness> validPair = generate_invalid_constraints_Permutation();
    EXPECT_FALSE(BairWitnessChecker::verify_constraintsPermutation(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test soundness of BairWitnessChecker::verify_constraints
 * the both Assignment & Permutation constraints should not feet
 */
TEST(BairWitnessChecker,verify_constraints_Assignment_N_Permutation_soundness){
    pair<BairInstance,BairWitness> validPair = generate_invalid_constraints_both();
    EXPECT_FALSE(BairWitnessChecker::verify_constraintsAssignment(validPair.first,validPair.second));
    EXPECT_FALSE(BairWitnessChecker::verify_constraintsPermutation(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test completeness of BairWitnessChecker::verify_permutations
 */
TEST(BairWitnessChecker,verify_permutations_completeness){
    pair<BairInstance,BairWitness> validPair = generate_valid_permutations();
    EXPECT_TRUE(BairWitnessChecker::verify_permutation(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test soundness of BairWitnessChecker::verify_permutations
 */
TEST(BairWitnessChecker,verify_permutations_soundneness){
    pair<BairInstance,BairWitness> validPair = generate_invalid_permutations();
    EXPECT_FALSE(BairWitnessChecker::verify_permutation(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test completeness of BairWitnessChecker::verify_boundary
 */
TEST(BairWitnessChecker,verify_boundary_completeness){
    pair<BairInstance,BairWitness> validPair = generate_valid_boundary();
    EXPECT_TRUE(BairWitnessChecker::verify_boundary(validPair.first,validPair.second));
}

/**
 * @brief   GTEST function to test soundness of BairWitnessChecker::verify_boundary
 */
TEST(BairWitnessChecker,verify_boundary_soundness){
    pair<BairInstance,BairWitness> invalidPair = generate_invalid_boundary();
    EXPECT_FALSE(BairWitnessChecker::verify_boundary(invalidPair.first,invalidPair.second));
}

} //PCP_UTEST namespace
