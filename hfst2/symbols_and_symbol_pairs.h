


// ---------- Symbols ----------

const Symbol epsilon() {
  return Label::get_epsilon();
}

Symbol insert_symbol( char *s ) {
  return TheAlphabet.add_symbol( s );
}

Symbol insert_symbol( char *s, int p ) {
  TheAlphabet.add_symbol( s, p );
  return p;
}

bool is_equal( Symbol s1, Symbol s2 ) {
  return (s1 == s2);
}

const char *get_name( Symbol s ) {
  return TheAlphabet.code2symbol( s );
}

int get_position( Symbol s ) {
  return (int) s;
}



// ---------- Symbol pairs ----------

SymbolPair *make_pair( Symbol s1, Symbol s2 ) {
  return new Label( s1, s2 );
}

Symbol get_input_symbol( SymbolPair *p ) {
  return p->lower_char();
}

Symbol get_output_symbol( SymbolPair *p ) {
  return p->upper_char();
}

PairSet *make_empty_pair_set() {
  return new ::Alphabet::PublicLabelSet();
}

PairSet *insert_pair( SymbolPair *p, PairSet *Pi ) {
  ::Alphabet::insert_pair( *p, *Pi );
  delete p;
  return Pi;
}

bool has_pair( SymbolPair *p, PairSet *Pi ) {
  return ::Alphabet::has_pair( *p, *Pi );
}


