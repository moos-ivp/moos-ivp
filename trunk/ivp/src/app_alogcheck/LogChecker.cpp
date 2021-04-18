/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: LogChecker.h                                         */
/*    DATE: July 29, 2010                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdio>
#include "LogChecker.h"
#include "MBUtils.h"

using namespace std;

// --------------------------------------------------------
// Constructor 

LogChecker::LogChecker() 
{
  m_verbose = false;
  
  // Store the values of variable in a buffer
  m_info_buffer = new InfoBuffer();
  
  // By default, set the output file to be standard out
  m_output_file = stdout;
} 

// --------------------------------------------------------
// Destructor 

LogChecker::~LogChecker() 
{
  // If the output file is not NULL and is not standard out, close the file
  if((m_output_file != NULL) && (m_output_file != stdout))
    fclose(m_output_file);
  
  // Delete the InfoBuffer
  delete(m_info_buffer);
} 

// --------------------------------------------------------
// Procedure: parseInputFile()
//   Purpose: Parses an input file for conditions.
//   Returns: true if at least one flag has been read from specified file
//            false otherwise.

bool LogChecker::parseInputFile(string input_file)
{
  if(input_file.empty())
    return(false);
  
  FILE* input_p = fopen(input_file.c_str(), "r");
  // Check if the input file could be opened
  if(input_p == NULL)
    return(false);
  
  // Should be set to true if at least one flag is found
  bool valid_flag_found = false;
  
  string line = "";
  while( (line = getNextRawLine( input_p )) != "eof" ){
    // If the line is empty, continue to the next itteration of the loop
    if( line.empty() )
      continue;
    // END check line.empty()
    
    // Replace tabs with spaces and strip black line ends
    line = findReplace( stripBlankEnds(line), "\t", " ");
    // Strip all possible comments
    line = stripComment(stripComment(stripComment(line, "%"), "#"), "//");

    // The Argument will be the first string
    string argument = tolower( biteString(line, ' ') );
    // The Flag will be remainer of the string
    string flag = stripQuotes( stripBlankEnds(line) );
    
    // If the argument is empty, continue on to the next itteration
    if( argument.empty() ){
      continue;
    } // END check !svector.empty()
    
    
    // Check if the argument matches one of the conditions
    if(argument == "start"){
      // Try to add the start flag
      if( this->addStartFlag( flag ) ){
	valid_flag_found = true;
      }// END check if add start flag was successful
    } else if(argument == "end"){
      // Try to add the end flag
      if( this->addEndFlag( flag ) ){
	valid_flag_found = true;
      } // END check if add end flag was successful
    } else if(argument == "pass"){
      // Try to add the pass flag
      if( this->addPassFlag( flag ) ){
	valid_flag_found = true;
      } // END check if add pass flag was successful
    } else if(argument == "fail"){
      // Try to add the fail flag
      if( this->addFailFlag( flag ) ){
	valid_flag_found = true;
      }// END check if add fail flag was successful
    } // END check argument
    
  } // END while-loop over input file
  
  fclose(input_p);
  return(valid_flag_found);
} 

// --------------------------------------------------------
// Procedure: check()
//   Purpose: Checks the specified alogfile for the various conditions. 
//   Returns: true if the specified condition was able to be added to 
//               the specified vector of flags 
//            false otherwise.

bool LogChecker::check(string alogfile, string output_file) 
{
  bool start_satisfied = false;
  bool end_satisfied = false;
  bool fail_satisfied = false;
  bool pass_satisfied = false;
  
  // Check if the alogfile is an empty string
  if(alogfile.empty()){
    fprintf(stdout, "Input filename cannot be empty.\n");
    fprintf(stdout, "Exiting now.\n");
    return(false);
  } 

  // Check if the alog file and the output_file are the same
  if(alogfile == output_file) {
    fprintf(stdout,
	    "Input and output .alog files cannot be the same.\n");
    fprintf( stdout, "Exiting now.\n");
    return(false);
  } 
  
  // Open the alogfile
  FILE* m_alogfile = fopen(alogfile.c_str(), "r");
  // Check if the alogfile was able to be opened
  if(!m_alogfile) {
    fprintf( stdout, "Input file not found or unable to open.\n");
    fprintf( stdout, "Exiting now.\n");
    return(false);
  } 
  
  // Open the output file if one has been specified
  if(output_file != "") {
    FILE* m_file_out = fopen(output_file.c_str(), "w");
    if(m_file_out != NULL){
      // If the output file is not NULL and it is not stdout, close the file
      if((m_output_file != NULL) && (m_output_file != stdout))
	fclose(m_output_file);
      // Set the output file
      m_output_file = m_file_out;
    } 
    else
      fprintf(stdout, "Output could not be created. Using standard out.\n");
  } 
  

  // If the start flags are empty, they will be considered satisfied
  start_satisfied = m_start_flags.empty();
  
  // Read in lines from the alog file looking for the start condition
  while( !start_satisfied ){
    // Get a ling from the log file
    ALogEntry entry = getNextRawALogEntry(m_alogfile);
    // Check if the end of the file has been reached.
    if(entry.getStatus() == "eof" ){
      if(m_alogfile){
	fclose(m_alogfile);
	m_alogfile = NULL;
      }
      return false;
    } else {
      // A valid entry has been parsed - Update the info buffer
      if(updateInfoBuffer(entry)){
	// If the info buffer has been updated successfully check
	// the start flags - this will automatically update
	// m_start_satisfied
	start_satisfied = checkStartFlags();
      }
    }
  }

  // Check if the checker is verbose
  if(m_verbose) {
    if(m_start_flags.empty()) {
      fprintf(m_output_file,
	      "No start conditions specified. Starting from the beginning of "
	      "the log file.\n");
    } 
    else {
      // Start flags have been specified - Check if it was satisfied
      if (start_satisfied) {
	fprintf(m_output_file,
		"The start consitions have been satisfied.\n");
      } 
      else {
	fprintf(m_output_file,
		"The end of the log was reached and the start "
		"conditions were NOT satisfied.\n");
      }
    }
  } 
  
  // Read in lines from the alog file looking for the pass/fail flags
  while(!end_satisfied) {
    // Get a ling from the log file
    ALogEntry entry = getNextRawALogEntry(m_alogfile);
    // Check if the end of the file has been reached.
    if(entry.getStatus() == "eof")
      break;
    else {
      // A valid entry has been parsed - Update the info buffer
      if(updateInfoBuffer(entry)) {
	// If the info buffer has been updated successfully check
	// the end_flags, pass_flags, and fail_flags
	end_satisfied = checkEndFlags();
	pass_satisfied = checkPassFlags();
	fail_satisfied = checkFailFlags();
      } 
    }
  } 
  
  // Close the alog file
  if(m_alogfile) {
    fclose(m_alogfile);
    m_alogfile = NULL;
  } 
  
  // Return true if the pass conditions have been satisfied and the
  // fail conditions have NOT been satisfied
  if(pass_satisfied && !fail_satisfied)
    return(true);
  else
    return(false);
} 

// --------------------------------------------------------
// Procedure: updateInfoBuffer
//   Purpose: Updates an info buffer to maintain a list of variable 
//            and their values at the current time.
//   Returns: true if the info buffer was updated; false otherwise

bool LogChecker::updateInfoBuffer(ALogEntry entry)
{
  string name  = entry.getVarName();
  string sdata = entry.getStringVal();
  double ddata = entry.getDoubleVal();
  
  // Check if the name is empty and if the
  if(!name.empty()) {
    // Update the timestamp
    m_info_buffer->setValue( m_timestamp, entry.getTimeStamp() );
    
    // Check if m_vars contains this variable - only update if true
    if(vectorContains(m_vars, name)) {
      // Set the value depending on the type
      if(entry.isNumerical()) 
	return(m_info_buffer->setValue(name, ddata));
      else
	return(m_info_buffer->setValue(name, sdata));
    } 
  } 
  return(false);
}

// --------------------------------------------------------
// Procedure: addFlag()
//   Purpose: Adds specified value to specified set of logic conditions.
//   Returns: true if the specified condition was able to be added to
//                 the specified vector of flags; 
//            false otherwise.

bool LogChecker::addFlag(string value, vector<LogicCondition> &flags)
{
  LogicCondition new_condition;
  if(new_condition.setCondition(value)) {
    // Add the condition to the flags
    flags.push_back(new_condition);
    // Update m_vars with the variables involved in this condition
    m_vars = mergeVectors( m_vars, new_condition.getVarNames() );
    m_vars = removeDuplicates(m_vars);
    return(true);
  } 
  else 
    return(false);
} 

// --------------------------------------------------------
// Procedure: addFlag()
//   Purpose: Checks the info buffer to see if the specified flags 
//            have been satisfied.
//   Returns: true if all of the specified logic conditions have been 
//            satisfied; false otherwise.

bool LogChecker::checkFlags(vector<LogicCondition> flags) 
{
  // If the info buffer isn't initialized, return false
  if(!m_info_buffer)
    return(false);
  
  // If the flags are empty, return false
  if(flags.empty())
    return(false);
  
  unsigned int i, j, vsize, csize;
  
  // Phase 1: get all the variable names from all present conditions.
  vector<string> all_vars;
  csize = flags.size();
  for(i=0; i<csize; i++) {
    vector<string> svector = flags[i].getVarNames();
    all_vars = mergeVectors(all_vars, svector);
  }
  all_vars = removeDuplicates(all_vars);
  
  // Phase 2: get values of all variables from the info_buffer and
  // propogate these values down to all the logic conditions.
  vsize = all_vars.size();
  for(i=0; i<vsize; i++) {
    string varname = all_vars[i];
    bool ok_s, ok_d;
    string s_result = m_info_buffer->sQuery(varname, ok_s);
    double d_result = m_info_buffer->dQuery(varname, ok_d);
    
    for(j=0; (j<csize) && (ok_s); j++)
      flags[j].setVarVal(varname, s_result);
    for(j=0; (j<csize) && (ok_d); j++)
      flags[j].setVarVal(varname, d_result);
  }
  
  // Phase 3: evaluate all logic conditions. Return true only if all
  // conditions evaluate to be true.
  for(i=0; i<csize; i++) {
    bool satisfied = flags[i].eval();
    if(!satisfied)
      return(false);
  } 
    
  return(true);
} 








