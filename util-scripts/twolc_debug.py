#!/usr/bin/env python3

import os, re, argparse
from subprocess import Popen, PIPE

parser = argparse.ArgumentParser(prog='twolc_debug', description='Tells which twol rules eliminate which elements of a given form')
parser.add_argument('-t', '--twol', nargs=1, dest='twolcFile', help='The compiled twol.hfst file')
parser.add_argument('form', nargs='+', help='A form or forms to parse')

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
	count = 0
	for line in output.split('\n'):
		if line != "":
			count+=1
			yield (count, reRuletext.match(line).groups()[0])

def get_forms(form, numRules, twolcFile):
	#echo "с ү й > {I} п" | hfst-strings2fst -S | hfst-duplicate -n 28 | hfst-compose .deps/ky.twol.hfst | hfst-fst2strings
	p1 = Popen(["echo", form], stdout=PIPE)
	p2 = Popen(["hfst-strings2fst", "-S"], stdin=p1.stdout, stdout=PIPE)
	p3 = Popen(["hfst-duplicate", "-n", str(numRules)], stdin=p2.stdout, stdout=PIPE)
	p4 = Popen(["hfst-compose", twolcFile], stdin=p3.stdout, stdout=PIPE)
	p5 = Popen(["hfst-fst2strings"], stdin=p4.stdout, stdout=PIPE)
	p1.stdout.close()
	p2.stdout.close()
	p3.stdout.close()
	p4.stdout.close()
	output = p5.communicate()[0].decode('utf-8')
	count = 0
	for block in output.split('--'):
		count += 1
		yield (count, block.strip('\n').split('\n'))

def get_all_forms(blocks):
	allForms = set()
	for block in blocks:
		for line in blocks[block]:
			allForms.add(line)
	return allForms

def main_loop(twolcFile, form):
	rules = {}
	for (num, rule) in get_rule_names(twolcFile):
		rules[num] = rule
	numRules = len(rules)
	blocks = {}
	for (count, block) in get_forms(form, numRules, twolcFile):
		blocks[count] = block

	allForms = get_all_forms(blocks)

	for i in range(1,numRules):
		eliminatedForms = allForms - set(blocks[i])
		if eliminatedForms != set():
			print(rules[i])
			print("\teliminates", eliminatedForms)

args = parser.parse_args()

if args.form != None and args.twolcFile != None:
	for form in args.form:
		print('\n'+form+'\n')
		main_loop(args.twolcFile[0], form)
#elif args.twolcFile != None:
else:
	parser.print_help()
	
