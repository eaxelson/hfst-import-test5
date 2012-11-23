import libhfst
import os

tr1 = libhfst.HfstBasicTransducer()
tr1.add_state(1)
tr1.set_final_weight(1, 0)
tr1.add_transition(0, libhfst.HfstBasicTransition(1, "@_UNKNOWN_SYMBOL_@", "foo", 0) )
# tr1 is [ @_UNKNOWN_SYMBOL_@:foo ]

tr2 = libhfst.HfstBasicTransducer()
tr2.add_state(1)
tr2.add_state(2)
tr2.set_final_weight(2, 0)
tr2.add_transition(0, libhfst.HfstBasicTransition(1, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 0) )
tr2.add_transition(1, libhfst.HfstBasicTransition(2, "bar", "bar", 0) )
# tr2 is [ [ @_IDENTITY_SYMBOL_@:@_IDENTITY_SYMBOL_@ ] [ bar:bar ] ]

# The disjunction should be
disj = libhfst.HfstBasicTransducer()
disj.add_state(1)
disj.add_state(2)
disj.set_final_weight(2, 0)

disj.add_transition(0, libhfst.HfstBasicTransition(1, "@_IDENTITY_SYMBOL_@", "@_IDENTITY_SYMBOL_@", 0) )
disj.add_transition(0, libhfst.HfstBasicTransition(1, "foo", "foo", 0) )

disj.add_transition(0, libhfst.HfstBasicTransition(2, "@_UNKNOWN_SYMBOL_@", "foo", 0) )
disj.add_transition(0, libhfst.HfstBasicTransition(2, "bar", "foo", 0) )
disj.add_transition(1, libhfst.HfstBasicTransition(2, "bar", "bar", 0) )


ofile = open("testfile.att", "wb")
ofile.write("0 1 a b 0.4\n")
ofile.write("1 c d\n")
ofile.close()

types = [libhfst.TROPICAL_OPENFST_TYPE, libhfst.SFST_TYPE, libhfst.FOMA_TYPE]

for type in types:
  
  if not libhfst.HfstTransducer.is_implementation_type_available(type):
    continue
  
  print("expanding unknowns")
  
  Tr1 = libhfst.HfstTransducer(tr1, type)
  Tr2 = libhfst.HfstTransducer(tr2, type)
  Disj = libhfst.HfstTransducer(disj, type)
  
  Tr1.disjunct(Tr2).minimize()
  # Tr1 is expanded to [ @_UNKNOWN_SYMBOL_@:foo | bar:foo ]
  # Tr2 is expanded to 
  # [ [ @_IDENTITY_SYMBOL_@:@_IDENTITY_SYMBOL_@ | foo:foo ] [ bar:bar ] ]
  
  assert(Tr1.compare(Disj))
  

  print("testing NotValidAttFormatException")
  
  ifile = libhfst.hfst_open("testfile.att", "rb")
  try:
    t = libhfst.HfstTransducer(ifile, type, "@_EPSILON_SYMBOL_@")
    assert(False)
  except libhfst.NotValidAttFormatException:
    pass
  
  ifile.close()

os.remove("testfile.att")
