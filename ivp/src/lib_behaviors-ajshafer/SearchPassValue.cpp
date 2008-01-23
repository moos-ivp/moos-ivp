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
#include <math>

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

SearchPassValue::SearchPassValue()
{
}


//----------------------------------------------------------------
// Procedure: evalValue(time)
// Value is the log of the number of "passes" - 1 (to set 0 time = 0 value

double SearchPassValue::evalValue(double d_time) const
{
	return log10( d_time );
}
  
