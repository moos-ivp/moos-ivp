// Miscellaneous Utilities - modem utilities
//
// set of routines which work with modem data
//
// Author: Alexander Bahr abahr@mit.edu
// Created: 18 July 2007
// Last modified: 23 July 2007
//
///////////////////////////////////////////////////////////////////////////////

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MU.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// Generate the  checksum for an NMEA string
//
// INPUT ARGUMENTS:
//		NMEA_string:		thestring for which the checksum will be generated
//		verbose:			print debug messages if set to TRUE
//
// RETURN ARGUMENT:
//		success_flag:		TRUE, the NMEA string contains the two delimiters $ and *
//		
// MODIFIED INPUT ARGUMENTS:
//		check_sum:			checksum for NMEA string as hex characters
///////////////////////////////////////////////////////////////////////////////
bool GenerateNMEAchecksum(string NMEA_string,string *check_sum, bool debug)
{
	// This is a modified version of the VerifyChecksum() code written by
	// Andrew Patrikalakis (anrp@mit.edu)
	// The original code can be found in <MOOSPATH>/lib/anrp_util/NMEAMessage.h
	
	const char *str;
	char *st, *ed;
	int sum = 0;
	
	bool success_flag=false;
	
	if(debug) MOOSTrace("MU_GenerateNMEAchecksum: generate checksum for %s\n",NMEA_string.c_str());
	
	str=NMEA_string.c_str();
	
	std::stringstream ss;
	
	// find delimiting characters of NMEA string $ and *	
	st = strchr(str, '$');
	ed = strchr(str, '*');

	if (!st || !ed) 
	{
	if(debug) MOOSTrace("Missing $/*\n");
	
	}
	else
	{
		success_flag=true;
	}
	
	// set pointer to first character after $
	st++;
	
	// iterate
	for (st; st < ed; st++) 
	{
		sum ^= *st;
	}
	
	if(debug) MOOSTrace("MU_GenerateNMEAchecksum: Checksum in decimal %d\n",sum);
	
	// convert decimal checksum to hex format 
	ss.setf(std::ios_base::hex,std::ios_base::basefield);
	ss << uppercase << sum;
	ss >> *check_sum;
	
	if(debug) MOOSTrace("MU_GenerateNMEAchecksum: Checksum in hexadecimal %s\n",(*check_sum).c_str());
	
	return success_flag;
}




