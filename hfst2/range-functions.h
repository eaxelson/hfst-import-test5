
Range *create_empty_range();
Range *insert_value( Symbol c, Range *r );
Range *append_range( Range *r1, Range *r2 );
Range *complement_range( Range *r, KeySet *Si );
KeyPairSet *define_pair_range(Range *r1, Range *r2, bool final);

Ranges *create_empty_ranges();
Ranges *insert_range( Range *r, Ranges *rs );
KeyPairSet *define_pair_ranges( Ranges *rs1, Ranges *rs2 );

