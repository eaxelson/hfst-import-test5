#!/usr/bin/env python3

import os, re, argparse
from subprocess import Popen, PIPE
from colorama import Fore, Back

parser = argparse.ArgumentParser(prog='twolc_debug', description='Tells which twol rules exclude which potential outputs of a given form')
parser.add_argument('-t', '--twol', nargs=1, dest='twolcFile', help='The compiled twol.hfst file')
parser.add_argument('form', nargs='+', help='A form or forms to parse')
parser.add_argument('-c', '--correct', dest='correct', help='Correct output of twolc')
parser.add_argument('-s', '--showforms', dest='showforms', action='store_true', help='Show all forms created and excluded by rule')
#parser.add_argument('-w', '--why', dest='why', action='store_true', help='Instead of output forms, tell why the correct form is not being produced')

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
	p5 = Popen(["hfst-fst2strings", "-c 10"], stdin=p4.stdout, stdout=PIPE)
	p1.stdout.close()
	p2.stdout.close()
	p3.stdout.close()
	p4.stdout.close()
	output = p5.communicate()[0].decode('utf-8')
	for block in output.split('--'):
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

def get_rules_allowing_correct(allForms, ruleSet, correct):
	outRules = set()
	for (rule, forms) in ruleSet:
		allowedForms = allForms - forms
		if correct in allowedForms:
			outRules.add(rule)
	return outRules

def get_rules_with_nothing(ruleSet, correct):
	outRules = set()
	for (rule, forms) in ruleSet:
		if len(forms)==0: # or forms == ['']:
			outRules.add(rule)
	return outRules

def main_loop(twolcFile, inputForm, correct, showforms):
	rules = []
	blocks = []
	ruleSet = []

	# There must be a more efficient way to do this
	# problem is, get_forms needs numRules
	for rule in get_rule_names(twolcFile):
		rules += [rule]
	numRules = len(rules)
	for block in get_forms(inputForm, numRules, twolcFile):
		blocks += [block]
	for (rule, block) in zip(rules, blocks):
		ruleSet += [(rule, block)]

	allForms = get_all_forms(blocks)

	rulesExcludingCorrect = get_rules_excluding_correct(ruleSet, correct)
	rulesAllowingCorrect = get_rules_allowing_correct(allForms, ruleSet, correct)
	rulesWithNoExcludes = get_rules_with_nothing(ruleSet, correct)
	correctAllowed = correct in allForms

	if showforms:
		#TODO: ideally make a table or something
		for (rule, excludedForms) in ruleSet:
			allowedForms = allForms - excludedForms
			print(rule+" EXCLUDES:")
			for form in excludedForms:
				if form==correct:
					print("\t"+Back.RED+form+Back.RESET)
				else:
					print("\t"+form)
			print(rule+" ALLOWS:")
			for form in allowedForms:
				if form==correct:
					print("\t"+Back.GREEN+form+Back.RESET)
				else:
					print("\t"+form)

	#TODO: make less ugly
	#Instead of why_loop:
	print()
	print("These rules allow the correct form: \n\t"+str(rulesAllowingCorrect))
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
	else:
		print()
		print("It looks like the correct form is generated somewhere (either as possible or exclude).")


args = parser.parse_args()

if args.form != None and args.twolcFile != None:
	for form in args.form:
		print('\n'+form+'\n')
		main_loop(args.twolcFile[0], form, args.correct, args.showforms)

#elif args.twolcFile != None:
else:
	parser.print_help()	
