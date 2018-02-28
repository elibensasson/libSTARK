# libSTARK: a C++ library for zk-STARK systems

## Overview

The library implements a scalable and transparent argument of knowledge (STARK) system, with or without zero knowledge (ZK), that may be interactive or non-interactive, as described in the [zk-STARK paper](https://eprint.iacr.org/2018/046). The main properties of (zk)-STARK systems are: 
+ **universality**: the system can be applied to any computation specified by an algebraic internal representation (AIR) or by a permuted algebraic intermediate representatin (PAIR) as defined in the [zkSTARK paper](https://eprint.iacr.org/2018/046). The former (AIR) is relevant to "memory efficient" computations and the latter (PAIR) for computaitons that require random access memory (RAM). 
+ **transparency**: all messages sent by the verifier, including queries to the proof oracles, are public random coins (i.e., the protocol is "Arthur-Merlin"); in particular.
+ **scalability**: as an asymptotic function of the number of cycles (T) required by the computation whose integrity is being proved, both of the following conditions hold:
  + **prover scalability**: prover running time scales _quasi-linearly_ in T, i.e., like T poly log T
  + **verifier scalability**: verifier running time scales _poly-logarithmically_ in T, i.e., like poly log T.
+ **"post-quantum security"**: the cryptogrpahic primitives that underlie the security of the system are either the existence of a family of collision resistant hash functions (for an interactive system) or common access to a random function (the "random oracle" model for a non-interactive setting); in particular, quantum computers are not known to break system security at time of writing.

## How to run the code
