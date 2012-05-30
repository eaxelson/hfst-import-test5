#!/usr/bin/env python3

from twolclib import twolclib

inputfile = "twolc_test.cv.dat"

cvTwolc = twolclib('/home/jonathan/quick/apertium/svn/incubator/apertium-cv-tr/.deps/cv.LR.twol.hfst')
cvTwolc.lexc('/home/jonathan/quick/apertium/svn/incubator/apertium-cv-tr/.deps/cv.LR.lexc.hfst')
cvTwolc.load_rules()

#inputs = [
#	(None, 'музея', 'музей<n><dat>')
#]
#cvTwolc.add_inputs(inputs)
#cvTwolc.get_phonolforms()

inputs = []
for line in open(inputfile, 'r'):
	if line[0] != '#' and line[0] != '\n':
		forms = line.split(',')
		if forms[0].strip() == '':
			forms[0] = None
		else:
			forms[0] = forms[0].strip()
		newinput = (forms[0], forms[1].strip(), forms[2].strip())
		#print(newinput)
		inputs += [newinput]

if cvTwolc.debug:
	print(inputs)
cvTwolc.add_inputs(inputs)
cvTwolc.get_phonolforms()
#print(cvTwolc.forms, cvTwolc.rules)

##kyTwolc.get_output_of_all_rules()
cvTwolc.process_input_forms()
#print(cvTwolc.forms)
cvTwolc.process_output_forms()
#print(cvTwolc.forms)
##print(kyTwolc.forms)
##print(kyTwolc.rules[24])
##print(kyTwolc.get_forms_with_incorrect_output())
##incorrectOutputs = kyTwolc.get_forms_with_incorrect_output()
##rulesExcludingOutputs = kyTwolc.get_rules_excluding_correct()
##print(rulesExcludingOutputs)

print("====")
outputs = cvTwolc.get_rules_excluding_correct()
for output in outputs:
	rules = outputs[output]
	print("The following rule(s) prevent the correct output of %s:" % output)
	for rule in rules:
		print("--> rule %s: %s" % ( rule, cvTwolc.rules[rule]['name'])) 
	if len(rules)==0:
		print("    no rules prevent correct output!")


#for inputform in inputs:
#	print(inputform)
#	for rule in cvTwolc.get_rules_excluding_form(inputform[0], inputform[1]):
#		# This outputs the twol rule(s) that exclude(s) the correct output
#		print(cvTwolc.rules[rule]['name'])

cvTwolc.close()
