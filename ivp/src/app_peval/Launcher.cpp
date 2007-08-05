/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Launcher.cpp                                         */
/*    DATE: Mar 6th, 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "Launcher.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "MBTimer.h"
#include "Populator_NavPlot.h"
#include "Populator_LogPlots.h"
#include "Evaluator.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: launch

bool Launcher::launch()
{
  // Apply defaults? vehicle_0 ..N-1 to unnamed log files?
  // Check for parameter consistency? Dup slog? Dup labels?

  checkParams();
  handleSkews();
  handleNavPlots();
  handleLogPlots();

  int i;
  Evaluator evaluator;
  for(i=0; i<navplots.size(); i++)
    evaluator.addNavPlot(navplots[i]);
  for(i=0; i<logplots.size(); i++)
    evaluator.addLogPlot(logplots[i]);

  evaluator.evaluate(1.0);
  evaluator.makeLatexTable("foo.tex");

  return(true);
}


//--------------------------------------------------------------
// Procedure: handleParam()
// 
// o Each parameter is interpreted as either the name of an slog 
//   file, or the name of a vehicle associated with an slog file.
// o For example, the parameter sequence:
//   file1.slog  file2.slog  alpha  beta
//   would associated vehicle name alpha with file1.slog

void Launcher::handleParam(string param)
{
  if(strContains(param, ".slog")) {
    slog_files.push_back(param);
    slog_files_skew.push_back(0);
  }
  else
    slog_files_vehiname.push_back(param);
}


//--------------------------------------------------------------
// Procedure: checkParam()s
// 
// o Called after all params have been passed in.
// o Fills in default vehicle names if some missing
// o Checks against duplicate slog filenames
// o Checks against duplicate vehicle-names

bool Launcher::checkParams()
{
  int fsize = slog_files.size();
  for(int i=0; i<fsize; i++) {
    int lsize = slog_files_vehiname.size();
    if(i > (lsize-1)) {
      slog_files_vehiname.push_back("V_" + intToString(i));
    }
  }


  cout << "Total files: " << fsize << endl;
  for(int j=0; j<fsize; j++) {
    cout << "File[" << j << "]: " << slog_files[j] << endl;
  }
  
  int lsize = slog_files_vehiname.size();
  for(int k=0; k<fsize; k++) {
    cout << "VehicleName[" << k << "]: " << slog_files_vehiname[k] << endl;
  }
}


//--------------------------------------------------------------
// Procedure: handleSkews
// 
// o First read in the LOGSTART time stamps from each file and 
//   determine the minimum and set it to min_skew.
// o Then associate a skew with each slog file. One of them will
//   be zero (the one with the lowest LOGSTART value).

void Launcher::handleSkews()
{
  // Read in the skews from each slog file
  int i;
  double min_skew = 0;
  for(i=0; i<slog_files.size(); i++) {
    vector<string> svector = fileBuffer(slog_files[i], 5);
    int vsize = svector.size();
    for(int j=0; j<vsize; j++) {
      string line = compactConsecutive(svector[j], ' ');
      vector<string> evector = parseString(line, ' ');
      if(evector.size() >= 3)
	if(evector[1] == "LOGSTART") {
	  double skew = atof(evector[2].c_str());
	  slog_files_skew[i] = skew;
	  if((i==0) || (skew<min_skew))
	    min_skew = skew;
	}
    }
  }
  
  // Apply min_skew to all so the earliest has skew of zero
  for(i=0; i<slog_files_skew.size(); i++)
    slog_files_skew[i] -= min_skew;
}

//--------------------------------------------------------------
// Procedure: handleNavPlots
//   
//   Build all the navplots from the vector of slog files.

void Launcher::handleNavPlots()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Parsing slog files to build NavPlots..." << endl;


  for(int i=0; i<slog_files.size(); i++) {
    Populator_NavPlot pop_np;
    pop_np.set_skew(slog_files_skew[i]);
    bool ok = pop_np.set_file(slog_files[i]);
    if(!ok) {
      cout << "Problem with file " << slog_files[0] << ". Exiting" << endl;
      exit(0);
    }
    ok = pop_np.populate();
    if(!ok) {
      cout << "Could not determine X,Y trajectory in file " << slog_files[i];
      cout << ". Exiting now." << endl;
      exit(0);
    }
    NavPlot navplot = pop_np.get_navplot();
    navplot.set_vehicle(slog_files_vehiname[i]);
    navplots.push_back(navplot);
  }

  parse_timer.stop();
  cout << "Done: NavPlot parse time: " << parse_timer.get_float_cpu_time();
  cout << endl << endl;
}

//--------------------------------------------------------------
// Procedure: handleLogPlots
//   
//   Build all the loglots from the vector of slog files.

void Launcher::handleLogPlots()
{
  MBTimer parse_timer;
  parse_timer.start();
  cout << "Parsing slog files to build LogPlots..." << endl;

  for(int i=0; i<slog_files.size(); i++) {
    Populator_LogPlots pop_lp;
    pop_lp.set_skew(slog_files_skew[i]);
    pop_lp.set_vname("V_" + intToString(i)); 
    bool ok = pop_lp.set_file(slog_files[i]);
    if(!ok) {
      cout << "Problem with file " << slog_files[i] << ". Exiting" << endl;
      exit(0);
    }
    
    pop_lp.populate();
   
    LogPlot plot1 = pop_lp.get_logplot("DIST_TO_REGION");
    plot1.set_varname("DIST_TO_REGION");
    logplots.push_back(plot1);
  }
  
  parse_timer.stop();
  cout << "Done: LogPlot parse time: " << parse_timer.get_float_cpu_time();
  cout << endl << endl;
}




