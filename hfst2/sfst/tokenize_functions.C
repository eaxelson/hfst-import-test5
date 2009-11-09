#include "tokenize_functions.h"

namespace HFST {

  vector<Key> * get_key_string( const char * input_string, 
				KeyTable * kt, Alphabet &alpha) {
    vector<Key> * result = new vector<Key>;
    //char * input = (char*)(malloc(2000));
    //char * original_input = input;
    //strcpy(input,input_string);
    char * input = strdup(input_string);
    char * original_input = input;
    char * symbol;
    while ( *input != 0 ) {
      symbol = int2utf8(utf8toint(&input));
      if (*symbol == 0)
	{
	  break;
	}
      Symbol s;
      if ( not alpha.is_symbol(symbol) ) {
	s = alpha.add_symbol(symbol);
      }
      else {
	s = alpha.get_symbol(symbol);
      }
      Key k;
      if ( not kt->is_symbol(s) ) {
	k = kt->get_unused_key();
	kt->associate_key(k,s);
      }
      else {
	k = kt->get_key(s);
      }
      result->push_back(k);
    } 
    free(original_input);
    return result;
  }

  Transducer * get_trie_universal_language( Transducer * t ) {
    LabelSet universal_language_labels;
    Transducer * universal = new Transducer;
    Node * start = universal->root_node();
    NodeNumbering t_num(*t);
    for ( size_t i=0; i<t_num.number_of_nodes(); ++i ) {
      Node * n = t_num.get_node(i);
      for (ArcsIter arcs(n->arcs()); arcs; arcs++) {
	Arc a = *arcs;
	if ( universal_language_labels.find(a.label()) == 
	     universal_language_labels.end()) {
	  universal_language_labels.insert(a.label());
	  start->add_arc(a.label(),start,t);
	}
      }
    }
    start->set_final(1);
    return universal;
  }
  
  KeyPairVector * combine_string_pair_levels(vector<Key> * input_level,
					     vector<Key> * output_level) {

    KeyPairVector * result = new KeyPairVector;

    if ( input_level->size() <= output_level->size() ) {
      
      KeyVector::iterator output_position = 
	output_level->begin();

      for ( KeyVector::iterator it = input_level->begin();
	    it != input_level->end();
	    ++it ) {
	result->push_back(new KeyPair(*it,
				      *output_position));
	++output_position;
      }
      for ( KeyVector::iterator it = output_position;
	    it != output_level->end();
	    ++it ) {
	result->push_back(new KeyPair(0,*it));
      }
    }
    else {
      KeyVector::iterator input_position =
	input_level->begin();
      
      for ( KeyVector::iterator it = output_level->begin();
	    it != output_level->end();
	    ++it ) {
	result->push_back(new KeyPair(*input_position,
				      *it));
	++input_position;	
      }
      
      for ( KeyVector::iterator it = input_position;
	    it != input_level->end();
	    ++it ) {
	result->push_back(new KeyPair(*it,0));
      }
    }
    return result;
  }

  Transducer * pair_up(vector<Key> * input_level,
		       vector<Key> * output_level) {
    
    Transducer * result = new Transducer;
    Node * n = result->root_node();
    
    if ( input_level->size() <= output_level->size() ) {
      vector<Key>::iterator out_it = output_level->begin();
      for ( vector<Key>::iterator it = input_level->begin();
	    it != input_level->end();
	    ++it ) {
	Node * nn = result->new_node();
	n->add_arc(Label(*it,*out_it),nn,result);
	n = nn;
	++out_it;
      }
      for ( vector<Key>::iterator it = out_it;
	    it != output_level->end();
	    ++it ) {
	Node * nn = result->new_node();
	n->add_arc(Label(0,*it),nn,result);
	n = nn;      
      }
      n->set_final(1);
    }
    else {
      vector<Key>::iterator in_it = input_level->begin();
      for ( vector<Key>::iterator it = output_level->begin();
	    it != output_level->end();
	    ++it ) {
	Node * nn = result->new_node();
	n->add_arc(Label(*in_it,*it),nn,result);
	n = nn;
	++in_it;
      }
      for ( vector<Key>::iterator it = in_it;
	    it != input_level->end();
	    ++it ) {
	Node * nn = result->new_node();
	n->add_arc(Label(*in_it,0),nn,result);
	n = nn;      
      }
      n->set_final(1);
    }
    return result;
  }
  
  KeyPairVector * tokenize_stringpair( Transducer * tokenizer, 
				    vector<Key> * input_string,
				    vector<Key> * output_string) {
    vector<Key> * input_level =
      find_first_output_string(tokenizer, input_string);
    vector<Key> * output_level =
      find_first_output_string(tokenizer, output_string);
    
    if ( input_level == NULL ) {
      delete output_level;
      return NULL;
    }
    if ( output_level == NULL ) {
      delete input_level;
      return NULL;
    }
    
    KeyPairVector * result = 
      combine_string_pair_levels(input_level,output_level);
    delete input_level;
    delete output_level;
    return result;
  }

  KeyPairVector * longest_prefix_tokenize_pair(Transducer * tokenizer,
					      KeyVector * string_vector1,
					      KeyVector * string_vector2) {
    KeyVector * input_level =
      longest_prefix_tokenization(tokenizer,
				  string_vector1);
    KeyVector * output_level =
      longest_prefix_tokenization(tokenizer,
				  string_vector2);

    if ( input_level == NULL ) {
      delete output_level;
      return NULL;
    }
    if ( output_level == NULL ) {
      delete input_level;
      return NULL;
    }
    KeyPairVector * result = 
      combine_string_pair_levels(input_level,output_level);
    delete input_level;
    delete output_level;
    return result;

  }
  

  Key join( vector<Key> * symbol_string, KeyTable * kt, Alphabet &alpha ) {
    char symbol[100];
    symbol[0] = 0;
    for ( vector<Key>::iterator it = symbol_string->begin();
	  it != symbol_string->end();
	  ++it ) {
      Key k = *it;
      if ( not kt->is_key(k) ) {
	char * error_msg = (char*)(malloc(200));
	sprintf(error_msg,
		"\nERROR There is no symbol associated to the key %u!\n",
		k);
	throw error_msg;
      }
      Symbol s = kt->get_key_symbol(k);
      if ( not alpha.is_symbol(s) ) {
	char * error_msg = (char*)(malloc(200));
	sprintf(error_msg,
		"\nERROR The symbol %u associated to the key %u!\n"
		"has no symbol-string corresponding to it!\n",
		s,k);
	throw error_msg;
      }
      const char * symbol_name = alpha.get_symbol_name(s);
      strcat(symbol,symbol_name);
    }
    if ( not alpha.is_symbol(symbol) ) {
      Symbol sym = alpha.add_symbol(symbol);
      Key sym_key = kt->get_unused_key();
      kt->associate_key(sym_key,sym);
      return sym_key;
    }
    Symbol sym = alpha.get_symbol(symbol);
    if ( not kt->is_symbol(sym) ) {
      Key sym_key = kt->get_unused_key();
      kt->associate_key(sym_key,sym);
      return sym_key;      
    }
    return kt->get_key(sym);
  }

  Transducer * gather_symbols(Node * n1, Node * n2, 
			      Transducer * symbol_maker, KeyTable * kt,
			      Alphabet &alpha, vector<Key> * symbol,
			      Key marker) {
    if ( n1->is_final() ) {
      Node * finished_node = symbol_maker->new_node();
      finished_node->set_final(1);
      Key finished_symbol = join(symbol,kt,alpha);
      n2->add_arc(Label(marker,finished_symbol),
		  finished_node,
		  symbol_maker);
    }
    for ( ArcsIter arcs(n1->arcs());
	  arcs;
	  arcs++ ) {
      Arc a = *arcs;
      Node * next_node = symbol_maker->new_node();
      
      n2->add_arc(Label(a.label().lower_char(),0),
		  next_node,symbol_maker);

      symbol->push_back(a.label().lower_char());

      gather_symbols(a.target_node(),next_node,
		     symbol_maker,kt,alpha,symbol,
		     marker);
      symbol->erase(symbol->end()-1);
    }
    return symbol_maker;
  }


  Transducer * multicharacter_symbol_maker( Transducer * split_symbols,
					    KeyTable * kt,
					    Key marker,
					    Alphabet &TheAlphabet) {
    Transducer * symbol_maker = new Transducer;
    vector<Key> * symbol = new vector<Key>;
    gather_symbols(split_symbols->root_node(),
		   symbol_maker->root_node(),
		   symbol_maker,kt, TheAlphabet,
		   symbol,marker);
    delete symbol;
    return symbol_maker;
  }

  void add_key_string( vector<Key> * key_string, 
		       Transducer * trie_transducer) {
    Node * n = trie_transducer->root_node();

    for ( vector<Key>::iterator it = key_string->begin();
	  it != key_string->end();
	  ++it ) {
      Key k = *it;
      bool found_transition = false;
      for ( ArcsIter arcs(n->arcs());
	    arcs;
	    arcs++ ) {
	Arc a = *arcs;
	if ( a.label().lower_char() == k ) {
	  n = a.target_node();
	  found_transition = true;
	  break;
	}
      }
      if (not found_transition) {
	Node * old_n = n;
	n = trie_transducer->new_node();
	old_n->add_arc(Label(k),n,trie_transducer);
      }
    }
    n->set_final(1);
  }

  void add_key_pair_string( KeyPairVector * key_pair_string, 
			    Transducer * trie_transducer ) {
    Node * n = trie_transducer->root_node();
    
    for ( KeyPairVector::iterator it = key_pair_string->begin();
	  it != key_pair_string->end();
	  ++it ) {
      KeyPair * kp = *it;
      Key kp_input_key = kp->lower_char();
      Key kp_output_key = kp->upper_char();

      bool found_transition = false;
      for ( ArcsIter arcs(n->arcs());
	    arcs;
	    arcs++ ) {
	Arc a = *arcs;
	Key a_input_key = a.label().lower_char();
	Key a_output_key = a.label().upper_char();

	if ( (a_input_key == kp_input_key) and
	     (a_output_key == kp_output_key) ) {
	  n = a.target_node();
	  found_transition = true;
	  break;
	}
      }
      if (not found_transition) {
	Node * old_n = n;
	n = trie_transducer->new_node();
	old_n->add_arc(*kp,
		       n,
		       trie_transducer);
      }
    }
    n->set_final(1);
  }

  Transducer * split_symbols( KeySet * keys,
			      KeyTable * kt,
			      Alphabet &TheAlphabet ) {
    Transducer * multicharacter_sequences = new Transducer();
    for ( KeySet::iterator it = keys->begin();
	  it != keys->end();
	  ++it ) {
      Key k = *it;
      if ( not kt->is_key(k) ) {
	char * error_msg = (char*)(malloc(200));
	sprintf(error_msg,
		"\nERROR: The key %u is not associated to a symbol!\n",
		k);
	throw error_msg;
      }
      Symbol s = kt->get_key_symbol(k);
      if ( not TheAlphabet.is_symbol(s) ) {
	char * error_msg = (char*)(malloc(200));
	sprintf(error_msg,
		"\nERROR: The symbol %u corresponding to the key %u\n"
		"is not associated to a string symbol!\n",
		s,k);
	throw error_msg;
      }
      const char * symbol = TheAlphabet.get_symbol_name(s);
      vector<Key> * split_symbol = 
	get_key_string(symbol,kt,TheAlphabet);
      add_key_string(split_symbol,multicharacter_sequences);
      delete split_symbol;
    }
    return multicharacter_sequences;
  }

  Transducer * key_string_to_transducer( vector<Key> * key_string ) {
    Transducer * path_transducer = new Transducer;
    Node * n = path_transducer->root_node();
    for ( vector<Key>::iterator it = key_string->begin();
	  it != key_string->end();
	  ++it ) {
      Node * nn = path_transducer->new_node();
      n->add_arc(Label(*it),nn,path_transducer);
      n = nn;
    }
    n->set_final(1);
    return path_transducer;
  }


  char * fst_to_pairstring( Transducer * path,
			    KeyTable * kt,
			    Alphabet &alpha,
			    bool spaces,
			    bool epsilons) {

    size_t string_length = 0;

    vector<const char*> string_components;

    Node * n = path->root_node();

    const char * pair_separator = ":";
    const char * token_separator = " ";

    bool empty_transducer = true;

    // Follow arcs starting at the start state
    // up to the final state.
    while ( true ) {
      if ( n->is_final() )
	break;

      ArcsIter arcs(n->arcs());
      if ( not arcs ) {
	if ( not empty_transducer ) {
	  throw "\nThe transducer is not a path!\n";
	  exit(1);
	}
	else {
	  return NULL;
	}
      }

      empty_transducer = false;

      Arc a = *arcs;
      n = a.target_node();
      Key lower = a.label().lower_char();
      Key upper = a.label().upper_char();

      if (!epsilons && lower == Label::epsilon && upper == Label::epsilon ) {}
      else {

      if ( not (kt->is_key(lower) and kt->is_key(upper)) ) {
	throw "A Key given with no corresponding symbol!\n";
	exit(1);
      }
      if (lower != upper) {
	Symbol lower_symbol = kt->get_key_symbol(lower);
	if ( not alpha.is_symbol(lower_symbol)) {
	  throw "A Key given with no corresponding symbol!\n";
	  exit(1);
	}
	const char * lower_string = 
	  alpha.code2symbol(lower_symbol);
	string_components.push_back(lower_string);
	string_components.push_back(pair_separator);
	string_length += strlen(lower_string)+1;
      }
      Symbol upper_symbol = kt->get_key_symbol(upper);
      if ( not alpha.is_symbol(upper_symbol)) {
	throw "A Key given with no corresponding symbol!\n";
	exit(1);
      }

      const char * upper_string = 
	alpha.code2symbol(upper_symbol);

      string_components.push_back(upper_string);
      string_length += strlen(upper_string);

      if ( spaces ) {
	string_components.push_back(token_separator);
	++string_length;
      } 
      }
    }

    if ( string_components.begin() == string_components.end() )
      return strdup("");

    char * result = 
      (char*)(malloc(sizeof(char)*(string_length+1)));
    *result=0;
    char * res = result;

    vector<const char*>::iterator tokens_end;
    if ( spaces )
      tokens_end = string_components.end()-1;
    else
      tokens_end = string_components.end();

    for (vector<const char*>::const_iterator it = 
	   string_components.begin();
	 it < tokens_end;
	 ++it ) {
      for ( const char * p = *it; *p != 0; ++p ) {
	*res = *p;
	++res;
      }
    }
    *res = 0;
    return result;
  }

  char * vector_to_pairstring( KeyVector * kv,
			       KeyTable * kt,
			       Alphabet &alpha ) {
    vector<const char*> symbol_strings;
    size_t string_length = 0;
    char * symbol_separator = (char*)" ";

    for ( KeyVector::iterator it = kv->begin();
	  it != kv->end();
	  ++it ) {
      Key k = *it;
      if ( not kt->is_key(k) ) {
	throw "A Key given with no corresponding symbol!\n";
	exit(1);
      }
      Symbol s = kt->get_key_symbol(k);
      if ( not alpha.is_symbol(s) ) {
	throw "A Key given with no corresponding symbol!\n";
	exit(1);
      }
      const char * symbol_name = alpha.code2symbol(s);

      symbol_strings.push_back(symbol_name);
      symbol_strings.push_back(symbol_separator);

      string_length += strlen(symbol_name)+1;

    }
    if ( symbol_strings.begin() == symbol_strings.end() )
      return strdup("");

    char * result = 
      (char*)(malloc(sizeof(char)*string_length+1));
    *result = 1;
    char * res = result;
    for (vector<const char*>::const_iterator it = 
	   symbol_strings.begin();
	 it < symbol_strings.end()-1;
	 ++it ) {
      for ( const char * p = *it; *p != 0; ++p ) {
	*res = *p;
	++res;
      }
    }
    *res = 0;
    return result;
  }

  char * vector_to_pairstring( KeyPairVector * kv,
			       KeyTable * kt,
			       Alphabet &alpha ) {
    vector<const char*> symbol_strings;
    size_t string_length = 0;
    char * symbol_separator = (char*)":";
    char * pair_separator = (char*)" ";

    for ( KeyPairVector::iterator it = kv->begin();
	  it != kv->end();
	  ++it ) {

      Key k_in = (*it)->lower_char();
      Key k_out = (*it)->upper_char();

      if( not (kt->is_key(k_in) and kt->is_key(k_out)) ) {
	throw "A Key given with no corresponding symbol!\n";
	exit(1);
      }
      Symbol s_in = kt->get_key_symbol(k_in);
      Symbol s_out = kt->get_key_symbol(k_out);
      if ( not (alpha.is_symbol(s_in) and alpha.is_symbol(s_out)) ) {
	throw "A Key given with no corresponding symbol!\n";
	exit(1);
      }
      const char * symbol_name = alpha.code2symbol(s_in);
      
      symbol_strings.push_back(symbol_name);
      string_length += strlen(symbol_name);	  
      if ( s_in != s_out ) {
	symbol_strings.push_back(symbol_separator);
	symbol_name = alpha.code2symbol(s_out);
	symbol_strings.push_back(symbol_name);
	string_length += strlen(symbol_name)+1;	  
      }
      symbol_strings.push_back(pair_separator);
      ++string_length;
    }

    if ( symbol_strings.begin() == symbol_strings.end() )
      return strdup("");

    char * result = 
      (char*)(malloc(sizeof(char)*string_length+1));
    *result = 1;
    char * res = result;
    for (vector<const char*>::const_iterator it = 
	   symbol_strings.begin();
	 it < symbol_strings.end()-1;
	 ++it ) {
      for ( const char * p = *it; *p != 0; ++p ) {
	*res = *p;
	++res;
      }
    }
    *res = 0;
    return result;
  }

};

