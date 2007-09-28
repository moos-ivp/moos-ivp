// Miscellaneous Utils - modify string
//
// set of routines to modify strings
//
// Author: Alexander Bahr abahr@mit.edu
// Created: 20 August 2006
// Last modified: 20 Agust 2007
//
///////////////////////////////////////////////////////////////////////////////

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MU.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// capitalizes string and leaves input string intact
//
// INPUT ARGUMENTS:
//		input_string:		string with comma-separated <keyword>=<value> pairs 
//
// RETURN ARGUMENT:
//		success_flag:		always TRUE
//
// MODIFIED INPUT ARGUMENTS:
//		ouput_string:		if keyword was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

bool MU_capitalize(string input_string, string *output_string)
{
	// make copy of input string
	(*output_string)=input_string;
	
	// capitalize output_string
	std::transform((*output_string).begin(), (*output_string).end(), (*output_string).begin(), (int(*)(int)) toupper);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// capitalizes string and leaves input string intact
//
// INPUT ARGUMENTS:
//		input_string:		string which will get capitalized 
//
// RETURN ARGUMENT:
//		ouput_string:		if keyword was encountered it contains the value
///////////////////////////////////////////////////////////////////////////////

string MU_capitalize(string input_string)
{
	
	string output_string;
	
	// make copy of input string
	output_string=input_string;
	
	// capitalize output_string
	std::transform(output_string.begin(), output_string.end(), output_string.begin(), (int(*)(int)) toupper);
	
	return output_string;
}

