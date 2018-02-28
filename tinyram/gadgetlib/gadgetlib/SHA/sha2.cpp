/** @file
 *****************************************************************************
 SHA2-specific gadgets.

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#include "sha2.hpp"
#include <gadgetlib/extra_gadgets.hpp>
#define ROTR(A, i, k) A[((i)+(k)) % 32]

//using ::std::shared_ptr;
//using ::std::string;
//using ::std::vector;
//using ::std::cout;
//using ::std::cerr;
//using ::std::endl;

namespace gadgetlib {
namespace sha2 {

const unsigned long SHA256_K[64] =  {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const unsigned long SHA256_H[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

void generate_SHA256_IV(ProtoboardPtr pb, ::std::vector<VariableArray>& IV) {
   for (int i=0; i<8; ++i) {
      IV.push_back(VariableArray(32));
      for (int j=0; j<32; ++j) {
         int iv_val = (SHA256_H[i] >> j) & 1;
         //int iv_val = (SHA256_H[i] >> (31-j)) & 1;
         pb->addRank1Constraint(1, IV[i][j], iv_val, "iv");
         pb->val(IV[i][j]) = iv_val;
      }
   }
}

MsgSchedule_Gadget::MsgSchedule_Gadget(
   ProtoboardPtr pb, const ::std::vector<VariableArray>& W)
   : Gadget(pb), W_(W) {}

void MsgSchedule_Gadget::init() {
   for(int i=0; i<SHA2RND-16; ++i) {
      result0_.push_back( ::std::make_shared<VariableArray>(32) );
      result1_.push_back( ::std::make_shared<VariableArray>(32) );
      ssigma0_.push_back( SMALLSIGMA_Gadget::create(pb_,
         7, 18, 3, W_[i+16-15], *result0_[i]) );
      ssigma1_.push_back( SMALLSIGMA_Gadget::create(pb_,
         17, 19, 10, W_[i+16-2], *result1_[i]) );
      add4_.push_back( ADD_Gadget::create(pb_,
         W_[i+16-16], *result0_[i], W_[i+16-7], *result1_[i], W_[i+16]) );
   }
}

void MsgSchedule_Gadget::generateConstraints() {
   for (auto& curGadget : ssigma0_)
      curGadget->generateConstraints();
   for (auto& curGadget : ssigma1_)
      curGadget->generateConstraints();
   for (auto& curGadget : add4_)
      curGadget->generateConstraints();
}

void MsgSchedule_Gadget::generateWitness() {
   for(int i=0; i<SHA2RND-16; ++i) {
      ssigma0_[i]->generateWitness();
      ssigma1_[i]->generateWitness();
      add4_[i]->generateWitness();
   }
}

GadgetPtr MsgSchedule_Gadget::create(ProtoboardPtr pb,
   const ::std::vector<VariableArray>& W) {
   addGadgetToTraceback("MsgSchedule_Gadget");
   GadgetPtr pGadget(new MsgSchedule_Gadget(pb, W));
   pGadget->init();
   removeGadgetFromTraceback("MsgSchedule_Gadget");
   return pGadget;
}

RoundFunc_Gadget::RoundFunc_Gadget(ProtoboardPtr pb,
      const VariableArray& a, const VariableArray& b,
      const VariableArray& c, const VariableArray& d,
      const VariableArray& e, const VariableArray& f,
      const VariableArray& g, const VariableArray& h,
      const VariableArray& curr_W, const unsigned long curr_K,
      const VariableArray& new_a, const VariableArray& new_e)
      : Gadget(pb),
        a_(a), b_(b), c_(c), d_(d),
        e_(e), f_(f), g_(g), h_(h),
        curr_W_(curr_W), curr_K_(curr_K),
        new_a_(new_a), new_e_(new_e),
        s0_(32), s1_(32), ch_(32), maj_(32), add_(32) {}

void RoundFunc_Gadget::init() {
   compute_s0_ = BIGSIGMA_Gadget::create(pb_,
         2, 13, 22, a_, s0_);
   compute_s1_ = BIGSIGMA_Gadget::create(pb_,
         6, 11, 25, e_, s1_);
   compute_ch_ = MUX_Gadget::create(pb_,
         e_, f_, g_, ch_);
   compute_maj_ = MAJ_Gadget::create(pb_,
         a_, b_, c_, maj_);
   compute_add_ = ADD_Gadget::create(pb_,
         h_, s1_, ch_, curr_W_, curr_K_, add_);
   compute_new_e_ = ADD_Gadget::create(pb_,
         d_, add_, new_e_);
   compute_new_a_ = ADD_Gadget::create(pb_,
         s0_, maj_, add_, new_a_);
}

void RoundFunc_Gadget::generateConstraints() {
   compute_s0_->generateConstraints();
   compute_s1_->generateConstraints();
   compute_ch_->generateConstraints();
   compute_maj_->generateConstraints();
   compute_add_->generateConstraints();
   compute_new_e_->generateConstraints();
   compute_new_a_->generateConstraints();
}

void RoundFunc_Gadget::generateWitness() {
   compute_s0_->generateWitness();
   compute_s1_->generateWitness();
   compute_ch_->generateWitness();
   compute_maj_->generateWitness();
   compute_add_->generateWitness();
   compute_new_e_->generateWitness();
   compute_new_a_->generateWitness();
}

GadgetPtr RoundFunc_Gadget::create(ProtoboardPtr pb,
   const VariableArray& a, const VariableArray& b,
   const VariableArray& c, const VariableArray& d,
   const VariableArray& e, const VariableArray& f,
   const VariableArray& g, const VariableArray& h,
   const VariableArray& curr_W, const unsigned long curr_K,
   const VariableArray& new_a, const VariableArray& new_e) {
   addGadgetToTraceback("RoundFunc_Gadget");
   GadgetPtr pGadget(new RoundFunc_Gadget(pb,
      a, b, c, d, e, f, g, h,
      curr_W, curr_K, new_a, new_e));
   pGadget->init();
   removeGadgetFromTraceback("RoundFunc_Gadget");
   return pGadget;
}

CompressFunc_Gadget::CompressFunc_Gadget(ProtoboardPtr pb,
   const ::std::vector<VariableArray>& prev,
   const ::std::vector<VariableArray>& inp,
   const ::std::vector<VariableArray>& out)
   : Gadget(pb), prev_(prev), inp_(inp), out_(out) {}

void CompressFunc_Gadget::init() {
   ::std::vector<VariableArray> W;
   for(int i=0; i<16 && i<SHA2RND; ++i)
      W.push_back(inp_[i]);
   for(int i=0; i<SHA2RND-16; ++i)
      W.push_back(VariableArray(32));
   compute_msgsched_ = MsgSchedule_Gadget::create(pb_, W);
      
   round_a_.push_back(prev_[0]);
   round_b_.push_back(prev_[1]);
   round_c_.push_back(prev_[2]);
   round_d_.push_back(prev_[3]);
   round_e_.push_back(prev_[4]);
   round_f_.push_back(prev_[5]);
   round_g_.push_back(prev_[6]);
   round_h_.push_back(prev_[7]);

   for (size_t i=0; i<SHA2RND; ++i) {
      round_h_.push_back(round_g_[i]);
      round_g_.push_back(round_f_[i]);
      round_f_.push_back(round_e_[i]);
      round_d_.push_back(round_c_[i]);
      round_c_.push_back(round_b_[i]);
      round_b_.push_back(round_a_[i]);
      round_a_.push_back(VariableArray(32));
      round_e_.push_back(VariableArray(32));
      round_funcs_.push_back(RoundFunc_Gadget::create(pb_,
         round_a_[i], round_b_[i], round_c_[i], round_d_[i],
         round_e_[i], round_f_[i], round_g_[i], round_h_[i],
         W[i], SHA256_K[i], round_a_[i+1], round_e_[i+1]));
   }

   add_funcs_.push_back(ADD_Gadget::create(pb_, round_a_[0], round_a_[SHA2RND], out_[0]));
   add_funcs_.push_back(ADD_Gadget::create(pb_, round_b_[0], round_b_[SHA2RND], out_[1]));
   add_funcs_.push_back(ADD_Gadget::create(pb_, round_c_[0], round_c_[SHA2RND], out_[2]));
   add_funcs_.push_back(ADD_Gadget::create(pb_, round_d_[0], round_d_[SHA2RND], out_[3]));
   add_funcs_.push_back(ADD_Gadget::create(pb_, round_e_[0], round_e_[SHA2RND], out_[4]));
   add_funcs_.push_back(ADD_Gadget::create(pb_, round_f_[0], round_f_[SHA2RND], out_[5]));
   add_funcs_.push_back(ADD_Gadget::create(pb_, round_g_[0], round_g_[SHA2RND], out_[6]));
   add_funcs_.push_back(ADD_Gadget::create(pb_, round_h_[0], round_h_[SHA2RND], out_[7]));
}

void CompressFunc_Gadget::generateConstraints() {
   compute_msgsched_->generateConstraints();
   for (size_t i=0; i<SHA2RND; ++i)
      round_funcs_[i]->generateConstraints();
   for (size_t i=0; i<8; ++i)
      add_funcs_[i]->generateConstraints();
}

void CompressFunc_Gadget::generateWitness() {
   compute_msgsched_->generateWitness();
   for (size_t i=0; i<SHA2RND; ++i)
      round_funcs_[i]->generateWitness();
   for (size_t i=0; i<8; ++i)
      add_funcs_[i]->generateWitness();
}

GadgetPtr CompressFunc_Gadget::create(ProtoboardPtr pb,
   const ::std::vector<VariableArray>& prev,
   const ::std::vector<VariableArray>& inp,
   const ::std::vector<VariableArray>& out) {
   addGadgetToTraceback("CompressFunc_Gadget");
   GadgetPtr pGadget(new CompressFunc_Gadget(pb,
      prev, inp, out));
   pGadget->init();
   removeGadgetFromTraceback("CompressFunc_Gadget");
   return pGadget;
}

#ifdef TOY_SHA2_INCLUDE

ToySHA2_Gadget::ToySHA2_Gadget(ProtoboardPtr pb,
   const VariableArray& prev,
   const VariableArray& inp,
   const VariableArray& out)
   : Gadget(pb), prev_(prev), inp_(inp), out_(out) {}

void ToySHA2_Gadget::init() {
   VariableArray word_prev_;
   VariableArray word_inp_;
   std::vector<VariableArray> expand_prev_;
   std::vector<VariableArray> expand_inp_;
   std::vector<VariableArray> expand_out_;

   if (prev_.size() < 32) {
      word_prev_ = VariableArray(32);
      for(int i=0; i<32; ++i)
         word_prev_[i] = prev_[i % prev_.size()];
   }
   else
      word_prev_ = prev_;
   for(int i=0; i<8; ++i)
      expand_prev_.push_back(word_prev_);

   if (inp_.size() < 32) {
      word_inp_ = VariableArray(32);
      for(int i=0; i<32; ++i)
         word_inp_[i] = inp_[i % inp_.size()];
   }
   else
      word_inp_ = inp_;
   for(int i=0; i<16 && i<SHA2RND; ++i)
      expand_inp_.push_back(word_inp_);

   if (out_.size() < 32) {
      VariableArray word_out_(32);
      for(int i=0; i<out_.size(); ++i)
         word_out_[i] = out_[i];
      expand_out_.push_back(word_out_);
   }
   else
      expand_out_.push_back(out_);
   for(int i=0; i<8-1; ++i)
      expand_out_.push_back(VariableArray(32));

   sha256_compress_ = CompressFunc_Gadget::create(pb_,
      expand_prev_, expand_inp_, expand_out_);
}

void ToySHA2_Gadget::generateConstraints() {
   sha256_compress_->generateConstraints();
}

void ToySHA2_Gadget::generateWitness() {
   sha256_compress_->generateWitness();
}

GadgetPtr ToySHA2_Gadget::create(ProtoboardPtr pb,
   const VariableArray& prev,
   const VariableArray& inp,
   const VariableArray& out) {
   addGadgetToTraceback("ToySHA2_Gadget");
   GadgetPtr pGadget(new ToySHA2_Gadget(pb,
      prev, inp, out));
   pGadget->init();
   removeGadgetFromTraceback("ToySHA2_Gadget");
   return pGadget;
}

#endif //of TOY_SHA2_INCLUDE

BIGSIGMA_Gadget::BIGSIGMA_Gadget(ProtoboardPtr pb,
   const size_t rot1, const size_t rot2, const size_t rot3,
   const VariableArray& input, const VariableArray& result)
   : Gadget(pb), input_(input), result_(result) {
   for(int i=0; i<result_.size(); ++i)
      xorGadgets_.push_back(XOR_Gadget::create(pb_, 
         ROTR(input_,i,rot1), ROTR(input_,i,rot2), ROTR(input_,i,rot3),
         result_[i]));
}

void BIGSIGMA_Gadget::init() {}

void BIGSIGMA_Gadget::generateConstraints() {
   for (auto& curGadget : xorGadgets_)
      curGadget->generateConstraints();
}

void BIGSIGMA_Gadget::generateWitness() {
   for (auto& curGadget : xorGadgets_)
      curGadget->generateWitness();
}

GadgetPtr BIGSIGMA_Gadget::create(ProtoboardPtr pb,
   const size_t rot1, const size_t rot2, const size_t rot3,
   const VariableArray& input, const VariableArray& result) {
	addGadgetToTraceback("BIGSIGMA_Gadget");
   GadgetPtr pGadget(new BIGSIGMA_Gadget(pb,
      rot1, rot2, rot3, input, result));
   pGadget->init();
	removeGadgetFromTraceback("BIGSIGMA_Gadget");
   return pGadget;
}

SMALLSIGMA_Gadget::SMALLSIGMA_Gadget(ProtoboardPtr pb,
   const size_t rot1, const size_t rot2, const size_t shift,
   const VariableArray& input, const VariableArray& result)
   : Gadget(pb), input_(input), result_(result) {
   for(int i=0; i<result_.size(); ++i) {
      int t = i+shift;
      if(t < result_.size())
         xorGadgets_.push_back(XOR_Gadget::create(pb_, 
            ROTR(input_,i,rot1), ROTR(input_,i,rot2), input_[t], result_[i]));
      else
         xorGadgets_.push_back(XOR_Gadget::create(pb_, 
            ROTR(input_,i,rot1), ROTR(input_,i,rot2), result_[i]));
   }
}

void SMALLSIGMA_Gadget::init() {}

void SMALLSIGMA_Gadget::generateConstraints() {
   for (auto& curGadget : xorGadgets_)
      curGadget->generateConstraints();
}

void SMALLSIGMA_Gadget::generateWitness() {
   for (auto& curGadget : xorGadgets_)
      curGadget->generateWitness();
}

GadgetPtr SMALLSIGMA_Gadget::create(ProtoboardPtr pb,
   const size_t rot1, const size_t rot2, const size_t shift,
   const VariableArray& input, const VariableArray& result) {
	addGadgetToTraceback("SMALLSIGMA_Gadget");
   GadgetPtr pGadget(new SMALLSIGMA_Gadget(pb,
      rot1, rot2, shift, input, result));
   pGadget->init();
	removeGadgetFromTraceback("SMALLSIGMA_Gadget");
   return pGadget;
}

} // namespace sha2
} // namespace gadgetlib
