#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

#include "TrieBuilder.h"
#include "IdentityTransformer.h"
#include "ToEmptyStringTransformer.h"
#include "LogSequenceWeightTable.h"
#include "LexicalStreamTransformer.h"

#define BOUNDARY  "||"
#define MARK      "<M>"
#define MARK1     "<KNOWN_PREFIX_AND_SUFFIX>"
#define MARK2     "<UNKNOWN_PREFIX>"
#define MARK3     "<UNKNOWN_SUFFIX>"
#define MARK4     "<UNKNOWN_PREFIX_AND_SUFFIX>"

using hfst::internal_identity;
using hfst::internal_epsilon;
using hfst::TROPICAL_OPENFST_TYPE;
using hfst::HfstOutputStream;

int main(int argc, char * argv[])
{
  if (argc != 4)
    {
      std::cerr << "Usage: " << argv[0] 
		<< " prefix_file suffix_file output_file"
		<< std::endl;
      exit(1);
    }

  std::ifstream prefix_in(argv[1]);
  StreamTokenizer prefix_tok_in(prefix_in);

  std::ifstream suffix_in(argv[2]);
  StreamTokenizer suffix_tok_in(suffix_in);

  IdentityTransformer id;
  ToEmptyStringTransformer to_empty;

  std::cerr << "Building prefix transducer." << std::endl;
  LogSequenceWeightTable prefix_weight_table(id,
					     to_empty,
					     prefix_tok_in.get_stream());

  TrieBuilder prefix_trie_builder(prefix_weight_table);
  HfstTransducer prefix_fst = prefix_trie_builder.get_model().minimize();

  HfstTransducer optional_prefix_fst(prefix_fst);
  optional_prefix_fst.optionalize();

  std::cerr << "Got penalty weight " 
	    << prefix_weight_table.get_penalty_weight() << std::endl;

  std::cerr << "Building suffix transducer." << std::endl;
  LogSequenceWeightTable suffix_weight_table(id,
					     to_empty,
					     suffix_tok_in.get_stream());

  TrieBuilder suffix_trie_builder(suffix_weight_table);
  HfstTransducer suffix_fst = suffix_trie_builder.get_model().minimize();

  HfstTransducer optional_suffix_fst(prefix_fst);
  optional_suffix_fst.optionalize();

  std::cerr << "Got penalty weight " 
	    << suffix_weight_table.get_penalty_weight() << std::endl;

  std::cerr << "Building model." << std::endl;
  HfstTransducer mark(internal_epsilon,MARK,TROPICAL_OPENFST_TYPE);

  HfstTransducer universal_prefix(internal_identity,TROPICAL_OPENFST_TYPE);
  universal_prefix.set_final_weights(prefix_weight_table.get_penalty_weight());
  universal_prefix.repeat_star();
  HfstTransducer marked_universal_prefix(mark);
  marked_universal_prefix.concatenate(universal_prefix).minimize();
  
  HfstTransducer universal_suffix(internal_identity,TROPICAL_OPENFST_TYPE);
  universal_suffix.set_final_weights(suffix_weight_table.get_penalty_weight());
  universal_suffix.repeat_star();
  HfstTransducer marked_universal_suffix(mark);
  marked_universal_suffix.concatenate(universal_suffix).minimize();

  
  HfstTransducer known_prefix_and_suffix
    (internal_epsilon,MARK1,TROPICAL_OPENFST_TYPE);
  known_prefix_and_suffix
    .concatenate(optional_prefix_fst)
    .concatenate(mark)
    .concatenate(optional_suffix_fst)
    .minimize();

  HfstTransducer unknown_prefix
    (internal_epsilon,MARK2,TROPICAL_OPENFST_TYPE);
  unknown_prefix
    .concatenate(universal_prefix)
    .concatenate(mark)
    .concatenate(suffix_fst)
    .minimize();

  HfstTransducer unknown_suffix
    (internal_epsilon,MARK3,TROPICAL_OPENFST_TYPE);
  unknown_suffix
    .concatenate(prefix_fst)
    .concatenate(mark)
    .concatenate(universal_suffix)
    .minimize();

  HfstTransducer unknown_prefix_and_suffix
    (internal_epsilon,MARK4,TROPICAL_OPENFST_TYPE);
  unknown_prefix_and_suffix
    .concatenate(universal_prefix)
    .concatenate(mark)
    .concatenate(universal_suffix)
    .minimize();

  
  HfstTransducer boundary_fst(BOUNDARY,TROPICAL_OPENFST_TYPE);

  HfstTransducer model_center(known_prefix_and_suffix);
  model_center
    .disjunct(unknown_prefix)
    .disjunct(unknown_suffix)
    .disjunct(unknown_prefix_and_suffix)
    .minimize();

  HfstTransducer model(boundary_fst);
  model
    .concatenate(boundary_fst)
    .concatenate(model_center)
    .concatenate(boundary_fst)
    .concatenate(boundary_fst)
    .minimize();

  //model.input_project();

  HfstOutputStream fst_out(argv[3],TROPICAL_OPENFST_TYPE);
  fst_out << model;
}
