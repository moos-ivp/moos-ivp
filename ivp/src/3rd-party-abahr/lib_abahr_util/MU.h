#ifndef MU_H_
#define MU_H_

#include <stdio.h>
#include <string>
#include <iterator>
#include <list>
#include <sstream>
#include <cctype> 
#include <string>
#include <algorithm>

using namespace std;

bool MU_get_from_comma_string(std::string sCommaString , std::string sKeyword, double *parameter_double, bool debug);
bool MU_get_from_comma_string(std::string sCommaString , std::string sKeyword, std::string *parameter_string, bool debug);

bool MU_get_nth_from_comma_string(std::string sCommaString , int parameter_position, double *parameter_double, bool debug);
bool MU_get_nth_from_comma_string(std::string sCommaString , int parameter_position, std::string *parameter_string, bool debug);
bool MU_get_nth_from_comma_string(std::string sCommaString , int parameter_position, int *parameter_int, bool debug);

bool MU_parse_string(string delimited_string , string format_string, std::list<string> *parameter_list, bool debug);

bool MU_capitalize(string input_string, string *output_string);
string MU_capitalize(string input_string);

bool GenerateNMEAchecksum(string NMEA_string,std::string *check_sum, bool debug);

bool MU_is_number(std::string test_string,bool debug);

#endif /*MU_H_*/
