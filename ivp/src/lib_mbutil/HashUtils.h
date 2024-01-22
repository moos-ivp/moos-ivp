/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: HashUtils.h                                          */
/*    DATE: Nov 2nd, 2022                                        */
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
 
#ifndef HASH_UTILS_HEADER
#define HASH_UTILS_HEADER

#include <vector>
#include <string>

std::string missionHash();
std::string missionHashShort(std::string);

std::string hashAlphaNum(unsigned int len=6);
std::string hashAlphaUpper(unsigned int len=6);

std::string hashRandAdj();

std::string getCurrYear(bool full=false);
std::string getCurrMonth();
std::string getCurrDay();
std::string getCurrHour();
std::string getCurrMinute();
std::string getCurrSeconds();
std::string getCurrSecsUTC();
double      getCurrTimeUTC();

std::string randomWord(const std::vector<std::string>& );

std::vector<std::string> adjectives4();
std::vector<std::string> nouns4();
std::vector<std::string> names4();

#endif

