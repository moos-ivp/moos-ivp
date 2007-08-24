/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PopulatorTSet.cpp                                    */
/*    DATE: Jan 14th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "TrajectorySet.h"
#include "PopulatorTSet.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: populate
//      Note: An empty vector is the indication that something
//            went wrong in either finding the file or parsing it.

vector<TrajectorySet> PopulatorTSet::populate(string filename)
{
  FILE *f = fopen(filename.c_str(), "r");
  
  if(!f) {
    cout << "Could not find File: " << filename << endl;
    return(m_vtset);
  }
  
  cout << "Successfully found file: " << filename << endl;
  fclose(f);
  
  vector<string> file_vector = fileBuffer(filename);
  int lineCount = file_vector.size();
  
  for(int i=0; i<lineCount; i++) {
    string line = stripBlankEnds(file_vector[i]);
    if((line.length()!=0) && ((line)[0]!='/')) {
      bool res = handleLine(line);
      if(!res) {
	cout << " Problem with line " << i+1;
	cout << " in the file: " << filename << endl;
	cout << line << endl;
	m_vtset.clear();
	return(m_vtset);
      }
    }
  }

  return(m_vtset);
}


//-------------------------------------------------------------
// Procedure: handleLine

bool PopulatorTSet::handleLine(string line)
{
  // Comments are anything to the right of a "#" or "//"
  line = stripComment(line, "//");
  line = stripBlankEnds(line);

  if(line.size() == 0)  // Either blank or comment line
    return(true);  
  
  TrajectorySet new_tset;

  vector<string> svector = parseString(line, '#');
  int vsize = svector.size();
  for(int i=1; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> evector = parseString(svector[i], ',');
    if(evector.size() != 6)
      return(false);
    for(int j=0; j<6; j++) 
      evector[j] = stripBlankEnds(evector[j]);
    double sx = atof(evector[0].c_str());
    double sy = atof(evector[1].c_str());
    double sv = atof(evector[2].c_str());
    double sh = atof(evector[3].c_str());
    double dx = atof(evector[4].c_str());
    double dy = atof(evector[5].c_str());
    new_tset.addTrajectory(sx, sy, sv, sh, dx, dy);
  }
  
  m_vtset.push_back(new_tset);
  return(true);
}


