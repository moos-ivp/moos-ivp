/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: main.cpp                                            */
/*    DATE: Sep 15 2005                                         */
/****************************************************************/

#include <string>
#include <iostream>
#include "IO_Utilities.h"
#include "MBUtils.h"
#include "IvPFunction.h"
#include "Tuple3D.h"
#include "TupleTools.h"

using namespace std;

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  if(argc != 3)
    return(0);

  // Build the OF/PDMap from the given input file
  IvPFunction *of = readFunction(argv[1]);
  if(!of) {
    cout << "Could not build OF from File: " << argv[1] << endl;
    return(0);
  }
  else
    cout << "Successfully read OF from file " << argv[1] << endl;


  // Create the Tuples
  PDMap* pdmap = of->getPDMap();
  vector<Tuple3D> tvector = tupelize(pdmap);
  


  // Write the Tuples to the given output file
  FILE *f = fopen(argv[2], "w");
  if(!f) {
    cout << "Error opening file: " << argv[2] << endl;
    return(false);
  }

  for(int i=0; i<tvector.size(); i++) {
    string xstr = dstringCompact(doubleToString(tvector[i].x));
    string ystr = dstringCompact(doubleToString(tvector[i].y));
    string zstr = dstringCompact(doubleToString(tvector[i].z));
    fprintf(f, "%s %s %s\n", xstr.c_str(), ystr.c_str(), zstr.c_str());
  }
  
  fclose(f);
}
