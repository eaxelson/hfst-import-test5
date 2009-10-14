#include "fst-regression.h"
#include <assert.h>


/*! 
  \fn vector<char*> * read_string( char * str )

  \brief  Return the tokenization of the string \e str into utf-8 characters and multi-character symbols of the form \p <CODE><...></CODE>.

  \param  str a C string.
  \return \e tokens a vector of C strings each representing a utf-8 character.
  \pre \e str is set.
*/
vector<char*> * read_string( char * str ){

  assert( str );

  vector<char*> * tokens = new vector<char*>;

  int code;
  char * token;

  int lt_code = (int) '<';
  char * gt = (char*)">";

  // This uses the operation in SFST, which reads a utf8-char from a string into 
  // an integer. The integer is then converted back into a C string.
  while ( (code = utf8_to_int(&str)) ){
    if ( not (code == lt_code) ) {

      // utf8-characters are at most 4 bytes long. An additional character
      // is needed for the end of char buffer \0.
      token = new char[6];

      strcpy(token,int_to_utf8(code));
    }

    // Here we've encountered a multi-character symbol of the form <...>.
    else {
      // Start reading from the < character.
      char * tok_begin = str;
      tok_begin -= 1;

      // get the index of the last character before > and
      // give token the value <...>. Multicharacter symbols, that are
      // over 39 characters long won't work.
      unsigned int span = strcspn(str,gt);
      token = new char[40];
      strncpy(token,tok_begin,span+2);
      str += span+1;
    }
    tokens->push_back( token ); 
  }
  return tokens;
}

/*! 
  \fn void erase(const int BUFFERSIZE, char * byte_string)

  \brief Set every byte of the string \e byte_string to the null character.
  \param BUFFERSIZE the length of \e byte_string.
  \param byte_string

  \pre \e byte_string is set.

*/
void erase(const int BUFFERSIZE, char * byte_string) {

  assert( byte_string );

  for(int i = 0; i < BUFFERSIZE; ++i) {
    byte_string[i] = '\0';
  }
}

/*!
  \fn void process_line(const char * file_name, const int file_row_number, char * str, 
                        vector<vector<char*>*> * str_codes, const int BUFFERSIZE)
  \brief Tokenize a line of text \e str into utf-8 characters and multi-character symbols of the form <CODE><...></CODE>.

  If the line \e str is too long, i.e. exceeds the length \e BUFFERSIZE, the user is warned. If \e str is reconized to be in an
  encoding, that differs from ASCII and utf-8, the user is warned.

  \param file_name the name of the file containing the line \e str.
  \param file_row_number the row number of the line \e str in the file \e file_name.
  \param str the line.
  \param str_codes a vector containing the lines, that have already been tokenized.
  \param BUFFERSIZE the maximum allowed length of a line.
  
  \pre \e file_name is set.
  \pre \e str is set.
  \pre \e str_codes is set.
  \post The last entry of \e str_codes is a <I>vector<char*>*</I>, that contains the line str tokenized into utf-8 characters
        and multi-character symbols.
*/
void process_line(const char * file_name, const int file_row_number,
	    char * str, vector<vector<char*>*> * str_codes, const int BUFFERSIZE){

  assert( file_name );
  assert( str );
  assert( str_codes );

  char * old_str = str;
  /* Tokenize the string str. */
  vector<char*> * codes = read_string( str );

  if ( (codes->size() == 0) && (str[0] != '\0') ) {
    fprintf(stderr, "!!! Warning !!!\n");
    fprintf(stderr, "The string beginning \"%.10s\" on line %d in file\n",old_str,file_row_number); 
    fprintf(stderr, "\"%s\" includes non-codable characters! Please make\n",file_name);
    fprintf(stderr, "sure that \"%s\" is in utf8-format, as this is the\n",file_name);
    fprintf(stderr, "most likely cause for the problem!\n\n");
  }
  str_codes->push_back( codes );
  erase(BUFFERSIZE, old_str);
} 

/*! \fn void read_utf8file( FILE * utf8file, const char * file_name, vector<vector<char*>*> * analyses, 
                            vector<vector<vector<char*>*>*> * surface_forms)

  \brief Read the entries from a test-file (\e utf8file) in utf-8 or ASCII encoding.

  The test-file is a file in utf8 encoding consisting of blocks of lines separated by multiple new-lines.
  The first line of a block is an analysis-form of a word-form and the rest of the lines are its 
  surface-forms. An example of a test-file:
  \verbatim
  bus<N><PL>
  buses 
  busses
  
  cat<N><PL>
  cats
  ...
  \endverbatim

  \param utf8file a test-file in utf-8 or ASCII encoding.
  \param file_name the name of the test-file.
  \param analyses a vector for storing the analysis-forms in \e utf8file.
  \param surface_forms a vector for storing the surface-forms in \e utf8file.

  \pre \e utf8file is set.
  \pre \e file_name is set.
  \pre \e analyses is set to an empty vector
  \pre \e surface_forms is set to an empty vector
  
  \post \e analyses points to a vector containing pointers to vectors containing analysis-forms
  tokenized into utf8-characters and multi-character symbols <CODE><...></CODE> (represented as C strings). 
  In the example above analyses would take the form 
  \f[ ( (b,u,s,\langle N\rangle ,\langle PL\rangle ), (c,a,t, \langle N\rangle ,\langle PL\rangle ) ).\f]

  \post \e surface_forms points to a vector, that has the same size as \e analyses. Corresponding to
  the ith element of the vector \e analyses, the ith element of \e surface_forms is a vector
  containing the surface forms of  the ith  block in \e utf8file (in exactly the same format as above). 
  In the example above, the vector \e suface_forms would take the forms 
  \f[ ( ( (b,u,s,e,s), (b,u,s,s,e,s) ), ( (c,a,t,s) ) ).\f]

  \post The file utf8file is not closed!
*/
void read_utf8file( FILE * utf8file, const char * file_name,
		    vector<vector<char*>*> * analyses, vector<vector<vector<char*>*>*> * surface_forms) {

  bool is_analysis = true;
  bool first_block = true;
  vector<vector<char*>*> * surface_form_vector = NULL; //new vector<vector<char*>*>;
  int BUFFERSIZE = 1000;
  char c;
  char * byte_string = new char[BUFFERSIZE];   // Lines longer than BUFFERSIZE/4 utf8 characters
                                               // may be broken down to parts.
  char * beginning = byte_string;

  int number_of_characters = 0;
  int file_row_number = 1;
  int current_row_number = 1;
  
  while ( (c = fgetc(utf8file)) != EOF){
    current_row_number = file_row_number;
    if (number_of_characters == (BUFFERSIZE - 1)) {
      fprintf(stderr,"!!! Warning !!!\n");
      fprintf(stderr,"Line %d beginning \"%.10s\" in file %s is over\n",current_row_number,beginning,file_name); 
      fprintf(stderr,"%d bytes long! It will be chopped down to smaller \n",BUFFERSIZE);
      fprintf(stderr,"pieces! This will cause problems!\n\n");

      if ( is_analysis ) {
	surface_forms->push_back( surface_form_vector );
	surface_form_vector = new vector<vector<char*>*>;
	process_line(file_name,current_row_number,beginning,analyses,BUFFERSIZE); 
      }
      else { process_line(file_name,current_row_number,beginning,surface_form_vector,BUFFERSIZE); }

      byte_string = beginning;
      number_of_characters = 0;
      is_analysis = false;
    }

    if ( c == '\n' ){
      ++file_row_number;
      bool next_is_analysis = false;
      while ( (c = fgetc(utf8file)) == '\n' ) {
	++file_row_number;
	next_is_analysis = true;
      }

      if ( is_analysis ) {
	if ( not first_block ) { surface_forms->push_back( surface_form_vector ); }
	else { first_block = false; }
	surface_form_vector = new vector<vector<char*>*>;
	process_line(file_name,current_row_number,beginning,analyses,BUFFERSIZE); 
      }
      else { process_line(file_name,current_row_number,beginning,surface_form_vector,BUFFERSIZE); }
      
      byte_string = beginning;
      number_of_characters = 0;
      is_analysis = next_is_analysis;
    }
    if (c != EOF) { *byte_string = c; }
    ++byte_string;
    ++number_of_characters;
  }

  surface_forms->push_back( surface_form_vector );

  if (strlen(beginning) > 0) {
    process_line(file_name,current_row_number,beginning,surface_form_vector,BUFFERSIZE);
  }
  delete[] beginning;
}

/*!
  \fn vector<Character> * recode(const char * file_name, const char * alphabet_file_name, const vector<char*> * tokens, const Alphabet * sigma)
  \brief Re-code a vector of tokens ( given as C strings ) into a vector of Characters.

  The token/Character-code correspondence is determined by the alphabet sigma.

  \param file_name name of the test-file.
  \param alphabet_file_name name of the file, where the alphabet sigma is defined.
  \param tokens the vector containing the tokens.
  \param sigma the alphabet, that determines the coding of the tokens into Characte- codes.
  \return \e codes a vector containing the Character-codes. 

  \pre \e file_name is set.
  \pre \e alphabet_file_name is set
  \pre \e tokens is set.
*/
vector<Character> * recode(const char * file_name, const char * alphabet_file_name, const vector<char*> * tokens, const Alphabet * sigma){

  assert( file_name );
  assert( alphabet_file_name );
  assert( tokens );

  vector<Character> * codes = new vector<Character>;
  for(vector<char*>::const_iterator it = tokens->begin(); it != tokens->end(); ++it) {
    int new_code = sigma->symbol2code( *it );
    if ( new_code == EOF ) {
      delete codes;
      char * p = new char[1000];
      sprintf(p,"Error: The character \"%s\" in the file \"%s\" is not \nincluded in the alphabet of the lexicon file \"%s\"!",
	     (*it),file_name,alphabet_file_name);
      throw p;
    }
    codes->push_back(new_code);
  }
  return codes;
}

/*! 
  \fn void print_form(ostream &out, const vector<Character> * form, const Alphabet &sigma)
  
  \brief Display the vector \e form in human-readable format.

  Write the vector \e form to the stream \e out so that each Character-code is displayed as the
  symbol it represents.

  \param out the out-put stream used (cout, cerr, ...).
  \param form the vector containing the character codes.
  \param sigma the alphabet giving the correspondance between Character codes and symbols.

  \pre \e form is set.
*/
void print_form(ostream &out, const vector<Character> * form, const Alphabet &sigma) {

  assert( form );

  for (vector<Character>::const_iterator it = form->begin(); it != form->end(); ++it) {
    out << sigma.code2symbol( *it );
  }
  out << '\n';
}

/*! 
  \fn int trace_rule( const vector<Label> * pair_string, const Transducer * rule, const Alphabet &sigma, bool &fails )

   \brief When processing the string \e pair_string, the transducer \e rule halts at some index of the string. Return this index.

   The transducer rule should be deterministic. Otherwise the result might
   not be the correct one.
   If the execution stops in a final state, \e fails will be set to \e false. 
   Otherwise it will be set to \e true.

   \param pair_string the input string for the transducer rule.
   \param rule the rule-transducer.
   \param sigma the alphabet of the rule-transducer.
   \param fails a boolean indicating whether the execution of the transducer fails or not.

   \pre \e pair_string is set.
   \pre \e rule is set to a deterministic transducer.
   \return \e counter the index in \e pair_string, where the execution of the transducer \e t stops.

   \post \e fails is \e true if the transducer fails to accept \e pair_string and \e false otherwise.
*/
int trace_rule( const vector<Label> * pair_string, const Transducer * rule, const Alphabet &sigma, bool &fails ) {

  assert( pair_string );
  assert( rule );

  /* Start at the root_node of t. */
  const Node * n = rule->root_node();
  fails = false;
  int counter = 0;

  /* Use the transducer until the vector<Label> * pair_string runs out or 
     there are no possible transitions. */
  for ( vector<Label>::const_iterator it = pair_string->begin(); it != pair_string->end(); ++it ) {
    n = n->target_node( *it );
    if ( not n ) { fails = true; return counter; }
    ++counter;
  }

  if ( not n->is_final() ) { fails = true; }

  return counter;
}

/*! 
  \fn void add_path_to_trie(const vector<Label> * path, Transducer * t)

  \brief Add the string of pairs \e path to the strings accepted by the transducer \e t.

  The transducer \e t has to be a trie. Otherwise the result of this operation might over-generate.

  \param path the string of Character codes to be added into the transducer.
  \param t the transducer.

  \pre \e path is set.
  \pre \e t is set to a trie.
  
  \post The transducer \e t accepts a string of pairs \f$ S \f$ iff, \e t accepted \f$ S \f$  
        before the operation add_path or \f$ S = \f$ \e path.
*/
void add_path_to_trie(const vector<Label> * path, Transducer * t) {
  
  assert( path );
  assert( t );

  Node * n = t->root_node();

  for (vector<Label>::const_iterator label = path->begin(); label != path->end(); ++label) {

    Node * next = n->target_node( *label );

    if ( not next ) { next = t->new_node(); }

    n->add_arc( *label, next, t );
    n = next;

  }

  n->set_final( 1 );

}

/*!
  \fn trace(const vector<Character> * analysis, const vector<Character> * surface, const vector<Transducer*> * all_analyses_by_rules,
	    const vector<Transducer*> * Rules, vector<char*> * rule_names, const Alphabet &sigma, Transducer * all_allowed_forms)
	    
   \brief Return a boolean indicating whether all rules in the vector \e Rules accept the pair of strings  \e analysis:surface.

   If one of the rules doesn't accept the string-pair, a message is displayed to the user.
   The function extends the transducer \e all_allowed forms by the pair of strings \e analysis:surface. 

   \param analysis the analysis form of the pair.
   \param surface the surface form of the pair.
   \param all_analyses_by_rules a vector of transducers giving 
          all possible surface forms correpsonding to \e analysis for each rule in \e Rules.
   \param Rules the vector containing the rule-transducers.
   \param rule_names The names of the rules.
   \param sigma the alphabet of the rules and the strings.
   \param all_allowed_forms a transducer that will accept exactly the surface-forms corrsponding to \e analyses given in the test-file.
   \returns \e all_ok is \e true if all of the rules accepted the pair analysis:surface and \e false otherwise.

   \pre \e analysis is set.
   \pre \e surface is set.
   \pre the size of \e analysis equals that of \e surface.
   \pre \e all_analyses_by_rules is set.
   \pre \e Rules is set.
   \pre \e all_allowed_rules is set.

   \post \e all_allowed_forms accepts exactly the pair of strings it accepted before and 
         the pair analysis:surface.
*/
bool trace(const vector<Character> * analysis, const vector<Character> * surface, const vector<Transducer*> * all_analyses_by_rules,
	   const vector<Transducer*> * Rules, vector<char*> * rule_names, const Alphabet &sigma, Transducer * all_allowed_forms) {

  assert( analysis );
  assert( surface );
  assert( analysis->size() == surface->size() );
  assert( all_analyses_by_rules);
  assert( Rules );
  assert( all_allowed_forms );


  vector<Label> * pair_string = new vector<Label>;
  int fails_at = 0;
  bool fails = false;
  bool all_ok = true;

  for ( unsigned int i = 0; i < analysis->size(); ++i) { 
    pair_string->push_back( Label( analysis->at(i), surface->at(i) ) ); 
  }

  add_path_to_trie(pair_string, all_allowed_forms);

  Transducer * possible_pairs;

  for (unsigned int i = 0; i < Rules->size(); ++i) {
  
    possible_pairs = all_analyses_by_rules->at(i);
    fails_at = trace_rule(pair_string, possible_pairs ,sigma,fails);

    if ( fails ) {
      all_ok = false;
      cout << "   Rule " << '"' << rule_names->at(i) << '"' << " isn't working. It fails\n";
      cout << "   to generate the form:\n";
      int spaces = 0;
      cout << "   ";
      for ( vector<Character>::const_iterator it = surface->begin(); it != surface->end(); ++it ){
	const char * c = sigma.code2symbol( *it );
	cout << c;
	if ( fails_at > 0 ) { spaces += strlen( c ); }
	--fails_at;
      }
      cout << "\n   ";
      for ( int i = 0; i < spaces ; ++i ) { cout << ' '; }
      cout << "^ THE GENERATION PROCESS STOPS HERE.\n\n";
    }
    
  }
  delete pair_string; pair_string = NULL;
  return all_ok;
}

/*! 
  \fn vector<Transducer*> * make_analyses(const vector<Character> * analysis, const vector<Transducer*> * Rules, const Alphabet &sigma )
  
  \brief Return the composition of the path represented by the vector \e analysis with each of the rules in \e Rules.
 
  Return a \e vector<Transducer*> \e all-analyses s.t. The ith transducer in \e all-analyses corresponds 
  to the minimal deterministic transducer, that accepts a string pair, iff 
  
  1. the analysis string of the pair is \e analysis and
 
  2. The ith rule-transducer in \e Rule accepts the pair-string.
  
  \param analysis the analysis form
  \param Rules the rules, that the path given by analysis is composed with.
  \param sigma the alphabet of analysis and Rules. 
  \returns \e all-analyses the results of the composition.
  \pre \e analysis is set.
  \pre \e Rules is set to a non-empty vector.

  \post \e all-analyses has the same size as \e Rules.
*/
vector<Transducer*> * make_analyses( const vector<Character> * analysis, const vector<Transducer*> * Rules, const Alphabet &sigma ) {

  assert( analysis );
  assert( Rules );
  assert( Rules->size() > 0 );

  /* 
     Given vector<Character> * analysis = < c1, c2, ..., cn >, make  
     Transducer analysis_transducer, which accepts exactly the language
     consisting of the sole string  c1:c1 c2:c2 ... cn:cn.
  */

  vector<Label> * labels = new vector<Label>;

  for( vector<Character>::const_iterator it = analysis->begin(); it != analysis->end(); ++it) {
    labels->push_back( Label( *it ) );
  }

  Transducer analysis_transducer = Transducer( *labels );
  analysis_transducer.alphabet.copy( sigma );

  delete labels; labels = NULL;

  vector<Transducer*> * all_analyses = new vector<Transducer*>;

  /* Compose the Transducer analysis_transducer with each rule in the 
     vector<Transducer*> * Rule and store the result in the 
     vector<Transducer*> all_analyses */

  Transducer * t;
  Transducer * temp;

  for (vector<Transducer*>::const_iterator rule = Rules->begin(); rule != Rules->end(); ++rule) {

    t = &analysis_transducer.copy();

    temp = t;
    t = &(t->operator||( **rule ));
    delete temp;

    temp = t;
    t = &(t->determinise());
    delete temp;

    temp = t;
    t = &(t->minimise());
    delete temp;

    all_analyses->push_back( t );

  }
  
  assert( all_analyses->size() == Rules->size() );

  return all_analyses;
}


/*! 
  \fn vector<vector<Character>*> * get_surface_forms1( Node * n , NodeNumbering &node_numbers, int visited[] )
  \brief Return a vector containing at most twenty surface forms (reversed) generated by the transducer \e t.

  At most twenty forms are returned. No cyclic paths are taken.
  The surface forms are reversed for efficiency reasons, so this function should be used through the function 
  get_suface_forms.

  \param n a node in t (initially n is the root node of t).
  \param node_numbers a numbering for the nodes of t.
  \param visited a vector coding information about, whether, or not, a node has been visited already.
  \returns \e paths a vector containing the twenty first surface forms generated by the transducer \e t. 
           If no surface forms were generated, NULL is returned.

  
  \pre \e n is set.
*/
vector<vector<Character>*> * get_surface_forms1( Node * n , NodeNumbering &node_numbers, int visited[] ) {
  
  assert( n );
  const Arcs * arcs = n->arcs();
  
  if ( arcs->is_empty() ) {
    
    /* Final state with no continuing transitions. */
    if ( n->is_final() ) {
      vector<vector<Character>*> * v = new vector<vector<Character>*>;
      v->push_back( new vector<Character> );
      return v;
    }

    /*
      Non-final state with no continuing transitions. This branch
      of the generation process dies.
    */
    visited[ node_numbers[ n ] ] = 1;
    return NULL;
  }
  
  /* Don't visit this state again during this branch of the generation process. */
  visited[ node_numbers[ n ] ] = 1;

  vector<vector<Character>*> * paths = new vector<vector<Character>*>;
  vector<vector<Character>*> * continuations = NULL;

  for (ArcsIter it(arcs); it; it++) {

    Arc arc = *it;
    Node * target_node = arc.target_node();
    Label l = arc.label();

    /* Don't visit states, that have been visited during this branch 
       of the generation process. */
    if ( visited [ node_numbers[ target_node ] ] == 1 ) { continue; }
    
    /* Get the continuation for the surface forms that beginning l.upper_char() */
    continuations = get_surface_forms1( arc.target_node(), node_numbers, visited);

    /* Empty language. */
    if ( not continuations ) { continue; }
    
    /* Build the surface-form (backwards). Return maximally 20 surface forms. */
    for( vector<vector<Character>*>::iterator cont = continuations->begin(); cont != continuations->end(); ++cont ){
      
      (*cont)->push_back( l.upper_char() );
      paths->push_back( *cont );
      
      if ( not (paths->size() < 20) ) {
	cout << "  There were more than twenty superfluous forms generated. Only the\n";
	cout << "  twenty first ones will be shown.\n";
	for( vector<vector<Character>*>::iterator rest = cont + 1; rest != continuations->end(); ++cont ) { 
	  delete *rest; *rest = NULL;
	}
	break; 
      }
    }
    
    delete continuations;

    /* Return maximally 20 surface-forms. */
    if ( not (paths->size() < 20) ) { break; }
    
  }
  
  /* If this is a final state, every surface-string ending here is a valid surface form. */
  if ( n->is_final() ) { paths->push_back( new vector<Character> ); }

  /* We're returning the empty language. */
  if ( paths->size() == 0 ) { delete paths; paths = NULL; }

  /* This branch is now dead, so it's okay to visit it again at some point. */
  visited[ node_numbers[ n ] ] = 0;

  return paths;

}

/*!
  \fn vector<vector<Character>*> * get_surface_forms( Transducer * t )
  \brief Return a vector containing at most twenty surface-forms generated by the transducer \e t.
  
  \param t a transducer.
  \return \e forms a vector containing twenty surface forms generated by \e t.

  \pre \e t is set.

*/
vector<vector<Character>*> * get_surface_forms( Transducer * t ) {

  assert( t );

  NodeNumbering node_numbers = NodeNumbering(*t);

  int visited[node_numbers.number_of_nodes()];

  vector<vector<Character>*> * forms = get_surface_forms1( t->root_node(), node_numbers, visited );

  if ( not forms ) { forms = new vector<vector<Character>*>; }

  for(vector<vector<Character>*>::iterator it = forms->begin(); it != forms->end(); ++it) {
    vector<Character> * v = new vector<Character>;
    for(vector<Character>::reverse_iterator token = (*it)->rbegin(); token != (*it)->rend(); ++token) {
      v->push_back( *token );
    }
    delete *it; *it = v;
  }

  assert( forms );

  return forms;

}
/*! \fn derivations(const vector<Character> * analysis, const vector<vector<Character>*> * surface_forms, 
		    const  vector<Transducer*> * Rules, vector<char*> * rule_names, const Alphabet &sigma)

   \brief Does the relation given by the string \e analysis and the 
          strings \e surface_forms agree with that given by the vector  \e Rules?

   Test if every surface form in \e surface_forms corresponds to the analysis form \e analysis for
   each of the rule transducers in \e Rules. Also, test if there are any additional surface forms, that 
   correspond to analysis_form.
 
   Diagnostics is displayed to the user.

   \param  analysis the analysis form given as a vector of Character codes of the alphabet 
           sigma.
   \param  surface_forms a vector of surface-forms given as a vector of Character codes of the 
           alphabet \e sigma.
   \param  Rules the vector of rule transducers.
   \param  sigma the alphabet of the rules Rules and the analysis and surface forms.
   \param  rule_names a vector contining the names of the rules in Rules.
   
   \pre \e analysis is set.
   \pre \e surface_forms is set.
   \pre \e Rules is set and is non-empty.
   \pre \e rule_names is se and has the same size as Rules.

*/
void derivations(const vector<Character> * analysis, const vector<vector<Character>*> * surface_forms, 
		 const vector<Transducer*> * Rules, vector<char*> * rule_names, const Alphabet &sigma) {

  assert( analysis );
  assert( surface_forms );
  assert( Rules );
  assert( rule_names );
  assert( Rules->size() > 0 );
  assert( Rules->size() == rule_names->size() );

  bool all_ok = true;

  cout << "THE ANALYSIS FORM "; cout.flush(); print_form(cout, analysis, sigma ); cout << '\n';

  Transducer * all_allowed_forms = new Transducer();
  all_allowed_forms->alphabet.copy( sigma );

  vector<Transducer*> * all_analyses_by_rules = make_analyses( analysis, Rules, sigma );

  for (vector<vector<Character>*>::const_iterator surface_form = surface_forms->begin(); 
       surface_form != surface_forms->end(); 
       ++surface_form) {

    all_ok = trace(analysis, *surface_form, all_analyses_by_rules, 
		   Rules, rule_names, sigma, all_allowed_forms);

  }

  if ( all_ok ) { cout << "  All surface forms were generated.\n\n"; }

  Transducer * residual = &(all_analyses_by_rules->at(0)->copy());
  Transducer * temp;

  for (vector<Transducer*>::iterator it = all_analyses_by_rules->begin(); 
       it != all_analyses_by_rules->end(); ++it) {
    temp = residual;
    residual = &(residual->operator&( **it ));

    delete temp; temp = NULL;
    delete *it; *it = NULL;
  }

  delete all_analyses_by_rules; all_analyses_by_rules = NULL;
  
  temp = residual;
  residual = &(residual->operator/( *all_allowed_forms ));

  delete temp;
  delete all_allowed_forms;
  
  if ( not residual->is_empty() ) {
    cout << "  There were superfluous forms generated:\n";
    vector<vector<Character>*> * forms = get_surface_forms( residual );
    for (vector<vector<Character>*>::iterator form = forms->begin(); form != forms->end(); ++form) {
      cout << "    "; print_form(cout,*form,sigma);
      delete *form; *form = NULL;
    }
    delete forms;
    cout << '\n';
  }
  else {
    cout << "  There were no superfluous surface forms generated.\n\n";
  }
  delete residual;
  cout << '\n';
}


