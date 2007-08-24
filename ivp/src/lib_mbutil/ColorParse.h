/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ColorParse.h                                         */
/*    DATE: Aug 19th 2006                                        */
/*****************************************************************/

#ifndef COLOR_PARSE_UTIL_HEADER
#define COLOR_PARSE_UTIL_HEADER

#include <vector>
#include <string>

std::vector<double> colorParse(const std::string&);
std::vector<double> colorHexToVector(const std::string&);
std::vector<double> colorDecToVector(const std::string&);
std::string         colorNameToHex(const std::string&);

double rOfRGB(const std::string);
double gOfRGB(const std::string);
double bOfRGB(const std::string);

#endif
