/** @file
 *****************************************************************************
 SHA2-specific gadgets.

 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef LIBSNARK_SHA2_INCLUDE_SHA2_GADGET_HPP_
#define LIBSNARK_SHA2_INCLUDE_SHA2_GADGET_HPP_

#include <gadgetlib/gadget.hpp>
#define SHA2RND 64
#define TOY_SHA2_INCLUDE

namespace gadgetlib {
namespace sha2 {

void generate_SHA256_IV(ProtoboardPtr pb, ::std::vector<VariableArray>& IV);

class MsgSchedule_Gadget : public Gadget {
private:
   MsgSchedule_Gadget(ProtoboardPtr pb, const ::std::vector<VariableArray>& W);
   virtual void init();
   const ::std::vector<VariableArray> W_;
   //const ::std::vector<VariableArray>& W_;
   ::std::vector<GadgetPtr> ssigma0_;
   ::std::vector<GadgetPtr> ssigma1_;
   ::std::vector<GadgetPtr> add4_;
   ::std::vector< ::std::shared_ptr<VariableArray> > result0_;
   ::std::vector< ::std::shared_ptr<VariableArray> > result1_;
   DISALLOW_COPY_AND_ASSIGN(MsgSchedule_Gadget);
public:
   static GadgetPtr create(ProtoboardPtr pb, const ::std::vector<VariableArray>& W);
   void generateConstraints();
   void generateWitness();
}; // class MsgSchedule_Gadget 

class RoundFunc_Gadget : public Gadget {
private:
   RoundFunc_Gadget(ProtoboardPtr pb,
      const VariableArray& a, const VariableArray& b,
      const VariableArray& c, const VariableArray& d,
      const VariableArray& e, const VariableArray& f,
      const VariableArray& g, const VariableArray& h,
      const VariableArray& curr_W, const unsigned long curr_K,
      const VariableArray& new_a, const VariableArray& new_e);
   virtual void init();
   const VariableArray a_, b_, c_, d_, e_, f_, g_, h_;
   const unsigned long curr_K_;
   const VariableArray curr_W_, new_a_, new_e_;
   const VariableArray s0_, s1_, ch_, maj_, add_;
   GadgetPtr compute_s0_, compute_s1_;
   GadgetPtr compute_ch_, compute_maj_, compute_add_;
   GadgetPtr compute_new_a_, compute_new_e_;
   DISALLOW_COPY_AND_ASSIGN(RoundFunc_Gadget);
public:
   static GadgetPtr create(ProtoboardPtr pb,
      const VariableArray& a, const VariableArray& b,
      const VariableArray& c, const VariableArray& d,
      const VariableArray& e, const VariableArray& f,
      const VariableArray& g, const VariableArray& h,
      const VariableArray& curr_W, const unsigned long curr_K,
      const VariableArray& new_a, const VariableArray& new_e);
   void generateConstraints();
   void generateWitness();
}; // class RoundFunc_Gadget

class CompressFunc_Gadget : public Gadget {
private:
   CompressFunc_Gadget(ProtoboardPtr pb,
      const ::std::vector<VariableArray>& prev,
      const ::std::vector<VariableArray>& inp,
      const ::std::vector<VariableArray>& out);
   virtual void init();
   const ::std::vector<VariableArray> prev_;
   const ::std::vector<VariableArray> inp_;
   const ::std::vector<VariableArray> out_;
   ::std::vector<VariableArray> round_a_;
   ::std::vector<VariableArray> round_b_;
   ::std::vector<VariableArray> round_c_;
   ::std::vector<VariableArray> round_d_;
   ::std::vector<VariableArray> round_e_;
   ::std::vector<VariableArray> round_f_;
   ::std::vector<VariableArray> round_g_;
   ::std::vector<VariableArray> round_h_;
   GadgetPtr compute_msgsched_;
   ::std::vector<GadgetPtr> round_funcs_;
   ::std::vector<GadgetPtr> add_funcs_;
   DISALLOW_COPY_AND_ASSIGN(CompressFunc_Gadget);
public:
   static GadgetPtr create(ProtoboardPtr pb,
      const ::std::vector<VariableArray>& prev,
      const ::std::vector<VariableArray>& inp,
      const ::std::vector<VariableArray>& out);
   void generateConstraints();
   void generateWitness();
};

#ifdef TOY_SHA2_INCLUDE

class ToySHA2_Gadget : public Gadget {
private:
   ToySHA2_Gadget(ProtoboardPtr pb,
      const VariableArray& prev,
      const VariableArray& inp,
      const VariableArray& out);
   virtual void init();
   GadgetPtr sha256_compress_;
   const VariableArray prev_;
   const VariableArray inp_;
   const VariableArray out_;
   DISALLOW_COPY_AND_ASSIGN(ToySHA2_Gadget);
public:
   static GadgetPtr create(ProtoboardPtr pb,
      const VariableArray& prev,
      const VariableArray& inp,
      const VariableArray& out);
   void generateConstraints();
   void generateWitness();
};

#endif //of TOY_SHA2_INCLUDE

class BIGSIGMA_Gadget : public Gadget {
private:
   BIGSIGMA_Gadget(ProtoboardPtr pb,
      const size_t rot1,
      const size_t rot2,
      const size_t rot3,
      const VariableArray& input,
      const VariableArray& result);
   virtual void init();
   const VariableArray input_;
   const VariableArray result_;
   ::std::vector<GadgetPtr> xorGadgets_;
   DISALLOW_COPY_AND_ASSIGN(BIGSIGMA_Gadget);
public:
   static GadgetPtr create(ProtoboardPtr pb,
      const size_t rot1, const size_t rot2, const size_t rot3,
      const VariableArray& input, const VariableArray& result);
   void generateConstraints();
   void generateWitness();
}; // class BIGSIGMA_Gadget 

class SMALLSIGMA_Gadget : public Gadget {
private:
   SMALLSIGMA_Gadget(ProtoboardPtr pb,
      const size_t rot1,
      const size_t rot2,
      const size_t shift,
      const VariableArray& input,
      const VariableArray& result);
   virtual void init();
   const VariableArray input_;
   const VariableArray result_;
   ::std::vector<GadgetPtr> xorGadgets_;
   DISALLOW_COPY_AND_ASSIGN(SMALLSIGMA_Gadget);
public:
   static GadgetPtr create(ProtoboardPtr pb,
      const size_t rot1, const size_t rot2, const size_t shift,
      const VariableArray& input, const VariableArray& result);
   void generateConstraints();
   void generateWitness();
}; // class SMALLSIGMA_Gadget 

} // namespace sha2
} // namespace gadgetlib

#endif // LIBSNARK_SHA2_INCLUDE_SHA2_GADGET_HPP_
