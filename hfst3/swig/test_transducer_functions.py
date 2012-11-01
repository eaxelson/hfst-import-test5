import libhfst

def compare_alphabets(t1, t2):
    alpha1 = t1.get_alphabet()
    alpha2 = t2.get_alphabet()
    return alpha1 == alpha2
    


types = [libhfst.sfst_type(), libhfst.tropical_openfst_type(), libhfst.foma_type()]

for type in types:

    if not libhfst.HfstTransducer.is_implementation_type_available(type):
        continue


    print("Function compare")

    t1 = libhfst.HfstTransducer("foo", "bar", type)
    t2 = libhfst.HfstTransducer("foo", "@_EPSILON_SYMBOL_@", type)
    t3 = libhfst.HfstTransducer("@_EPSILON_SYMBOL_@", "bar", type)
    t2.concatenate(t3).minimize()
    # Alignments must be the same
    assert(not t1.compare(t2))

    t4 = libhfst.HfstTransducer("foo", "bar", type)
    t5 = libhfst.HfstTransducer("@_EPSILON_SYMBOL_@", type)
    t4.concatenate(t5)
    # One transducer is minimal, the other is not
    assert(t1.compare(t4))

    # Weights
    if type == libhfst.tropical_openfst_type():
        t6 = libhfst.HfstTransducer("foo", "bar", type)
        t6.set_final_weights(0.3)
        t7 = libhfst.HfstTransducer("foo", "bar", type)
        t7.set_final_weights(0.1)

        # Weights differ
        assert(not t6.compare(t7))

        t8 = libhfst.HfstTransducer("@_EPSILON_SYMBOL_@", type)
        t8.set_final_weights(0.2)
        t7.concatenate(t8)
        # Weights are the same on each path
        assert(t6.compare(t7))


    print("Function compose")

    t1 = libhfst.HfstTransducer("foo", "bar", type)
    t1.set_final_weights(2)
    t2 = libhfst.HfstTransducer("bar", "baz", type)
    t2.set_final_weights(3)
    t3 = libhfst.HfstTransducer("foo", "baz", type)
    t3.set_final_weights(5)
    t1.compose(t2)
    assert(t1.compare(t3))
    

    print("Function shuffle")

    tok = libhfst.HfstTokenizer()
    t1 = libhfst.HfstTransducer("abc", "abc", tok, type)
    t2 = libhfst.HfstTransducer("cde", "cde", tok, type)
    t1.shuffle(t2)
    
    t3 = libhfst.HfstTransducer("abc", "abC", tok, type)
    try:
        t3.shuffle(t2)
        assert(False)
    except libhfst.TransducersAreNotAutomataException:
        assert(True)


    print("Function convert")

    t1 = libhfst.HfstTransducer("foo", "bar", type)
    t2 = libhfst.HfstTransducer("foo", "bar", type)
    # Go through all implementation formats and get back to the original one
    for conv_type in types:
        t1.convert(conv_type)
        assert(compare_alphabets(t1, t2))
    t1.convert(type)
    assert(t1.compare(t2))
    assert(compare_alphabets(t1, t2))


    print("Function extract_paths(_fd)")

    # Create a transducer that contains animals
    tok = libhfst.HfstTokenizer()
    cat = libhfst.HfstTransducer("cat", "cats", tok, type)
    cat.set_final_weights(3)
    dog = libhfst.HfstTransducer("dog", "dogs", tok, type)
    dog.set_final_weights(2.5)
    mouse = libhfst.HfstTransducer("mouse", "mice",  tok, type)
    mouse.set_final_weights(1.7)
    animals = libhfst.HfstTransducer(type)
    animals.disjunct(cat)
    animals.disjunct(dog)
    animals.disjunct(mouse)
    animals.minimize()

    results = libhfst.extract_paths(animals, 3, 0)
    #print results

    # Add an animal with two possible plurals
    hippopotamus1 = libhfst.HfstTransducer("hippopotamus", "hippopotami", tok, type)
    hippopotamus1.set_final_weights(1.2)
    hippopotamus2 = libhfst.HfstTransducer("hippopotamus", "hippopotamuses", tok, type)
    hippopotamus2.set_final_weights(1.4)
    animals.disjunct(hippopotamus1)
    animals.disjunct(hippopotamus2)
    animals.minimize()

    results = libhfst.extract_paths(animals, 5, 0)
    #print results

    # Convert into optimized lookup format
    animals_ol = libhfst.HfstTransducer(animals)
    if type == libhfst.tropical_openfst_type():
        animals_ol.convert(libhfst.hfst_olw_type())
    else:
        animals_ol.convert(libhfst.hfst_ol_type())
    
    result = libhfst.detokenize_paths(animals_ol.lookup("hippopotamus"))
    for res in result:
        print res[0]
        print res[1]
    
    if type == libhfst.tropical_openfst_type:
        best_animals = libhfst.HfstTransducer(animals)
        best_animals.n_best(3)
        assert(libhfst.detokenize_paths(best_animals.lookup("mouse"))[0] == "mice")
        assert(libhfst.detokenize_paths(best_animals.lookup("hippopotamus"))[0] == "hippopotami")
        assert(libhfst.detokenize_paths(best_animals.lookup("hippopotamus"))[1] == "hippopotamuses")
    
    
    print("Function insert_freely")

    t1 = libhfst.HfstTransducer("a", "b", type)
    t1.insert_freely(("c", "d"))

    t2 = libhfst.HfstTransducer("a", "b", type)
    tr = libhfst.HfstTransducer("c", "d", type)
    t2.insert_freely(tr)
    assert(t1.compare(t2))

    cd_star = libhfst.HfstTransducer("c", "d", type)
    cd_star.repeat_star()
    ab = libhfst.HfstTransducer("a", "b", type)
    test = libhfst.HfstTransducer(type)
    test.assign(cd_star)
    test.concatenate(ab)
    test.concatenate(cd_star)

    assert(t1.compare(test))
    assert(t2.compare(test))

    unk2unk = libhfst.HfstTransducer("@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", type)
    unk2unk.insert_freely(("c", "d"))
    dc = libhfst.HfstTransducer("d", "c", type)

    empty = libhfst.HfstTransducer(type)
    assert(not unk2unk.intersect(dc).compare(empty))

    unk2unk_ = libhfst.HfstTransducer("@_UNKNOWN_SYMBOL_@", "@_UNKNOWN_SYMBOL_@", type)
    cd_ = libhfst.HfstTransducer("c", "d", type)
    unk2unk_.insert_freely(cd_)

    dc_ = libhfst.HfstTransducer("d", "c", type)
    empty_ = libhfst.HfstTransducer(type)
    assert(not unk2unk_.intersect(dc_).compare(empty))
