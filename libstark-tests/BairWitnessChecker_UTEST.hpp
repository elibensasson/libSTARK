#ifndef Bair_UTEST_HPP
#define Bair_UTEST_HPP

#include <languages/Bair/BairInstance.hpp>
#include <languages/Bair/BairWitness.hpp>

namespace PCP_UTESTS{

/***************************************************
 *
 * This generates a valid Bair pair,
 * with no constraints and no permutations.
 * the only relevant test here is boundary constraints.
 *
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_valid_boundary();

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with no constraints and no permutations.
 * the only relevant test here is boundary constraints.
 *
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_invalid_boundary();

/***************************************************
 *
 * This generates a valid Bair pair,
 * with some empty constraints and valid permutations.
 * The boundary is empty.
 * The coloring is random.
 * 
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_valid_permutations();

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with some empty constraints and valid permutations,
 * except of one, which is not a permutation.
 * The boundary is empty.
 * The coloring is random.
 * 
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_invalid_permutations();

/***************************************************
 *
 * This generates a valid Bair pair,
 * with some constraints and valid permutations.
 * The boundary is empty.
 * The coloring is random.
 * 
 * Generation method:
 * generates all parameters but the
 * constraint systems.
 * And defines each constraint system to be satisfied
 * exactly from the parameters given.
 * 
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_valid_constraints();

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with constraints and valid permutations.
 * The boundary is empty.
 * The coloring is random.
 * 
 * Generation method:
 * generates all parameters but the
 * constraint systems.
 * And defines each constraint system to be satisfied
 * exactly from the parameters given,
 * except of one victim constraints system that is
 * chosen randomly, one victim index 'i',
 * such that the test of that index 
 * (color(i),color(perm(i))) will fail.
 *
 * In this case the victim is constraintsAssignment
 * 
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_invalid_constraints_Assignment();

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with constraints and valid permutations.
 * The boundary is empty.
 * The coloring is random.
 * 
 * Generation method:
 * generates all parameters but the
 * constraint systems.
 * And defines each constraint system to be satisfied
 * exactly from the parameters given,
 * except of one victim constraints system that is
 * chosen randomly, one victim index 'i',
 * such that the test of that index 
 * (color(i),color(perm(i))) will fail.
 *
 * In this case the victim is constraintsPermutation
 * 
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_invalid_constraints_Permutation();

/***************************************************
 *
 * This generates an invalid Bair pair,
 * with constraints and valid permutations.
 * The boundary is empty.
 * The coloring is random.
 * 
 * Generation method:
 * generates all parameters but the
 * constraint systems.
 * And defines each constraint system to be satisfied
 * exactly from the parameters given,
 * except of one victim constraints system that is
 * chosen randomly, one victim index 'i',
 * such that the test of that index 
 * (color(i),color(perm(i))) will fail.
 *
 * In this case the victims are both constraintsAssignment & constraintsPermutation
 * 
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_invalid_constraints_both();

/***************************************************
 *
 * This generates a Bair pair
 * from EGCP pair that is reduced from TinyRAM
 * Should be a valid pair
 * 
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> reduceFromTinyRAM(const int t);

/***************************************************
 *
 * This generates some valid Bair pair
 * 
 ***************************************************/
std::pair<libstark::BairInstance,libstark::BairWitness> generate_valid_pair();

} //PCP_UTEST namespace

#endif // Bair_UTEST_HPP
