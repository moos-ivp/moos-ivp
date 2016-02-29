/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    FILE: main.cpp                                             */
/*    DATE: Feb 2016                                             */
/*****************************************************************/

#include "MBUtils.h"
#include "MBTimer.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include <cmath>
#include <stdlib.h>
#include <iostream>
#include "CPAEngineX.h"
#include "CPAEngineOld.h"
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

  int  loops = 1;

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi == "-h") || (argi == "--help"))
      showHelpAndExit();
    if((argi == "-o") || (argi == "--old")) {
      old_cpa = true;
      new_cpa = false;
    }
    else if((argi == "-b") || (argi == "--both")) {
      both_cpa = true;
      new_cpa = false;
    }
    else if(isNumber(argi)) {
      loops = atoi(argi.c_str());
    }
  }

  double cnx = 100;
  double cny = 100;
  double osx = 200;
  double osy = 150;

  MBTimer timer;
  timer.start();

  unsigned int total = 0;

  //CPAEngineX test_engine(cny, cnx, 45, 2.0, osy, osx);
  //exit(0);

  
  if(new_cpa) {
    for(int i=0; i<loops; i++) {
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
  }

  if(old_cpa) {
    for(int i=0; i<loops; i++) {
      for(double cnv=0; cnv<2.5; cnv+=0.1) {
	for(double cnh=0; cnh<360; cnh+=1) {      
	  CPAEngineOld engine(cny, cnx, cnh, cnv, osy, osx);
	  for(double osv=0; osv<2.5; osv+=0.1) {
	    for(double osh=0; osh<360; osh=osh+1) {
	      total++;
	      engine.evalCPA(osh, osv, 60);
	    }
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
	    total+=2;;
	    double roc, rocx;
	    double cpax = enginex.evalCPA(osh, osv, 60, &rocx);
	    double cpa  = engine.evalCPA(osh, osv, 60, &roc);
	    double delta = cpa - cpax;
	    if(delta < 0)
	      delta = -delta;
	    double delta_roc = roc - rocx;
	    if(delta_roc < 0)
	      delta_roc = -delta_roc;
	    if(delta_roc > 0.00001) {
	      cout << endl << "Mismatch: roc:" << roc << " rocx:" << rocx << endl;
	      cout << "CPAEngine::evalROC(): " << engine.evalROC(osh, osv) << endl;
	      cout << "CPAEngineX::evalROC(): " << enginex.evalROC(osh, osv) << endl;
	    }
	    if(delta > 0.00001) {
	      cout << endl << "Mismatch: cpa:" << cpa << " cpax:" << cpax << endl;
	      cout << "Range: " << distPointToPoint(osx, osy, cnx, cny) << endl;
	      cout << "Contact: " << endl;
	      cout << "  cnv: " << cnv << endl;
	      cout << "  cnh: " << cnh << endl;
	      cout << "Ownship: " << endl;
	      cout << "  osv: " << osv << endl;
	      cout << "  osh: " << osh << endl << endl;
	    }
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
