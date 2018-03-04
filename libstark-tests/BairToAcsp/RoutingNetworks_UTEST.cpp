#include <reductions/BairToAcsp/Routing/TandemNetwork.hpp>
#include <reductions/BairToAcsp/Routing/MatrixNetwork.hpp>
#include <reductions/BairToAcsp/Routing/BenesNetwork.hpp>
#include <reductions/BairToAcsp/Routing/FoldNetwork.hpp>
#include <reductions/BairToAcsp/Routing/BitReverseButterfliesNetwork.hpp>
#include <reductions/BairToAcsp/Routing/BtrflyBasedPermutation.hpp>
#include <reductions/BairToAcsp/Routing/DeBruijnPermutationNet.hpp>
#include <reductions/BairToAcsp/Routing/LongDeBruijnPermutationNet.hpp>
#include <reductions/BairToAcsp/Routing/SymmetricDeBruijnNetwork.hpp>
#include <reductions/BairToAcsp/Routing/LongSymmetricDeBruijnNetwork.hpp>
#include <common/Infrastructure/Infrastructure.hpp>

#include <gtest/gtest.h>
#include <vector>
#include <memory>

namespace{
using libstark::BairToAcsp::RoutingNetwork;
using libstark::BairToAcsp::TandemNetwork;
using libstark::BairToAcsp::matrixNetwork;
using libstark::BairToAcsp::BenesNetwork;
using libstark::BairToAcsp::foldLeftNetwork;
using libstark::BairToAcsp::foldRightNetwork;
using libstark::BairToAcsp::BitReverseButterfliesNetwork;
using libstark::BairToAcsp::BtrflyBasedPermutation;
using libstark::BairToAcsp::DeBruijnPermutationNet;
using libstark::BairToAcsp::LongDeBruijnPermutationNet;
using libstark::BairToAcsp::SymmetricDeBruijnNetwork;
using libstark::BairToAcsp::LongSymmetricDeBruijnNetwork;
using libstark::BairToAcsp::Deg2PermutationRoutingNet;
using libstark::Sequence;
using Infrastructure::IsPower2;
using Infrastructure::Log2;
using Infrastructure::POW2;
using std::vector;
using std::rand;
using std::unique_ptr;
using std::move;

typedef RoutingNetwork::layerID_t layerID_t;
typedef RoutingNetwork::labelID_t labelID_t;
typedef RoutingNetwork::dataID_t dataID_t;
typedef TandemNetwork::netsVec_t netsVec_t;

/****************************************************************
 *                Auxiliary classes templates
 ****************************************************************/
template<class element_t>
class generateElement {
public:
	virtual element_t operator()() const = 0;
	virtual ~generateElement(){;}
};

/**
 * @class specificOrder
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

vector<dataID_t> generateRandomData(const labelID_t& height){
	vector<dataID_t> res(height);
	for (unsigned int i=0; i<height; i++){
		res[i] = rand()%100;
	}
	return res;
}

/********************
 * Random permutation
 *******************/

class randOrderGen : public generateElement<size_t> {
	public:
		randOrderGen(size_t numElements): availabel(numElements){
			for(size_t i=0; i < numElements; i++) availabel[i] = i;
		}
		size_t operator()() const{
			assert(!availabel.empty());
			size_t elementIndex = rand() % availabel.size();
			size_t retElement = availabel[elementIndex];
			availabel.erase( availabel.begin() + elementIndex );
			return retElement;
		}
	private:
		mutable vector<size_t> availabel;
};

class randPermutation : public expliciteSequence<size_t> {
public:
	randPermutation(size_t numElements):
		expliciteSequence<size_t>(numElements,randOrderGen(numElements),0){};
};

/************************
 * Identity permutation
 * **********************/

class identityPermutation : public Sequence<size_t> {
public:
	size_t getElementByIndex(Sequence<size_t>::index_t index)const {
		return index;
	}
};

/****************************************
 * Verifiers
 * *************************************/

void verifyPermutation(const RoutingNetwork& net, const Sequence<dataID_t>& perm){
	const auto lastCol = net.width()-1;
    for (labelID_t w=0; w<net.height(); w++){
        const auto currSrc = net.getDataID(0,w);
        const auto currDest = net.getDataID(lastCol,w);
		
        const auto expectedDest = perm.getElementByIndex(currSrc);
		EXPECT_EQ(expectedDest,currDest);
	}
}

void verifyFirstCol(const RoutingNetwork& net){
    for (labelID_t w=0; w<net.height(); w++){
        const auto currVal = net.getDataID(0,w);
		
		EXPECT_EQ(w,currVal);
	}
}

/***************************************************************
 *                            GTESTS
 **************************************************************/

TEST(RoutingNetworks,tandemNetworks){

	const int numNetworks = rand()%10;
	const int height = rand()%10;
	vector<vector<dataID_t>> data;
	
	netsVec_t nets;

	//Generate first column
	data.push_back(vector<dataID_t>(generateRandomData(height)));

	//generate all data and networks
	for (int i=0; i<numNetworks; i++){
		unique_ptr<matrixNetwork> net(new matrixNetwork());
		net->numLayers = 1 + rand()%10;
		net->numNodes = height;
		net->data.push_back(vector<dataID_t>(data[data.size()-1]));
		while(net->data.size() < net->numLayers){
			data.push_back(vector<dataID_t>(generateRandomData(height)));
			net->data.push_back(vector<dataID_t>(data[data.size()-1]));
		}
		nets.push_back(move(net));
	}

	//construct the concatinated network
	TandemNetwork net(move(nets));

	EXPECT_EQ(data.size(),net.width());
	if (numNetworks == 0){
		EXPECT_EQ(0,net.height());
	}
	else{
		EXPECT_EQ(height,net.height());
	}
	
	for (layerID_t i=0; i < net.width(); i++){
		for (labelID_t w=0; w < net.height(); w++){
			EXPECT_EQ(data[i][w],net.getDataID(i,w));
		}
	}
}

TEST(RoutingNetworks,BenesPermutation){
	const layerID_t k = 1 + rand()%5;
	const labelID_t numElements = 1<<k;
	
	randPermutation perm(numElements);
	BenesNetwork net(k);
	net.rout(perm);
 
	//Verify dimensions
	EXPECT_EQ(net.width(), 2*k+1);
	EXPECT_EQ(net.height(), 1<<k);


	//Verify it is same permutation
	verifyPermutation(net,perm);

	//Verify first column is ordered
    verifyFirstCol(net);

	//Verify it is Benes network permutation
	for(layerID_t i=0; i<k; i++){
		for (labelID_t w=0; w<numElements; w++){
			
			const labelID_t mask = 1<<(k-i-1);
			
			//verify reverse butterfly (aka first half)
			{
			const auto currData = net.getDataID(i,w);
			const auto neighbor1 = net.getDataID(i+1,w);
			const auto neighbor2 = net.getDataID(i+1,w^mask);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
			}
			
			//verify butterfly (aka second half)
			{
			const auto currData = net.getDataID(2*k-i,w);
			const auto neighbor1 = net.getDataID(2*k-(i+1),w);
			const auto neighbor2 = net.getDataID(2*k-(i+1),w^mask);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
			}
		}
	}
}

TEST(RoutingNetworks,FoldLeft_k_is_2){
	const layerID_t k = 2;
	const layerID_t numLayers = 1 + k/2 + k%2; //= 1+ceil(k/2)
	const dataID_t expectation[][(1<<k)] = {{0,1,2,3},{0,1,3,2}};
	
	foldLeftNetwork net(k);
	
	for (layerID_t l=0; l< numLayers; l++){
		for (labelID_t w=0; w< (1<<k); w++){
			EXPECT_EQ(expectation[l][w], net.getDataID(l,w));
		} 
	}
}

TEST(RoutingNetworks,FoldLeft_k_is_3){
	const layerID_t k = 3;
	const layerID_t numLayers = 1 + k/2 + k%2; //= 1+ceil(k/2)
	const dataID_t expectation[][(1<<k)] = {{0,1,2,3,4,5,6,7},{0,1,2,3,5,4,7,6},{0,1,2,3,5,4,7,6}};
	
	foldLeftNetwork net(k);

	for (layerID_t l=0; l< numLayers; l++){
		for (labelID_t w=0; w< (1<<k); w++){
			EXPECT_EQ(expectation[l][w], net.getDataID(l,w));
		} 
	}
}

TEST(RoutingNetworks,FoldRight_k_is_3){
	const layerID_t k = 3;
	const layerID_t numLayers = 1 + k/2 ; //= 1 + floor(k/2)
	const dataID_t expectation[][(1<<k)] = {{0,1,2,3,4,5,6,7},{0,5,2,7,4,1,6,3}};
	
	foldRightNetwork net(k);
	
	for (layerID_t l=0; l< numLayers; l++){
		for (labelID_t w=0; w< (1<<k); w++){
			EXPECT_EQ(expectation[l][w], net.getDataID(l,w));
		} 
	}
}

TEST(RoutingNetworks,FoldRight_k_is_4){
	const layerID_t k = 4;
	const layerID_t numLayers = 1 + k/2 ; //= 1 + floor(k/2)
	const dataID_t expectation[][(1<<k)] = {
						{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
						{0,1,6,7,4,5,2,3,8,9,14,15,12,13,10,11},
						{0,9,6,15,4,13,2,11,8,1,14,7,12,5,10,3}};
	
	foldRightNetwork net(k);
	
	for (layerID_t l=0; l< numLayers; l++){
		for (labelID_t w=0; w< (1<<k); w++){
			EXPECT_EQ(expectation[l][w], net.getDataID(l,w));
		} 
	}
}

TEST(RoutingNetworks,BitReverseButterflies){
	const layerID_t k = 1 + rand()%10;
	
	BitReverseButterfliesNetwork net(k);

	//Verify dimensions
	EXPECT_EQ(net.width(), 2*k+1);
	EXPECT_EQ(net.height(), 1<<k);

	//Verify it is butterflies network permutation
	for(layerID_t l=0; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = 1<<(l%k);
			
			const auto currData = net.getDataID(l,w);
			const auto neighbor1 = net.getDataID(l+1,w);
			const auto neighbor2 = net.getDataID(l+1,w^mask);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
		}
	
	}

	//verify it is the bit reverse permutation
	const layerID_t lastLayer = net.width()-1;
	for (labelID_t w=0; w < net.height(); w++){
		for(size_t i=0; i<k ; i++){
			const labelID_t mask1 = 1<<i;
			const labelID_t mask2 = 1<<(k-i-1);
			const labelID_t bit1 = (w&mask1)>>i;
			const labelID_t bit2 = (net.getDataID(lastLayer,w)&mask2)>>(k-i-1);
			EXPECT_EQ(bit1,bit2);
		}
	}
}

TEST(RoutingNetworks,BtrflyBasedPermutation){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	BtrflyBasedPermutation net(k);
	net.rout(perm);

	//Verify dimensions
	EXPECT_EQ(net.width(), 4*k+1);
	EXPECT_EQ(net.height(), numElements);
	
	//Verify it is same permutation
    verifyPermutation(net,perm);

	//Verify first column is ordered
    verifyFirstCol(net);

	//Verify it is butterflies network permutation
	for(layerID_t l=0; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = 1<<(l%k);
			
			const auto currData = net.getDataID(l,w);
			const auto neighbor1 = net.getDataID(l+1,w);
			const auto neighbor2 = net.getDataID(l+1,w^mask);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
		}
	
	}
}

TEST(RoutingNetworks,DeBruijnBasedPermutation){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	DeBruijnPermutationNet net(k);
	net.rout(perm);

	//Verify dimensions
	EXPECT_EQ(net.width(), 4*k+1);
	EXPECT_EQ(net.height(), numElements);
	
	//Verify it is same permutation
    verifyPermutation(net,perm);

	//Verify first column is ordered
    verifyFirstCol(net);

	//Verify it is DeBruijn network permutation
	for(layerID_t l=0; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			const auto neighbor1 = net.getDataID(l+1,neighbor1_label);
			const auto neighbor2 = net.getDataID(l+1,neighbor2_label);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
		}
	
	}
}

TEST(RoutingNetworks,DeBruijnBasedPermutation_RoutingBit_completeness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	DeBruijnPermutationNet net(k);
	net.rout(perm);
	
	//Verify it is DeBruijn network permutation
	for(layerID_t l=0; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_TRUE(currData == neighbor[net.routingBit(l,w)]);
		}
	
	}
}

TEST(RoutingNetworks,DeBruijnBasedPermutation_RoutingBit_soundness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	DeBruijnPermutationNet net(k);
	net.rout(perm);
	
	//Verify it is DeBruijn network permutation
	for(layerID_t l=0; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_FALSE(currData == neighbor[1-net.routingBit(l,w)]);
		}
	
	}
}

TEST(RoutingNetworks,LongDeBruijnBasedPermutation){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	LongDeBruijnPermutationNet net(k);
	net.rout(perm);

	//Verify dimensions
	EXPECT_EQ(net.height(), numElements);
	EXPECT_TRUE(net.width() >= 4*k+1);
	EXPECT_TRUE(IsPower2(net.width()));
	EXPECT_TRUE(POW2(Log2(net.width())-1) < 4*k+1);
	
	//Verify it is same permutation
    verifyPermutation(net,perm);

	//Verify first column is ordered
    verifyFirstCol(net);
    
    //Verify it is DeBruijn network permutation
	for(layerID_t l=0; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			const auto neighbor1 = net.getDataID(l+1,neighbor1_label);
			const auto neighbor2 = net.getDataID(l+1,neighbor2_label);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
		}
	
	}
}

TEST(RoutingNetworks,LongDeBruijnBasedPermutation_RoutingBit_completeness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	LongDeBruijnPermutationNet net(k);
	net.rout(perm);
	
	//Verify it is DeBruijn network permutation
	for(layerID_t l=0; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_TRUE(currData == neighbor[net.routingBit(l,w)]);
		}
	
	}
}

TEST(RoutingNetworks,LongDeBruijnBasedPermutation_RoutingBit_soundness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	LongDeBruijnPermutationNet net(k);
	net.rout(perm);
	
	//Verify it is DeBruijn network permutation
	for(layerID_t l=0; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_FALSE(currData == neighbor[1-net.routingBit(l,w)]);
		}
	
	}
}

TEST(RoutingNetworks,SymmetricDeBruijn){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	SymmetricDeBruijnNetwork net(k);
	net.rout(perm);

	//Verify dimensions
	EXPECT_EQ(net.height(), numElements);
	EXPECT_TRUE(net.width() == 2*k+1);

	//Verify it is same permutation
    verifyPermutation(net,perm);

	//Verify first column is ordered
    verifyFirstCol(net);
    
    //Verify it is Symmetric DeBruijn network permutation
	for(layerID_t l=0; l < (net.width()-1)/2; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
            //verify for reverse network (first half)
            {
			const auto currData = net.getDataID(k-l,w);
			const auto neighbor1 = net.getDataID(k-(l+1),neighbor1_label);
			const auto neighbor2 = net.getDataID(k-(l+1),neighbor2_label);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
            }

            //verify for straight network (second half)
            {
			const auto currData = net.getDataID(k+l,w);
			const auto neighbor1 = net.getDataID(k+(l+1),neighbor1_label);
			const auto neighbor2 = net.getDataID(k+(l+1),neighbor2_label);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
            }
		}
	
	}
}

TEST(RoutingNetworks,SymmetricDeBruijn_RoutingBit_completeness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	SymmetricDeBruijnNetwork net(k);
	net.rout(perm);
	
    const labelID_t mask = (1<<k)-1;
			
	//Verify it is reverse DeBruijn network permutation in first half
	for(layerID_t l=0; l < k; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const auto neighbor1_label = ((w<<1) | (w>>(k-1))) & mask;
            const auto shifted1 = ((1<<1) | (1>>(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ shifted1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_TRUE(currData == neighbor[net.routingBit(l,w)]);
		}
	
	}
	
    //Verify it is straight DeBruijn network permutation in second half
	for(layerID_t l=k; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_TRUE(currData == neighbor[net.routingBit(l,w)]);
		}
	
	}
}

TEST(RoutingNetworks,SymmetricDeBruijn_RoutingBit_soundness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	SymmetricDeBruijnNetwork net(k);
	net.rout(perm);
	
    const labelID_t mask = (1<<k)-1;
			
	//Verify it is reverse DeBruijn network permutation in first half
	for(layerID_t l=0; l < k; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const auto neighbor1_label = ((w<<1) | (w>>(k-1))) & mask;
            const auto shifted1 = ((1<<1) | (1>>(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ shifted1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_FALSE(currData == neighbor[1-net.routingBit(l,w)]);
		}
	
	}
	
    //Verify it is straight DeBruijn network permutation in second half
	for(layerID_t l=k; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_FALSE(currData == neighbor[1-net.routingBit(l,w)]);
		}
	
	}
}

TEST(RoutingNetworks,LongSymmetricDeBruijn){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	LongSymmetricDeBruijnNetwork net(k);
	net.rout(perm);

	//Verify dimensions
	EXPECT_EQ(net.height(), numElements);
	EXPECT_TRUE(IsPower2(net.width()+3));
    EXPECT_TRUE(net.width() >= 2*k+1);
    EXPECT_TRUE((net.width()+1)/2 -1 < 2*k+1);

	//Verify it is same permutation
    verifyPermutation(net,perm);

	//Verify first column is ordered
    verifyFirstCol(net);

    const layerID_t center = (net.width()-1)/2;
    //Verify it is Symmetric DeBruijn network permutation
	for(layerID_t l=0; l < center; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
            //verify for reverse network (first half)
            {
			const auto currData = net.getDataID(center-l,w);
			const auto neighbor1 = net.getDataID(center-(l+1),neighbor1_label);
			const auto neighbor2 = net.getDataID(center-(l+1),neighbor2_label);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
            }

            //verify for straight network (second half)
            {
			const auto currData = net.getDataID(center+l,w);
			const auto neighbor1 = net.getDataID(center+(l+1),neighbor1_label);
			const auto neighbor2 = net.getDataID(center+(l+1),neighbor2_label);
			
			EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
            }
		}
	
	}
}

TEST(RoutingNetworks,LongSymmetricDeBruijn_RoutingBit_completeness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	LongSymmetricDeBruijnNetwork net(k);
	net.rout(perm);
	
    const labelID_t mask = (1<<k)-1;
    const layerID_t center = (net.width()-1)/2;
			
	//Verify it is reverse DeBruijn network permutation in first half
	for(layerID_t l=0; l < center; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const auto neighbor1_label = ((w<<1) | (w>>(k-1))) & mask;
            const auto shifted1 = ((1<<1) | (1>>(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ shifted1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_TRUE(currData == neighbor[net.routingBit(l,w)]);
		}
	
	}
	
    //Verify it is straight DeBruijn network permutation in second half
	for(layerID_t l=center; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_TRUE(currData == neighbor[net.routingBit(l,w)]);
		}
	
	}

}

TEST(RoutingNetworks,LongSymmetricDeBruijn_RoutingBit_soundness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	LongSymmetricDeBruijnNetwork net(k);
	net.rout(perm);
	
    const labelID_t mask = (1<<k)-1;
    const layerID_t center = (net.width()-1)/2;
			
	//Verify it is reverse DeBruijn network permutation in first half
	for(layerID_t l=0; l < center; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const auto neighbor1_label = ((w<<1) | (w>>(k-1))) & mask;
            const auto shifted1 = ((1<<1) | (1>>(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ shifted1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_FALSE(currData == neighbor[1-net.routingBit(l,w)]);
		}
	
	}
	
    //Verify it is straight DeBruijn network permutation in second half
	for(layerID_t l=center; l < net.width()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const auto neighbor1_label = ((w>>1) | (w<<(k-1))) & mask;
			const auto neighbor2_label = neighbor1_label ^ 1;
			
			const auto currData = net.getDataID(l,w);
			labelID_t neighbor[] = { net.getDataID(l+1,neighbor1_label) , net.getDataID(l+1,neighbor2_label) };
			
			EXPECT_FALSE(currData == neighbor[1-net.routingBit(l,w)]);
		}
	
	}
}

TEST(RoutingNetworks,LongSymmetricDeBruijn_wings){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	LongSymmetricDeBruijnNetwork net(k);
	net.rout(perm);

	//Verify dimensions
	EXPECT_EQ(net.height(), numElements);
	EXPECT_TRUE(IsPower2(net.getWingWidth()+1));
    EXPECT_TRUE(net.getWingWidth() >= k+1);
    EXPECT_TRUE((net.getWingWidth()+1)/2 -1 < k+1);

    //verify last column is common
    for (labelID_t w=0; w< net.height(); w++){
        const auto v1 = net.getDataID(0,net.getWingWidth()-1,w);
        const auto v2 = net.getDataID(1,net.getWingWidth()-1,w);
        EXPECT_EQ(v1,v2);
    }

	//Verify it is same permutation
    for (labelID_t w=0; w< net.height(); w++){
        const auto src = net.getDataID(0,0,w);
        const auto dst = net.getDataID(1,0,w);
        const auto expectedDst = perm.getElementByIndex(src);
        EXPECT_EQ(expectedDst,dst);
    }

	//Verify first column is ordered
    for (labelID_t w=0; w< net.height(); w++){
        const auto data = net.getDataID(0,0,w);
        EXPECT_EQ(w,data);
    }
    
    //Verify each wing is a DeBruijn network
	for(layerID_t l=0; l < net.getWingWidth()-1; l++){
		for (labelID_t w=0; w < net.height(); w++){
			
			const labelID_t mask = (1<<k)-1;
			
			const auto neighbor1_label = ((w<<1) | (w>>(k-1))) & mask;
			const auto w1 = w^1;
            const auto neighbor2_label = ((w1<<1) | (w1>>(k-1))) & mask;
			
            //verify for each network
            for(short netID=0; netID <2; netID++){
                const auto currData = net.getDataID(netID,l,w);
                const auto neighbor1 = net.getDataID(netID,l+1,neighbor1_label);
                const auto neighbor2 = net.getDataID(netID,l+1,neighbor2_label);

                EXPECT_TRUE((currData == neighbor1) || (currData == neighbor2));
            }
		}
	
	}
}

TEST(RoutingNetworks,LongSymmetricDeBruijn_Wings_RoutingBit_completeness){
	const layerID_t k = 1 + rand()%10;
	const labelID_t numElements = 1<<k;
	const randPermutation perm(numElements);
	
	LongSymmetricDeBruijnNetwork net(k);
	net.rout(perm);
	
    const labelID_t mask = (1<<k)-1;
			
	//Verify it is reverse DeBruijn network
	for (short netID=0; netID < 2; netID++){
        for(layerID_t l=0; l < net.getWingWidth()-1; l++){
            for (labelID_t w=0; w < net.height(); w++){

                const auto neighbor1_label = ((w<<1) | (w>>(k-1))) & mask;
                const auto shifted1 = ((1<<1) | (1>>(k-1))) & mask;
                const auto neighbor2_label = neighbor1_label ^ shifted1;

                const auto currData = net.getDataID(netID,l,w);
                labelID_t neighbor[] = { net.getDataID(netID,l+1,neighbor1_label) , net.getDataID(netID,l+1,neighbor2_label) };

                EXPECT_TRUE(currData == neighbor[net.routingBit(netID,l,w)]);
            }

        }
    }

}

TEST(RoutingNetworks,LongSymmetricDeBruijn_Wings_RoutingBit_soundness){
    const layerID_t k = 1 + rand()%10;
    const labelID_t numElements = 1<<k;
    const randPermutation perm(numElements);

    LongSymmetricDeBruijnNetwork net(k);
    net.rout(perm);

    const labelID_t mask = (1<<k)-1;

    //Verify it is reverse DeBruijn network
    for(short netID=0; netID<2; netID++){
        for(layerID_t l=0; l < net.getWingWidth()-1; l++){
            for (labelID_t w=0; w < net.height(); w++){

                const auto neighbor1_label = ((w<<1) | (w>>(k-1))) & mask;
                const auto shifted1 = ((1<<1) | (1>>(k-1))) & mask;
                const auto neighbor2_label = neighbor1_label ^ shifted1;

                const auto currData = net.getDataID(netID,l,w);
                labelID_t neighbor[] = { net.getDataID(netID,l+1,neighbor1_label) , net.getDataID(netID,l+1,neighbor2_label) };

                EXPECT_FALSE(currData == neighbor[1-net.routingBit(netID,l,w)]);
            }
        }
    }
}
} // anonimus namespace
