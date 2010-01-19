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
	print "  -i, --input=I       read input from I.dic and I.aff"
	print "  -o, --output=O      write output to O.hlexc and O.twolc"
	print



def main():
	input_file_prefix = False
	output_file_prefix = False
	dic_file = stdin
	dic_file_name = False
	aff_file = stdin
	aff_file_name = False
	hlexc_file = stdout
	hlexc_file_name = False
	htwolc_file = stdout
	htwolc_file_name = False

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
			input_file_prefix = arg
		elif opt in ("-o", "--output"):
			output_file_prefix = arg
		else:
			usage()
	if input_file_prefix:
		dic_file_name = input_file_prefix + ".dic"
		aff_file_name = input_file_prefix + ".aff"
		dic_file = open(dic_file_name, "r", "UTF-8")
		aff_file = open(aff_file_name, "r", "UTF-8")
	else:
		print "no --input spec, exit"
		exit(1)
	if output_file_prefix:
		hlexc_file_name = output_file_prefix + ".hlexc"
		htwolc_file_name = output_file_prefix + ".htwolc"
		hlexc_file = open(hlexc_file_name, "w", "UTF-8")
		htwolc_file = open(htwolc_file_name, "w", "UTF-8")
	else:
		print "no --output= specified, using stdout"
	# read flags needed at beginning of the lexc
	any_flags = False
	print >> htwolc_file, "Alphabet"
	flagnames = dict()
	pairs = dict()
	pairs["<"] = True
	pairs[">"] = True
	pairs["<:0"] = True
	pairs[">:0"] = True
	pairs["0"] = True
	pairs["0:0"] = True
	print >> htwolc_file, "%<:0 %>:0 %< %> 0:%< 0:%>"
	aff_line = aff_file.readline()
	while aff_line:
		if aff_line.startswith("PFX"):
			if not any_flags:
				print >> hlexc_file, "Multichar_Symbols"
				any_flags = True
			pfxflag = aff_line.split()[1].strip()
			if not flagnames.has_key(pfxflag):
				print >> hlexc_file, "@R.%s.ON@ @P.%s.ON@ @D.%s.ON@ @C.%s@ " %(pfxflag, pfxflag, pfxflag, pfxflag)
				flagnames[pfxflag] = True
		if (aff_line.startswith("PFX") or aff_line.startswith("SFX")) and len(aff_line.split()) > 4:
			aff_fields = aff_line.split()
			for char in aff_fields[2]:
				if char in "-!@+:/\\^":
					char = "%" + char
				if not pairs.has_key(char):
					print >> htwolc_file, "%s " %(char),
					pairs[char] = True
				if not pairs.has_key(char + ":0"):
					print >> htwolc_file, "%s:0 " %(char),
					pairs[char + ":0"] = True
		aff_line = aff_file.readline()
	aff_file.seek(0)
	dic_size = int(dic_file.readline())
	print >> hlexc_file, "LEXICON Root"
	print >> hlexc_file, "##:0	Real_Root	;"
	print >> hlexc_file
	print >> hlexc_file, "LEXICON Real_Root"
	for i in range(dic_size):
		dic_line = dic_file.readline()
		print "reading dictionary %d of %d\r" %(i, dic_size),
		dic_fields = dic_line.split("/")
		dic_string = False
		dic_conts = False
		if (len(dic_fields) == 2):
			dic_string = dic_fields[0]
			dic_conts = dic_fields[1].strip()
		elif (len(dic_fields) < 2):
			dic_string = dic_line.strip()
			dic_conts = ""
		else:
			print "Broken line %s in %s near line %d" %(dic_line, dic_file_name,
					i)
			continue
		print >> hlexc_file, "%s	Final	;" %(dic_string)
		for contchar in dic_conts:
			print >> hlexc_file, "%s	%s	;" %(dic_string, contchar)
		for char in dic_string:
			if char in "-!@+/\\^:":
				char = "%" + char
			if not pairs.has_key(char):
				print >> htwolc_file, "%s " % (char)
				pairs[char] = True
	print >> htwolc_file, ";"
	print >> htwolc_file, "Rules"
	dic_file.close()
	print >> hlexc_file
	print >> hlexc_file, "LEXICON Final"
	killflags = ""
	for flag in flagnames.keys():
		killflags += "@D.%s.ON@" %(flag)
	print >> hlexc_file, "##:%s	#	;" %(killflags)
	aff_line = aff_file.readline()
	while aff_line:
		aff_fields = aff_line.split()
		if aff_line.startswith("SFX"):
			print >> hlexc_file, "LEXICON %s" % (aff_fields[1])
			print >> htwolc_file, "\"%s deletions and contexts\"" % (aff_fields[1])
			print >> htwolc_file, "%s => " %(aff_fields[1]),
			for i in range(int(aff_fields[3])):
				suff_line = aff_file.readline()
				suff_fields = suff_line.split()
				suffix_parts = suff_fields[3].split("/")
				suffix_string = False
				suffix_conts = False
				if len(suffix_parts) == 2:
					suffix_string = suffix_parts[0]
					suffix_conts = suffix_parts[1].strip()
				elif len(suffix_parts) < 2:
					suffix_string = suff_fields[3].strip()
					suffix_conts = ""
				print >> hlexc_file, "%s%%>%s	Final	;" % (suff_fields[1], suffix_string)
				for cont in suffix_conts:
					print >> "%s%%>%s	%s	;" %(suff_fields[1], suffix_string, cont)
				context = ""
				if suff_fields[4] != ".":
					lbr = False
					firstbr = False
					inbr = False
					for char in suff_fields[4]:
						if lbr and char == '^':
							context += '\[' + " "
							lbr = False
							firstbr = True
						elif lbr:
							context += '[' + char + " "
							lbr = False
							firstbr = True
						elif inbr and char == ']':
							context += ']'
							inbr = False
							firstbr = False
							lbr = False
						elif inbr:
							context += "| " + char + " "
						elif char == '[':
							lbr = True
						elif firstbr:
							context += char + " "
							firstbr = False
							inbr = True
						else:
							context += char + " "
				replacement = ""
				if suff_fields[2] != '0':
					for char in suff_fields[2]:
						replacement += char + " "
				print >> htwolc_file, "%s 0:%%< %s _ %%> ;" % (context, replacement)
		elif aff_line.startswith("PFX"):
			print >> hlexc_file, "LEXICON %s" % (aff_fields[1])
			print >> hlexc_file, "@R.%s.ON@@C.%s@	Final	;" %(aff_fields[1], aff_fields[1])
			print >> hlexc_file
			print >> hlexc_file, "LEXICON Prefixes"
			print >> htwolc_file, "Rule \"%s deletions and contexts\"" % (aff_fields[1])
			print >> htwolc_file, "%s => " %(aff_fields[1]),
			for i in range(int(aff_fields[3])):
				pref_line = aff_file.readline()
				pref_fields = pref_line.split()
				prefix_parts = pref_fields[3].split("/")
				prefix_string = False
				prefix_conts = False
				if len(prefix_parts) == 2:
					prefix_string = prefix_parts[0]
					prefix_conts = prefix_parts[1].strip()
				elif len(prefix_parts) < 2:
					prefix_string = pref_fields[3].strip()
					prefix_conts = ""
				print >> hlexc_file, "@P.%s.ON@%s%%<%s	Real_Root	;" %(pref_fields[1], prefix_string, pref_fields[1])
				for cont in prefix_conts:
					print >> "@P.%s.ON@%s%%<%s	%s	;" %(pref_fields[1], prefix_string, pref_fields[1], pref_fields[1])
				context = ""
				if suff_fields[4] != ".":
					lbr = False
					inbr = False
					firstbr = False
					for char in suff_fields[4]:
						if lbr and char == '^':
							context += '\[' + " "
							lbr = False
							firstbr = True
						elif lbr:
							context += '[' + char + " "
							lbr = False
							firstbr = True
						elif inbr and char == ']':
							context += ']'
							inbr = False
						elif inbr:
							context += "| " + char + " "
						elif char == '[':
							lbr = True
						elif firstbr:
							context += char + " "
							inbr = True
							firstbr = False
							lbr = False
						else:
							context += char + " "
				replacement = ""
				if suff_fields[2] != '0':
					for char in suff_fields[2]:
						replacement += char + " "
				print >> htwolc_file, "%%< %s _ 0:%%> %s ;" % (replacement, context)

		elif aff_line.startswith("COMPOUNDFLAG"):
			compoundlex = aff_line.split()[1].strip()
			print >> hlexc_file, "LEXICON %s" % (compoundlex)
			print >> hlexc_file, "	Real_Root	;"
			print >> hlexc_file
		else:
			aff_line = aff_file.readline()
			continue
		aff_line = aff_file.readline()
	hlexc_file.close()
	htwolc_file.close()
	exit()


if __name__ == "__main__":
	main()

