#!/usr/bin/env python3

import os, re, argparse
from subprocess import Popen, PIPE

parser = argparse.ArgumentParser(prog='twolc_debug', description='Tells which twol rules exclude which potential outputs of a given form')
parser.add_argument('-t', '--twol', nargs=1, dest='twolcFile', help='The compiled twol.hfst file')
parser.add_argument('form', nargs='+', help='A form or forms to parse')
parser.add_argument('-c', '--correct', dest='correct', help='Correct output of twolc')
#parser.add_argument('-p', '--potential', dest='potential', action='store_true', help='Show all potential outputs produced by rule, as opposed to all outputs excluded by rule')
parser.add_argument('-s', '--showforms', dest='showforms', action='store_true', help='Show all forms created and excluded by rule')
parser.add_argument('-w', '--why', dest='why', action='store_true', help='Instead of output forms, tell why the correct form is not being produced')

#twolcFile = "/home/jonathan/quick/apertium/svn/incubator/apertium-tr-ky/.deps/ky.twol.hfst"
#form = "с ү й > {I} п"

reRuletext = re.compile('^name: "(.*)"')

# get list of rule names in correct order
def get_rule_names(twolcFile):
	global reRuletext
	#hfst-summarize twolcFile | fgrep name
	p1 = Popen(["hfst-summarize", twolcFile], stdout=PIPE)
	p2 = Popen(["fgrep", "name"], stdin=p1.stdout, stdout=PIPE)
	p1.stdout.close()
	output = p2.communicate()[0].decode('utf-8')
	#output = output.decode('utf-8')
	#count = 0
	for line in output.split('\n'):
		if line != "":
			#count+=1
			#yield (count, reRuletext.match(line).groups()[0])
			yield reRuletext.match(line).groups()[0]

# get list of all forms allowed by the grammar
def get_forms(form, numRules, twolcFile):
	#echo "с ү й > {I} п" | hfst-strings2fst -S | hfst-duplicate -n 28 | hfst-compose .deps/ky.twol.hfst | hfst-fst2strings
	p1 = Popen(["echo", form], stdout=PIPE)
	p2 = Popen(["hfst-strings2fst", "-S"], stdin=p1.stdout, stdout=PIPE)
	p3 = Popen(["hfst-duplicate", "-n", str(numRules)], stdin=p2.stdout, stdout=PIPE)
	p4 = Popen(["hfst-compose", twolcFile], stdin=p3.stdout, stdout=PIPE)
	# , "-N 10"
	p5 = Popen(["hfst-fst2strings", "-c 1"], stdin=p4.stdout, stdout=PIPE)
	p1.stdout.close()
	p2.stdout.close()
	p3.stdout.close()
	p4.stdout.close()
	output = p5.communicate()[0].decode('utf-8')
	#count = 0
	for block in output.split('--'):
		#count += 1
		#yield (count, block.strip('\n').split('\n'))
		#yield block.strip('\n').split('\n')
		thisBlock = set()
		for form in block.strip('\n').split('\n'):
			if	re.search(':', form):
				thisBlock.add(form.split(':')[1])
		yield thisBlock

def get_all_forms(blocks):
	allForms = set()
	for block in blocks:
		for line in block:
			allForms.add(line)
	return allForms

def get_rules_excluding_correct(ruleSet, correct):
	outRules = set()
	for (rule, forms) in ruleSet:
		if correct in forms:
			outRules.add(rule)
	return outRules

def get_rules_with_nothing(ruleSet, correct):
	outRules = set()
	for (rule, forms) in ruleSet:
		if len(forms)==0: # or forms == ['']:
			outRules.add(rule)
	return outRules

def main_loop(twolcFile, inputForm, correct, showforms, why):
	#rules = {}
	#for (num, rule) in get_rule_names(twolcFile):
	#	rules[num] = rule
	#blocks = {}
	#for (count, block) in get_forms(inputForm, numRules, twolcFile):
	#	blocks[count] = block

	rules = []
	blocks = []

	for rule in get_rule_names(twolcFile):
		rules += [rule]

	numRules = len(rules)
	for block in get_forms(inputForm, numRules, twolcFile):
		blocks += [block]


	#ruleSet = {}
	#for i in range(1,numRules):
	#	ruleSet[i] = (rules[i], blocks[i])
	ruleSet = []
	for (rule, block) in zip(rules, blocks):
		ruleSet += [(rule, block)]

	allForms = get_all_forms(blocks)

	rulesExcludingCorrect = get_rules_excluding_correct(ruleSet, correct)
	#rulesAllowingCorrect = get_rules_allowing_correct(ruleSet, correct)
	rulesWithNoExcludes = get_rules_with_nothing(ruleSet, correct)
	correctAllowed = correct in allForms

	if showforms:
		for (rule, excludedForms) in ruleSet:
			allowedForms = allForms - excludedForms
			print(rule+" EXCLUDES:")
			for form in excludedForms:
				print("\t"+form)
			print(rule+" ALLOWS:")
			for form in allowedForms:
				print("\t"+form)
		#print(showforms)
		#TODO:
		# this will output
		# a table of forms created and excluded by each rule
		# in place of the crap below

	##TODO: make less ugly
	#if potential:
	#	for i in range(1,numRules):
	#		potentialForms = set(blocks[i])
	#		print(rules[i])
	#		if potentialForms != set() and potentialForms != {''}:
	#			print("\tcreates", )
	#			nothing = True
	#			for form in potentialForms:
	#				if form != "":
	#					#print(form.split(':')[1], correct)
	#					if form.split(':')[1] == correct:
	#						print('\t\t\033[1;31m'+form+'\033[1;m', )
	#					else:
	#						print('\t\t'+form, )
	#					nothing = False
	#			if nothing:
	#				print("\t\t\033[1;31mNOTHING\033[1;m")
	#		else:
	#			print("\t\t\033[1;31mNOTHING\033[1;m")
	#else:
	#	for i in range(1,numRules):
	#		excludedForms = allForms - set(blocks[i])
	#		print(rules[i])
	#		if excludedForms != set() and excludedForms != {''}:
	#			print("\texcludes", )
	#			nothing = True
	#			for form in excludedForms:
	#				if form != "":
	#					#print(form.split(':')[1], correct)
	#					if form.split(':')[1] == correct:
	#						print('\t\t\033[1;31m'+form+'\033[1;m', )
	#					else:
	#						print('\t\t'+form, )
	#					nothing = False
	#			if nothing:
	#				print("\t\t\033[1;31mNOTHING\033[1;m")
	#		else:
	#			print("\t\t\033[1;31mNOTHING\033[1;m")

	#Instead of why_loop:
	print()
	print("These rules don't exclude any possible forms: \n\t"+str(rulesWithNoExcludes))
	print()
	print("Furthermore, these rules exclude the correct form: \n\t"+str(rulesExcludingCorrect))
	if not correctAllowed:
		print()
		print("However, it looks like the correct form was never generated!")
	if len(rulesWithNoExcludes)==0 and len(rulesExcludingCorrect)==0 and correctAllowed:
		print()
		print("There doesn't seem to be anything wrong, ‹"+correct+"› should be output\n")



#def why_loop(twolcFile, form, correct, potential):
#	rules = {}
#	for (num, rule) in get_rule_names(twolcFile):
#		rules[num] = rule
#	numRules = len(rules)
#	blocks = {}
#	for (count, block) in get_forms(form, numRules, twolcFile):
#		blocks[count] = block
#
#	allForms = get_all_forms(blocks)
#
#	found = 0
#	excluded = False
#	correctPre = re.sub(' ','',form)+":"+correct
#	for i in range(1,numRules):
#		potentialForms = set(blocks[i])
#		excludedForms = allForms - set(blocks[i])
#		if potentialForms != set() and potentialForms != {''} :
#			if correctPre in excludedForms:
#				print(rules[i]+" excludes "+correctPre)
#				excluded = True
#			if correctPre in potentialForms:
#				#print(rules[i]+" creates form "+form)
#				found += 1
#			#else:
#			#	print(rules[i]+" does not create "+correctPre)
#				
#		else:
#			print("No forms output by rule "+rules[i]+"!")
#
#	if found==0:
#		print("Correct form ‹ "+correct+" › never created!")
#	elif not excluded:
#		print("This form should be working!")


args = parser.parse_args()

if args.form != None and args.twolcFile != None:
	for form in args.form:
		print('\n'+form+'\n')
		main_loop(args.twolcFile[0], form, args.correct, args.showforms, args.why)
		#if not args.why:
		#	main_loop(args.twolcFile[0], form, args.correct, args.potential)
		#else:
		#	why_loop(args.twolcFile[0], form, args.correct, args.potential)
#elif args.twolcFile != None:
else:
	parser.print_help()	
