class MOOSSpoken:
	def __init__(self, pronunciation, app):
		self._app     = app
		self._pronunciation  = pronunciation
		self.__repr__ = lambda: self._pronunciation
	def _Notify(self, variable, value):
		command = ("%s_%s" % (self, variable), "%s" % value)
		self._app.Notify(*command)
		return command
