#!/usr/bin/env python
# -*- coding: utf8 -*-
# script to convert hunspell dic + aff to Xerox lexc/twolc

from sys import stdin, stdout, argv, exit
from os.path import basename
from getopt import getopt, GetoptError
from codecs import open
from math import log

def usage():
	"""
	Standard usage message.
	"""
	print "Usage: %s [settings]" %(basename(argv[0]))
	print "Convert blah"
	print
	print "  -h, --help          print this help"
	print "  -i, --input=I       read input from I.tex"
	print "  -o, --output=O      write output to O.?.twolc"
	print



def main():
	input_file = stdin
	input_file_name = False
	htwolc_files = [stdout]*7
	htwolc_file_prefix = False

	try:
		opts, args = getopt(argv[1:], "hi:o:", 
					["help", "input=", "output="])
	except GetoptError:
			usage()
			exit(2)
	for opt, arg in opts:
		if opt in ("-h", "--help"):
			usage()
			exit()
		elif opt in ("-i", "--input"):
			input_file_name = arg
		elif opt in ("-o", "--output"):
			htwolc_file_prefix = arg
		else:
			usage()
	if htwolc_file_prefix:
		for i in range(1, 7):
			htwolc_files[i] = open(htwolc_file_prefix +"."+str(i)+".htwolc", 'w', 'UTF-8')
	else:
		print "no --output spec, exit"
		exit(1)
	if input_file_name:
		input_file = open(input_file_name, 'r', 'UTF-8')
	else:
		print "no --input= specified, using stdin"
	
	line = input_file.readline().strip()
	in_patterns = False
	contexts = list()
	alphabet = set()
	for i in range(0, 8):
		contexts.append([])
	# collect patterns from file
	while line:
		if not in_patterns:
			if line.startswith('\patterns'):
				in_patterns = True
			line = input_file.readline().strip()
			continue
		if '%' in line:
			line = line[:line.find('%')].strip()
		if '^^' in line:
			print '^^ found, please replace by proper UTF-8'
			exit(1)
		if not line or line == '':
			line = input_file.readline().strip()
			continue
		elif line.startswith('}'):
			break
		things = line.split()
		for thing in things:
			for i in range(1,7):
				if str(i) in thing:
					if i % 2 == 0:
						contexts[i] += [' '.join(thing.replace(str(i), '_', 1).replace('1', '').replace('2','').replace('3','').replace('4','').replace('5','').replace('6','').replace('.','#'))]
					else:
						context = ' (0:%-) '.join(thing.replace(str(i), '_', 1).replace('1', '').replace('2', '').replace('3', '').replace('4', '').replace('5','').replace('.','#')).replace('(0:%-) _', '_').replace('_ (0:%-)', ' _ ')
						if thing.find(str(i)) < 1:
							context = '\\0:%- ' + context
						elif thing.find(str(i)) > (len(thing) - 2):
							context = context + ' \\0:%-'
						contexts[i] += [context]

					alphabet.update(set(thing))
		line = input_file.readline().strip()
	input_file.close()
	# make up alphabets
	for i in range(1,7):
		print >> htwolc_files[i], "Alphabet"
		print >> htwolc_files[i], "%-"
		if i % 2 == 0:
			print >> htwolc_files[i], '%-:0'
		else:
			print >> htwolc_files[i], '0:%-'
		print >> htwolc_files[i], ' '.join(alphabet)
		print >> htwolc_files[i], ";"
	for i in range(1, 7):
		print >> htwolc_files[i], "Rules"
		if len(contexts[i]) <= 1:
			if i % 2 == 0:
				print >> htwolc_files[i], '"No hyphen deletions %d"' %(i)
				print >> htwolc_files[i], "%-:0 /<= _ ;"
			else:
				print >> htwolc_files[i], '"No hyphen insertions %d"' %(i)
				print >> htwolc_files[i], "0:%- /<= _ ;"
		else:
			if i % 2 == 0:
				print >> htwolc_files[i], '"Hyphen deletion rule %d"' %(i)
				print >> htwolc_files[i], "%-:0 <=> "
			else:
				print >> htwolc_files[i], '"Hyphen insertion rule %d"' %(i)
				print >> htwolc_files[i], "0:%- <=> "
			print >> htwolc_files[i], " ;\n".join(contexts[i])
			print >> htwolc_files[i], " ; "
		htwolc_files[i].close()

	exit()


if __name__ == "__main__":
	main()

