#!/usr/bin/python

import sys
import os.path

#===============================================================================

def parse_slog_header(infile):
   """Returns two dictionaries.
   
   The first dictionary maps disambiguated variable names to column numbers 
   (starting at 1, not 0).  Raises an error is something went wrong.
   
   The second dictionary has the disambiguated variable names as its keys, and 
   the dependent value is the original, ambiguous variable name.  This 
   dictionary doesn't have any entries for variables that occurred just once in
   the header.
   """
   
   # Record the number of occurrences of each name, in case we enounter 
   # duplicates.  We'll handle duplicates by appending a __i to their name, 
   # where i is a disambiguating number, starting at __1 for each distinct name.
   num_var_occurrences = {}
   disambig_to_ambig_dict = {}
   variable_renaming_required = False
   
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
      #if var_name in var_name_to_col_number:
         #raise "Something's wrong: " + var_name + " appears twice in the header?"
      
      # Phew... FINALLY we can just record the info we wanted...
      if var_name in num_var_occurrences:
         variable_renaming_required = True
         
         if num_var_occurrences[var_name] == 1:
            # We need to rename the previous occurrence of this variable in
            # 'var_name_to_col_number', because at the time we inserted it into
            # the map we didn't know there were multiple occurrences of that
            # var name...
            temp = var_name_to_col_number[var_name]
            del var_name_to_col_number[var_name]
            disambig_name = var_name + "__1"
            
            var_name_to_col_number[disambig_name] = temp
            disambig_to_ambig_dict[disambig_name] = var_name
         
         num_var_occurrences[var_name] = num_var_occurrences[var_name] + 1
         
         disambig_name = var_name + "__" + str(num_var_occurrences[var_name])
         var_name_to_col_number[disambig_name] = col_number
         disambig_to_ambig_dict[disambig_name] = var_name
      else:
         num_var_occurrences[var_name] = 1
         var_name_to_col_number[var_name] = col_number
      
      col_number += 1
      s = infile.readline().strip()
   
   
   # Let the user know about any variable renaming we did...
   if variable_renaming_required:
      print >> sys.stderr, \
"""
** PLEASE NOTE ***
Some columns in the supplied slog file had the same variable name.

Since this script requires you to specifically name the variables that are to be
included or excluded from the output file, this is a problem.  To get around it,
this script is internally renaming such variables to make them unique.

For example if the variable FOO is used three times, you should refer to the 
three different occurrences as FOO__1 FOO__2 and FOO__3 when dealing with this
script.  *This script will likely break if you already have a variable named,
for example, FOO__1.*

The specific renames performed on the supplied input file are:
      
"""
      for (base_varname, num_occurrences) in num_var_occurrences.iteritems():
         if num_occurrences > 1:
            print >> sys.stderr, base_varname + " ==> "
            for n in range(1, num_occurrences+1):
               disamb_name = base_varname + "__" + str(n)
               #print disamb_name, var_name_to_col_number
               assert(disamb_name in var_name_to_col_number)
               print >> sys.stderr, "     " + disamb_name
            print >> sys.stderr, ""
      
      print >> sys.stderr, \
"""

* THESE VARIABLES WILL APPEAR IN THE OUTPUT FILE IN THEIR ORIGINAL, POTENTIALLY
* AMBIGUOUS FORM.  I.E., FOO__2 and FOO__3 WILL BOTH BE LISTED AS 'FOO' IN THE
* OUTPUT FILE.
"""
   
   
   infile.readline()
   
   # We can't do the following check at the moment, because we rename variables
   # that have multiple occurrences. -CJC
   #
   ## Confirm that the column headers, which state the variable names, match what
   ## was claimed earlier in the header.  This is maybe being overly cautious, 
   ## but could save someone hours of hear-pulling if/when this actually detects
   ## a problem...
   #s = infile.readline().strip()
   #fields = s.split()
   #for i in range(1, len(fields)):
      #var_name = fields[i]
      #if var_name_to_col_number[var_name] != i:
         #raise "The first part of the slog header said that variable '" + \
            #var_name + "' would appear in column " + \
            #var_name_to_col_number[var_name] + ", but the " + \
            #"line showing the column headers shows this variable in column " + \
            #str(i)

   return (var_name_to_col_number, disambig_to_ambig_dict)

#===============================================================================

def get_matching_col_name(c, col_names):
   """
   If 'c' matches any of the elements of 'col_names' using a case-insensitive
   string comparison, this returns that element from 'col_names'.
   (This function assumes that at most one match will occur.)
   
   Otherwise this returns None.
   """
   for name in col_names:
      if c.capitalize() == name.capitalize():
         return name
      
   return None

#===============================================================================

def invert_dict(d):
   """
   Returns a dictionary whose keys and values have swapped places.
   
   Should crash if the values aren't all unique.
   """
   inverted_d = {}
   for (key, val) in d.iteritems():
      inverted_d[val] = key
      
   return inverted_d

#===============================================================================

def print_help_and_exit():
   s = \
      "Usage: \n" + \
      "To specify the output column using the command line:\n" + \
      "   slog-filt.py <in-file> --[hide]vars var1 [var2 ...] [--select <query>] [--output <out-file>]\n" + \
      "or\n" + \
      "To specify the output column interactively:\n" + \
      "   slog-filt.py <in-file> [--select <query>] [--output <out-file>]\n"
   
   sys.exit(s)

#===============================================================================

def get_output_vars_by_prompting(var_name_to_col_number):
   """
   Prompts the user for a list of column names he wants to appear in the 
   filtered output.  Returns that list.   
   """
   cols_to_output = []
   col_number_to_var_name = invert_dict(var_name_to_col_number)
   
   for col_idx in range(1, len(col_number_to_var_name)+1):
      col_name = col_number_to_var_name[col_idx]
         
      satisfied = False
      while not satisfied:
         choice = raw_input("Output " + col_name + " [Y]: ").upper()
         if choice in ['N', 'NO']:
            satisfied = True
         elif choice in ['', 'Y', 'YES']:
            cols_to_output.append(col_name)
            satisfied = True
         else:
            print "Please enter Y or N."        

   return cols_to_output

#===============================================================================

def get_output_vars_list(argv, var_name_to_col_number):
   
   # How does the user want to specify the output columns? ...
   if '--vars' in argv:
      if '--hidevars' in argv:
         print_help_and_exit()
      else:
         first_var_idx = argv.index('--vars') + 1
         spec_type='vars'
   elif '--hidevars' in argv:
         first_var_idx = argv.index('--hidevars') + 1
         spec_type='hidevars'
   else:
      spec_type='interactive'
   
   
   if spec_type == 'interactive':
      output_vars_list = get_output_vars_by_prompting(var_name_to_col_number)
   else:
      # Figure out what columns the user specified.  Stop when we get to the
      # next command-line arg that begins with "--", or when we run out of
      # args...
      user_var_list = []
      for arg in argv[first_var_idx :] :
         if arg.startswith("--"):
            break
         user_var_list.append(arg)
      
      # Courtesy hack: If a column specified by the use doesn't appear in the
      # slog file, but it *does* match if we ignore case, then adjust the user-
      # provided column name's case accordingly...       
      unmatchable_names = []
      
      for i in range(0, len(user_var_list)):
         user_var_name = user_var_list[i]
         matching_col_name = get_matching_col_name(user_var_name, 
               var_name_to_col_number.keys())
               
         if user_var_name != matching_col_name:
            if matching_col_name == None:
               unmatchable_names.append(user_var_name)
            else:
               user_var_list[i] = matching_col_name
      
      # Complain if we have a genuinely unrecognizable column name...
      if len(unmatchable_names) > 0:
         print >> sys.stderr, \
            "You listed some variables that aren't in the supplied slog file:\n" + \
            ", ".join(unmatchable_names)
         print_help_and_exit()


      if spec_type == 'vars':
         # Include the variables in the sequence specified by the user.
         output_vars_list = user_var_list
      else:  # spec_type == 'hidevars'
         # Include the variables in the sequence in which they appear in the 
         # input slog file...
         output_vars_list = []
         col_number_to_var_name = invert_dict(var_name_to_col_number)
   
         for col_idx in range(1, len(col_number_to_var_name)+1):
            col_name = col_number_to_var_name[col_idx]
            if not col_name in user_var_list:
               output_vars_list.append(col_name)
         
   
   return output_vars_list
   
#===============================================================================

def get_input_file(argv):
   filename=argv[1]
   
   if not os.path.exists(filename):
      print >> sys.stderr, "The file " + filename + \
         " doesn't seem to exist."
      print_help_and_exit()

   return open(filename, "r")

#===============================================================================

def get_output_file(argv):
   if argv[-2] == '--output':
      filename = argv[-1]
      
      if os.path.exists(filename):
         print >> sys.stderr, "The file " + filename + \
            " already exists (I don't overwrite files.)"
         print_help_and_exit()
      
      return file(filename, 'w')
   else:      
      return sys.stdout

#===============================================================================

def split_query_string(query_string):
   # If the first or last symbol in query_string is a '@', then the first or
   # last element (respectively) in query_split will be the empty string.  
   # Eliminate those - they make subsequent processing harder if left in...
   query_split = query_string.split('@')
   
   if query_string[0] == '@':
      assert(query_split[0] == "")
      query_split = query_split[1:]
   
   if query_string[-1] == '@':
      assert(query_split[-1] == "")
      query_split = query_split[:-1]

   return query_split

#===============================================================================

def find_col_names_in_query(query_string, query_split, valid_col_names):
   """
   'query_split' must be the result of split_query_string().
   
   Returns a dictionary.  Keys are indices into the list produced by splitting
   the query string.
   
   Dependent values are either None (if a match couldn't be found) or
   the name of the valid column name that matches.
   """
   return_dict = {}
   
   # Do some basic validation: Are all symbols valid?
   # The basic structure should be [...][@var@[...]]*
   # So we ought to see an even number of '@' symbols.  That means that if we
   # split the string around the '@' symbols, we ought to have an odd number of
   # elements.
   #
   # Also, if we split('@') the query string, either all even numbered, or all 
   # odd numbered,elements in the resulting list are variable names, and we can 
   # know which are which by just looking at the first character in the query 
   # string.

   if (query_string.count('@') % 2) == 1:
      print >> sys.stderr, "The query string looks wrong: it has an odd number of '@' symbols"
      print_help_and_exit()
      
   if query_string[0] == '@':
      query_col_name_indices = range(0, len(query_split), 2)
   else:
      query_col_name_indices = range(1, len(query_split), 2)
   
   for i in query_col_name_indices:
      col_name = query_split[i]
      return_dict[i] = get_matching_col_name(col_name, valid_col_names)

   return return_dict
   
#===============================================================================

def get_query_expression(argv, var_name_to_col_number):
   if not "--select" in argv:
      return None
   
   select_idx = argv.index("--select")
   if select_idx == (len(argv) - 1):
      print >> sys.stderr, "You need a query string to follow '--select'"
      print_help_and_exit()
      
   return argv[select_idx + 1]

#===============================================================================
            
def validate_query_string(query_string, valid_col_names):
   query_split = split_query_string(query_string)
   query_col_names = find_col_names_in_query(query_string, query_split, valid_col_names)
   
   unmatched_names = []
   for (idx, name) in query_col_names.iteritems():
      if name == None:
         unmatched_names.append(query_split[idx])
         
   if len(unmatched_names) > 0:      
      print >> sys.stderr, "Your query string has some column names that don't " + \
         "appear in the slog file: " + ",".join(unmatched_names)
      print_help_and_exit()
   
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

def line_meets_select_criteria(line_dict, query_string, valid_col_names):
   query_split = split_query_string(query_string)
   query_col_names = find_col_names_in_query(query_string, query_split, valid_col_names)
   
   # We convert 'NaN' values to None, because it will probably lead to about the
   # expected behavior in terms of how the query works...
   
   for (split_idx, col_name) in query_col_names.iteritems():
      col_value = line_dict[col_name]
      if col_value == "NaN":
         query_split[split_idx] = "None"
      else:
         query_split[split_idx] = col_value
   
   bound_query = " ".join(query_split)
   
   try:
      return eval(bound_query)
   except Exception, inst:   
      print >> sys.stderr, "Problem evaluating query string:\n" + \
         "    " + bound_query + "\n\n"
         
      sys.exit(str(inst))

#===============================================================================

def main(argv):
   if len(argv) == 1:
      print_help_and_exit()      
   
   f = get_input_file(argv)
   
   # Quick check to ensure that the user isn't passing any unrecognized 
   # switches...
   problem = False
   for a in argv[1:]:
      if a.startswith('-'):
         if a not in ['--select', '--vars', '--hidevars', '--output']:
            problem = True
            print >> sys.stderr, \
               "Unrecognized command-line switch: " + a
   if problem:
      print >> sys.stderr, ""
      print_help_and_exit()
   
   # Copy the first 5 lines of the input file, verbatim.  Actually, we'll print
   # these a little later, after we've completed validating the command-line
   # parms.  Otherwise it will look odd for this script to print the first part
   # of an slog file and *then* complain about the command-line args...
   first_header_lines = ""
   for i in range(5):
      first_header_lines = first_header_lines + f.readline().strip() + "\n"
   
   # OK, NOW we can parse the header
   (var_name_to_col_number, disambig_to_ambig_dict) = parse_slog_header(f)
   valid_col_names = var_name_to_col_number.keys()
   
   # We needed to parse the header so we can determine the output columns.
   output_vars = get_output_vars_list(argv, var_name_to_col_number)
   
   # If the user gave an explicit list of output variables, but left out TIME,
   # assume it was a mistake...
   if ('--vars' in argv) and ('TIME' not in output_vars):
      output_vars = ['TIME'] + output_vars
   
   if len(output_vars) == 0:
      sys.exit("No output columns selected, so nothing to do.")
   
   if "--select" in argv:
      query_string = get_query_expression(argv, var_name_to_col_number)
      validate_query_string(query_string, var_name_to_col_number.keys())
   else:
      query_string = None
   
   #----------------------------------------------------------------------------
   # Print a reconstucted header to stdout.  Order the columns according to how
   # they were specified on the command-line in the "--vars" section...
   #----------------------------------------------------------------------------
   outfile = get_output_file(argv)      
   print >> outfile, first_header_lines
   
   for i in range(len(output_vars)):
      var_name = output_vars[i]
      if var_name in disambig_to_ambig_dict:
         var_name = disambig_to_ambig_dict[var_name]
      print >> outfile, "%%   (" + str(i+1) + ") " + var_name
      
   print >> outfile, ""
   print >> outfile, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"   
   
   var_name = output_vars[0]
   start_col_for_each_var = {var_name:0}
   s = "%% " + var_name
   for i in range(1, len(output_vars)):
      var_name = output_vars[i]
      start_col = 1 + 19 * i
      start_col_for_each_var[var_name] = start_col
      
      needed_padding = start_col - len(s)
      
      if var_name in disambig_to_ambig_dict:
         s += (' ' * needed_padding) + disambig_to_ambig_dict[var_name]
      else:
         s += (' ' * needed_padding) + var_name
         
   print >> outfile, s   
   
   #----------------------------------------------------------------------------         
   # Process the data lines...
   #----------------------------------------------------------------------------
   a_line = f.readline()
   while a_line != "":
      a_line = a_line.strip()
      if (a_line != "") and (not a_line.startswith('%')):
         line_dict = parse_one_line(a_line, var_name_to_col_number)
         
         if query_string != None:
            if not line_meets_select_criteria(line_dict, query_string, valid_col_names):
               a_line = f.readline()
               continue
         
         s = ""
         for i in range(len(output_vars)):
            var_name = output_vars[i]
            var_value = line_dict[var_name]
            
            needed_padding = start_col_for_each_var[var_name] - len(s)
            s += (' ' * needed_padding) + var_value
         print >> outfile, s
      
      a_line = f.readline()
         
#===============================================================================

if __name__ == '__main__':
   main(sys.argv)
