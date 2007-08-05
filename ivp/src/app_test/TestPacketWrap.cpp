/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: TestPacketWrap.cpp                                   */
/*    DATE: May 2006                                             */
/*****************************************************************/

#include <string>
#include <iostream>
#include "MBUtils.h"
#include "IvPFunction.h"
#include "IO_Utilities.h"
#include "OF_Reflector.h"
#include "AOF_SCALAR.h"
#include "AOF_Rings.h"
#include "MBTimer.h"
#include "FunctionEncoder.h"

using namespace std;

int testPacketWrap(int argc, char*argv[])
{
  int psize = 10;
  if(argc > 1)
    psize = atoi(argv[1]);

  string test = "ABCDEFGHIJKLMNOPQRSTUVWXYZ***ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  test += "***" + test;
  test += "***" + test;
  test += "***" + test;

  vector<string> svector = IvPFunctionToVector(test, "foo", psize);
  for(int i=0; i<svector.size(); i++) 
    cout << i << ": [" << svector[i] << "]" << endl;

}
