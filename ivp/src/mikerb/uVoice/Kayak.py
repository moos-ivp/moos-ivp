from Language import pronounced
from MOOSSpoken import MOOSSpoken

class Kayak(MOOSSpoken):
	def __init__(self, pronunciation, app):
		MOOSSpoken.__init__(self, pronunciation, app)
		
	def follow(self, target):
		return self._Notify("follow", target)
		
	def formation(self, formation):
		return self._Notify("formation", formation)
		
	@pronounced("return")
	def return_func(self):
		return self._Notify("return", "true")
		
	def resume(self):
		return self._Notify("return", "false")
