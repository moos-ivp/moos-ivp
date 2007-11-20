#!/usr/bin/python
import sys
import os.path

#===============================================================================

def print_usage_and_exit():
   sys.exit("Usage: slog-to-taml.py  <slog-filename>  <taml-filename>")

#===============================================================================

def slog_to_taml(slog_file, taml_file):
   pass

#===============================================================================

def main(argv):
   if len(argv) != 2:
      print_usage_and_exit()
   
   slog_filename, taml_filename = argv
   
   if not os.path.isfile(slog_filename):
      sys.exit("The file \"" + slog_filename + "\" doesn't exist or isn't a file.")
      
   if os.path.exists(taml_filename):
      sys.exit("The TAML file \"" + taml_filename + "\" already exists.")
   
   slog_file = open(slog_filename, "r")
   taml_file = open(taml_filename, "w")
   
   slog_to_taml(slog_file, taml_file)

#===============================================================================
   
if __name__ == '__main__':
   main(sys.argv)