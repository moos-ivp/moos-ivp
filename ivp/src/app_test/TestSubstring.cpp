/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestSubString.cpp                                    */
/*    DATE:                                                      */
/*****************************************************************/

#include <vector>
#include <string>
#include <iostream>
#include "MBUtils.h"

using namespace std;

int testSubstring(int argc, char *argv[])
{
  string str1 = " alpha = a[b]23 [beta <= 99] ";
  string str2 = "abcdefghijklmnopqrstuvwxyz";

  string str2b;
  str2b.assign(str2, 4, 100);

  cout << "[" << str1 << "]" << endl;
  cout << "[" << str2 << "]" << endl;
  cout << "[" << str2b << "]" << endl;

  int pos = str1.find_last_of('[');
  int len = str1.length();

  cout << "pos: " << pos << endl;
  cout << "len: " << len << endl;

  string str_a, str_b;
  str_a.assign(str1, 0, pos-1);
  str_b.assign(str1, pos, len);

  cout << "[" << str_a << "]" << endl;
  cout << "[" << str_b << "]" << endl;


  return(0);
}
