# Set variables to be passed down to subdirectories

# Some of our make rules include redirection of stdout / stderr.  The syntax
# for doing this varies among different shells, including csh vs. bash.  So
# force use of csh to ensure we have no problems.
#
# Use sh (which is a link to dash or bash) because that's the standard 
# for makefiles.  It also doesn't load config files like ~/.cshrc or even
# ~/.bashrc, so an individual user's config files can't mess up the scripting
# inside the makefile.

#CXX         = gcc32
#CXX         = gcc-4.0
#CXX         = g++-2.95
DEBUG_LEVEL = -g
#DEBUG_LEVEL = -O
BIN_DIR     = $(CURDIR)/bin
DATA_DIR    = $(CURDIR)/../../data
LIB_DIR     = .
#FP          = -fPIC

