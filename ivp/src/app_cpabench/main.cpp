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
  bool old_cpa  = false;
  bool new_cpa  = true;
  bool both_cpa = false;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi == "-h") || (argi == "--help"))
      showHelpAndExit();
    if((argi == "-o") || (argi == "--old")) {
      old_cpa = true;
      new_cpa = false;
    }
    if((argi == "-b") || (argi == "--both")) {
      both_cpa = true;
      new_cpa = false;
    }
  }

  double cnx = 100;
  double cny = 100;
  double osx = 200;
  double osy = 104;

  MBTimer timer;
  timer.start();

  unsigned int total = 0;

  if(new_cpa) {
    CPAEngineX enginex;
    for(double cnv=0; cnv<2.5; cnv+=0.1) {
      for(double cnh=0; cnh<360; cnh+=1) {      
	enginex.reset(cny, cnx, cnh, cnv, osy, osx);
	for(double osv=0; osv<2.5; osv+=0.1) {
	  for(double osh=0; osh<360; osh=osh+1) {
	    total++;
	    enginex.evalCPA(osh, osv, 60);
	  }
	}
      }
    }
  }

  if(old_cpa) {
    for(double cnv=0; cnv<2.5; cnv+=0.1) {
      for(double cnh=0; cnh<360; cnh+=1) {      
	CPAEngine engine(cny, cnx, cnh, cnv, osy, osx);
	for(double osv=0; osv<2.5; osv+=0.1) {
	  for(double osh=0; osh<360; osh=osh+1) {
	    total++;
	    engine.evalCPA(osh, osv, 60);
	  }
	}
      }
    }
  }
  
  if(both_cpa) {
    CPAEngineX enginex;
    for(double cnv=0; cnv<2.5; cnv+=0.1) {
      for(double cnh=0; cnh<360; cnh+=1) {      
	enginex.reset(cny, cnx, cnh, cnv, osy, osx);
	CPAEngine  engine(cny, cnx, cnh, cnv, osy, osx);
	for(double osv=0; osv<2.5; osv+=0.1) {
	  for(double osh=0; osh<360; osh=osh+1) {
	    total++;
	    double cpax = enginex.evalCPA(osh, osv, 60);
	    double cpa  = engine.evalCPA(osh, osv, 60);
	    double delta = cpa - cpax;
	    if(delta < 0)
	      delta = -delta;
	    if(delta > 0.00001)
	      cout << endl << "Mismatch: cpa:" << cpa << " cpax:" << cpax << endl;
	  }
	}
      }
    }
  }

  timer.stop();
  double elapsed_time = timer.get_float_cpu_time();

  double rate = ((double)(total)) / elapsed_time;

  unsigned long int ul_rate = rate;

  cout << endl << "done, total=" << uintToCommaString(total) << endl;
  cout << "rate: " << ulintToCommaString(ul_rate) << " cpa calcs per sec." << endl;

  return(0);
}


//--------------------------------------------------------
// Procedure: showHelpAndExit()

void showHelpAndExit()
{ 
  cout << "Usage:                                                   " << endl;
  cout << "  cpabench            (Runs the test  CPAEngineX)        " << endl;
  cout << "  cpabench --old,-o   (Runs the trunk CPAEngine)         " << endl;
  cout << "  cpabench --both,-b  (Runs both, checks for mismatches) " << endl;

  exit(0);
}
