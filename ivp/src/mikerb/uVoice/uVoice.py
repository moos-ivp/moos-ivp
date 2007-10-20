#!/usr/bin/python
import sys
import MOOS
import Sphinx
import Festival
import Language
from Kayak import Kayak
from Formation import Formation
from Group import Group
from Misc import Misc


class uVoice(MOOS.CMOOSApp):
	def __init__(self, speakables = None):
		#needed for SWIG shadowing, otherwise mysterious segfault upon Run()
		MOOS.CMOOSApp.__init__(self)
		self.state = None
		if speakables:
			self.register_speakables(speakables)
	
	def __del__(self):
		self.sphinx.stop()
		MOOS.CMOOSApp.__del__(self)
	
	def register_speakables(self, speakables):
		self.language = Language.Language(speakables)
		self.sphinx = Sphinx.Sphinx(self.language.commands) 

	def say(self, text):
		text = Language.pronunciation(text)
		print text 
		Festival.say(text)
			
	def Iterate(self):
		self.sphinx.update_recognized()
		if len(self.sphinx.recognized):
			phrase = self.sphinx.recognized.pop().lower()
			self.say("heard " + phrase)
			try:
				result = self.language.execute(phrase)
				if result:
					self.say(result)
			except (AttributeError, TypeError, NameError):
				self.say(sys.exc_info()[1])
			
		if self.state != self.sphinx.state:
			self.state = self.sphinx.state
			self.say(self.state)
		return True

	def OnNewMail(self, NewMail):
		#for m in NewMail:
		#	print m.GetKey()
		return True

	def OnConnectToServer(self):
		#self.Register('DB_TIME', 1.0)
		return True

	def OnStartUp(self):
		return True

if __name__ == "__main__":

	app = uVoice()

	zero = Kayak("zero", app)
	one = Kayak("one", app)
	two = Kayak("two", app)
	three = Kayak("three", app)
	four = Kayak("four", app)
	five = Kayak("five", app)
	six = Kayak("six", app)
	seven = Kayak("seven", app)
	kayaks = [zero, one, two, three, four, five, six, seven]

	alpha = Formation("alpha", app)
	beta = Formation("beta", app)
#	gamma = Formation("gamma", app)
#	delta = Formation("delta", app)
#	formations = [alpha, beta, gamma, delta]
	formations = [alpha, beta]

	all = Group("all", kayaks, app)

	computer = Misc("computer", app) 
	other = [computer, all]

	speakables = kayaks + formations + other
	app.register_speakables(speakables)
	app.Run('uVoice', 'uVoice.moos')

