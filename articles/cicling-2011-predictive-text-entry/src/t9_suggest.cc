#include <iostream>
#include <cstdlib>
#include <cstdio>

#include "Sequencer.h"
#include "HMMTagger.h"
#include "SequenceModel.h"
#include "SuggestionsInContext.h"

#define MAXBUFFER 500000

using hfst::TROPICAL_OPENFST_TYPE;
using hfst::HfstTwoLevelPaths;
using hfst::HfstTwoLevelPath;
using hfst::StringPairVector;
using hfst::internal_epsilon;
using hfst::SFST_TYPE;
#include <ctime>

HfstTokenizer tokenizer;

class CodingFailed
{};

using hfst::implementations::ComposeIntersectLexicon;
using hfst::implementations::ComposeIntersectRule;

float multiply_weights(float f)
{ return 0.7*f; }

StringVector split_at_spaces
(const std::string &l)
{
  std::string symbol = " ";

  size_t last_tab_pos = std::string::npos;
  size_t tab_pos = std::string::npos;
  
  StringVector result;

  while ((tab_pos = l.find(symbol,last_tab_pos + 1)) != std::string::npos)
    {
      std::string token = l.substr(last_tab_pos + 1, 
				   tab_pos - last_tab_pos - 1);
      if (token.size() != 0)
	{ result.push_back(token); }
      last_tab_pos = tab_pos;
    }

  if (last_tab_pos + 1 != l.size())
    { result.push_back(l.substr(last_tab_pos + 1)); }

  return result;
}

StringVector pad_and_code_to_numbers
(const std::string &input, HfstTransducer &coder)
{
  HfstTransducer input_fst(input,tokenizer,TROPICAL_OPENFST_TYPE);
  HfstTwoLevelPaths output_paths;
  input_fst.compose(coder).output_project().extract_paths(output_paths);

  if (output_paths.size() != 1)
    { throw CodingFailed(); }

  StringPairVector v = output_paths.begin()->second;
  StringVector result;
  result.push_back("||");
  result.push_back("||");
  for (StringPairVector::const_iterator it = v.begin();
       it != v.end();
       ++it)
    { result.push_back(it->first); }
  result.push_back("||");
  result.push_back("||");
  return result;
}

bool is_complex_symbol(const std::string &symbol)
{
  bool val =
    symbol != "<L>" and
    symbol.size() > 0 and 
    symbol[0] == '<' and 
    symbol[symbol.size() - 1] == '>'; 

  return val;
}

void add_complex_symbol(HfstBasicTransducer &basic_output_copy,
			const std::string &complex_symbol,
			float weight,
			HfstState source, HfstState target)
{
  StringVector tokenized_symbol = tokenizer.tokenize_one_level(complex_symbol);

  HfstState temp_target;
  for (StringVector::const_iterator it = tokenized_symbol.begin() + 1;
       it != tokenized_symbol.end() - 1;
       ++it)
    {
      temp_target = basic_output_copy.add_state();

      HfstBasicTransition transition(temp_target,*it,*it,0.0);
      basic_output_copy.add_transition(source,transition);

      source = temp_target;
    }
  
  HfstBasicTransition transition(target,internal_epsilon,internal_epsilon,weight);
  basic_output_copy.add_transition(source,transition);
}

HfstTransducer &restore_letters(HfstTransducer &output)
{
  HfstBasicTransducer basic_output(output);
  HfstBasicTransducer basic_output_copy;
  (void)basic_output_copy.add_state(basic_output.get_max_state());

  HfstState s = 0;

  for (HfstBasicTransducer::const_iterator it = basic_output.begin();
       it != basic_output.end();
       ++it)
    {
      const HfstBasicTransducer::HfstTransitions &transitions = *it;
      for (HfstBasicTransducer::HfstTransitions::const_iterator jt = 
	     transitions.begin();
	   jt != transitions.end();
	   ++jt)
	{
	  if (is_complex_symbol(jt->get_input_symbol()))
	    {
	      add_complex_symbol(basic_output_copy,jt->get_input_symbol(),
				 jt->get_weight(),s,jt->get_target_state());
	    }
	  else
	    {
	      HfstBasicTransition transition
		(jt->get_target_state(),
		 jt->get_input_symbol(),
		 jt->get_output_symbol(),
		 jt->get_weight());
	      
	      basic_output_copy.add_transition(s,transition);
	    }
	}

      if (basic_output.is_final_state(s))
	{ basic_output_copy.set_final_weight(s,basic_output.get_final_weight(s)); }

      ++s;
    }
  output = HfstTransducer(basic_output_copy,TROPICAL_OPENFST_TYPE);
  output.minimize();
  return output;
}

using hfst::HfstTransducerVector;

int main(int argc, char * argv[])
{
  if (argc != 7)
    {
      std::cerr << "Usage: cat input_1_word_/_line | " << argv[0] 
		<< " letter_n_gram_lexical_model letter_n_gram_sequence_model"
		<< " sequencer_model morpheme_concatenation_model "
		<< "txt_to_nr_fst vowel_harmony_rules "
		<< std::endl;
      exit(1);
    }

  //clock_t START_LOAD = clock();

  HMMTagger letter_n_gram_model(argv[1],argv[2]);

  Sequencer sequencer(argv[3]);

  SequenceModel morpheme_sequence_model(argv[4]);

  HfstInputStream coder_stream(argv[5]);
  HfstTransducer coder(coder_stream);

  HfstTransducerVector harmony_rule_vector;
  HfstInputStream harmony_in(argv[6]);
  while (harmony_in.is_good())
    { harmony_rule_vector.push_back(HfstTransducer(harmony_in)); }
  
  assert(harmony_rule_vector.size() > 1);

  // We should delete these rules, when the program exits, but it doesn't
  // really make any difference in this case.
  hfst::implementations::ComposeIntersectRule * first_rule = 
    new hfst::implementations::ComposeIntersectRule(harmony_rule_vector[0]);
  hfst::implementations::ComposeIntersectRule * second_rule = 
    new hfst::implementations::ComposeIntersectRule(harmony_rule_vector[1]);

  hfst::implementations::ComposeIntersectRulePair * rules = 
    new hfst::implementations::ComposeIntersectRulePair
    (first_rule,second_rule);

    for (HfstTransducerVector::const_iterator it = 
	   harmony_rule_vector.begin() + 2;
         it != harmony_rule_vector.end();
         ++it)
    { 
      rules = new hfst::implementations::ComposeIntersectRulePair
        (new hfst::implementations::ComposeIntersectRule(*it),rules); 
    }



  //clock_t STOP_LOAD = clock();

  //  std::cerr << "Model loaded in " 
  //	    << (STOP_LOAD - START_LOAD) / (1.0 * CLOCKS_PER_SEC)
  //	    << std::endl;

  //clock_t START_INPUT = clock();
  

  char line[MAXBUFFER];
  while (std::cin.peek() != EOF)
    {
      std::cin.getline(line,MAXBUFFER);

      StringVector tokenized_line = split_at_spaces(line);
      
      for (StringVector::const_iterator it = tokenized_line.begin(); 
	   it != tokenized_line.end();
	   ++it)	
	{	  
	  //clock_t START_HMM = clock();

	  StringVector number_input = pad_and_code_to_numbers(*it,coder);
	  
	  HfstBasicTransducer n_gram_weighted_input = 
	    letter_n_gram_model.get_all_taggings(number_input);
	  
	  HfstTransducer n_gram_weighted_fst
	    (n_gram_weighted_input,TROPICAL_OPENFST_TYPE);

	  n_gram_weighted_fst.transform_weights(multiply_weights);

	  n_gram_weighted_fst.substitute(DEFAULT_SYMBOL,
					 internal_epsilon,
					 true,
					 false);

	  //n_gram_weighted_fst.substitute("||",
	  //				 internal_epsilon,
	  //				 true,
	  //				 true);
	  
	  n_gram_weighted_fst.minimize();

	  n_gram_weighted_fst.substitute
	    (StringPair("||","<DEFAULT>"),
	     StringPair("||","||")).minimize();
	  
	  //clock_t STOP_HMM = clock();

	  //std::cerr << "Letter n-gram model applied in " 
	  //	    << (STOP_HMM - START_HMM) / (1.0 * CLOCKS_PER_SEC)
	  //  << std::endl;
	  
	  //clock_t START_SEQUENCER = clock();

	  
	  HfstTransducer sequenced_input = sequencer[n_gram_weighted_fst];

	  HfstBasicTransducer basic_sequenced_input(sequenced_input);

	  //clock_t STOP_SEQUENCER = clock();

	  //std::cerr << "Sequencer applied in " 
	  //	    << (STOP_SEQUENCER - START_SEQUENCER) / 
	  // (1.0 * CLOCKS_PER_SEC)
	  //    << std::endl;
	  
	  //clock_t START_MORPHEME_SEQUENCE = clock();

	  HfstBasicTransducer output_basic_fst = 
	    morpheme_sequence_model.tag_input(basic_sequenced_input);

	  //clock_t STOP_MORPHEME_SEQUENCE = clock();

	  //	  std::cerr << "Morpheme sequence model applied in " 
	  //	    << (STOP_MORPHEME_SEQUENCE - START_MORPHEME_SEQUENCE) / 
	  //(1.0 * CLOCKS_PER_SEC)
	  // 	    << std::endl;

	  //clock_t START_OUTPUT = clock();

	  HfstTransducer output(output_basic_fst,TROPICAL_OPENFST_TYPE);
	  
	  output.substitute(DEFAULT_SYMBOL,
			    "<L>").minimize();
	  
	  //clock_t START_LETTER_RESTORATION = clock();

	  output.substitute(StringPair("<L>",internal_epsilon),
			    StringPair("<L>","<L>")).minimize();

	  output.substitute(DEFAULT_SYMBOL,
			    internal_epsilon).
	    output_project()
	    .minimize();

	  output = restore_letters(output);
	  
	  ComposeIntersectLexicon output_lex(output);
	  output = 
	    HfstTransducer(output_lex.compose_with_rules(rules),TROPICAL_OPENFST_TYPE);
	  

	  output.substitute("||",internal_epsilon);
	  output.substitute("<L>",internal_epsilon);

	  output
	    .output_project()
	    .n_best(10)	     
	    .minimize();

	  //std::cerr << output << std::endl;

	  //clock_t STOP_LETTER_RESTORATION = clock();

	  //std::cerr << "Letters restored in " 
	  //	    << (STOP_LETTER_RESTORATION - START_LETTER_RESTORATION) / 
	  // (1.0 * CLOCKS_PER_SEC)
	  //	    << std::endl;

	  HfstTwoLevelPaths output_paths;
	  
	  output.extract_paths(output_paths);

	  //clock_t STOP_OUTPUT = clock();

	  //std::cerr << "Output generated in " 
	  //	    << (STOP_OUTPUT - START_OUTPUT) / (1.0 * CLOCKS_PER_SEC)
	  //	    << std::endl;

	  std::cout << *it << "\t";
	  //bool found = false;
	  //size_t pos = 0;
	  for (HfstTwoLevelPaths::const_iterator jt = output_paths.begin();
	       jt != output_paths.end();
	       ++jt)
	    {
	      HfstTwoLevelPath output_path = *jt;

	      std::string output_string = "";

	      for (StringPairVector::const_iterator kt = 
		 output_path.second.begin();
		   kt != output_path.second.end();
		   ++kt)
		{ 
		  //std::cout << (kt->first == "<DEFAULT>" ? "" : kt->first); 
		  output_string += 
		    (kt->first == "<DEFAULT>" ? "" : kt->first); 
		}

	      //if (output_string == *it)
	      //	{ 
	      //	  found = true;
	      //	  std::cout << pos; 
	      //	  break;
	      //	}
	      //++pos;
	      std::cout << output_string << "\t";
	    }
	  
	  //if (not found)
	  // { std::cout << "infty"; }

	  std::cout << std::endl;
	}
    }

  //clock_t STOP_INPUT = clock();

  //std::cerr << "Input processed in " 
  //	    << (STOP_INPUT - START_INPUT) / (1.0 * CLOCKS_PER_SEC)
  //	    << std::endl;

}
