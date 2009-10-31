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

#include <vector>
#include <string>
#include "ALogEntry.h"

std::string getTimeStamp(const std::string& line);
std::string getVarName(const std::string& line);
std::string getSourceName(const std::string& line);
std::string getDataEntry(const std::string& line);

std::string getNextRawLine(FILE*);
ALogEntry   getNextRawALogEntry(FILE*);

void   stripInsigDigits(std::string& line);
void   shiftTimeStamp(std::string& line, double logstart);
double getLogStart(const std::string& line);
void   addVectorKey(std::vector<std::string>&, 
		    std::vector<bool>&, std::string);

double getEpochSecsFromTimeOfDay(std::string, int format=0);
double getEpochSecsFromTimeOfDay(double hour, double min, double sec);
double getEpochSecsFromDayOfYear(std::string, int format=0);
double getEpochSecsFromDayOfYear(double day, double month, double year);

#endif 
