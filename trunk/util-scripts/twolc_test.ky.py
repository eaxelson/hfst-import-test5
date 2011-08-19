#!/usr/bin/env python3

from twolclib import twolclib

inputfile = "twolc_test.ky.dat"

kyTwolc = twolclib('/home/jonathan/quick/apertium/svn/incubator/apertium-tr-ky/.deps/ky.twol.hfst')
kyTwolc.load_rules()

inputs = []
for line in open(inputfile, 'r'):
	if line[0] != '#' and line[0] != '\n':
		forms = line.split(',')
		newinput = (forms[0].strip(), forms[1].strip(), forms[2].strip())
		print(newinput)
		inputs += [newinput]

kyTwolc.add_inputs(inputs)

#kyTwolc.get_output_of_all_rules()
kyTwolc.process_input_forms()
kyTwolc.process_output_forms()
#print(kyTwolc.forms)
#print(kyTwolc.rules[24])
#print(kyTwolc.get_forms_with_incorrect_output())
#incorrectOutputs = kyTwolc.get_forms_with_incorrect_output()
#rulesExcludingOutputs = kyTwolc.get_rules_excluding_correct()
#print(rulesExcludingOutputs)

for inputform in inputs:
	for rule in kyTwolc.get_rules_excluding_form(inputform[0], inputform[1]):
		# This outputs the twol rule(s) that exclude(s) the correct output
		print(kyTwolc.rules[rule]['name'])

kyTwolc.close()
