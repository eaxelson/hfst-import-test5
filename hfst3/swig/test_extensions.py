import libhfst
import StringIO

output = StringIO.StringIO()

tr = libhfst.HfstTransducer('a', 'b', libhfst.tropical_openfst_type())
paths = libhfst.extract_paths(tr)
for path in libhfst.detokenize_paths(paths):
    print >>output, "%s:%s  %f" % (path.input, path.output, path.weight)

tr = libhfst.HfstTransducer('a', 'b', libhfst.tropical_openfst_type())
tr.convert(libhfst.hfst_olw_type())
for path in libhfst.detokenize_paths(tr.lookup("a")):
    print >>output, "%s  %f" % (path.output, path.weight)

