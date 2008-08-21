/*****************************************************************/
/*    NAME: Michael Benjamin and Henrik Schmidt                  */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogUtils.h                                           */
/*    DATE: August 7th, 2008                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef LOG_UTILS_HEADER
#define LOG_UTILS_HEADER

#include <string>

std::string getVarName(const std::string& line);
std::string getTimeStamp(const std::string& line);
std::string getDataEntry(const std::string& line);

std::string getNextRawLine(FILE*);

void   stripInsigDigits(std::string& line);
void   shiftTimeStamp(std::string& line, double logstart);
double getLogStart(const std::string& line);
void   addVectorKey(std::vector<std::string>&, 
		    std::vector<bool>&, std::string);

#endif 
