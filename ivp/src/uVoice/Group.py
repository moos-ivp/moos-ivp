from MOOSSpoken import MOOSSpoken
from Language import pronounced

class Group(MOOSSpoken):
	def __init__(self, pronunciation, members, app):
		MOOSSpoken.__init__(self, pronunciation, app)
		self._members = members

	def formation(self, formation):
		for member in self._members:
			member.formation(formation)
		return "kayaks to formation %s" % formation

	@pronounced("return")
	def return_func(self):
		for member in self._members:
			member.return_func()
		return "kayaks returning to base"

	def resume(self):
		for member in self._members:
			member.resume()
		return "kayaks resuming their business"
	

