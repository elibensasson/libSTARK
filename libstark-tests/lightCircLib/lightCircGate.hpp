#ifndef LIGHT_CIRCUIT_GATES_HPP_
#define LIGHT_CIRCUIT_GATES_HPP_

#include "common/Infrastructure/Infrastructure.hpp"
#include "common/Utils/ErrorHandling.hpp"
#include <algebraLib/FieldElement.hpp>
#include <algebraLib/PolynomialDegree.hpp>
#include "common/langCommon/Sequence.hpp"

#include <vector>
#include <map>
#include <set>
#include <functional>

namespace libstark {
namespace LightCircLib {

/******************************************************************************/
/*************************** Various Gate Classes *****************************/
/******************************************************************************/

typedef enum {UNKNOWN, GENERIC, ADDITION, TIMES, DIVISION, EXPO, CONSTANT, CONSTANT_0, CONSTANT_1, NEG, VANISHING, PHI, PHI_INVERSE, INPUT_G, OUTPUT, NAND} GateType;

class lightCircGate {
	
protected:

	std::vector<lightCircGate*> m_inputs;	///A list of input gates.
	  
	/**
	 * @brief   Calculates the value of the gate from its inputs evaluations vector
	 * @param   inputVals inputs evaluations
	 * @return  value of gate
	 */
	virtual Algebra::FieldElement calculateEvaluation(const LazyVector<Algebra::FieldElement>& inputVals)const = 0;
	
    virtual Algebra::PolynomialDegree calculateDegree(const std::vector<Algebra::PolynomialDegree>& inputDegrees)const = 0;
public:

    typedef std::map<lightCircGate const*,Algebra::FieldElement> evaluationMap_t;
    typedef std::map<lightCircGate const*,Algebra::PolynomialDegree> degreeCalcMap_t;

	/****************************************/
	/******* Constructors and Getters *******/
	/****************************************/

	/** Adds a new gate as input for this gate. */
	void addInputGate(lightCircGate* newInput);

	/** Adds a new gate as input for this gate. */
	void setInputGate(lightCircGate* newInput);

	/** Returns the list of gates that are the input of this gate */
	const std::vector<lightCircGate*>& getInputsVector() const { return m_inputs; }
	
	/** Returns the amount of gates that are the input of this gate */
	uint64_t getInputsAmount() const { return m_inputs.size(); }

	/*******************************/
	/***** Abstract Functions ******/
	/*******************************/

	/** Returns a copy of a gate itself, not including its neighbors! */
	virtual lightCircGate* shallowCopy() const = 0;

	/** Returns the gate type */
	virtual GateType getType() const = 0;
	
	/**
	 * @brief   Evaluates the gate according to its functionality and goal (this one is thread safe)
	 * @param   evaluations a mapping between gate addresses and there evaluated values
	 * (if already evaluated). At evaluation request a gate will first check if it
	 * was already evaluated by checking if its address mapped to any value, if it does
	 * that value is returned, otherwise it will be calculated
	 * @return  The value the gate outputs
	 */
	Algebra::FieldElement evaluateGate(lightCircGate::evaluationMap_t& evaluations)const;
	
    Algebra::PolynomialDegree getDegree(lightCircGate::degreeCalcMap_t& degrees)const;
	
	/*********************************************/
	/***** Gate Profiling and Manipulations ******/
	/*********************************************/

	/** Class Destructor */
	virtual ~lightCircGate(){};
};

/**************************************************/
/********** lightAdditionGate Class *************/
/**************************************************/
/**
* Addition gate. Its value equals the summation of its inputs.
*/
class lightAdditionGate : public lightCircGate{

protected:	
	/**
	 * @brief   Calculates the value of the gate from its inputs evaluations vector
	 * @param   inputVals inputs evaluations
	 * @return  value of gate
	 */
	Algebra::FieldElement calculateEvaluation(const LazyVector<Algebra::FieldElement>& inputVals)const;

    Algebra::PolynomialDegree calculateDegree(const std::vector<Algebra::PolynomialDegree>& inputDegrees)const;

public:

	/** Returns a copy of a gate itself, not including its neighbors! */
	lightCircGate* shallowCopy()const{ return new lightAdditionGate();}

	/** Returns the gate type */
	GateType getType() const{return ADDITION;}
};

/**************************************************/
/************ lightMultiplicationGate Class **************/
/**************************************************/
/**
* Times gate. Its value equals the multiplication of its inputs.
*/
class lightMultiplicationGate : public lightCircGate {
protected:	
	/**
	 * @brief   Calculates the value of the gate from its inputs evaluations vector
	 * @param   inputVals inputs evaluations
	 * @return  value of gate
	 */
	Algebra::FieldElement calculateEvaluation(const LazyVector<Algebra::FieldElement>& inputVals)const;

    Algebra::PolynomialDegree calculateDegree(const std::vector<Algebra::PolynomialDegree>& inputDegrees)const;
public:
	/** Returns a copy of a gate itself, not including its neighbors! */
	lightCircGate* shallowCopy()const {return new lightMultiplicationGate();}

	/** Returns the gate type */
	GateType getType() const{return TIMES;}
};


/**************************************************/
/************ lightExpoGate Class ***************/
/**************************************************/
/**
* Exponentiation gate. Its value equals its SINGLE input in some power (m_expo).
*/
class lightExpoGate : public lightCircGate {

private:
	unsigned long m_expo;			//The exponent of the gate

protected:	
	/**
	 * @brief   Calculates the value of the gate from its inputs evaluations vector
	 * @param   inputVals inputs evaluations
	 * @return  value of gate
	 */
	Algebra::FieldElement calculateEvaluation(const LazyVector<Algebra::FieldElement>& inputVals)const;

    Algebra::PolynomialDegree calculateDegree(const std::vector<Algebra::PolynomialDegree>& inputDegrees)const;
public:

	/** Class Constructor */
	lightExpoGate(const unsigned long expo) : m_expo(expo){};

	/** Returns a copy of a gate itself, not including its neighbors! */
	lightCircGate* shallowCopy()const{return new lightExpoGate(m_expo);}

	/** Returns the exponent defined for this gate */
	unsigned long getExpo() const { return m_expo; }

	/** Returns the gate type */
	GateType getType() const{return EXPO;}
};

/**************************************************/
/********* lightConstGate Class **************/
/**************************************************/
/**
* Constant gate. Has no input gates, and a constant value.
*/
class lightConstGate : public lightCircGate {

private:
	Algebra::FieldElement m_value;	///The constant value

protected:	
	/**
	 * @brief   Calculates the value of the gate from its inputs evaluations vector
	 * @param   inputVals inputs evaluations
	 * @return  value of gate
	 */
	Algebra::FieldElement calculateEvaluation(const LazyVector<Algebra::FieldElement>& inputVals)const{return m_value;}

    Algebra::PolynomialDegree calculateDegree(const std::vector<Algebra::PolynomialDegree>& inputDegrees)const;
public:

	/** Class Constructor */
	lightConstGate(const Algebra::FieldElement& value):m_value(value){};

	/** Returns a copy of a gate itself, not including its neighbors! */
	lightCircGate* shallowCopy()const{return new lightConstGate(m_value);}

	/** Returns the constant value of this gate. */
	Algebra::FieldElement getValue() const { return m_value; }

	/** Returns the gate type */
	GateType getType() const{return CONSTANT;}
};

/**************************************************/
/********** lightInputGate Class ****************/
/**************************************************/
class lightInputGate : public lightCircGate {

private:
    std::function<Algebra::FieldElement (const typename lightCircGate::evaluationMap_t& evaluations)> m_lazyValue;

protected:	
	/**
	 * @brief   Calculates the value of the gate from its inputs evaluations vector
	 * @param   inputVals inputs evaluations
	 * @return  value of gate
	 */
	Algebra::FieldElement calculateEvaluation(const LazyVector<Algebra::FieldElement>& inputVals)const{
        _COMMON_FATAL("No evaluation for light input gate");
    }

    Algebra::PolynomialDegree calculateDegree(const std::vector<Algebra::PolynomialDegree>& inputDegrees)const{
        _COMMON_FATAL("No degree query for light input gate");
    }

public:

	/** Returns a copy of a gate itself, not including its neighbors! */
	lightCircGate* shallowCopy()const{return new lightInputGate();}

	/** Returns the gate type */
	GateType getType() const{return INPUT_G;}
};

/**************************************************/
/********** lightOutGate Class ***************/
/**************************************************/
/**
* An output gate. Has a SINGLE input, with the same value as him.
*/
class lightOutGate : public lightCircGate {

protected:	
	/**
	 * @brief   Calculates the value of the gate from its inputs evaluations vector
	 * @param   inputVals inputs evaluations
	 * @return  value of gate
	 */
	Algebra::FieldElement calculateEvaluation(const LazyVector<Algebra::FieldElement>& inputVals)const;

    Algebra::PolynomialDegree calculateDegree(const std::vector<Algebra::PolynomialDegree>& inputDegrees)const;
public:

	/** Returns a copy of a gate itself, not including its neighbors! */
	lightCircGate* shallowCopy()const{return new lightOutGate();}

	/** Returns the gate type */
	GateType getType() const{return OUTPUT;}
};

}   //namespace LightCircLib
}	//namespace libstark


#endif	//LIGHT_CIRCUIT_GATES_HPP_
