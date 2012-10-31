import libhfst

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


