/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MBUtils.h                                            */
/*    DATE: (1996-2005)                                          */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/
 
#ifndef MBUTILS_HEADER_EX
#define MBUTILS_HEADER_EX

#include <vector>
#include <string>
#include <time.h>

std::vector<std::string> parseString(const std::string&, char);
std::vector<std::string> parseString(const std::string&, 
				     const std::string&);
std::vector<std::string> chompString(const std::string&, char);
std::vector<std::string> sortStrings(std::vector<std::string>);
std::vector<std::string> mergeVectors(std::vector<std::string>,
				      std::vector<std::string>);
std::vector<std::string> removeDuplicates(const std::vector<std::string>&);

bool vectorContains(const std::vector<std::string>&, 
		    const std::string&);

std::string biteString(std::string&, char);
std::string stripBlankEnds(const std::string&);
std::string tolower(const std::string&);
std::string toupper(const std::string&);
std::string truncString(const std::string&, int, std::string="");
std::string boolToString(bool);
std::string intToString(int);
std::string floatToString(float, int=0);
std::string doubleToString(double, int=0);
std::string dstringCompact(const std::string&);
std::string compactConsecutive(const std::string&, char);
std::string findReplace(const std::string&, char, char);
std::string findReplace(const std::string&, const std::string&, 
			 const std::string&);
std::string padString(const std::string&, int, bool=true);
std::string stripComment(const std::string&, const std::string&);
std::string stripQuotes(const std::string&);
std::string doubleToHex(double);

std::string packageToString(const std::vector<std::string>&);

bool strContains(const std::string&, const std::string&);

bool  tokParse(const std::string&, const std::string&, 
		char, char, std::string&);
bool  tokParse(const std::string&, const std::string&, 
		char, char, double&);

std::string tokStringParse(const std::string&, const std::string&, 
			   char, char);
double tokDoubleParse(const std::string&, const std::string&, 
		      char, char);

bool  isNumber(const std::string&, bool=true);
bool  isQuoted(const std::string&);


int   getArg(int, char**, int, const char*, const char *s=0);
int   validateArgs(int, char **, std::string);

float snapToStep(float, float v=1.0);
float snapDownToStep(float, float v=1.0);

bool  okFileToRead(std::string);
bool  okFileToWrite(std::string);

void  millipause(int milliseconds);

#endif









