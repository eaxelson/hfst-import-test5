#
#   Test file for HfstBasicTransducer.
#   Some of the functions are already tested in other test files,
#   because some HfstTransducer functions are implemented only 
#   for HfstBasicTransducer.
#

import libhfst
import os

print("HfstBasicTransducer construction")

# Create an HfstBasicTransducer [a:b c:d] with weight 3.0
t = libhfst.HfstBasicTransducer()
assert(not t.is_final_state(0))
s1 = t.add_state()
assert(s1 == 1)
t.add_transition(0, libhfst.HfstBasicTransition(s1, "a", "b", 1.2))
assert(not t.is_final_state(s1))
s2 = t.add_state()
assert(s2 == 2)
t.add_transition(s1, libhfst.HfstBasicTransition(s2, "c", "d", 0.8))
assert(not t.is_final_state(s2))
t.set_final_weight(s2, 1.0)
assert(t.is_final_state(s2) && t.get_final_weight(s2) == 1.0)

# Take a copy
tc = libhfst.HfstBasicTransducer(t);

print("HfstBasicTransducer exceptions")

# Asking the weight of a non-final state 
for s in range(5):
    if s != s2:
        try:
            w = t.get_final_weight(s)
            assert(False)
        except libhfst.HfstException:
            pass

# Reading a file in non-valid AT&T format
ofile = open('test.att', 'wb')
ofile.write('0\n0\t1\ta\tb\n1\t2\tb\n2\n')
ofile.close()
ifile = libhfst.HfstFile('test.att')

try:
    linecount = 0
    foo = libhfst.HfstBasicTransducer.read_in_att_format(ifile, '@0@', linecount);
    ifile.close()
    os.remove('test.att')
    assert(False)
except libhfst.HfstException:
    ifile.close()
    os.remove('test.att')

  
print("HfstBasicTransducer: symbol handling")

  t.add_symbol_to_alphabet("foo");
  t.prune_alphabet();
  const std::set<std::string> alphabet = t.get_alphabet();
  assert(alphabet.size() == 7);
  assert(alphabet.find("a") != alphabet.end());
  assert(alphabet.find("b") != alphabet.end());
  assert(alphabet.find("c") != alphabet.end());
  assert(alphabet.find("d") != alphabet.end());
  assert(alphabet.find("foo") == alphabet.end());

  
  print("HfstBasicTransducer: substitute");

  HfstBasicTransducer tr;
  tr.add_state();
  tr.add_transition(0, HfstBasicTransition(s1, "a", "b", 0));
  tr.add_transition(0, HfstBasicTransition(s1, "a", "b", 0));
  tr.set_final_weight(1, 0);

  StringPairSet sps;
  sps.insert(StringPair("A","B"));
  sps.insert(StringPair("C","D"));
  tr.substitute(StringPair("a","b"), sps);

  print("HfstBasicTransducer: EmptyStringException");
  
  try {
    HfstBasicTransducer empty_symbol;
    empty_symbol.add_transition(0, HfstBasicTransition(0, "", "", 0));
    assert(false);
  }
  catch (EmptyStringException e) {
    ;
  }

  
  print("HfstBasicTransducer: unknown and indentity symbols");

  {
    // In the xerox formalism used here, "?" means the unknown symbol
    // and "?:?" the identity pair 
    
    HfstBasicTransducer tr1;
    tr1.add_state(1);
    tr1.set_final_weight(1, 0);
    tr1.add_transition
      (0, HfstBasicTransition(1, "@_UNKNOWN_SYMBOL_@", "foo", 0) );
    
    // tr1 is now [ ?:foo ]
    
    HfstBasicTransducer tr2;
    tr2.add_state(1);
    tr2.add_state(2);
    tr2.set_final_weight(2, 0);
    tr2.add_transition
      (0, HfstBasicTransition(1, "@_IDENTITY_SYMBOL_@", 
                  "@_IDENTITY_SYMBOL_@", 0) );
    tr2.add_transition
      (1, HfstBasicTransition(2, "bar", "bar", 0) );
    
    // tr2 is now [ [ ?:? ] [ bar:bar ] ]
    
    if (HfstTransducer::is_implementation_type_available(SFST_TYPE))
      {
	ImplementationType type = SFST_TYPE;
	HfstTransducer Tr1(tr1, type);
	HfstTransducer Tr2(tr2, type);
	Tr1.disjunct(Tr2);
    
	// Tr1 is now [ [ ?:foo | bar:foo ]  |  [[ ?:? | foo:foo ] [ bar:bar ]]]
      }
  }


  print("HfstBasicTransducer: iterating through");

  { 
    unsigned int source_state=0;

    for (HfstBasicTransducer::const_iterator it = t.begin();
     it != t.end(); it++ )
      {
    for (HfstBasicTransducer::HfstTransitions::const_iterator tr_it 
           = it->begin(); tr_it != it->end(); tr_it++)
      {
        std::cerr << source_state << "\t"
              << tr_it->get_target_state() << "\t"
              << tr_it->get_input_symbol() << "\t"
              << tr_it->get_output_symbol() << "\t"
              << tr_it->get_weight() << std::endl;
      }

    if (t.is_final_state(source_state)) 
      {
        std::cerr << source_state << "\t"
              << t.get_final_weight(source_state) << std::endl;
      }
    
    source_state++;
      }
  }

  
}

