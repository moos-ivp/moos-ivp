#!/usr/bin/python

import os
import sys
import subprocess

def print_usage():
   sys.exit("Usage: svn-bulk-rename <root-dir> <input-filename> <output-filename>")
   
if (len(sys.argv) != 4):
   print_usage()
   
root_dir = sys.argv[1]
input_filename = sys.argv[2]   
output_filename = sys.argv[3]   
   
for walk_data in os.walk(root_dir):
   parent_dir = walk_data[0]
   child_dirs = walk_data[1]
   child_files = walk_data[2]
   
   if input_filename in child_files:
      cmd = [
         'svn',
         'mv',
         parent_dir + "/" + input_filename,
         parent_dir + "/" + output_filename
         ]
      print "Running command: " + str(cmd)
      subprocess.check_call(cmd)
   