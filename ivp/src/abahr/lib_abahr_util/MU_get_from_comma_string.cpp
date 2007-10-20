// Miscellaneous Utils - get from comma-separated string
//
// set of routines which fetches values from comma-delimited strings
//
// Author: Alexander Bahr abahr@mit.edu
// Created: 9 August 2006
// Last modified: 16 July 2007
//
///////////////////////////////////////////////////////////////////////////////

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MU.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// return a DOUBLE value for a specified keyword, i.e. [...],<keyword>=<value>,[...]
//
// INPUT ARGUMENTS:
//		sCommaString:		string with comma-separated <keyword>=<value> pairs 
//		sKeyword:			keyword for which the value is requested
//		debug:				print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if keyword was encountered; FALSE if not
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_double:	if keyword was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool MU_get_from_comma_string(string sCommaString , string sKeyword, double *parameter_double, bool debug)
{
	bool success_flag=true;
	
	if(sCommaString.find(sKeyword,0) != string::npos)
	{
		double return_double;
		
		// chop off everything up the = after sKeyword
		sCommaString=sCommaString.substr(sCommaString.find(sKeyword)+sKeyword.size()+1);
		//MOOSTrace("sCommaString: %s\n", sCommaString.c_str());
		
		// chop off everything up to the next ,
		sCommaString=sCommaString.substr(0,sCommaString.find(","));
		//MOOSTrace("Left-over string: %s\n",sCommaString.c_str());
				
		sscanf(sCommaString.c_str(),"%lf",&return_double);
		
		*parameter_double=return_double;
		
		if(debug)
		{
			MOOSTrace("MU_get_from_comma_string: %s = %lf\n",sKeyword.c_str(),return_double);
		}
	}
	
	else
	{
		if(debug)
		{
			MOOSTrace("MU_get_from_comma_string: Keyword not encountered\n");
			success_flag=false;
		}
	}
	
	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// return a STRING value for a specified keyword, i.e. [...],<keyword>=<value>,[...]
//
// INPUT ARGUMENTS:
//		sCommaString:		string with comma-separated <keyword>=<value> pairs 
//		sKeyword:			keyword for which the value is requested
//		debug:				print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if keyword was encountered; FALSE if not
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_string:	if keyword was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool MU_get_from_comma_string(string sCommaString , string sKeyword, string *parameter_string, bool debug)
{
	bool success_flag=true;
	
	if(sCommaString.find(sKeyword,0) != string::npos)
	{
		// chop off everything up the = after sKeyword
		sCommaString=sCommaString.substr(sCommaString.find(sKeyword)+sKeyword.size()+1);
		//MOOSTrace("sCommaString: %s\n", sCommaString.c_str());
		
		// chop off everything up to the next ,
		sCommaString=sCommaString.substr(0,sCommaString.find(","));
		//MOOSTrace("Left-over string: %s\n",sCommaString.c_str());
				
		*parameter_string=sCommaString;
		
		if(debug)
		{
			MOOSTrace("MU_get_from_comma_string: %s = %s\n",sKeyword.c_str(),sCommaString.c_str());
		}
	}
	
	else
	{
		if(debug)
		{
			MOOSTrace("MU_get_from_comma_string: Keyword not encountered\n");
			success_flag=false;
		}
	}
	
	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// return the nth value, which has to be a DOUBLE, from a comma sperated string
// of the form [...],<value>,<value>,[...] 
//
// INPUT ARGUMENTS:
//		sCommaString:		string with comma-separated values 
//		value_position:		position of the value
//		debug:				print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if parameter existed; FALSE if not
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_double:	if keyword was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool MU_get_nth_from_comma_string(string sCommaString , int parameter_position, double *parameter_double, bool debug)
{
	bool success_flag=false;
	
	int comma_counter=0;
	string::size_type parameter_start=0;
	string::size_type parameter_end=0;
	string::size_type pos_counter=-1;	// offset the first increment
	
	double return_double;
	
	do
	{
		
		// update counter with position of next , 
		pos_counter=sCommaString.find(",",pos_counter+1);
		
		// set marker for start of parameter_string, skip comma when not at first element
		if(parameter_end!=0)
		{
			parameter_start=parameter_end+2;	// skip comma between this and previous parameter
		}	
		
		// check if not at end of string yet
		if(pos_counter!=string::npos)
		{
			
			comma_counter++;
			
			// set marker for the end of parameter_string to one before comma
			parameter_end=pos_counter-1;
		}
		
		// if end of string was reached, set marker for end of parameter_string to end of string
		else
		{
			parameter_end=sCommaString.size();
		}
	}
	// check if end of string reached or comma after parameter reached
	while((pos_counter!=string::npos) && (comma_counter<parameter_position));
	
	// check that there were at least as many commas as required for the parameter position
	if(comma_counter>=parameter_position-1)
	{
		
		// check if there was a non-zero length double
		if(parameter_end>=parameter_start)
		{
			// extract string between commas
			sCommaString=sCommaString.substr(parameter_start,(parameter_end-parameter_start+1));
			
			sscanf(sCommaString.c_str(),"%lf",&return_double);
	
			*parameter_double=return_double;
			
			success_flag=true;
	
			if(debug)
			{
				MOOSTrace("MU_get_nth_from_comma_string: value#%i = %lf\n",parameter_position,return_double);
			}
		}
		else
		{
			if(debug)
			{
				MOOSTrace("MU_get_nth_from_comma_string: %ith parameter is empty\n",parameter_position);
			}
		}
	}
	
	else
	{
		if(debug)
		{
			MOOSTrace("MU_get_nth_from_comma_string: %ith parameter does not exist\n",parameter_position);
		}
	}
	
	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// return the nth value, which has to be an INT, from a comma sperated string
// of the form [...],<value>,<value>,[...] 
//
// INPUT ARGUMENTS:
//		sCommaString:		string with comma-separated values 
//		value_position:		position of the value
//		debug:				print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if parameter existed; FALSE if not
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_double:	if keyword was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool MU_get_nth_from_comma_string(string sCommaString , int parameter_position, int *parameter_int, bool debug)
{
	bool success_flag=false;
	
	int comma_counter=0;
	string::size_type parameter_start=0;
	string::size_type parameter_end=0;
	string::size_type pos_counter=-1;	// offset the first increment
	
	int return_int;
	
	do
	{
		
		// update counter with position of next , 
		pos_counter=sCommaString.find(",",pos_counter+1);
		
		// set marker for start of parameter_string, skip comma when not at first element
		if(parameter_end!=0)
		{
			parameter_start=parameter_end+2;	// skip comma between this and previous parameter
		}	
		
		// check if not at end of string yet
		if(pos_counter!=string::npos)
		{
			
			comma_counter++;
			
			// set marker for the end of parameter_string to one before comma
			parameter_end=pos_counter-1;
		}
		
		// if end of string was reached, set marker for end of parameter_string to end of string
		else
		{
			parameter_end=sCommaString.size();
		}
	}
	// check if end of string reached or comma after parameter reached
	while((pos_counter!=string::npos) && (comma_counter<parameter_position));
	
	// check that there were at least as many commas as required for the parameter position
	if(comma_counter>=parameter_position-1)
	{
		
		// check if there was a non-zero length double
		if(parameter_end>=parameter_start)
		{
			// extract string between commas
			sCommaString=sCommaString.substr(parameter_start,(parameter_end-parameter_start+1));
			
			return_int=atoi(sCommaString.c_str());
	
			(*parameter_int)=return_int;
			
			success_flag=true;
	
			if(debug)
			{
				MOOSTrace("MU_get_nth_from_comma_string: value#%i = %i\n",parameter_position,return_int);
			}
		}
		else
		{
			if(debug)
			{
				MOOSTrace("MU_get_nth_from_comma_string: %ith parameter is empty\n",parameter_position);
			}
		}
	}
	
	else
	{
		if(debug)
		{
			MOOSTrace("MU_get_nth_from_comma_string: %ith parameter does not exist\n",parameter_position);
		}
	}
	
	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// return the nth value, which has to be a STRING, from a comma sperated string
// of the form [...],<value>,<value>,[...] 
//
// INPUT ARGUMENTS:
//		sCommaString:		string with comma-separated values 
//		value_position:		position of the value
//		debug:				print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if parameter existed; FALSE if not
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_string:	if keyword was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool MU_get_nth_from_comma_string(std::string sCommaString , int parameter_position, std::string *parameter_string, bool debug)
{
	bool success_flag=false;
	
	int comma_counter=0;
	string::size_type parameter_start=0;
	string::size_type parameter_end=0;
	string::size_type pos_counter=-1;	// offset the first increment
	
	double return_double;
	
	do
	{
		
		// update counter with position of next , 
		pos_counter=sCommaString.find(",",pos_counter+1);
		
		// set marker for start of parameter_string, skip comma when not at first element
		if(parameter_end!=0)
		{
			parameter_start=parameter_end+2;	// skip comma between this and previous parameter
		}	
		
		// check if not at end of string yet
		if(pos_counter!=string::npos)
		{
			
			comma_counter++;
			
			// set marker for the end of parameter_string to one before comma
			parameter_end=pos_counter-1;
		}
		
		// if end of string was reached, set marker for end of parameter_string to end of string
		else
		{
			parameter_end=sCommaString.size();
		}
	}
	// check if end of string reached or comma after parameter reached
	while((pos_counter!=string::npos) && (comma_counter<parameter_position));
	
	// check that there were at least as many commas as required for the parameter position
	if(comma_counter>=parameter_position-1)
	{
		
		// check if there was a non-zero length double
		if(parameter_end>=parameter_start)
		{
			// extract string between commas
			sCommaString=sCommaString.substr(parameter_start,(parameter_end-parameter_start+1));
			
			*parameter_string=sCommaString.c_str();
			
			success_flag=true;
	
			if(debug)
			{
				MOOSTrace("MU_get_nth_from_comma_string: value#%i = %s\n",parameter_position,sCommaString.c_str());
			}
		}
		else
		{
			if(debug)
			{
				MOOSTrace("MU_get_nth_from_comma_string: %ith parameter is empty\n",parameter_position);
			}
		}
	}
	
	else
	{
		if(debug)
		{
			MOOSTrace("MU_get_nth_from_comma_string: %ith parameter does not exist\n",parameter_position);
		}
	}
	
	return success_flag;
}

///////////////////////////////////////////////////////////////////////////////
// return a list of strings from a comma-separated string
//
// INPUT ARGUMENTS:
//		comma_string:		comma separated string 
//		debug:				print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if at comma_string is not empty, i.e. at least one
//							value is in the list
//
// MODIFIED INPUT ARGUMENTS:
//		string_list:		list of strings of the in-between-comma sections 
///////////////////////////////////////////////////////////////////////////////

bool MU_separate_comma_string(std::string comma_string,std::list<std::string> *string_list, bool debug)
{
	bool success_flag=false;
	
	int comma_counter=0;
	string::size_type parameter_start=0;
	string::size_type parameter_end=0;
	string::size_type pos_counter=-1;	// offset the first increment
		
	string list_element;
		
	// clear list of strings
	(*string_list).clear();
	
	// check if string is empty
	if((comma_string.empty()) && (debug))
	{
		MOOSTrace("MU_separate_comma_string: Comma string is empty\n");
	}
	else
	{
		do
		{
			
			// update counter with position of next , 
			pos_counter=comma_string.find(",",pos_counter+1);
			
			// set marker for start of parameter_string, skip comma when not at first element
			if(parameter_end!=0)
			{
				parameter_start=parameter_end+2;	// skip comma between this and previous parameter
			}	
			
			// check if not at end of string yet
			if(pos_counter!=string::npos)
			{
				
				comma_counter++;
				
				// set marker for the end of parameter_string to one before comma
				parameter_end=pos_counter-1;
			}
			
			// if end of string was reached, set marker for end of parameter_string to end of string
			else
			{
				parameter_end=comma_string.size();
			}
			
			// check if there was a non-zero length double
			if(parameter_end>=parameter_start)
			{
				// extract string between commas
				list_element=comma_string.substr(parameter_start,(parameter_end-parameter_start+1));
				
				// add new string element to list
				(*string_list).push_front(list_element);
				
				if(debug)
				{
					MOOSTrace("MU_separate_comma_string: Adding %s to list of strings\n",list_element.c_str());
				}
			}
			else
			{
				// add empty element to list
				(*string_list).push_front(list_element);	
				
				if(debug)
				{
					MOOSTrace("MU_separate_comma_string: Adding empty element\n");
				}
			}
		}
		// check if end of string reached or comma after parameter reached
		while(pos_counter!=string::npos);
		
		// check that there were at least as many commas as required for the parameter position
		if((comma_counter=0) && debug)
		{
			MOOSTrace("MU_separate_comma_string: Only 1 element added to list. No commas encountered\n");
			success_flag=true;
			
		}
		else
		{
			success_flag=true;
		}
	}
	return success_flag;
}

