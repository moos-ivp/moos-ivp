/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: main.cpp                                             */
/*    DATE: Nov 25th, 2006                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "XYEncoders.h"
#include "GridGenerator.h"

using namespace std;

void help_message();

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int res = 0;

  // Look for a request for help
  res = getArg(argc, argv, 0, "-h", "--help");
  if(res) {
    help_message();
    return(0);
  }

  GridGenerator ggen;

  // BEGIN pulling parameters from the command line arguments
  string polygon = "label,A : -200,-100:-200,-300:300,-300:300,0:100,0";
  res = getArg(argc, argv, 1, "-p", "--poly");
  if(res)
    polygon = argv[res];

  string grid_element = "10,10";
  res = getArg(argc, argv, 1, "-g", "--gel         ");
  if(res)
    grid_element = argv[res];

  string output_file;
  res = getArg(argc, argv, 1, "-f", "--file");
  if(res)
    output_file = argv[res];

  string xfig_output_file;
  res = getArg(argc, argv, 1, "-x", "--xfig");
  if(res)
    xfig_output_file = argv[res];

  string rand_request;
  res = getArg(argc, argv, 1, "-r", "--rand");
  if(res)
    rand_request = argv[res];

  string aof_config = "4,70,0,100,true";
  res = getArg(argc, argv, 1, "-a", "--aof");
  if(res)
    aof_config = argv[res];



  // BEGIN initializing the grid size and shape
  string init_str = polygon + "@" + grid_element;
  bool ok = ggen.initializeGrid(init_str);
  if(!ok) {
    cout << "Unable to initialize Grid!!!!" << endl;
    exit(0);
  }

  // BEGIN initializing the AOF ("rings,exp,base,range,rpeak")
  ok = ggen.initializeAOF(aof_config);
  if(!ok) {
    cout << "Unable to initialize AOF!!!!" << endl;
    exit(0);
  }

  
  // BEGIN Assigning RANDOM weights to the grid
  if(rand_request != "") {
    vector<string> svector = parseString(rand_request, ',');
    if(svector.size() == 2) {
      double lowval = atof(svector[0].c_str());
      double highval = atof(svector[1].c_str());
      ggen.setRandWeights(lowval, highval);
    }
  }


  // BEGIN Assigning AOF_RINGS weights to the grid
  if(aof_config != "") {
    ggen.setAOFWeights();
  }


  // BEGIN Writing the generated grid to the requested file
  bool written_ok = false;
  if(output_file != "")
    written_ok = ggen.writeToFile(output_file);

  if(!written_ok)
    cout << "Grid was not written to file!" << endl;
  else
    cout << "Grid was successfully written to file " << output_file << endl;

  // BEGIN Writing the generated grid to the requested XFIG file
  bool xwritten_ok = false;
  if(xfig_output_file != "")
    xwritten_ok = ggen.writeToFigFile(xfig_output_file);

  if(!xwritten_ok)
    cout << "Grid was not written to xfig file!" << endl;
  else
    cout << "Grid was successfully written to xfig file " << xfig_output_file << endl;
}

//--------------------------------------------------------
// Procedure: help_message()

void help_message()
{
  cout << endl;
  cout << "Usage: gridgen                                       " << endl;
  cout << " Parameter        Defaults Description               " << endl;
  cout << " ----------       -------- ------------------------  " << endl;
  cout << "-p,--polygon               polygon bound             " << endl;
  cout << "-g,--gel            25,25  size of grid element      " << endl;
  cout << "-f,--file                  name of output file       " << endl;
  cout << "                                                     " << endl;
  cout << "-h,--help                  Display this help message " << endl;
  cout << endl;
}




