import libhfst
import StringIO

output = StringIO.StringIO()

tr = libhfst.HfstTransducer('a', 'b', libhfst.TROPICAL_OPENFST_TYPE)
paths = libhfst.extract_paths(tr)
for path in libhfst.detokenize_paths(paths):
    print >>output, "%s:%s  %f" % (path.input, path.output, path.weight)

tr = libhfst.HfstTransducer('a', 'b', libhfst.TROPICAL_OPENFST_TYPE)
tr.convert(libhfst.HFST_OLW_TYPE)
for path in libhfst.detokenize_paths(tr.lookup("a")):
    print >>output, "%s  %f" % (path.output, path.weight)

