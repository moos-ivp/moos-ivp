/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Evaluator.cpp                                        */
/*    DATE: March 4th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <math.h>
#include "Evaluator.h"
#include "PEVAL_Utilities.h"

using namespace std;


//---------------------------------------------------------------
// Procedure: evaluate

void Evaluator::evaluate(double step_time)
{
  double t;
  int i, j;
  int vnum = navplots.size();
  vreport = VReport(vnum);

  // Determine Collision information
  for(i=0; i<vnum; i++) {
    double min_time = navplots[i].get_min_time();
    double max_time = navplots[i].get_max_time();    

    cout << "Vehicle " << i << ": " << endl;
    cout << "  min_time: " << min_time;
    cout << "  max_time: " << max_time << endl;

    for(t=min_time; t<max_time; t+=step_time) {
      for(j=0; j<vnum; j++) {
	if(i != j) {
	  double dist = calcVehicleDist(i, j, t);
	  vreport.addDistInfo(i, step_time, dist);
	}
      }
    }
    

    // Handle Distance to Station info for this Vehicle
    int lix = -1;
    for(int k=0; k<logplots.size(); k++) {
      string varname = logplots[k].get_varname();
      string vehicle = logplots[k].get_vehicle();

      cout << "[" << k << "]" << endl;
      cout << "  varname: " << varname << endl;
      cout << "  i-vehi_name: " << navplots[i].get_vehicle() << endl;
      cout << "  k-vehi_name: " << logplots[i].get_vehicle() << endl;

      if(vehicle == navplots[i].get_vehicle())
	if(varname == "DIST_TO_REGION")
	  lix = k;
    }
    if(lix != -1) {
      for(t=min_time; t<max_time; t+=step_time) {
	double dist = logplots[lix].get_value_by_time(t);
	vreport.addStatInfo(i, step_time, dist);
      }
    }
    else
      cout << "No DIST_TO_REGION Info" << endl;



    // Handle Inter-Vehicle Proximity Distance for this Vehicle
    for(t=min_time; t<max_time; t+=step_time) {
      for(j=0; j<vnum; j++) {
	if(i != j) {
	  double dist = calcVehicleDist(i, j, t);
	  vreport.addDistInfo(i, step_time, dist);
	}
      }
    }
  }

  printVReport(vreport);

  return;
}



//---------------------------------------------------------------
// Procedure: makeLatexTable(file)

void Evaluator::makeLatexTable(string file)
{
  latexVReportToFile(vreport, file);
}


//---------------------------------------------------------------
// Procedure: calcVehicleDist

double Evaluator::calcVehicleDist(int ix1, int ix2, double time)
{
  if((ix1 == ix2)             ||
     (ix1 < 0) || (ix2 < 0)   ||
     (ix1 >= navplots.size()) ||
     (ix2 >= navplots.size()))
    return(0);

  double x1 = navplots[ix1].get_xval_by_time(time); 
  double y1 = navplots[ix1].get_yval_by_time(time); 
  double x2 = navplots[ix2].get_xval_by_time(time); 
  double y2 = navplots[ix2].get_yval_by_time(time); 

  double delta_x = x1-x2;
  double delta_y = y1-y2;
  
  return(hypot(delta_x, delta_y));
}





