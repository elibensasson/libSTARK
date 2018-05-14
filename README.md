# libSTARK: a C++ library for zk-STARK systems

## Overview

The libSTARK library implements _scalable_ and _transparent argument of knowledge_ (STARK) systems. These systems can be executed with, or without, _zero knowledge_ (ZK), and may be designed as either _interactive_ or _non-interactive_ protocols. The theoretical constructions which this library implements are described in detail in the [zk-STARK paper](https://eprint.iacr.org/2018/046). Briefly, the main properties of (zk)-STARK systems are: 
+ **universality**: the system can be applied to any computation specified by an algebraic intermediate representation (AIR) or by a permuted algebraic intermediate representation (PAIR) as defined in the [zkSTARK paper](https://eprint.iacr.org/2018/046). The former (AIR) is relevant to "memory efficient" computations and the latter (PAIR) for computations that require random access memory (RAM).
+ **transparency**: all messages sent by the verifier, including queries to the proof oracles, are public random coins (i.e., the protocol is "Arthur-Merlin").
+ **scalability**: as an asymptotic function of the number of cycles (T) required by the computation whose integrity is being proved, both of the following conditions hold:
  + **prover scalability**: prover running time scales _quasi-linearly_ in T, i.e., like T poly log T.
  + **verifier scalability**: verifier running time scales _poly-logarithmically_ in T, i.e., like poly log T.
+ **"post-quantum security"**: the cryptographic primitives that underlie the security of the system are either the existence of a family of collision resistant hash functions (for an interactive system) or common access to a random function (the "random oracle" model for a non-interactive setting); in particular, quantum computers are not known to break system security at time of writing.

## Disclaimer

The code is **academic grade**, meant for academic peer review and evaluation. It very likely contains _multiple serious security flaws_, and should not be relied upon for any other purpose.

## Dependencies
### Hardware acceleration
* sse4 (https://en.wikipedia.org/wiki/SSE4)
  * CLMUL (https://en.wikipedia.org/wiki/CLMUL_instruction_set) for basic arithmetics in characteristic 2 fields.
  * SIMD (https://en.wikipedia.org/wiki/SIMD) instructions for FFT acceleration.
* aes (https://en.wikipedia.org/wiki/AES_instruction_set) to accelerate Merkle tree construction and verification.

### Compiler
The code was tested with gcc version 7.3.0 (https://gcc.gnu.org), using c++11 standard. But should probably work with most common versions of gcc.
#### Compilation on macOS
In order to compile on macOS please use this thread:
https://github.com/elibensasson/libSTARK/issues/2

### Libraries (all should be available in standard Linux distribution package managers)
* OpenMP (https://en.wikipedia.org/wiki/OpenMP) for parallelization.
* googletest (https://github.com/google/googletest) for **unit-tests compilation only**

## How to run the code
### Compilation:
```
git clone https://github.com/elibensasson/libSTARK.git
cd libSTARK
make -j8
```

### STARK for DPM (DNA fingerprint blacklist)
Arguments format:
```
./stark-dpm <database file path> <fingerprint file path> [-s<security parameter>]
```
for example:
```
./stark-dpm examples-dpm/database.txt examples-dpm/fp_no_match.txt
```
The above execution results in execution of STARK simulation over the DPM blacklist program, with the database represented by examples-dpm/database.txt, the suspect's fingerprint in examples-dpm/fp_nomatch.txt, and soundness error at most 2<sup>-120</sup>. The prover generates in this case a proof for the claim that the fingerprint does not perfectly match any entry in the database.

A single fingerprint is represented by a single line, each line contains 20 pairs delimited by spaces, each pair contains two 8 bit numbers in hexadecimal basis, separated by a single period. A database is a file where each line represents a fingerprint.

### STARK for TinyRAM programs
Arguments format:
```
./stark-tinyram <TinyRAM assembly file path> -t<trace length log_2> [-s<security parameter]>
```
for example:
```
./stark-tinyram examples-tinyram/collatz.asm -t10 -s120
```
The above execution results in execution of STARK simulation over the collatz program, using at most 1023 (which is 2<sup>10</sup>-1) machine steps, and soundness error at most 2<sup>-120</sup>.

### Execution results
In the simulation the Prover and Verifier interact, the Prover generates a proof and the Verifier verifies it.
During the executions the specifications of generated BAIR and APR, measurements, and Verifier's decision, are printed to the standard output.

### Unit-tests
Compilation:
```
make -j8 tests
```
Execution:
```
./bin/algebralib-tests/algebralib_tests
./bin/libstark-tests/libstark-tests
./bin/stark-tinyram-tests/stark-tinyram-tests
```

## Academic literature (partial list, reverse chronological order)

1. Scalable perfect zero knowledge IOP systems [[BCGV](https://link.springer.com/chapter/10.1007%2F978-3-662-49099-0_2), [BCFGRS](https://link.springer.com/chapter/10.1007%2F978-3-319-70503-3_6)].
1. A STARK without ZK [[SCI](https://link.springer.com/chapter/10.1007%2F978-3-319-56617-7_19)].
1. Survey of alternative (non-STARK) proof systems [[WB](https://dl.acm.org/citation.cfm?doid=2641562)].
1. Interactive Oracle Proofs [[BCS](https://link.springer.com/chapter/10.1007%2F978-3-662-53644-5_2), [RRR](https://dl.acm.org/citation.cfm?doid=2897518.2897652)].
1. PCPs with scalable (quasi-linear) proof size [[BS](http://epubs.siam.org/doi/10.1137/050646445), [D](https://dl.acm.org/citation.cfm?doid=1236457.1236459)].
1. ZK-PCPs [[K](https://dl.acm.org/citation.cfm?doid=129712.129782), [KPT](https://dl.acm.org/citation.cfm?doid=258533.258643), [IMS](https://link.springer.com/chapter/10.1007%2F978-3-642-28914-9_9)].
1. Probabilistically checkable proofs (PCPs) [[AS](https://dl.acm.org/citation.cfm?doid=273865.273901), [ALMSS](https://dl.acm.org/citation.cfm?doid=278298.278306)] and PCPs of Proximity (PCPPs) [[BGHSV](http://epubs.siam.org/doi/10.1137/S0097539705446810), [DR](http://epubs.siam.org/doi/10.1137/S0097539705446962)].
1. Scalable (poly-logarithmic) verification of computations [[BFLS](https://dl.acm.org/citation.cfm?doid=103418.103428), 
[BGHSV](http://ieeexplore.ieee.org/document/1443079/)].
1. Interactive and ZK proof systems [[BM](https://www.sciencedirect.com/science/article/pii/0022000088900281?via%3Dihub), [GMR](https://dl.acm.org/citation.cfm?doid=22145.22178), [BFL](https://link.springer.com/article/10.1007%2FBF01200056)].
