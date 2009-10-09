#include "hwfst-tokenize-functions.h"

namespace HWFST {

KeyVector * get_key_string( const char * input_string, 
			    KeyTable * kt, Alphabet &alpha) {
  KeyVector * result = new KeyVector;
  //char * input = (char*)(malloc(200));
  //char * original_input = input;
  //strcpy(input,input_string);
  char * input = strdup(input_string);
  char * original_input = input;
  char * symbol;
  while ( *input != 0 ) {
    symbol = int2utf8(utf8toint(&input));
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
  
Transducer * get_trie_universal_language( Transducer &t ) {
  LabelSet universal_language_labels;
  
  Transducer * universal = get_new_transducer();
  StateId start = universal->Start();
  
  for ( StateIterator states(t);
	not states.Done();
	states.Next() ) {
    
    StateId n = states.Value();
    
    for ( ArcIterator arcs(t,n);
	  not arcs.Done();
	  arcs.Next() ) {
      Arc a = arcs.Value();
      
      Label l = Label(a.ilabel,a.olabel);
      
      if ( universal_language_labels.find(l) == 
	   universal_language_labels.end()) {
	universal_language_labels.insert(l);
	universal->AddArc(start,Arc(a.ilabel,
				    a.olabel,
				    Weight::One(),
				    start));
      }
    }
  }

  set_final(*universal,start);
  
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
  
KeyPairVector * pair_up_vector(KeyVector * input_level,
			    KeyVector * output_level) {
  
  KeyPairVector * result = new KeyPairVector;
  
  if ( input_level->size() <= output_level->size() ) {
    
    KeyVector::iterator out_it = output_level->begin();
    
    for ( KeyVector::iterator it = input_level->begin();
	  it != input_level->end();
	  ++it ) {
      KeyPair * kp = new KeyPair(*it,*out_it);
      result->push_back(kp);
      ++out_it;
    }
    
    for ( KeyVector::iterator it = out_it;
	  it != output_level->end();
	  ++it ) {
      KeyPair * kp = new KeyPair(0,*it);
      result->push_back(kp);
    }
  }
  else {
    KeyVector::iterator in_it = input_level->begin();
    for ( KeyVector::iterator it = output_level->begin();
	  it != output_level->end();
	  ++it ) {

      KeyPair * kp = new KeyPair(*in_it,*it);
      result->push_back(kp);
      ++in_it;
      
    }
    for ( KeyVector::iterator it = in_it;
	  it != input_level->end();
	  ++it ) {
      KeyPair * kp = new KeyPair(*it,0);
      result->push_back(kp);
    }
  }
  return result;
}

  /*  
Transducer * tokenize_stringpair( Transducer &tokenizer, 
				  KeyVector * input_string,
				  KeyVector * output_string) {
  KeyVector * input_level =
    find_first_output_string(tokenizer, input_string);
  KeyVector * output_level =
    find_first_output_string(tokenizer, output_string);
  
  if ( input_level == NULL ) {
    delete output_level;
    return NULL;
  }
  if ( output_level == NULL ) {
    delete input_level;
    return NULL;
  }
  
  Transducer * result = pair_up(input_level,output_level);
  delete input_level;
  delete output_level;
  return result;
  }*/

KeyPairVector * tokenize_stringpair_to_vector( Transducer &tokenizer, 
					       KeyVector * input_string,
					       KeyVector * output_string) {
  KeyVector * input_level =
    find_first_output_string(tokenizer, input_string);
  KeyVector * output_level =
    find_first_output_string(tokenizer, output_string);
  
  if ( input_level == NULL ) {
    delete output_level;
    return NULL;
  }
  if ( output_level == NULL ) {
    delete input_level;
    return NULL;
  }
  
  KeyPairVector * result = pair_up_vector(input_level,output_level);
  delete input_level;
  delete output_level;
  return result;
}

KeyPairVector * longest_prefix_tokenize_pair(Transducer &tokenizer,
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
  
Key join( KeyVector * symbol_string, KeyTable * kt, Alphabet &alpha ) {
  char symbol[100];
  symbol[0] = 0;
  for ( KeyVector::iterator it = symbol_string->begin();
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

  Transducer * gather_symbols(StateId n1, 
			      StateId n2,
			      Transducer &multicharacter_symbol_string_fst,
			      Transducer &symbol_maker, 
			      KeyTable * kt,
			      Alphabet &alpha, 
			      KeyVector * symbol,
			      Key marker) {
    if ( is_final(multicharacter_symbol_string_fst,n1) ) {
      Key finished_symbol = join(symbol,kt,alpha);

      StateId finished_node =
	add_arc(symbol_maker,
		n2,
		marker,
		finished_symbol);

      set_final(symbol_maker,finished_node);
    }

    for ( ArcIterator 
	    arcs(multicharacter_symbol_string_fst,n1);
	  not arcs.Done();
	  arcs.Next() ) {
      Arc a = arcs.Value();

      StateId next_node = 
	add_arc(symbol_maker,
		n2,
		a.ilabel,
		0);
		
      symbol->push_back(a.ilabel);

      gather_symbols(a.nextstate,
		     next_node,
		     multicharacter_symbol_string_fst,
		     symbol_maker,
		     kt,
		     alpha,
		     symbol,
		     marker);

      symbol->erase(symbol->end()-1);
    }
    return &symbol_maker;
  }


  Transducer * multicharacter_symbol_maker( Transducer &split_symbols,
					    KeyTable * kt,
					    Key marker,
					    Alphabet &TheAlphabet) {
    Transducer * symbol_maker = get_new_transducer();
    
    KeyVector * symbol = new KeyVector;
    gather_symbols(split_symbols.Start(),
		   symbol_maker->Start(),
		   split_symbols,
		   *symbol_maker,kt, TheAlphabet,
		   symbol,marker);
    delete symbol;
    return symbol_maker;
  }

  void add_key_string( KeyVector * key_string, 
		       Transducer &trie_transducer,
		       float weight,
		       bool sum_weights ) {

    StateId n = trie_transducer.Start();

    for ( KeyVector::iterator it = key_string->begin();
	  it != key_string->end();
	  ++it ) {
      Key k = *it;
      bool found_transition = false;
      for ( ArcIterator arcs(trie_transducer,n);
	    not arcs.Done();
	    arcs.Next() ) {
	Arc a = arcs.Value();
	if ( a.ilabel == k ) {
	  n = a.nextstate;
	  found_transition = true;
	  break;
	}
      }
      if (not found_transition) {
	n = add_arc(trie_transducer,
		    n,k,k);			    
      }
    }
    set_final(trie_transducer,n,weight,sum_weights);
  }

  void add_key_pair_string( KeyPairVector * key_string, 
			    Transducer &trie_transducer,
			    float weight,
			    bool sum_weights ) {

    StateId n = trie_transducer.Start();

    for ( KeyPairVector::iterator it = key_string->begin();
	  it != key_string->end();
	  ++it ) {
      KeyPair * kp = *it;
      Key k_in = kp->lower_char();
      Key k_out = kp->upper_char();

      bool found_transition = false;
      for ( ArcIterator arcs(trie_transducer,n);
	    not arcs.Done();
	    arcs.Next() ) {
	Arc a = arcs.Value();
	if ( (a.ilabel == k_in) and (a.olabel == k_out) ) {
	  n = a.nextstate;
	  found_transition = true;
	  break;
	}
      }
      if (not found_transition) {
	n = add_arc(trie_transducer,
		    n,k_in,k_out);			    
      }
    }
    set_final(trie_transducer,n,weight,sum_weights);
  }

  Transducer * split_symbols( KeySet * keys,
			      KeyTable * kt,
			      Alphabet &TheAlphabet ) {
    Transducer * multicharacter_sequences = get_new_transducer();
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
      KeyVector * split_symbol = 
	get_key_string(symbol,kt,TheAlphabet);
      add_key_string(split_symbol,*multicharacter_sequences);
      delete split_symbol;
    }
    return multicharacter_sequences;
  }

  Transducer * key_string_to_transducer( KeyVector * key_string ) {
    Transducer * path_transducer = get_new_transducer();
    StateId n = path_transducer->Start();
    for ( KeyVector::iterator it = key_string->begin();
	  it != key_string->end();
	  ++it ) {
      n = add_arc(*path_transducer,
		  n,*it,*it);
    }
    set_final(*path_transducer,n);
    return path_transducer;
  }


  char * fst_to_pairstring( Transducer &path,
			    KeyTable * kt,
			    Alphabet &alpha,
			    bool spaces) {

    size_t string_length = 0;

    vector<const char*> string_components;

    StateId n = path.Start();

    const char * pair_separator = ":";
    const char * token_separator = " ";

    bool empty_transducer = true;

    // Follow arcs starting at the start state
    // up to the final state.
    while ( true ) {
      if ( is_final(path,n) )
	break;

      ArcIterator arcs(path,n);
      if ( arcs.Done() ) {
	if ( not empty_transducer ) {
	  throw "\nThe transducer is not a path!\n";
	  exit(1);
	}
	else {
	  return NULL;
	}
      }

      empty_transducer = false;

      Arc a = arcs.Value();
      n = a.nextstate;
      Key lower = a.ilabel;
      Key upper = a.olabel;
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

    if ( string_components.begin() == string_components.end() )
      return strdup("");

    char * result = 
      (char*)(malloc(sizeof(char)*(string_length+1)));
    *result=0;
    char * res = result;
    
    vector<const char*>::const_iterator tokens_end;
    if (spaces)
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
    const char * symbol_separator = " ";

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
    const char * symbol_separator = ":";
    const char * pair_separator = " ";

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


Transducer * get_new_transducer( void ) {
  Transducer * nt = new Transducer;
  nt->SetStart(nt->AddState());
  return nt;
}

bool is_final(Transducer &t, StateId s) {
  return t.Final(s) != Weight::Zero();
} 
  

void set_final(Transducer &t, StateId s, float weight, bool sum_weights) {
  fst::TropicalWeight old_weight = t.Final(s);
  fst::TropicalWeight new_weight;
  if (sum_weights) {
    if (old_weight == fst::TropicalWeight::Zero())
      new_weight = fst::TropicalWeight(weight);
    else
      new_weight = fst::TropicalWeight(old_weight.Value() + weight);
  }
  else
    new_weight = fst::Plus(old_weight, fst::TropicalWeight(weight));
  t.SetFinal(s, new_weight);
}
  
void add_arc(Transducer &t,
	     StateId source,
	     StateId target,
	     Key ilabel,
	     Key olabel) {
  t.AddArc(source,Arc(ilabel,
		       olabel,
		       Weight::One(),
		       target));
}
  
StateId add_arc(Transducer &t,
	     StateId source,
	     Key ilabel,
	     Key olabel) {
      StateId target = t.AddState();
      t.AddArc(source,Arc(ilabel,
			   olabel,
			   Weight::One(),
			   target));
      return target;
  }
};

