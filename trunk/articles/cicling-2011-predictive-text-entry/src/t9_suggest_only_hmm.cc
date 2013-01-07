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
  if (argc != 4)
    {
      std::cerr << "Usage: cat input_1_word_/_line | " << argv[0] 
		<< " letter_n_gram_lexical_model letter_n_gram_sequence_model"
		<< " text_to_numbers_fst"
		<< std::endl;
      exit(1);
    }

  //clock_t START_LOAD = clock();

  HMMTagger letter_n_gram_model(argv[1],argv[2]);

  HfstInputStream coder_stream(argv[3]);
  HfstTransducer coder(coder_stream);

  char line[MAXBUFFER];
  while (std::cin.peek() != EOF)
    {
      std::cin.getline(line,MAXBUFFER);

      StringVector tokenized_line = split_at_spaces(line);
      
      for (StringVector::const_iterator it = tokenized_line.begin(); 
	   it != tokenized_line.end();
	   ++it)	
	{	  
	  StringVector number_input = pad_and_code_to_numbers(*it,coder);
	  
	  HfstBasicTransducer n_gram_weighted_input = 
	    letter_n_gram_model.get_all_taggings(number_input);
	  
	  HfstTransducer n_gram_weighted_fst
	    (n_gram_weighted_input,TROPICAL_OPENFST_TYPE);

	  HfstTransducer output(n_gram_weighted_fst);
	  
	  output.substitute("||",internal_epsilon);
	  output.substitute("<L>",internal_epsilon);

	  output
	    .output_project()
	    .n_best(10)	     
	    .minimize();

	  HfstTwoLevelPaths output_paths;
	  
	  output.extract_paths(output_paths);

	  std::cout << *it << "\t";
	  bool found = false;
	  size_t pos = 0;
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
		  output_string += 
		    (kt->first == "<DEFAULT>" ? "" : kt->first); 
		}

	      if (output_string == *it)
	      	{ 
	      	  found = true;
	      	  std::cout << pos; 
	      	  break;
	      	}
	      ++pos;
	    }
	  
	  if (not found)
	   { std::cout << "infty"; }

	  std::cout << std::endl;
	}
    }
}
