/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestSet.cpp                                          */
/*    DATE:                                                      */
/*****************************************************************/

#include <iostream>

using namespace std;

int testAUV(int argc, char *argv[])
{
  int stime = 1049;

  cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%% LOG FILE:  " << endl;
  cout << "%% FILE OPENED ON  Fri Aug 25 21:45:19 2006" << endl;
  cout << "%% LOGSTART               1156542319.12" << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
  cout << "%%   (1) TIME " << endl;
  cout << "%%   (2) NAV_X" << endl;
  cout << "%%   (3) NAV_Y" << endl;
  cout << "%%   (4) NAV_HEADING"     << endl;
  cout << "%%   (5) NAV_SPEED"       << endl;
  cout << ""                         << endl;
  cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"          << endl;
  cout << "%% TIME            NAV_X      NAV_Y    NAV_HEADING      NAV_SPEED" << endl;
  cout << "%% TIME            [0.0]      [0.0]    [0.0]            [0.0]"     << endl;

  for(double i=0; i<2000; i=i+0.1) {
    cout << (i+1049) << "               ";
    cout << (i*3)    << "        ";
    cout << "1000       90   5" << endl;
  }

}
