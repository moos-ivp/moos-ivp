/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PathUtils.cpp                                        */
/*    DATE: Jan 29th, 2012 Sunday Morning at CSAIL               */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <vector>
#include <cstdlib>
#include "PathUtils.h"
#include "GeomUtils.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: bruteShortestPath

XYSegList greedyPath(XYSegList segl, double sx, double sy)
{
  //return(segl);
  vector<unsigned int> vertex_visited;
  vector<double>       vertex_x;
  vector<double>       vertex_y;
  
  unsigned int i, j, vsize = segl.size();
  
  for(i=0; i<vsize; i++) {
    vertex_x.push_back(segl.get_vx(i));
    vertex_y.push_back(segl.get_vy(i));
    vertex_visited.push_back(false);
  }
  
  XYSegList new_segl;
  for(i=0; i<vsize; i++) {
    // Begin finding the next vertex
    double smallest_dist = -1;
    unsigned int best_ix = 0;
    for(j=0; j<vsize; j++) {
      if(!vertex_visited[j]) {
	double jx = vertex_x[j];
	double jy = vertex_y[j];
	double jdist = distPointToPoint(sx, sy, jx, jy);
	if((smallest_dist < 0) || (jdist < smallest_dist)) {
	  smallest_dist = jdist;
	  best_ix = j;
	}
      }
    }
    // Done finding the next vertex
    // Begin updating using the new next vertex info
    if(smallest_dist >= 0) {
      double nextx = vertex_x[best_ix];
      double nexty = vertex_y[best_ix];
      vertex_visited[best_ix] = true;
      new_segl.add_vertex(nextx, nexty);
      sx = nextx;
      sy = nexty;
    }
  }
  return(new_segl);
}








