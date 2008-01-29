/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: SearchPassValue.cpp                                  */
/*    DATE: 23 Jan 2008                                          */
/*****************************************************************/
 
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif

#include <iostream>
#include "SearchPassValue.h"
#include "MBUtils.h"
#include <math.h>

using std::string;
using std::cout;
using std::vector;

//-----------------------------------------------------------
// Procedure: Constructor

SearchPassValue::SearchPassValue(string config_str)
{
	// Configuration string currently looks like this:
	// param=value,param2=value2,max_value=doubleval

	configured = false;
	
	if (config_str == ""){
		return;
	}
		
	double temp_value = -1;
	bool valid = tokParse(config_str, "max_value", ',', '=', temp_value);
	
	if(valid && temp_value >= 0){
		max_value = temp_value;
		config_string = config_str;
		configured = true;
	}
}


//----------------------------------------------------------------
// Procedure: evalValue(time)
// Value is the log of the number of "passes" - 1 (to set 0 time = 0 value)

double SearchPassValue::evalValue(double d_time) const
{
	if (configured && (d_time >= 0)){
		double new_val = log10(d_time) - 1;
		return (new_val <= max_value) ? new_val : max_value;
	}
}
  
  
//----------------------------------------------------------------
// Procedure: toString

string SearchPassValue::toString()
{
	if(!configured)
		return("");

	return config_string;
	
}
