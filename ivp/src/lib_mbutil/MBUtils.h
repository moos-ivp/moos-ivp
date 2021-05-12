/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MBUtils.h                                            */
/*    DATE: (1996-2005)                                          */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef MBUTILS_HEADER_EX
#define MBUTILS_HEADER_EX

#include <vector>
#include <string>
#include <set>
#include <list>
#include <time.h>

std::vector<std::string> parseString(const std::string&, char);
std::vector<std::string> parseString(const std::string&, 
				     const std::string&);

std::vector<std::string> parseStringQ(const std::string&, char);
std::vector<std::string> parseStringQ(const std::string&, char, unsigned int);


std::vector<std::string> parseStringZ(const std::string&, char, 
				      const std::string&);

std::vector<std::string> parseStringToWords(const std::string&, char c=0);

std::vector<std::string> parseQuotedString(const std::string&, char separator);

std::vector<std::string> chompString(const std::string&, char);
std::vector<std::string> sortStrings(std::vector<std::string>);
std::vector<std::string> mergeVectors(std::vector<std::string>,
				      std::vector<std::string>);
std::vector<std::string> removeDuplicates(const std::vector<std::string>&);

bool vectorContains(const std::vector<std::string>&, 
		    const std::string&,
		    bool case_sensitive=true);

std::string augmentSpec(const std::string&, const std::string&, char=',');
std::string removeWhite(const std::string&);
std::string removeWhiteEnd(const std::string&);
std::string removeWhiteNL(const std::string&);

std::string biteString(std::string&, char);
std::string biteStringX(std::string&, char);
std::string biteString(std::string&, char, char);

std::string rbiteString(std::string&, char);
std::string rbiteStringX(std::string&, char);


std::string stripBlankEnds(const std::string&);
std::string stripBlankEnd(const std::string&);
std::string tolower(const std::string&);
std::string toupper(const std::string&);
std::string truncString(const std::string&, unsigned int newlen, 
			std::string="");
std::string boolToString(bool);
std::string uintToString(unsigned int);
std::string ulintToString(unsigned long int);
std::string intToString(int);
std::string intToCommaString(int);
std::string uintToCommaString(unsigned int);
std::string ulintToCommaString(unsigned long int);
std::string floatToString(float, int=5);
std::string setToString(const std::set<std::string>&);
std::string doubleToString(double, int=5);
std::string doubleToStringX(double, int=5);
std::string dstringCompact(const std::string&);
std::string compactConsecutive(const std::string&, char);
std::string findReplace(const std::string&, char, char);
std::string findReplace(const std::string&, const std::string&, 
			 const std::string&);
std::string padString(const std::string&, std::string::size_type, bool=true);
std::vector<std::string> padVector(const std::vector<std::string>&,
				   bool=true);
std::string stripComment(const std::string&, const std::string&);
std::string stripQuotes(const std::string&);
std::string stripBraces(const std::string&);
std::string stripChevrons(const std::string&);
std::string doubleToHex(double);

std::string svectorToString(const std::vector<std::string>&, char=',');

bool isValidIPAddress(const std::string&);
bool strContains(const std::string&, const std::string&);
bool strContains(const std::string&, const char);
bool strContainsWhite(const std::string&);
bool strBeginsWhite(const std::string&);
bool strBegins(const std::string&, const std::string&, bool=true);
bool strEnds(const std::string&, const std::string&, bool=true);

bool  tokParse(const std::string&, const std::string&, 
		char, char, std::string&);
bool  tokParse(const std::string&, const std::string&, 
		char, char, double&);
bool  tokParse(const std::string&, const std::string&, 
		char, char, bool&);

std::string tokStringParse(const std::string&, const std::string&, 
			   char, char);
double tokDoubleParse(const std::string&, const std::string&, 
		      char, char);

double minElement(const std::vector<double>&);
double maxElement(const std::vector<double>&);
double vclip(const double& var, const double& low, const double& high);
double vclip_min(const double& var, const double& low);
double vclip_max(const double& var, const double& high);
double randomDouble(double min=0, double max=1);

bool  isBoolean(const std::string&);
bool  isNumber(const std::string&, bool=true);
bool  isAlphaNum(const std::string&, const std::string& s="");
bool  isQuoted(const std::string&);
bool  isBraced(const std::string&);
bool  isChevroned(const std::string&);

int   getArg(int, char**, int, const char*, const char *s=0);
bool  scanArgs(int, char**, const char*, const char *a=0, const char *b=0);
int   validateArgs(int, char **, std::string);

double snapToStep(double, double v=1.0);
double snapDownToStep(double, double v=1.0);

bool  setPortTurnOnString(bool& boolval, std::string str, bool=true);
bool  setBooleanOnString(bool& boolval, std::string str, bool=true);
bool  setDoubleOnString(double& dval, std::string str);
bool  setUIntOnString(unsigned int& uint_val, std::string str);
bool  setPosUIntOnString(unsigned int& uint_val, std::string str);
bool  setIntOnString(int& int_val, std::string str);
bool  setPosDoubleOnString(double& dval, std::string str);
bool  setNonNegDoubleOnString(double& dval, std::string str);
bool  setNonWhiteVarOnString(std::string& svar, std::string str);
bool  setMinPartOfPairOnString(double& minval, double& maxval,
			       std::string str, bool negok=false);
bool  setMaxPartOfPairOnString(double& minval, double& maxval,
			       std::string str, bool negok=false);
bool  setDoubleRngOnString(double& dval, std::string str,
			   double minv, double maxv);

bool  okFileToRead(std::string);
bool  okFileToWrite(std::string);

std::string incIntString(std::string, int=1, bool=false);

void  millipause(int milliseconds);

std::string modeShorten(std::string, bool really_short=true);

std::vector<std::string> tokenizePath(const std::string&);

std::string parseAppName(const std::string&);

bool isKnownVehicleType(const std::string&);

unsigned int charCount(const std::string&, char);

std::vector<std::string> justifyLen(const std::vector<std::string>&,
				    unsigned int maxlen);
std::vector<std::string> justifyLen(const std::string&, unsigned int maxlen);

std::vector<std::string> joinLines(const std::vector<std::string>&,
				   bool preserve_line_count=false);

std::vector<std::string> breakLen(const std::vector<std::string>&,
				  unsigned int maxlen);
std::vector<std::string> breakLen(const std::string&,
				  unsigned int maxlen);

std::string checksumHexStr(std::string);

std::string stringListToString(std::list<std::string>, char sep_char=',');
std::string stringSetToString(std::set<std::string>, char sep_char=',');
std::string stringVectorToString(std::vector<std::string>, char sep_char=',');

std::string intToMonth(int, bool brief=false);

#endif
