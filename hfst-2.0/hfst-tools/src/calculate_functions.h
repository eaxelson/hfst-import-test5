
void add_epsilon_to_alphabet();
void read_symbol_table_text(istream& is);
void set_alphabet_defined( int i );

Key character_code( unsigned int c );
Key symbol_code( char *s );
unsigned int utf8_to_int( char *s );

bool define_transducer_variable( char *name, TransducerHandle t );
bool define_transducer_agreement_variable( char *name, TransducerHandle t );
bool define_range_variable( char *name, Range *r );

TransducerHandle copy_transducer_variable_value( char *name );
TransducerHandle copy_transducer_agreement_variable_value( char *name );
Range *copy_range_variable_value( char *name );
Range *copy_range_agreement_variable_value( char *name );

Range *complement_range( Range *r );

Range *add_value( Symbol c, Range *r );
Range *add_values( unsigned int c1, unsigned int c2, Range *r );
Range *add_var_values( char *name, Range *r );
Ranges *add_range( Range *r, Ranges *rs );

TransducerHandle insert_freely( TransducerHandle t, Key k1, Key k2 );
TransducerHandle make_mapping( Ranges *rs1, Ranges *rs2 );
TransducerHandle new_transducer( Range *r1, Range *r2, KeyPairSet *Pi );

TransducerHandle read_words( char* filename );
TransducerHandle explode( TransducerHandle t );
TransducerHandle result( TransducerHandle t, bool switch_flag, bool reverse=false );
TransducerHandle explode_and_minimise(TransducerHandle t);

void write_to_file( TransducerHandle t );
TransducerHandle read_transducer_and_harmonize( char *filename );

TransducerHandle make_rule(TransducerHandle t1, Range *r1, Twol_Type type, Range *r2, TransducerHandle t2, KeyPairSet *Pi);

void error2( char *message, char *input );

TransducerHandle read_transducer_text( char *filename, KeyTable *T, bool sfst=false );

