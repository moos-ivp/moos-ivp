#!/usr/bin/python

"""
This is a module for parsing SLOG files.

You probably only want to use the function 'parse_slog'.
"""

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

def parse_one_line(s, var_name_to_col_number):
   """Take in a single line of text, and the dictionary returned by the function
   parse_slog_header.  Returns a dictionary whose keys are the variable names
   and whose dependent values are the values from the line of text.
   
   Raises an exception if anything bad happens.
   """
   fields = s.split()
   if len(fields) != len(var_name_to_col_number):
      error_str = """
         The line of text had %s fields, but the
         slog file header said to expect %s headers.
         
         The line of text is:
         %s
         
         It's broken into these fields:
         %s
         """ % (str(len(fields)), str(len(var_name_to_col_number)), s, str(s.split()))
      
      sys.exit(error_str)
   
   d = {}
   for var_name, col_number in var_name_to_col_number.iteritems():
      var_value = fields[col_number-1]
      d[var_name] = var_value
      
   return d
   
#===============================================================================

def parse_slog(input_file):
   """ Returns a list of dictionaries.  Each dictionary bears the data from a 
   single row of the supplied file.
   
   Note that 'input_file' must be a Python file object, not a filename.
   """
   
   # Eat the first five lines.  We're not treating them as part of the actual
   # header...
   for i in range(5):
      input_file.readline()

   
   (unique_vars_to_col_nums, unique_vars_to_original_vars) = \
      parse_slog_header(input_file)
   
   lines = []
   
   for file_line in input_file.readlines():
      s = file_line.strip()
      if (s == "") or (s.startswith("%")):
         continue
      
      lines.append(parse_one_line(file_line, unique_vars_to_col_nums))
   
   return lines
   
#===============================================================================
   
def do_test(argv):
   print "Dictionaries produced from the file: " + argv[1]
   f = open(argv[1])
   for line in parse_slog(f):
      print line
   
#===============================================================================
   
if __name__ == '__main__':
   import sys
   do_test(sys.argv)