// Miscellaneous Utilities - number utilities
//
// set of routines which work with modem data
//
// Author: Alexander Bahr abahr@mit.edu
// Created: 23 July 2007
// Last modified: 9 Agust 2007
//
///////////////////////////////////////////////////////////////////////////////

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MU.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// checks if string contains a number
//
// INPUT ARGUMENTS:
//		test_string:		string which is tested for being a number
//		debug:				print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, if strng is a number; FALSE if not
//
///////////////////////////////////////////////////////////////////////////////
bool MU_is_number(string test_string,bool debug)
{
	bool return_value;
	
	size_t num_of_valid_characters;
	size_t num_of_characters;
	
	if(debug) MOOSTrace("MU_is_number: test string is \"%s\"\n",test_string.c_str());
	
	num_of_valid_characters=strspn(test_string.c_str(),"+-.0123456789");
	num_of_characters=test_string.size();
	
	if(num_of_valid_characters==num_of_characters)
	{
		// check if there is only one sign and that it is the first character
		if(test_string.find(".")==test_string.rfind(".")) 
		{
			if((test_string.find_first_of("+-")==test_string.find_last_of("+-")) && (test_string.find_first_of("+-")==test_string.find_last_of("+-")))
			{
				return_value=true;
				if(debug) MOOSTrace("MU_is_number: %s IS a number\n",test_string.c_str());
			}
			else
			{
				return_value=false;
				if(debug) MOOSTrace("MU_is_number: %s is NOT a number (too many or misplaced signs)\n",test_string.c_str());
			}
		}
		else
		{
			return_value=false;
			if(debug) MOOSTrace("MU_is_number: %s is NOT a number (too many fraction marks)\n",test_string.c_str());
		}
	}
	else
	{
		return_value=false;
		if(debug) MOOSTrace("MU_IsNumber: %s is NOT a number (non-permitted chracters)\n",test_string.c_str());
	}
	
	return return_value;
}

