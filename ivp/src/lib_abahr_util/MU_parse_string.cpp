#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MU.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// parse and check a delimited string
// the structure of the delimited string is specified in the string_format string
// which can have an infinite number: [S,N][,=;:]
// where [S,N] is a type descriptor and specifies a string (S) or a number (N)
// [,=;:] represents a delimiter , = ; or :
// Spaces are ignored
//
// Example:
//
//		S=S,N,S
//
//		matches a string like AUV_1 =MARCURA,5,AUV but NOT AUV_1 =MACURUA,yellow,AUV
//		and not AUV_1=MACURUA,yellow,AUV,7
//		as yellow is not a number and 7 is an extra argument
//
// INPUT ARGUMENTS:
//		delimited_string:	string with comma-separated <keyword>=<value> pairs 
//		format_string:		the string format as indicated in the explanation above
//		debug:				print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if the string matched the indicated format
//
// MODIFIED INPUT ARGUMENTS:
//		parameter_list:		list of the parameters between the delimiters
//							NOTE: the parameters are all strings and the 
//							strings representing numbers still need to be
//							converted outside this routine
///////////////////////////////////////////////////////////////////////////////

bool MU_parse_string(string delimited_string , string format_string, std::list<string> *parameter_list, bool debug)
{
	bool success_flag=true;
	bool last_was_delimiter=true;
	bool parameter_is_string=true;
	
	int start_position,end_position,delimiter_position;
	
	string format_string_element;
	string parameter_string;
	
	string::iterator format_pointer;
	
	// add final "," to format_string
	format_string=format_string+",";
	
	// add final "," to delimited_string
	delimited_string=delimited_string+",";
	
	//set format_pointer to the beginning of the format string
	format_pointer=format_string.begin();
	
	//set the start and end pointer to the beginning of the string
	start_position=0;
	end_position=0;
	
	// clear parameter list
	(*parameter_list).clear();
	
	if(debug) 
	{
		MOOSTrace("MU_parse_string: delimited string (with added \",\" at the end): \"%s\"\n",delimited_string.c_str());
		MOOSTrace("MU_parse_string: format string (with added \",\" at the end): \"%s\"\n",format_string.c_str());
	}
	
	// check if we got to the last character of the format_string or if parsing failed
	while((format_pointer!=format_string.end()) && success_flag)
	{

		// get the element of the format string at the actual format_string pointer position
		format_string_element=*format_pointer;
		
		if(debug) MOOSTrace("MU_parse_string: format string element is \"%s\"\n",format_string_element.c_str());
		
		// check if the format string element is a delimiter
		if(format_string_element.find_first_of(",=;:")!=string::npos)
		{
			// check if the last format_string parameter was already a delimiter
			if(format_pointer==format_string.begin())
			{
				if (debug) 
				{
					MOOSTrace("MU_parse_string: parsing ERROR: first character is delimiter\n");
					success_flag=false;
				}
			}		
			
			// check if the first character is a delimiter
			else if(last_was_delimiter)
			{
				if (debug) 
				{
					MOOSTrace("MU_parse_string: parsing ERROR: two successive delimiters \n");
					success_flag=false;
				}
			}
			
			// this is a legitimate delimiter, extract parameter
			else
			{
				// set the start position to the end position of the previous search+1 if the end
				// position is not at 0 (initialization)
				if(end_position!=0) start_position=end_position+2;
				
				// set the end position to the the character before the delimiter
				delimiter_position=delimited_string.find(format_string_element,start_position);
				
				if(delimiter_position==string::npos)
				{
					MOOSTrace("MU_parse_string: parsing ERROR: delimiter \"%s\" not encountered in remaining string \"%s\"\n",format_string_element.c_str(),delimited_string.substr(start_position).c_str());
					success_flag=false;
				} 
				
				else
				{
					if(debug) MOOSTrace("MU_parse_string: delimiter \"%s\" found at %d\n",format_string_element.c_str(),delimiter_position);
					
					// set end_position to one before delimiter
					end_position=delimiter_position-1;
					
					// extract string between the two delimiters
					parameter_string=delimited_string.substr(start_position,(end_position-start_position+1));
					
					if(debug) MOOSTrace("MU_parse_string: parameter_string: \"%s\"\n",parameter_string.c_str());
					
					if(!parameter_is_string)
					{
						if(MU_is_number(parameter_string,debug))
						{
							// if it is a number do not do anything, parameter will get added further down
						}
						
						else
						{
							// if it is NOT a number modify success_flag
							MOOSTrace("MU_parse_string: parsing ERROR: parameter_string \"%s\" is not a number when it should be\n",parameter_string.c_str());
							success_flag=false;
						}
					}
					
					// mark that this was a delimiter
					last_was_delimiter=true;
					
					// add parameter to list
					if(success_flag)
					{
						if(debug) MOOSTrace("MU_parse_string: adding parameter \"%s\"\n",parameter_string.c_str());
						(*parameter_list).push_back(parameter_string);
					}
				}
			}		
		}
		// if it is not a delimiter it is a type descriptor
		else
		{
			if((format_string_element=="S") || (format_string_element=="s"))
			{
				if(debug) MOOSTrace("MU_parse_string: parameter is of type string\n");
				parameter_is_string=true;
			}
			else if((format_string_element=="N") || (format_string_element=="n"))
			{
				if(debug) MOOSTrace("MU_parse_string: parameter is of type number\n");
				parameter_is_string=false;
			}
			else
			{
				if(debug) MOOSTrace("MU_parse_string: parsing ERROR: unrecognized type descriptor \"%s\"\n",format_string_element.c_str());
				success_flag=false;
			}
			
			// mark that this was NOT a delimiter
			last_was_delimiter=false;
		}
		
		// get to next character in format string
		format_pointer++;
	}
	
	return success_flag;
}
