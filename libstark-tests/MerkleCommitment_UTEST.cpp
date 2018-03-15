#include <protocols/common/CryptoCommitment/MerkleCommitment.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <stdint.h>

namespace{

using libstark::Protocols::CryptoCommitment::hashDigest_t;
using libstark::Protocols::CryptoCommitment::path_t;
using libstark::Protocols::CryptoCommitment::constructMerkleTree;
using libstark::Protocols::CryptoCommitment::getPathToBlock;
using libstark::Protocols::CryptoCommitment::verifyPathToBlock;
using libstark::Protocols::CryptoCommitment::hash;
using libstark::Protocols::CryptoCommitment::logBytesPerHash;
using libstark::Protocols::CryptoCommitment::SparceMerkleTree;
using std::vector;

TEST(MerkleCommitment,verifyTree){
    const short NUM_BLOCKS_LOG = 15;
    const short NUM_BYTES_LOG = NUM_BLOCKS_LOG+logBytesPerHash;
    vector<unsigned char> data(1UL<<NUM_BYTES_LOG);
    vector<hashDigest_t> merkleTree(1UL<<NUM_BLOCKS_LOG);

    //fill data
    for(size_t i=0; i<data.size(); i++){
        data[i] = (i<2? 1 : data[i-1] + data[i-2]);
    }

    //construct Merkle commitment
    constructMerkleTree(&data[0], NUM_BYTES_LOG, &merkleTree[0]);

    //verify tree
    for(size_t i=1; i < merkleTree.size()/2; i++){
        EXPECT_EQ(merkleTree[i],hash(&merkleTree[2*i]));
    }
}

TEST(MerkleCommitment,verifySubTree){
    const short NUM_BLOCKS_LOG = 15;
    const short NUM_BYTES_LOG = NUM_BLOCKS_LOG+logBytesPerHash;
    const short NUM_BLOCKS_PER_SIGMENT_LOG = NUM_BLOCKS_LOG/2;
    const short NUM_BYTES_PER_SIGMENT_LOG = NUM_BLOCKS_PER_SIGMENT_LOG+logBytesPerHash;
    
    //construct data
    vector<unsigned char> data(1UL<<NUM_BYTES_LOG);
    for(size_t i=0; i<data.size(); i++){
        data[i] = (i<2? 1 : data[i-1] + data[i-2]);
    }
    
    //construct referance
    vector<hashDigest_t> merkleTree_ref(1UL<<NUM_BLOCKS_LOG);
    constructMerkleTree(&data[0], NUM_BYTES_LOG, &merkleTree_ref[0]);

    //construct tree using segment subtrees
    vector<hashDigest_t> merkleTree(1UL<<NUM_BLOCKS_LOG);
    for(size_t i=0; i< (1UL<<(NUM_BYTES_LOG - NUM_BYTES_PER_SIGMENT_LOG)); i++){
        constructMerkleSubTree(&data[0],NUM_BYTES_LOG,NUM_BYTES_PER_SIGMENT_LOG,i,&merkleTree[0]);
    }
    const short left_src_log_len = NUM_BYTES_LOG - NUM_BYTES_PER_SIGMENT_LOG;
    constructMerkleTree(&merkleTree[1UL<<left_src_log_len], left_src_log_len+logBytesPerHash, &merkleTree[0]);

    //verify results
    for(size_t i=0; i< merkleTree.size(); i++){
        EXPECT_EQ(merkleTree[i],merkleTree_ref[i]);
    }

}

TEST(MerkleCommitment,verifyPaths){
    const short NUM_BLOCKS_LOG = 15;
    const short NUM_BYTES_LOG = NUM_BLOCKS_LOG+logBytesPerHash;
    vector<unsigned char> data(1UL<<NUM_BYTES_LOG);
    vector<unsigned char> merkleTree(data.size());

    //fill data
    for(size_t i=0; i<data.size(); i++){
        data[i] = (i<2? 1 : data[i-1] + data[i-2]);
    }

    //construct Merkle commitment
    const hashDigest_t root = constructMerkleTree(&data[0], NUM_BYTES_LOG, &merkleTree[0]);

    //verify all paths
    for(size_t i=0; i< (1UL<<NUM_BLOCKS_LOG); i++){
        hashDigest_t const*const currData = ((hashDigest_t*)&data[0])+(i & ~1UL);
        const path_t path = getPathToBlock(&merkleTree[0],NUM_BYTES_LOG,i);
        
        EXPECT_TRUE(verifyPathToBlock(currData,root,path,i));
    }
}

TEST(MerkleCommitment,SparceTree){
    const short NUM_BLOCKS_LOG = 6;
    const short NUM_BYTES_LOG = NUM_BLOCKS_LOG+logBytesPerHash;
    vector<unsigned char> data(1UL<<NUM_BYTES_LOG);
    const hashDigest_t* dataAsHashes((hashDigest_t*)&data[0]);
    vector<hashDigest_t> merkleTree(1UL<<NUM_BLOCKS_LOG);
    const int NUM_INDICES_TO_TEST = 10;

    //fill data
    for(size_t i=0; i<data.size(); i++){
        data[i] = (i<2? 1 : data[i-1] + data[i-2]);
    }

    //construct Merkle commitment
    const hashDigest_t root = constructMerkleTree(&data[0], NUM_BYTES_LOG, &merkleTree[0]);

    //Draw some indices to write and later read
    vector<size_t> indicesToTest;
    for(int i=0; i< NUM_INDICES_TO_TEST; i++){
        const size_t idxToAdd = rand() % (1<<(NUM_BLOCKS_LOG-1));
        indicesToTest.push_back(idxToAdd);
    }

    //Fill the sub tree
    SparceMerkleTree subTree(NUM_BYTES_LOG);
    {
        for(const auto& idx : indicesToTest){
            
            //fetch data
            std::array<hashDigest_t,2> currentData;
            currentData[0] = dataAsHashes[idx<<1];
            currentData[1] = dataAsHashes[(idx<<1)^1];

            //fetch path
            const path_t path = getPathToBlock(&merkleTree[0],NUM_BYTES_LOG,idx<<1);

            //add to subtree
            subTree.addPath(currentData,path,idx);

            //verify the subtree
            EXPECT_EQ(root, subTree.calculateRoot());
        }
    }

    //verify data integrity
    for(const auto& pairIdx : indicesToTest){
        for(int i=0; i<2; i++){
            const auto idx = (pairIdx<<1) ^ i;
            const auto dataFetched = subTree.readData(idx);
            EXPECT_EQ(dataAsHashes[idx],dataFetched);
        }
    }

    //Serialize and Deserialize test
    {
        const auto serialTree = subTree.toVector();
        SparceMerkleTree subTree_DS(NUM_BYTES_LOG);
        
        std::set<size_t> queriedIndices;
        for(const auto& pairIdx : indicesToTest){
            const auto idx = (pairIdx<<1);
            queriedIndices.insert(idx);
        }
        
        subTree_DS.DeSerialize(queriedIndices,serialTree);
       
        //verify the subtree
        EXPECT_EQ(root, subTree_DS.calculateRoot());
    }
}

}
