## This Makefile is HFST (version 3) based reproduction of spell checking
## described in an essay by Peter Norvig at <>

all: norvig1.time norvig2.time norvig1.log norvig2.log

big.txt:
	wget http://norvig.com/big.txt

# original python code:
# def words(text): return re.findall('[a-z]+', text.lower()) 
words: big.txt
	tr -s '[:punct:][:digit:]' ' ' < $< | tr -s '[:space:]' '\n' | tr '[:upper:]' '[:lower:]' > $@

# original python code:
# def train(features):
#    model = collections.defaultdict(lambda: 1)
#    for f in features:
#        model[f] += 1
#    return model
train: words
	sort < $< | uniq -c | sort -nr > $@

# tropical conversion for ofst-tropical automaton format
tropical: train
	awk -f tropicalize-uniq-c.awk --assign CS=`wc -l < words` < $< > $@

# compile dictionary from the probabilities transformed to tropical semiring
dictionary.default.hfst: tropical
	hfst-strings2fst -j -f openfst-tropical $< -v -o $@

# in original python code:
# alphabet = 'abcdefghijklmnopqrstuvwxyz'
#
# def edits1(word):
#   splits     = [(word[:i], word[i:]) for i in range(len(word) + 1)]
#   deletes    = [a + b[1:] for a, b in splits if b]
#   transposes = [a + b[1] + b[0] + b[2:] for a, b in splits if len(b)>1]
#   replaces   = [a + c + b[1:] for a, b in splits for c in alphabet if b]
#   inserts    = [a + c + b     for a, b in splits for c in alphabet]
#   return set(deletes + transposes + replaces + inserts)
# NB: 14 = ceil(-log(1/CS))
edit-distance-2.txt: edit-distance.bash
	./edit-distance.bash 2 14 a b c d e f g h i j k l m n o p q r s t u v w x y z > $@

# we compile automaton from the generated tabular representation
error.edit-distance-2.hfst: edit-distance-2.txt
	hfst-txt2fst -f openfst-tropical -v -o $@ $<

# optimisatory move; we don't use OpenFst format but one specifically for
# spell-checking
%.hfstol: %.hfst
	hfst-minimize -v -i $< |\
		hfst-preprocess-for-optimized-lookup-format -v |\
		hfst-fst2fst -f olw -v -o $@

norvig1.log: norvig1.tests error.edit-distance-2.hfstol dictionary.default.hfstol
norvig2.log: norvig2.tests error.edit-distance-2.hfstol dictionary.default.hfstol
norvig1.time: norvig1.tests error.edit-distance-2.hfstol dictionary.default.hfstol
norvig2.time: norvig2.tests error.edit-distance-2.hfstol dictionary.default.hfstol

# original python code:
# def spelltest(tests, bias=None, verbose=False):
#    import time
#    n, bad, unknown, start = 0, 0, 0, time.clock()
#    if bias:
#        for target in tests: NWORDS[target] += bias
#    for target,wrongs in tests.items():
#        for wrong in wrongs.split():
#            n += 1
#            w = correct(wrong)
#            if w!=target:
#                bad += 1
#                unknown += (target not in NWORDS)
#                if verbose:
#                    print 'correct(%r) => %r (%d); expected %r (%d)' % (
#                        wrong, w, NWORDS[w], target, NWORDS[target])
#    return dict(bad=bad, n=n, bias=bias, pct=int(100. - 100.*bad/n), 
#                unknown=unknown, secs=int(time.clock()-start) )
#
# NB: yeah I modded our c++ spell checker driver for this
%.time: %.tests
	@date '+%Y%m%dT%H%M%s,%N'
	time hfst-ospell-norvig error.edit-distance-2.hfstol dictionary.default.hfstol < $< > $@
	@cat $@
	@date '+%Y%m%dT%H%M%s,%N'

%.log: %.tests
	hfst-ospell-norvig -v error.edit-distance-2.hfstol dictionary.default.hfstol < $< > $@

clean:
	-rm -f norvig?.time norvig?.log *.hfst *.hfstol words train tropical edit-distance-2.txt
