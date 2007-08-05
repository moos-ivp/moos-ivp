/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: TestGeo.cpp                                          */
/*    DATE:                                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "CircularUtils.h"
#include "MBTimer.h"

using namespace std;

int testGeo(int argc, char *argv[])
{
  cout << "acos(0)" << acos(0.0)/M_PI * 180 << endl;
  cout << "acos(1)" << acos(1.0)/M_PI * 180 << endl;
  cout << "acos(0.5)" << acos(0.5)/M_PI * 180 << endl;

  bool result;
  string str;
  int i;

  double x1      = 0;
  double y1      = 0;
  double radius  = 100;
  double heading = 350;
  double dist    = 75;
  bool   port    = true;

  double rx, ry, rheading;

  double wall, cpu;

  XYArc arc;
  MBTimer timer;
  timer.start();

  int count = 10000000;
  if(argc > 1)
    if(isNumber(argv[1]))
      count = atoi(argv[1]);

  // TEST 1
  for(i=0; i<count; i++) {
    arcFlight(x1,y1,heading,radius,dist,port,rx,ry,rheading);
  }
  timer.stop();
  wall = timer.get_float_wall_time();
  cpu  = timer.get_float_cpu_time();
  cout << "(1) Total time: (Wall)" << wall << "  (CPU)" << cpu << endl;

  // TEST 2
  timer.reset();
  timer.start();
  for(i=0; i<count; i++) {
    arc = arcFlight(x1,y1,heading,radius,dist,port);
  }
  timer.stop();
  wall = timer.get_float_wall_time();
  cpu  = timer.get_float_cpu_time();
  cout << "(2) Total time: (Wall)" << wall << "  (CPU)" << cpu << endl;

  return(0);
}
