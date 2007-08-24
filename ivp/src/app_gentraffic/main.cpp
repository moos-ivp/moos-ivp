/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: July 31st 2007                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "TrajectorySet.h"
#include "TrajectoryGenerator.h"

using namespace std;

int main(int argc, char *argv[])
{
  int i, res;
  
  res = getArg(argc, argv, 0, "-h", "--help");
  if(res) {
    cout << "Usage: " << argv[0] << endl;
    cout << " -x  --xcenter   <0>     " << endl;
    cout << " -y  --ycenter   <0>     " << endl;
    cout << " -p  --platforms <2>     " << endl;
    cout << " -r  --radius    <100.0> " << endl;
    cout << " -s  --speed     <3.0>   " << endl;
    cout << " -v  --variance  <10.0>  " << endl;
    cout << " -c  --cases     <10>    " << endl;
    cout << " -n  --noseed            " << endl;
    cout << " -h  --help              " << endl;
    return(0);
  }
  
  int amt_cases     = 10;
  int amt_platforms = 2;
  double radius     = 100;
  double variance   = 10;
  double speed      = 3.0;
  double x_center   = 0;
  double y_center   = -150;
  bool   seed       = true;

  res = getArg(argc, argv, 1, "-x", "--xcenter");
  if(res && (res < argc))
    x_center = atof(argv[res]);
  
  res = getArg(argc, argv, 1, "-y", "--ycenter");
  if(res && (res < argc))
    y_center = atof(argv[res]);
  
  res = getArg(argc, argv, 1, "-p", "--platforms");
  if(res && (res < argc))
    amt_platforms = atoi(argv[res]);
  
  res = getArg(argc, argv, 1, "-r", "--radius");
  if(res && (res < argc))
    radius = atof(argv[res]);
  
  res = getArg(argc, argv, 1, "-s", "--speed");
  if(res && (res < argc))
    speed = atof(argv[res]);
  
  res = getArg(argc, argv, 1, "-c", "--cases");
  if(res && (res < argc))
    amt_cases = atoi(argv[res]);
  
  res = getArg(argc, argv, 1, "-v", "--variance");
  if(res && (res < argc))
    variance = atof(argv[res]);

  res = getArg(argc, argv, 0, "-n", "--noseed");
  if(res)
    seed = false;
  
  TrajectoryGenerator tgen;
  if(seed)
    tgen.randomSeed();

  for(i=0; i<amt_cases; i++) {
    tgen.generate(x_center, y_center, radius, variance, 
		  amt_platforms, speed);
    TrajectorySet tset = tgen.getTSet();
    string str = tset.asString("concise");
    cout << i << str << endl;
  }

  return(0);
}





