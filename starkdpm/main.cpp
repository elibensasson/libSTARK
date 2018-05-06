#include <iostream>
#include <string>
#include <protocols/protocol.hpp>
#include "AES160hashcLOCI.hpp"

using namespace stark_dpm;
using namespace stark_dpm::ACSP_FOR_AES160hashcLOCI;
using std::cout;
using std::endl;
using std::string;
using std::stoul;

const string securityPrefix = "-s";

void execute(long long len, int seed, bool measureVerifierOnly, const unsigned int securityParameter){
    AES160hashcLOCICommonParams params;
    params.length = len;
    params.seed = seed;

    libstark::BairInstance bair_instance = buildBairInstance(params);

    if(measureVerifierOnly){
       libstark::Protocols::simulateProtocol(bair_instance, securityParameter);
       return;
    }

    std::vector<std::vector<Algebra::FieldElement>> hashC;
    std::vector<Algebra::FieldElement> Result = AES160hashcLOCI::genHashchain(hashC, params.length, params.seed);
    AES160hashcLOCI::evalp::setParams(Result, Algebra::power(xFE(), 1 + params.length), params.seed);
    libstark::BairWitness bair_witness = buildBairWitness(params, hashC);

    libstark::Protocols::executeProtocol(bair_instance, bair_witness,securityParameter,false,false,true);
}

int main(int argc, char *argv[]) {

    long long length = 5;
    int seed = 127;
    bool measureVerifierOnly = false;
    if (argc >= 2) {
       length = std::stoll(argv[1]);
    }
    if (argc >= 3) {
       seed = std::stoi(argv[2]);
    }
    if (argc >= 4) {
       const std::string inp(argv[3]);
       const std::string verOnly("verifier_only");
       measureVerifierOnly = !verOnly.compare(inp);
    }

    unsigned int securityParameter = 60;
    for(int i=4; i< argc; i++){
        const string currArg(argv[i]);
        if(currArg.length()<3){
            continue;
        }

        const string prefix = currArg.substr(0,2);
        const unsigned int num(stoul(currArg.substr(2)));
        
        if(prefix == securityPrefix){
            securityParameter = num;
        }
    }

    if(securityParameter == 0){
        return 0;
    }

    execute(length, seed, measureVerifierOnly, securityParameter);

    return 0;
}
