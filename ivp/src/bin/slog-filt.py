#!/usr/bin/python

import sys

#===============================================================================

def parse_slog_header(infile):
   """Returns a dictionary that maps variable names to column numbers (starting
   at 1, not 0).  Raises an error is something went wrong.
   """
   
   # Discover the column number for each variable...
   s = infile.readline().strip()
   col_number = 1
   var_name_to_col_number = {}
   while s != "":
      fields = s.split()
      if (len(fields) != 3) or (fields[0] != '%%'):
         raise "This doesn't *look* like a column description line: " + s
      
      expected_var_num_string = "(" + str(col_number) + ")"
      if fields[1] != expected_var_num_string:
         raise "Something's wrong: I was expecting '" + expected_var_num_string + \
            "' but found '" + fields[1] + "'"
      
      var_name = fields[2]
      if var_name in var_name_to_col_number:
         raise "Something's wrong: " + var_name + " appears twice in the header?"
      
      # Phew... FINALLY we can just record the info we wanted...
      var_name_to_col_number[var_name] = col_number
      
      col_number += 1
      s = infile.readline().strip()
   
   infile.readline()
   
   # Confirm that the column headers, which state the variable names, match what
   # was claimed earlier in the header.  This is maybe being overly cautious, 
   # but could save someone hours of hear-pulling if/when this actually detects
   # a problem...
   s = infile.readline().strip()
   fields = s.split()
   for i in range(1, len(fields)):
      var_name = fields[i]
      if var_name_to_col_number[var_name] != i:
         raise "The first part of the slog header said that variable '" + \
            var_name + "' would appear in column " + \
            var_name_to_col_number[var_name] + ", but the " + \
            "line showing the column headers shows this variable in column " + \
            str(i)

   return var_name_to_col_number

#===============================================================================

def print_help_and_exit():
   s = "Usage: slog-filt.py <input-filename> --vars var1 [var2, var3, ...]"
   sys.exit(s)

#===============================================================================

def parse_cmdline_args(argv):
   """ Tries to dictionary with the following elements:
   'input-filename': Path/filename of the slog file to read in.
   'vars': A list of the variable names to appear in this program's output.
   If something goes wrong, this function calls print_help_and_exit.
   """
      
   if len(argv) < 4:
      print_help_and_exit()
      
   d = {
      'input-filename':argv[1]
      }
   
   if '--vars' not in argv:
      print_help_and_exit()
   vars_idx = argv.index('--vars')

   if vars_idx != 2:
      print_help_and_exit()

   d['vars'] = argv[(vars_idx + 1):]
   
   return d
   
#===============================================================================

def parse_one_line(s, var_name_to_col_number):
   """Take in a single line of text, and the dictionary returned by the function
   parse_slog_header.  Returns a dictionary whose keys are the variable names
   and whose dependent values are the values from the line of text.
   
   Raises an exception if anything bad happens.
   """
   
   fields = s.split()
   if len(fields) != len(var_name_to_col_number):
      raise "The line of text had " + str(len(fields)) + " fields, but the " + \
         "slog file header said to expect " + len(var_name_to_col_number) + \
         "headers."
   
   d = {}
   for var_name, col_number in var_name_to_col_number.iteritems():
      var_value = fields[col_number-1]
      d[var_name] = var_value
      
   return d
   
#===============================================================================

def main(argv):
   args = parse_cmdline_args(argv)
   f = open(args['input-filename'])
   
   #----------------------------------------------------------------------------
   # Copy the first 5 lines of the input file, verbatim...
   #----------------------------------------------------------------------------
   for i in range(5):
      s = f.readline()
      print s.strip()
   
   var_name_to_col_number = parse_slog_header(f)
   
   #----------------------------------------------------------------------------
   # Confirm that the variables listed on the command line are a subset of the
   # variables present in the slog file.  Avoid using Python 'sets' for this,
   # because Mike's computer still runs, I believe, Python 2.4...
   #----------------------------------------------------------------------------
   missing_vars = []
   for a_var in args['vars']:
      if a_var not in var_name_to_col_number:
         missing_vars.append(a_var)
         
   if len(missing_vars) > 0:
      sys.exit("You requested the output contains the following vars, but " + \
         "the input file didn't have them: " + str(missing_vars))
         
   #----------------------------------------------------------------------------
   # Print a reconstucted header to stdout.  Order the columns according to how
   # they were specified on the command-line in the "--vars" section...
   #----------------------------------------------------------------------------
   for i in range(len(args['vars'])):
      var_name = args['vars'][i]
      print "%%   (" + str(i+1) + ") " + var_name
      
   print ""
   print "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"   
   
   var_name = args['vars'][0]
   start_col_for_each_var = {var_name:0}
   s = "%% " + var_name
   for i in range(1, len(args['vars'])):
      var_name = args['vars'][i]
      start_col = 1 + 19 * i
      start_col_for_each_var[var_name] = start_col
      
      needed_padding = start_col - len(s)
      s += (' ' * needed_padding) + var_name
   print s   
   
   #----------------------------------------------------------------------------         
   # Process the data lines...
   #----------------------------------------------------------------------------
   a_line = f.readline()
   while a_line != "":
      a_line = a_line.strip()
      if (a_line != "") and (not a_line.startswith('%')):
         line_dict = parse_one_line(a_line, var_name_to_col_number)
         
         s = ""
         for i in range(len(args['vars'])):
            var_name = args['vars'][i]
            var_value = line_dict[var_name]
            
            needed_padding = start_col_for_each_var[var_name] - len(s)
            s += (' ' * needed_padding) + var_value
         print s
      
      a_line = f.readline()
         
#===============================================================================

if __name__ == '__main__':
   main(sys.argv)

#===============================================================================
