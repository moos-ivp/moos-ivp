/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestMap.cpp                                          */
/*    DATE:                                                      */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"

using namespace std;

int testIsNumber(int argc, char *argv[])
{
  bool result;
  string str;
  
  str = "Hello";
  result = isNumber(str);
  cout << "isNumber [" << str << "]: " <<result << endl;

  str = "123";
  result = isNumber(str);
  cout << "isNumber [" << str << "]: " <<result << endl;

  str = "123.456";
  result = isNumber(str);
  cout << "isNumber [" << str << "]: " <<result << endl;

  str = "123.456.7";
  result = isNumber(str);
  cout << "isNumber [" << str << "]: " <<result << endl;

  str = "123  ";
  result = isNumber(str,false);
  cout << "isNumber [" << str << "]: " <<result << endl;

  str = "  123";
  result = isNumber(str);
  cout << "isNumber [" << str << "]: " <<result << endl;

  str = "0.123456789";
  result = isNumber(str);
  cout << "isNumber [" << str << "]: " <<result << endl;

  return(0);
}
