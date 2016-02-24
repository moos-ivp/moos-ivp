/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 2016                                             */
/*****************************************************************/

#include "MBUtils.h"
#include "MBTimer.h"
#include "AngleUtils.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "CPAEngineX.h"
#include "CPAEngine.h"

using namespace std;

void showHelpAndExit();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{ 
  double cnx = 100;
  double cny = 100;
  double cnh = 45;
  double cnv = 1.0;

  double osx = 200;
  double osy = 104;

  MBTimer timer;
  timer.start();

  CPAEngine engine(cny, cnx, cnh, cnv, osy, osx);

  double cpa = engine.evalCPA(315, 1.0, 60);
  
  cout << "cpa: " << cpa << endl;

  unsigned int total = 0;
  for(int k=0; k<50; k++) {
    for(double i=0; i<5; i=i+0.001) {
      for(double j=0; j<360; j=j+1) {
	total++;
	double cpa = engine.evalCPA(j, i, 60);
	if(cpa == 0)
	  cpa++;
      }
    }
  }

  timer.stop();
  double elapsed_time = timer.get_float_wall_time();

  double rate = ((double)(total)) / elapsed_time;

  unsigned long int ul_rate = rate;

  cout << endl << "done, total=" << uintToCommaString(total) << endl;
  cout << "rate: " << ulintToCommaString(ul_rate) << " cpa calculations per sec." << endl;

  return(0);
}


//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{ 
  cout << "Usage:                                         " << endl;
  cout << "  trig360                                      " << endl;

  exit(0);
}
