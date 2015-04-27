print('Testing libhfst module...')

import libhfst

tr1 = libhfst.HfstTransducer('foo', 'bar', libhfst.TROPICAL_OPENFST_TYPE)
tr2 = libhfst.HfstTransducer('bar', 'baz', libhfst.TROPICAL_OPENFST_TYPE)

tr1.concatenate(tr2)
print(tr1)
print('--')
print(tr2)

print('Tests passed.')
