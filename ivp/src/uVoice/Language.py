#interesting stuff, no internals
def _dir(context):
	return [x for x in dir(context)
			if not x.startswith("_") 
			and not x.startswith("func_")
			and not x.startswith("im_")]

#decorate a method or function with an alternate pronunciation
def pronounced(pronunciation):
	def decorate(f):
		setattr(f, '_pronunciation', pronunciation)
		return f
	return decorate

#get the pronunciation of a python object
def pronunciation(item):
	p = getattr(item, '_pronunciation', None)
	if not p:
		p = getattr(item, '__name__', None)
	if not p and hasattr(item, 'im_func'):
		p = item.im_func.__name__
	if not p:
		p = str(item)
	return p

#recurse through the item and return a sample collection of commands that might
#be spoken
def usage(speakable):
	p = pronunciation(speakable)
	commands = set([p])
	for member_name in _dir(speakable):
		member = getattr(speakable, member_name)
		commands.update(p + " " + rest for rest in usage(member))
	return commands

class Language:
	def __init__(self, speakables):
		self.last_subject = None
		self.speakables = speakables

		self.commands = set()
		for speakable in speakables:
			self.commands.update(usage(speakable))
	
	def get_from_context(self, word, context):
		for member in _dir(context):
			member = getattr(context, member)
			if pronunciation(member) == word:
				return member
	
	def __getitem__(self, word):
		for speakable in self.speakables:
			if pronunciation(speakable) == word:
				return speakable
	
	#assumes [S]VO construction
	def execute(self, string):
		words = string.strip().split()
		subject = None
		verb    = None
		objects = []

		if not self[words[0]]:
			subject = self.last_subject
			verb    = self.get_from_context(words[0], subject)
			objects = [self[word] for word in words[1:]]
		else:
			subject = self[words[0]]
			self.last_subject = subject
			if len(words) == 1:
				return "Subject is %s" % pronunciation(subject)
			verb    = self.get_from_context(words[1], subject)
			objects = [self[word] for word in words[2:]]
		if not verb:
			raise NameError("Could not execute " + string)
		return verb(*objects)
		
