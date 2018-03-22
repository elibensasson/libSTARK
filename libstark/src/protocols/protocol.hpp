#ifndef PROTOCOL_HPP__
#define PROTOCOL_HPP__

#include "languages/Bair/BairInstance.hpp"
#include "languages/Bair/BairWitness.hpp"
#include "languages/Acsp/AcspInstance.hpp"
#include "languages/Acsp/AcspWitness.hpp"
#include <algebraLib/FieldElement.hpp>
#include <vector>
#include <map>
#include <memory>

namespace libstark{
namespace Protocols{

//
// There are many different ways  to define a generic protocol.
//
//

//
// Transcript data related types
//
class TranscriptMessage{
public:
virtual ~TranscriptMessage(){};
};

typedef std::unique_ptr<TranscriptMessage> msg_ptr_t;

//
// Protocol parties definitions
//
class PartieInterface{
public:
    //Receive a message and return True if protocol finished
    virtual void receiveMessage(const TranscriptMessage& msg) = 0;

    //Send a message based on current internal state
    virtual msg_ptr_t sendMessage() = 0; 

    virtual ~PartieInterface(){};
};

class verifierInterface : public PartieInterface{
public:
    virtual ~verifierInterface(){};
    virtual bool doneInteracting()const=0;
    virtual bool verify()const=0;

    virtual size_t expectedCommitedProofBytes()const = 0;
    virtual size_t expectedSentProofBytes()const = 0;
    virtual size_t expectedQueriedDataBytes()const = 0;
    
    virtual void fillResultsAndCommitmentRandomly()=0;
};


//
// A partie that wants to verify a very big and fixed input
// is closed to some code C.
//
// The verifier first interacts with the prover,
// and after that reads from the input.
//
typedef std::map<size_t,std::vector<Algebra::FieldElement*>> queriesToInp_t;
class IOPP_verifierInterface : public verifierInterface{
public:
    virtual ~IOPP_verifierInterface(){};
    virtual const queriesToInp_t& queriesToInput()const=0;
};

//
// Protocol execution algorithm
//
bool executeProtocol(PartieInterface& prover, verifierInterface& verifier, const bool onlyVerifierData = false);

bool executeProtocol(const BairInstance& instance, const BairWitness& witness, const unsigned short securityParameter, bool testBair = false, bool testAcsp = false, bool testPCP = false);
void simulateProtocol(const BairInstance& instance, const unsigned short securityParameter);

//printouts
namespace prn{
	void printBairInstanceSpec(const BairInstance& instance);
	void printAcspInstanceSpec(const AcspInstance& instance);
	void printAcspWitnessSpec(const AcspWitness& witness);
	void printAcspPairSpec(const AcspInstance& instance, const AcspWitness& witness);
}

} //namespace Protocols
} //namespace libstark

#endif //ifndef PROTOCOL_HPP__
