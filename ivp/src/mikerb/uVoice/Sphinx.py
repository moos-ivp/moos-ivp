import os
import re
import tempfile
from time import sleep

#should read these from a config file, or something!

sentence_path = "./sphinx2.sent"
lm_path = "./sphinx2.lm"
dict_path = "./sphinx2.dic"
hmm_path = "/usr/share/sphinx2/model/hmm/6k"

cmudict_path = "../../data/cmudict_sphinx"
lm_script_path = "./quick_lm.pl"
lm_command = "perl " + lm_script_path + " -s " + sentence_path \
				+ " -o " + lm_path + " >/dev/null 2>&1"

sphinx_command = "sphinx2-continuous -live TRUE -ctloffset 0 " + \
	"-ctlcount 100000000 -cepdir ./ctl -datadir ./ctl " + \
	"-agcemax TRUE -langwt 6.5 -fwdflatlw 8.5 -rescorelw 9.5 -ugwt 0.5 " + \
	"-fillpen 1e-10 -silpen 0.005 -inspen 0.65 -top 1 -topsenfrm 3 " + \
	"-topsenthresh -70000 -beam 2e-06 -npbeam 2e-06 -lpbeam 2e-05 " + \
	"-lponlybeam 0.0005 -nwbeam 0.0005 -fwdflat FALSE -fwdflatbeam 1e-08 " + \
	"-fwdflatnwbeam 0.0003 -bestpath TRUE -kbdumpdir . " + \
	"-lmfn " + lm_path + " -dictfn " + dict_path + \
	" -ndictfn " + hmm_path + "/noisedict " + "-phnfn " + hmm_path + \
	"/phone -mapfn " + hmm_path + "/map -hmmdir " + hmm_path + \
	" -hmmdirlist " + hmm_path + " -8bsen TRUE -sendumpfn " + hmm_path + \
	"/sendump -cbdir " + hmm_path

class Sphinx:
	def __init__(self, phrases):

		self.kill()
		
		self.recognized_regexp = re.compile("^\d+\: .+")
		self.unrecognized_regexp = re.compile("^\d+\:")
		
		#write out the sentence file
		sentenceFile = open(sentence_path, "w")
		for phrase in phrases:
			print >>sentenceFile, "<s> " + phrase  + " </s>"
		sentenceFile.close()

		#create the language model
		os.system(lm_command)

		#create dictionary
		cmudictFile = open(cmudict_path, "r")
		#each entry in the dictionary should be unique
		dictionary = set()
		
		words = set(" ".join(phrases).split(" "))
		#we want WORD or WORD(#)
		regexp = re.compile( "^(" + "|".join(words).upper() + ")[\s|\(]" )
		
		for line in cmudictFile.readlines():
			if regexp.match(line):
				dictionary.add(line)
		cmudictFile.close()
		
		dictFile = open(dict_path, "w")
		#must be sorted so WORD comes before WORD(2) etc
		l = list(dictionary)
		l.sort()
		for line in l:
			print >>dictFile, line,
		dictFile.close()
		
		#we'll store recognized phrases in a queue
		self.recognized = []
		#could be "ready", "listening", "recognized", or "unrecognized"
		self.state = None


		self.outfile = tempfile.mktemp()
		os.system("touch %s" % self.outfile)
		os.system("%s >&%s &" % (sphinx_command, self.outfile))
		self.server_out = open(self.outfile, 'r')

		while self.state == None:
			self.update_recognized()
			sleep(.1)

	def __del__(self):
		self.stop()

	def stop(self):
		self.server_out.close()
		os.system('rm %s' % self.outfile)
		self.kill()
	
	def kill(self):
		os.system('killall sphinx2-continuous >/dev/null 2>&1')
		os.system('killall -9 sphinx2-continuous >/dev/null 2>&1')
	
	def update_recognized(self):
		for line in self.server_out.readlines():
			if line.startswith("FATAL"):
				raise line
			elif line.startswith("READY"):
				self.state = "ready"
			elif line.startswith("Listening"):
				self.state = "listening"
			elif self.recognized_regexp.match(line):
				self.state = "recognized"
				self.recognized.append(line.split(" ", 1)[1])
			elif self.unrecognized_regexp.match(line):
				self.state = "unrecognized"

