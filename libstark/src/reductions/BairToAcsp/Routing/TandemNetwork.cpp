#include "TandemNetwork.hpp"
#include "common/Infrastructure/Infrastructure.hpp"


namespace libstark{
namespace BairToAcsp{

typedef TandemNetwork::layerID_t layerID_t;
typedef TandemNetwork::labelID_t labelID_t;
typedef TandemNetwork::dataID_t dataID_t;

TandemNetwork::TandemNetwork(netsVec_t&& networks) : networks_(std::move(networks)){}

layerID_t TandemNetwork::width()const{
	layerID_t amount = 1;
	for (const auto& network : networks_){
		amount += network->width() - 1;
		//-1 because for each network, the first layer is
		//shared with the last layer of previous network (if there is such)
	}
	return amount;
}

labelID_t TandemNetwork::height()const{
	//It assumes all the networks has the same amount of nodes
	//per layer.
	//Otherwise it does not describe a valid routing network
	if (networks_.size() > 0) return networks_[0]->height();
	return 0;
}

dataID_t TandemNetwork::getDataID(const layerID_t& l, const labelID_t& w)const{
	layerID_t i = 0;
	for (const auto& network : networks_){
		if (i+network->width() > l) return network->getDataID(l-i,w);
		i += network->width() - 1;
	}
	_COMMON_FATAL("Routing Network : out of range");
}

void TandemNetwork::reset(netsVec_t&& networks){
	networks_ = move(networks);
}

} //namespace BairToAcsp
} //namespace libstark
