import libhfst

types = [libhfst.sfst_type(), libhfst.tropical_openfst_type(), libhfst.foma_type()]

for type in types:

    if not libhfst.HfstTransducer.is_implementation_type_available(type):
        continue

    # Test the lexc parser
    print("Testing...")

    # (1) A file in valid lexc format
    print("valid file, parse... ")
    compiler = libhfst.LexcCompiler(type)
    compiler.parse("test_lexc.lexc")
    print("")
    parsed = compiler.compileLexical()
    assert(parsed != 0);
    tok = libhfst.HfstTokenizer()
    cat = libhfst.HfstTransducer("cat", tok, type)
    dog = libhfst.HfstTransducer("dog", tok, type)
    mouse = libhfst.HfstTransducer("mouse", tok, type)

    animals = libhfst.HfstTransducer(type)
    animals.disjunct(cat)
    animals.disjunct(dog)
    animals.disjunct(mouse)

    assert(animals.compare(libhfst.ptrvalue(parsed)))

