/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: FunctionEncoderTest.h                                */
/*    DATE: April 17th 2006 (Patriots Day)                       */
/*****************************************************************/

#ifndef FUNCTION_ENCODERTEST_HEADER
#define FUNCTION_ENCODERTEST_HEADER

#include <string>
#include "IvPFunction.h"

// Crreate an IvPFunction based on a string representation
IvPFunction *StringToIvPFunctionTest(const std::string&);

std::string doubleToString2(double);
std::string intToString2(int);

#endif

