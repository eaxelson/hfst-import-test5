#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <set>
#include <vector>
#include <hfst2/hfst.h>


using std::set;
using std::vector;

static const int UNWEIGHTED = 0;
static const int WEIGHTED = 1;

typedef vector<HFST::TransducerHandle> FstVector;
typedef vector<HFST::Key> KeyVector;
typedef vector<KeyVector*> KeyVectorVector;

static bool weighted = false;

// Variables to store the transducer. It may
// be weighted or unweighted.
HFST::TransducerHandle unweighted_fst;
HWFST::TransducerHandle weighted_fst;

struct compare_KeyVectors 
{
  bool operator() (const KeyVector * v1, const KeyVector * v2) const
  {
    if (debug)
      {
	assert(v1 != NULL);
	assert(v2 != NULL);
      }
    return *v1 < *v2;
  }
};
typedef set<KeyVector*,compare_KeyVectors> KeyVectorSet;

static HFST::KeyTable * fst_keys;
static HFST::KeyTable * non_epsilon_fst_keys;

// Tokenized transducer weighted or unweighted
static HFST::TransducerHandle unweighted_tokenizer = NULL;
static HWFST::TransducerHandle weighted_tokenizer = NULL;

static const char * internal_epsilon = "HFST::PAIR::TEST::EPSILON";
static HFST::Key epsilon_representation;

/* Compute the set difference of two KeyVectorSets. */
KeyVectorSet * set_difference(const KeyVectorSet * s1,
			      const KeyVectorSet * s2) 
{
  if (debug)
    {
      assert(s1 != NULL);
      assert(s2 != NULL);
    }

  KeyVectorSet * diff = new KeyVectorSet;
  KeyVectorSet::const_iterator s1_it = s1->begin();
  KeyVectorSet::const_iterator s2_it = s2->begin();
  while ((s1_it != s1->end()) and (s2_it != s2->end())) 
  {
    if (compare_KeyVectors()(*s1_it,*s2_it)) 
      {
	diff->insert(*s1_it);
	++s1_it;
      }
    else if (compare_KeyVectors()(*s2_it,*s1_it)) 
      {
	++s2_it;
      }
    else
      {
	++s1_it;
	++s2_it;
      }
  }
  while(s1_it != s1->end())
    {
      diff->insert(*s1_it);
      ++s1_it;
    }
  return diff;
}

/* Delete a KeyVectorSet and its KeyVector* elements. */
void delete_KeyVectorSet(KeyVectorSet * kvs)
{
  for(KeyVectorSet::iterator it = kvs->begin();
      it != kvs->end();
      ++it)
    {
      delete *it;
    }
  delete kvs;
}	 

/* Create the tokenizer, which will be used to convert input strings
   into KeyVectors. 

   The tokenizer transducer can't handle epsilon, so it has to be replaced by
   an internal epsilon not likely to be encountered (HFST::PAIR::TEST::EPSILON). */ 
namespace HFST 
{
  void make_tokenizer(void) 
  {
    epsilon_representation = fst_keys->get_unused_key();
    
    non_epsilon_fst_keys = 
      HFST::recode_key_table(fst_keys,internal_epsilon);
    
    unweighted_tokenizer = HFST::longest_match_tokenizer2(non_epsilon_fst_keys);
  }
}

namespace HWFST 
{
  void make_tokenizer(void) 
  {
    epsilon_representation = fst_keys->get_unused_key();
    
    non_epsilon_fst_keys = 
      HWFST::recode_key_table(fst_keys,internal_epsilon);
    
    weighted_tokenizer = HWFST::longest_match_tokenizer2(non_epsilon_fst_keys);
  }
}

/* Return the symbol number corresponding to the symbol name symbol_name.
   If symbol_name has no symbol number, return -1. */
int get_symbol_number(char * symbol_name) {
  if (not HFST::is_symbol(symbol_name)) 
    {
      if (debug)
	{
	  fprintf(stderr,"Symbol %s has no corresponding symbol number\n",
		  symbol_name);
	}
      return -1;
    }
  return HFST::get_symbol(symbol_name);
}


/* Return the symbol number key corresponding to the symbol number 
   symbol_number. If symbol_number has no key, return -1. */
int get_symbol_number_key(HFST::Symbol symbol_number, char * symbol_name) {
  if (not HFST::is_symbol(symbol_number,fst_keys)) 
    {
      if (debug)
	{
	  fprintf(stderr,"Symbol %s has no key\n",symbol_name);
	}
      return -1;
    }
  return HFST::get_key(symbol_number,fst_keys);
}

/* Divide the string input_string at spaces. Skip multiple spaces. The segments
   which are formed are symbol-pairs. These are divided at colon-characters.
   The string of symbol-pairs, which is formed,  is coded into a KeyPairVector 
   according to the coding given by KeyTable * rule_keys. */
KeyVector * split_at_spaces(char * input_string)
{
  // Result vector contans the input symbol-pair string tokenized.
  KeyVector * tokenized_input = new KeyVector;
  
  // If something goes wrong, the original string may be printed 
  // as part of a debug-message.
  char * original_input_string = strdup(input_string);

  // Store the result of splitting the inpu string.
  vector<char*> symbol_names;

  char * symbol_string = strtok(input_string," ");    

  // Input is empty string.
  if (symbol_string == NULL)
    {
      return tokenized_input;
    }

  // Divide the symbol-pair string into symbols at spaces.
  // and store the pairs in symbol_names.
  do {
    symbol_names.push_back(symbol_string);
  } while ((symbol_string = strtok(NULL," ")) != NULL);
  
  for(vector<char*>::iterator it = symbol_names.begin();
      it != symbol_names.end();
      ++it)
    {
      symbol_string = *it;

      // Safe way to obtain the symbol number corresponding to a symbol name.
      int symbol_number_int = get_symbol_number(symbol_string); 
      if (symbol_number_int == -1)
	{
	  char warning[1000];
	  sprintf(warning,
		  "The string \"%s\" can't be tokenized. It contains symbols not\n"
		  "declared in the alphabet of the input transducers\n",
		  original_input_string);
	  warn(warning);
	  free(original_input_string);
	  return NULL;
	}
      HFST::Symbol symbol_number =
	static_cast<HFST::Symbol>(symbol_number_int);

      int symbol_key_int = 
	get_symbol_number_key(symbol_number,symbol_string);
      // I'm almost positive, that the symbol key can't be undefined, if
      // the symbol number is defined, but you never know...
      if (debug)
	{
	  assert(symbol_key_int != -1);
	}
      HFST::Key symbol_key =
	static_cast<HFST::Key>(symbol_key_int);

      tokenized_input->push_back(symbol_key);
    }
  free(original_input_string);
  return tokenized_input;
}

// Delete epsilon representations.
KeyVector * delete_epsilons(KeyVector * v) 
{
  if (v == NULL)
    {
      return NULL;
    }

  KeyVector * new_v = new KeyVector;
  for(KeyVector::iterator it = v->begin();
      it != v->end();
      ++it)
    {
      if (epsilon_representation != *it)
	new_v->push_back(*it);
    }
  delete v;
  return new_v;
}

void display_key(HFST::Key k,
		 FILE * out) 
{
  HFST::Symbol s =
    HFST::get_key_symbol(k,fst_keys);
  const char * s_name = 
    HFST::get_symbol_name(s);
  fprintf(out,"%s", s_name);
}

void display_key_vector(KeyVector * kv,
			FILE * out,
			bool print_spaces)
{
  if (debug)
    {
      assert(kv != NULL);
    }

  for(KeyVector::iterator it = kv->begin();
      it != kv->end();
      ++it) {
    HFST::Key k = *it;
    display_key(k,out);
    if (print_spaces)
      {
	if ((it+1) != kv->end())
	  {
	    fprintf(out," ");
	  }
      }
  }

  fprintf(out,"\n");
}

/* Tokenize the input strings. */
namespace HFST
{
  KeyVector * tokenize(char * input_string) 
  {
    if (debug)
      {
	assert(input_string != NULL);
      }
    KeyVector * tokenized_input = 
      HFST::longest_match_tokenize(unweighted_tokenizer,
				   input_string,
				   non_epsilon_fst_keys);
    if (tokenized_input == NULL)
      {
	char warning[1000];
	sprintf(warning,
		"The string \"%s\" can't be tokenized. It contains symbols not\n"
		"declared in the alphabet of the input transducers\n",
		input_string);
	warn(warning);
	return NULL;
      }
    tokenized_input = delete_epsilons(tokenized_input);
    
    if (debug)
      {
	fprintf(stderr,"Tokenization: ");
	display_key_vector(tokenized_input,
			   stderr,
			   true);
	
      }
    
    return tokenized_input;
  }
}

namespace HWFST
{
  KeyVector * tokenize(char * input_string) 
  {
    if (debug)
      {
	assert(input_string != NULL);
      }
    KeyVector * tokenized_input = 
      HWFST::longest_match_tokenize(weighted_tokenizer,
				    input_string,
				    non_epsilon_fst_keys);
    if (tokenized_input == NULL)
      {
	char warning[1000];
	sprintf(warning,
		"The string \"%s\" can't be tokenized. It contains symbols not\n"
		"declared in the alphabet of the input transducers\n",
		input_string);
	warn(warning);
	return NULL;
      }
    tokenized_input = delete_epsilons(tokenized_input);
    
    if (debug)
      {
	fprintf(stderr,"Tokenization: ");
	display_key_vector(tokenized_input,
			   stderr,
			   true);
	
      }
    
    return tokenized_input;
  }
}

/* Print the KeyTable fst_keys. */
void display_fst_keys(void)
{
  if ( fst_keys == NULL ) 
    {
      fprintf(stderr,"No key table stored with the transducers.\n");
    }
  fprintf(stderr,"Key table\n");
  HFST::write_symbol_table(fst_keys,cout,false);
  fprintf(stderr,"\n");
}

/* Unless the key table fst_keys has been set, attempt to read a key table
   from the file named char * symbol_file_name. Do nothing, if fst_keys is
   already set. abort, if fst_keys is not set and alphabet cannot be read. 
*/
void get_alphabet(char * symbol_file_name) 
{
  if ( fst_keys != NULL )
    {
      return;
    }
  if ( symbol_file_name == NULL )
    {
      fprintf(stderr,"Error: No alphabet supplied either with the input\n"
	      "transducers or in a separate file.\n");
      exit(1);
    }
  ifstream sym_in(symbol_file_name);
  fst_keys = HFST::read_symbol_table(sym_in);
  if ( fst_keys == NULL )
    {
      fprintf(stderr,"Error: The file %s doesn't contain a symbol table.\n",
	      symbol_file_name);
      exit(1);
    }
}

bool input_fst_is_weighted(istream &in, const char * file_name)
{
  if ( not silent )
    {
      fprintf(stderr,"Read input transducer from %s.\n",
	      file_name);
    }
  int UNWEIGHTED_F = 0;
  int WEIGHTED_F = 1;
  int COMPACT_F = 2;
  int UNKNOWN_F = -2;
  
  int format = HFST::read_format(in);
  if ( format == UNWEIGHTED_F )
    {
      if (verbose)
	{
	  fprintf(stderr,"Input transducer is unweighted.\n");
	}
      return false;
    }
  if ( format == WEIGHTED_F )
    {
      if (verbose)
	{
	  fprintf(stderr,"Input transducer is weighted.\n");
	}
      return true;
    }
  if ( format == COMPACT_F )
    {
      fprintf(stderr,"ERROR: Transducer in %s is compact.\n\n",
	      file_name);
      exit(1);
    }
  if ( format == UNKNOWN_F )
    {
      fprintf(stderr,"ERROR: Transducer in %s has unknown format.\n\n",
	      file_name);
      exit(1);
    }
  if ( format == EOF )
    {
      fprintf(stderr,"ERROR: Transducer file %s is empty.\n\n",
	      file_name);
      exit(1);
    }
  return false;
}

namespace HFST 
{
  HFST::TransducerHandle read_fst(istream &in)
    {
      if ( not HFST::has_symbol_table(in))
	{
	  return HFST::read_transducer(in);
	}
      else
	{
	  fst_keys = HFST::create_key_table();
	      return HFST::read_transducer(in,fst_keys);
	}
    }
}

namespace HWFST 
{
  HWFST::TransducerHandle read_fst(istream &in)
    {
      if ( not HWFST::has_symbol_table(in))
	{
	  return HWFST::read_transducer(in);
	}
      else
	{
	  fst_keys = HWFST::create_key_table();
	      return HWFST::read_transducer(in,fst_keys);
	}
    }
}

/* Read the input transducer either given in STDIN or the file whose name is
   fst_file_name. */
void read_input_transducer(void)
{
  if (fst_file_name == NULL)
    {
      weighted = input_fst_is_weighted(cin,"<STDIN>");
    }
  else
    {
      ifstream fst_test_in(fst_file_name);
      weighted = input_fst_is_weighted(fst_test_in,fst_file_name);
      fst_test_in.close();
    }

  if (not weighted)
    {
      if ( fst_file_name == NULL )
	{
	  unweighted_fst = HFST::read_fst(cin);
	}
      else
	{
	  ifstream fst_in(fst_file_name);
	  unweighted_fst = HFST::read_fst(fst_in);
	}
    }
  else
    {

      if ( fst_file_name == NULL )
	{
	  weighted_fst = HWFST::read_fst(cin);
	}
      else
	{
	  ifstream fst_in(fst_file_name);
	  weighted_fst = HWFST::read_fst(fst_in);
	}
    }
}

/* Add all KeyVectors from form_vector to form_set. */
void add_forms(KeyVectorSet * form_set,
	       KeyVectorVector * form_vector)
{
  if (debug)
    {
      assert(form_set != NULL);
    }

  size_t old_form_set_size=0;
  if (debug)
    {
      old_form_set_size = form_set->size();
    }

  if (form_vector == NULL)
    {
      return;
    }

  size_t number_of_added_forms = 0;
  for(KeyVectorVector::iterator it = form_vector->begin();
      it != form_vector->end();
      ++it)
    {
      KeyVector * kv = *it;
      if (form_set->find(kv) == form_set->end())
	{
	  if (debug)
	    {
	      ++number_of_added_forms;
	    }
	  form_set->insert(kv);
	}
      else
	{
	  delete kv;
	}
    }
  if (debug)
    {
      fprintf(stderr,"Form set size is %zu\n",form_set->size());
      assert((old_form_set_size+number_of_added_forms) == form_set->size());
    }
}

/* Apply the transducer t on the strings in input_forms. 
   Return the set of results. */
namespace HFST
{
  KeyVectorSet * apply_transducer(HFST::TransducerHandle t,
				  KeyVectorSet * input_forms)
  {
    if (debug)
      {
	assert(t != NULL);
	assert(input_forms != NULL);
      }
    
    KeyVectorSet * output_forms = new KeyVectorSet;
    for (KeyVectorSet::iterator it = input_forms->begin();
	 it != input_forms->end();
	 ++it)
      {
	KeyVector * input_form = *it;
	KeyVectorVector * outputs = 
	  HFST::lookup_all(t,input_form);
	
	if (debug)
	  {
	    if (outputs != NULL)
	      fprintf(stderr,"There were %zu output forms\n",
		      outputs->size());
	    else
	      fprintf(stderr,"There were no output forms.");
	  }
	
	delete input_form;
	add_forms(output_forms,outputs);
	delete outputs;
      }
    delete input_forms;
    return output_forms;
  }
}

namespace HWFST {  
  KeyVectorSet * apply_transducer(HWFST::TransducerHandle t,
				  KeyVectorSet * input_forms)
  {
    if (debug)
      {
	assert(t != NULL);
	assert(input_forms != NULL);
      }
    
    KeyVectorSet * output_forms = new KeyVectorSet;
    for (KeyVectorSet::iterator it = input_forms->begin();
	 it != input_forms->end();
	 ++it)
      {
	KeyVector * input_form = *it;
	KeyVectorVector * outputs = 
	  HWFST::lookup_all(t,input_form);
	
	if (debug)
	  {
	    if (outputs != NULL)
	      fprintf(stderr,"There were %zu output forms\n",
		      outputs->size());
	    else
	      fprintf(stderr,"There were no output forms.");
	  }
	
	delete input_form;
	add_forms(output_forms,outputs);
	delete outputs;
      }
    delete input_forms;
    return output_forms;
  }
}

/* Display each of the forms in ouput_forms, on per line. */
void display_result_set(KeyVectorVector * output_forms,
			FILE * out,
			bool print_spaces = false,
			bool destructive = true)
{
  if (debug)
    {
      assert(output_forms != NULL);
    }
  for(KeyVectorVector::iterator it = output_forms->begin();
      it != output_forms->end();
      ++it)
    {
      KeyVector * kv = *it;
      display_key_vector(kv,out,print_spaces);
      if (destructive)
	{
	  delete kv;
	}
    }
}

bool only_white_space(char * str)
{
  if (debug)
    {
      assert(str != NULL);
    }

  for (char * p = str;
       *p != 0;
       ++p)
    {
      if ((*p != ' ') and (*p != '\t'))
	return false;
    }
  return true;
}

char * skip_white_space_lines(char * line, 
			      size_t line_size, 
			      istream &in)
{
  if (debug)
    {
      assert(line != NULL);
    }

  if (not in)
    {
      return NULL;
    }

  while(in and only_white_space(line))
    {
      getline(in,line,line_size);
    }
  if (not in)
    {
      return NULL;
    }
  return line;
}

namespace HFST {
  KeyVectorSet * read_test_output_strings(char * line,
					  size_t input_size,
					  istream &test_set_in)
  {
    if (debug)
      {
	assert(test_set_in);
      }
    KeyVectorSet * tokenized_output_forms =
      new KeyVectorSet;
    
    while (test_set_in.getline(line,input_size))
      {
	if (only_white_space(line))
	  {
	    break;
	  }
	KeyVector * tokenized_output_form;
	
	if (not spaces)
	  {
	    tokenized_output_form = HFST::tokenize(line);
	  }
	else
	  {
	    tokenized_output_form = split_at_spaces(line);
	  }
	if (tokenized_output_form == NULL)
	  {
	    delete_KeyVectorSet(tokenized_output_forms);
	    return NULL;
	  }
	tokenized_output_forms->insert(tokenized_output_form);
      }
    return tokenized_output_forms;
  }
}

namespace HWFST {
  KeyVectorSet * read_test_output_strings(char * line,
					  size_t input_size,
					  istream &test_set_in)
  {
    if (debug)
      {
	assert(test_set_in);
      }
    KeyVectorSet * tokenized_output_forms =
      new KeyVectorSet;
    
    while (test_set_in.getline(line,input_size))
      {
	if (only_white_space(line))
	  {
	    break;
	  }
	KeyVector * tokenized_output_form;
	
	if (not spaces)
	  {
	    tokenized_output_form = HWFST::tokenize(line);
	  }
	else
	  {
	    tokenized_output_form = split_at_spaces(line);
	  }
	if (tokenized_output_form == NULL)
	  {
	    delete_KeyVectorSet(tokenized_output_forms);
	    return NULL;
	  }
	tokenized_output_forms->insert(tokenized_output_form);
      }
    return tokenized_output_forms;
  }
}
