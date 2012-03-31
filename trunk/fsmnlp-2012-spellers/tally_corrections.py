import sys, re

misspelled_corrections_f = open(sys.argv[1], 'r')
# we also want to find out which words were in the dictionary in the first place
correct_corrections_f = open(sys.argv[2], 'r')
original_f = open(sys.argv[3], 'r')

total = 0
correct_corrections = 0

# gather the tokens in the dictionary
dictwords = set([])
for line in correct_corrections.readlines():
    try: # blank lines won't split
        inword, outword = line.split('\t')[:2]
        if inword == outword:
            dictwords.add(inword.strip())
    except:
        pass

misspelled_corrections = misspelled_corrections_f.readlines()
misspelled_linenum = 0

def search_to_next_correctionset():
    while misspelled_corrections[misspelled_linenum].strip() != '':
        misspelled_linenum += 1
        if misspelled_linenum >= len(misspelled_corrections):
            return
    misspelled_linenum += 1

def search_for_correct_correction(correct):
    found_correct = False
    while misspelled_corrections[misspelled_linenum].strip() != '':
        if misspelled_corrections[misspelled_linenum].strip() == correct:
            found_correct = True
        misspelled_linenum += 1
    misspelled_linenum += 1
    return found_correct

for original_line in original_f.readlines():
    correct = original_line.strip()
    if correct not in dictwords:
        search_to_next_correctionset()
        continue
    if re.match(r'".*" is in the lexicon',
                misspelled_corrections[misspelled_linenum]):
        search_to_next_correctionset()
        continue
    # we really have a misspelled word
    total += 1
    if re.match(r'Unable to correct ".*"!',
                misspelled_corrections[misspelled_linenum]):
        search_to_next_correctionset()
        continue
    # we might have actually corrected it
    corrected_correctly += search_for_correct_correction(correct)

print(float(correct_corrections)/total)
