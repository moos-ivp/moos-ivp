"""Python interface to Festival"""

from os import system

def say(text):
	system("echo '%s' | festival --tts" % str(text))

