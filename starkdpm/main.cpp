#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <protocols/protocol.hpp>
//#include <languages/Bair/BairWitnessChecker.hpp>
#include "AES160hashcLOCI.hpp"

using namespace stark_dpm;
using namespace stark_dpm::ACSP_FOR_AES160hashcLOCI;
using std::cout;
using std::endl;
using std::string;
using std::stoul;
using AES160hashcLOCI::dpmpair_t;
using AES160hashcLOCI::fingerprint_t;
using AES160hashcLOCI::database_t;

const string securityPrefix = "-s";


void printHelp(const string exeName){
    cout<<"Usage:"<<endl;
    cout<<"$>"<<exeName<<" <database file path> <fingerprint file path> [" + securityPrefix + "<security parameter>]"<<endl;
    cout<<endl<<"Example:"<<endl;
    cout<<"$>"<<exeName<<" examples-dpm/database.txt examples-dpm/fp_no_match.txt "+securityPrefix+"120"<<std::endl;
    cout<<endl<<"The above execution results in execution of STARK simulation over the DPM blacklist program, with the database represented by examples-dpm/database.txt,";
    cout<<" the suspects fingerprint in examples-dpm/fp_nomatch.txt, with soundness error at most 2^-120. The prover generates in this case a proof for the claim that the fingerprint does not perfectly match any entry in the database."<<endl;
    cout<<endl<<"A single fingerprint is represented by a single line, each line contains 20 pairs delimited by spaces, each pair contains two 8 bit numbers in hexadecimal basis, separated by a single period. A database is a file where each line represents a fingerprint."<<endl;
    cout<<endl<<"In the simulation the Prover and Verify interact, the Prover generates a proof and the Verifier verifies it. During the executions the specifications of generated BAIR and APR, measurements, and Verifiers decision, are printed to the standard output."<<endl;

}

void execute(const fingerprint_t& fprint,const database_t& db, const unsigned int securityParameter){
    AES160hashcLOCICommonParams params;
    params.length = db.size()*2;

    libstark::BairInstance bair_instance = buildBairInstance(params);

    std::vector<std::vector<Algebra::FieldElement>> hashC;
    std::vector<Algebra::FieldElement> Result = AES160hashcLOCI::genHashchain(hashC, db);
    AES160hashcLOCI::evalp::setParams(Result, Algebra::power(xFE(), 1+params.length), 1234); //TODO: seed arg instead of 1234 for randcoeffs
    libstark::BairWitness bair_witness = buildBairWitness(params, hashC, fprint);

    //std::cout << libstark::BairWitnessChecker::verify(bair_instance, bair_witness) << std::endl;
    libstark::Protocols::executeProtocol(bair_instance, bair_witness,securityParameter,false,false,true);
}

dpmpair_t readPair(const string pairStr){
    std::regex regex{R"([.]+)"}; // split on period
    std::sregex_token_iterator it{pairStr.begin(), pairStr.end(), regex, -1};
    std::vector<std::string> words{it, {}};
    
    if(words.size() != 2){
        cout<<"Each pair must contain 2 elements"<<std::endl;
        throw("bad format of DPM file");
    }

    dpmpair_t res;
    for(int i=0; i<2; i++){
        res[i] = std::stoul(words[i],0,16);
    }

    return res;
}

fingerprint_t readEntry(const string line){
    std::regex regex{R"([\s]+)"}; // split on space
    std::sregex_token_iterator it{line.begin(), line.end(), regex, -1};
    std::vector<std::string> words{it, {}};

    if(words.size() != 20){
        cout<<"Each line must contain 20 pairs"<<std::endl;
        throw("bad format of DPM file");
    }

    fingerprint_t fprint;
    for(unsigned int i=0; i<20; i++){
        fprint[i] = readPair(words[i]);
    }
    return fprint;
}

database_t readDatabaseFromFile(const string filename){
    std::ifstream ifs(filename);
    std::string content((std::istreambuf_iterator<char>(ifs)),std::istreambuf_iterator<char>());
        
    std::regex regex{R"([\n]+)"}; // split to lines
    std::sregex_token_iterator it{content.begin(), content.end(), regex, -1};
    std::vector<std::string> lines{it, {}};

    database_t db(lines.size());

    for(unsigned int i=0; i<lines.size() ;i++){
        db[i] = readEntry(lines[i]);
    }

    return db;
}

int main(int argc, char *argv[]) {
    if(argc < 3){
        printHelp(argv[0]);
        return 0;
    }

    database_t db = readDatabaseFromFile(argv[1]);
    fingerprint_t fprint;
    {
        database_t tmp = readDatabaseFromFile(argv[2]);
        if(tmp.size() <1){
            cout<<"bad format of fingerprint file, at least one fingerprint expected"<<endl;
            throw("bad format of fingerprint file");
        }
        fprint = tmp[0];
    }
    
    unsigned int securityParameter = 60;
    for(int i=3; i< argc; i++){
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
        printHelp(argv[0]);
        return 0;
    }


    execute(fprint,db, securityParameter);

    return 0;
}
