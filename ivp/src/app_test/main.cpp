/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp                                             */
/*    DATE:                                                      */
/*****************************************************************/

#include "TestEncode.h"
#include "TestTemplate.h"
#include "TestMap.h"
#include "TestSet.h"
#include "TestRelAng.h"
#include "TestDemux.h"
#include "TestChompString.h"
#include "TestPacketWrap.h"
#include "TestIsNumber.h"
#include "TestAUV.h"
#include "TestGeo.h"
#include "TestStringSort.h"
#include "TestSubstring.h"
#include "TestUniformMap.h"
#include "MBUtils.h"
#include <math.h>

#include <iostream>
using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{ 
  return(testUniformMap(argc, argv));
  return(testSubstring(argc, argv));
  return(testGeo(argc, argv));
  
  return(testStringSort(argc, argv));
  return(testAUV(argc, argv));
  return(testIsNumber(argc, argv));
  return(testDemux(argc, argv));

  return(testEncode(argc, argv));
  return(testPacketWrap(argc, argv));

  //-----------------------------------------------
  return(testTemplate(argc, argv));
  return(testSet(argc, argv));
  return(testMap(argc, argv));
  return(testRelAng(argc, argv));
  return(testChompString(argc, argv));
}
