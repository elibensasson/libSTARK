#ifndef BTRFLY_BASED_PERMUTATION_HPP__
#define BTRFLY_BASED_PERMUTATION_HPP__

#include "BenesNetwork.hpp"
#include "BitReverseButterfliesNetwork.hpp"
#include "PermutationRoutingNet.hpp"

namespace libstark{
namespace BairToAcsp{

/**
 * @class BtrflyBasedPermutation
 * @brief A class of networks that can implement any permutation over \f$2^k\f$ elements, using a composition of 4 butterfly networks of degree \f$k\f$
 *
 * Implementation details:
 * We know that we can rout any permutation on a Benes network, which is a composition of a reverse butterfly and a butterfly networks.
 * We use the graph isomorphism from reverse butterfly of degree \f$k\f$ to a butterfly network of the same degree
 * \f$ \phi(\ell_i,\omega) =  (\ell_i,\omega^R)\f$, this isomorphism maps a reverse butterfly to a butterfly that routs the same permutation in respect to the data,
 * but messes up the order of labels ,they are massed up with the bit reverse permutation. So in order to attach the result of this isomorphism to the butterfly network in the second half of the Benes network,
 * and to keep the permutation as it was originally in the Benes network, we need to permute the labels using the bit reverse permutation (which is the inverse of itself)
 * before the new butterfly network (the image of the reverse butterfly), and after it.
 * For this we can use BitReverseButterfliesNetwork that does each reverse using 2 butterfly network,
 * this way we get a network of 6 butterfly networks composed, that routs the original permutation.
 * We notice that the first 2 butterfly networks in this network is just the bit reverse permutation,
 * instead of using those we could just change the permutation using only the last for butterfly networks,
 * so instead of routing the permutation \f$ \pi \f$ on the Benes network we rout \f$ \tilde{\pi} \f$ which
 * defined by \f$ \tilde{\pi}(\omega) = \pi(\omega^R) \f$.
 * This change of permutation is done by the class indxBitReverse.
 */
class BtrflyBasedPermutation : public PermutationRoutingNet {
public:
	BtrflyBasedPermutation(const labelID_t k)
		:PermutationRoutingNet(1<<k),
		k_(k),
		benes_(k),
		bitRev_(k){;}
	
	layerID_t width() const {return 4*k_ + 1;} // = 1 + ceil(k/2)
	labelID_t height() const {return 1<<k_;}
	dataID_t getDataID(const layerID_t& l, const labelID_t& w) const;
	void rout(const permutation_t& permutation);

private:
	const layerID_t k_;
	BenesNetwork benes_;
	const BitReverseButterfliesNetwork bitRev_;
};

} //namespace BairToAcsp
} //namespace libstark

#endif //BTRFLY_BASED_PERMUTATION_HPP__
