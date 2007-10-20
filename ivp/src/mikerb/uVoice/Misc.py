from MOOSSpoken import MOOSSpoken
from sys import exit

class Misc(MOOSSpoken):
	def __init__(self, pronunciation, app):
		MOOSSpoken.__init__(self, pronunciation, app)
	
	def shutdown(self):
		self._app.__del__()
		exit()

