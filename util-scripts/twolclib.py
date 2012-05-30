import os, re, argparse
import tempfile
from subprocess import Popen, PIPE
from colorama import Fore, Back

class twolclib:
	
	twolcFile = ""
	tempDir = ""
	numRules = -1
	rules = {}
	forms = []
	# a list of "form"s (next)
	form = {"lexc": None, "input": None, "outputs": {}, "output": None, "intended": None}
	# lexc: string = the input to lexc that should generate "input" (next)
	# input: string = the output of lexc / input of twol
	# outputs: dict{int: set, ...} = contains the set of the possible outputs after each rule is applied
	# output: None or [string, ...] = the string(s) created by twol from the input, or None if not processed yet (an empty list or a list with an empty string means processed and no valid output)
	# intended: string = the intended output of twol based on the input string
	reRuletext = re.compile('^name: "(.*)"')
	reTwolcNum = re.compile('twol\.(.*)\.hfst')
	twolcFileName = "twol.%s.hfst"
	processAll = True
	lexc = None

	def __init__(self, twolcFile):
		self.twolcFile = twolcFile

	## get list of rule names in correct order
	#def get_rule_names(twolcFile):
	#	reRuletext = self.reRuletext
	#	#hfst-summarize twolcFile | fgrep name
	#	p1 = Popen(["hfst-summarize", twolcFile], stdout=PIPE)
	#	p2 = Popen(["fgrep", "name"], stdin=p1.stdout, stdout=PIPE)
	#	p1.stdout.close()
	#	output = p2.communicate()[0].decode('utf-8')
	#	#output = output.decode('utf-8')
	#	#count = 0
	#	for line in output.split('\n'):
	#		if line != "":
	#			#count+=1
	#			#yield (count, reRuletext.match(line).groups()[0])
	#			yield reRuletext.match(line).groups()[0]
	#
	## get list of all forms allowed by the grammar
	#def get_forms(self, form):
	#	twolcFile = self.twolcFile
	#	if self.numRules == -1:
	#		self.load_rules()
	#	numRules = self.numRules
	#	#echo "с ү й > {I} п" | hfst-strings2fst -S | hfst-duplicate -n 28 | hfst-compose .deps/ky.twol.hfst | hfst-fst2strings
	#	p1 = Popen(["echo", form], stdout=PIPE)
	#	p2 = Popen(["hfst-strings2fst", "-S"], stdin=p1.stdout, stdout=PIPE)
	#	p3 = Popen(["hfst-duplicate", "-n", str(numRules)], stdin=p2.stdout, stdout=PIPE)
	#	p4 = Popen(["hfst-compose", twolcFile], stdin=p3.stdout, stdout=PIPE)
	#	p5 = Popen(["hfst-fst2strings", "-c 10"], stdin=p4.stdout, stdout=PIPE)
	#	p1.stdout.close()
	#	p2.stdout.close()
	#	p3.stdout.close()
	#	p4.stdout.close()
	#	output = p5.communicate()[0].decode('utf-8')
	#	for block in output.split('--'):
	#		thisBlock = set()
	#		for form in block.strip('\n').split('\n'):
	#			if	re.search(':', form):
	#				thisBlock.add(form.split(':')[1])
	#		yield thisBlock
	
	def load_rules(self):
		# fst-split --prefix ky.twol --extension=.hfst .deps/ky.twol.hfst
		twolcFile = self.twolcFile
		if self.tempDir == "":
			self.tempDir = os.path.join(tempfile.gettempdir(), "twolclib/")
			tempDir = self.tempDir
			try:
				os.mkdir(tempDir)
			except OSError as e:
				#directory probably already exists
				pass
		else:
			tempDir = self.tempDir
		p1 = Popen(["hfst-split", "--prefix="+os.path.join(tempDir, "twol."), "--extension=.hfst", twolcFile], stdout=PIPE)
		output = p1.communicate()

		ruleFiles = os.listdir(tempDir)

		for ruleFile in ruleFiles:
			ruleNum = int(self.reTwolcNum.search(ruleFile).groups()[0])
			#print(ruleNum)
			rule = {}
			thisRuleFile = os.path.join(tempDir, ruleFile)
			#print(thisRuleFile)
			p2 = Popen(["hfst-summarize", thisRuleFile], stdout=PIPE)
			fileInfo = p2.communicate()[0].decode('utf-8')
			for line in fileInfo.split('\n'):
				if ':' in line:
					#if len(line.split(': '))==2:
					(param, value) = line.split(': ', 1)
					rule[param] = value.strip(' "')
			rule["filename"] = os.path.join(self.tempDir, self.twolcFileName % str(ruleNum))
			self.rules[ruleNum] = rule
		#print(output)
		return "TODO"

	def close(self):
		self.rmdir()
		#return

	def rmdir(self):
		for root, dirs, files in os.walk(self.tempDir, topdown=False):
			for name in files:
				os.remove(os.path.join(root, name))
			for name in dirs:
				os.rmdir(os.path.join(root, name))
		os.rmdir(self.tempDir)

	def add_inputs(self, inputForms):
		for inputList in inputForms:
			print("IL: ",inputList)
			#print("a: "+inputForm)
			if isinstance(inputList, tuple):
				if len(inputList)==2:
					(inputForm, outputForm) = inputList
					lexcForm = None
				elif len(inputList)==3:
					(inputForm, outputForm, lexcForm) = inputList
			elif isinstance(inputList, str):
				inputForm = inputList
				lexcForm = None
				outputForm = None
			print(self.input_in_forms(inputForm), self.lexc_in_forms(lexcForm))
			if not self.input_in_forms(inputForm) or not self.lexc_in_forms(lexcForm):
				thisForm = self.form.copy()
				thisForm["input"] = inputForm
				thisForm["lexc"] = lexcForm
				thisForm["intended"] = outputForm
				#print("b: "+str(thisForm))
				self.forms += [thisForm]
			#print("c: "+str(self.forms))


	#def get_output_of_all_rules(self): #, inputForm): #, twolcFile):
		# fst-split --prefix ky.twol --extension=.hfst .deps/ky.twol.hfst
		# hfst-summarize ky.twol10.hfst | fgrep name
		# for c in $(seq 1 35 ); do echo "и т > {N} {I}" | hfst-strings2fst -S | hfst-compose-intersect ky.twol$c.hfst | hfst-fst2strings > ky.twol.$c.out ; done
		#print(self.rules[10])
		#print(is_dict(self.rules))
		#print(isinstance(self.rules, dict))
		#thisForm["input"] = inputForm
	def process_input_forms(self):
		for formDict in self.forms:
			inputForm = formDict["input"]
			#print(inputForm)
			reInputForm = re.compile(re.sub(' ', '', inputForm)+':')
			#forms = {}
			formDict["outputs"] = {}
			for ruleNum, ruleData in self.rules.items():
				#thisForm = self.form
				#print(ruleNum, ruleData)
				#print(ruleNum)
				#print(inputForm, ruleData["name"])
				print(ruleNum,": ",ruleData["name"])
				p1 = Popen(["echo", inputForm], stdout=PIPE)
				p2 = Popen(["hfst-strings2fst", "-S"], stdin=p1.stdout, stdout=PIPE)
				p3 = Popen(["hfst-compose-intersect", ruleData["filename"]], stdin=p2.stdout, stdout=PIPE)
				p4 = Popen(["hfst-fst2strings", "-c1"], stdin=p3.stdout, stdout=PIPE)
				p1.stdout.close()
				p2.stdout.close()
				p3.stdout.close()
				output = p4.communicate()[0].decode('utf-8')
				#print(output)
				####print(output)
				#forms[ruleNum] = set(output.split('\n'))
				formDict["outputs"][ruleNum] = set()
				if self.processAll:
					for result in output.split('\n'):
						outputForm = reInputForm.sub("", result)
						if outputForm != "":
							#ruleSet.add(outputForm)
							formDict["outputs"][ruleNum].add(outputForm)
							#print(outputForm, )
				else:
					for result in output.split('\n'):
						outputForm = reInputForm.sub("", result)
						#print(outputForm+"≈"+formDict["intended"])
						if outputForm == formDict["intended"] and outputForm != "":
							formDict["outputs"][ruleNum].add(outputForm)
						
				#print("rule %s: %s forms" % (str(ruleNum), str(len(ruleSet))))
				print("rule %s: %s forms" % (str(ruleNum), str(len(formDict["outputs"][ruleNum]))))
				#formDict["outputs"][ruleNum] = ruleSet
				#print(output)
			#thisForm["outputs"] += [forms]
			#self.set_outputs_for_input(inputForm, forms)
			#self.forms += [thisForm]
	
	#def set_outputs_for_input(self, inputForm, outputForms):
	#	if self.input_in_forms(inputForm):
	#		for form in self.forms:
	#			if form.get('input') == inputForm:
	#				form["outputs"] = outputForms
	#				break
	#	else:
	#		thisForm = self.form
	#		thisForm["input"] = inputForm
	#		thisForm["outputs"] = outputForms
	#		self.forms += [thisForm]
	#
	def input_in_forms(self, inputForm):
		for form in self.forms:
			if form["input"] == inputForm:
				return True
		return False
	
	def lexc_in_forms(self, lexcForm):
		for form in self.forms:
			if form["lexc"] == lexcForm:
				return True
		return False

	#def process_form(inputForm, twolcFile):
	def process_output_forms(self):
		for formDict in self.forms:
			inputForm = formDict["input"]
			output = []
			# echo "к о й > {I} ш" | hfst-strings2fst -S | hfst-compose-intersect .deps/ky.twol.hfst | hfst-fst2strings
			p1 = Popen(["echo", inputForm], stdout=PIPE)
			p2 = Popen(["hfst-strings2fst", "-S"], stdin=p1.stdout, stdout=PIPE)
			p3 = Popen(["hfst-compose-intersect", self.twolcFile], stdin=p2.stdout, stdout=PIPE)
			p4 = Popen(["hfst-fst2strings", "-c1"], stdin=p3.stdout, stdout=PIPE)
			p1.stdout.close()
			p2.stdout.close()
			p3.stdout.close()
			commandOutput = p4.communicate()[0].decode('utf-8')
			for block in commandOutput.split('\n'):
				if block != "":
					#print("a: "+block)
					if ':' in block:
						output+=[block.split(':')[1]]
					else:
						output+=[block]
			#return output
			formDict["output"] = output
	
	
	def get_all_forms(blocks):
		allForms = set()
		for block in blocks:
			for line in block:
				allForms.add(line)
		return allForms
	
	def get_rules_excluding_correct(self, inForms=None): #ruleSet, correct):
		ruleSet = self.rules
		outForms = {}
		if inForms==None:
			for form in self.forms:
				outRules = self.get_rules_excluding_correct_oneform(form)
				outForms[form["input"]] = outRules
		else:
			for inForm in inForms:
				for form in self.forms:
					if form["input"] == inForm:
						outRules = self.get_rules_excluding_correct_oneform(form)
						outForms[form["input"]] = outRules
		return outForms

	def get_rules_excluding_correct_oneform(self, form, correct=None):
		if correct==None:
			correct = form["intended"]
		outputs = form["outputs"]
		outRules = set()
		#print(outputs)
		for ruleNum, forms in outputs.items():
			if correct not in forms:
				#print(str(ruleNum)+": "+str(ruleSet[ruleNum]))
				#print(str(correct)+": "+str(ruleNum))
				outRules.add(ruleNum)
		return outRules
	
	def get_rules_excluding_form(self, inForm, intendedForm):
		for form in self.forms:
			if form["input"] == inForm:
				return self.get_rules_excluding_correct_oneform(form, correct=intendedForm)

	def get_forms_with_incorrect_output(self):
		badForms = []
		for form in self.forms:
			if form["intended"] not in form["output"]:
				badForms += [form]
		return badForms
					

	#def get_rules_excluding_correct(ruleSet, correct):
	#	outRules = set()
	#	for (rule, forms) in ruleSet:
	#		if correct in forms:
	#			outRules.add(rule)
	#	return outRules
	
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
	
	def main_loop(twolcFile, inputForm, correct, showall, process):
		rules = []
		blocks = []
		ruleSet = []
	
		if not process:
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
		else:
			outputForms = process_form(inputForm, twolcFile)
			print(outputForms)

	def lexc(self, lexcFile):
		self.lexc = lexcFile
	
	def get_phonolforms(self):
		print("forms:", self.forms)
		for form in self.forms:
			lexcForm = form["lexc"]
			p1 = Popen(["echo", lexcForm], stdout=PIPE)
			p2 = Popen(["hfst-lookup", "-Xprint-space", "-q", self.lexc], stdin=p1.stdout, stdout=PIPE)
			output = p2.communicate()[0].decode('utf-8')
			for line in output.split('\n'):
				if '\t' in line:
					(inForm, outForm, time) = line.split('\t')
				#outForm = re.sub("{(.*?)}", " {\1} ", outForm)
				#outForm = re.sub(">", " > ", outForm)
				#outForm = re.sub("([Ѐ-Ӿ])([Ѐ-Ӿ])", "\1 \2", outForm)
				#outForm = re.sub("\s*", " ", outForm)
			outForm = re.sub('\s{1,2}', ' ', outForm)
			form["input"] = outForm
		print(self.forms)
