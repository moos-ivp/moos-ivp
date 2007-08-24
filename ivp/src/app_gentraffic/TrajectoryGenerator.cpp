/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: TrajectoryGenerator.cpp                              */
/*    DATE: July 16th, 2007                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <string>
#include <iostream>
#include <stdlib.h>
#include "TrajectoryGenerator.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"

using namespace std;

//-----------------------------------------------------------------
// Procedure: randomSeed

void TrajectoryGenerator::randomSeed()
{
  unsigned long tseed = time(NULL);
  unsigned long hostid = gethostid();
  unsigned long pid = (long)getpid();
  unsigned long seed = (tseed%999999);
  seed = ((rand())*seed*hostid)%999999;
  seed = (seed*pid)%999999;
  srand(seed);
}



//-----------------------------------------------------------------
// Procedure: generate

void TrajectoryGenerator::generate(double c_x, double c_y, double radius,
				   double variance, double amt, double spd)
{
  if(amt <= 0)
    return;

  int i, j;

  // Generate a random heading
  vector<double> headings;
  for(i=0; i<amt; i++) {
    double hdg = rand() % 360;
    headings.push_back(hdg);
  }
  
  // Calculate the random ranges to the center point
  if(variance < 1)
    variance = 1.0;
  vector<double> ranges;
  for(i=0; i<amt; i++) {
    double delta = (double)((rand() % (int)(variance*2))) - variance;
    double range  = radius + delta;
    
    //cout << "[" << i << "] radius:   " << radius << endl;
    //cout << "[" << i << "] delta:    " << delta << endl;
    //cout << "[" << i << "] variance: " << variance << endl;
    //cout << "[" << i << "] range:    " << range << endl;

    ranges.push_back(range);
  }
  
  // Calculate the starting points based on heading and range
  vector<double> xstarts;
  vector<double> ystarts;
  for(i=0; i<amt; i++) {
    double r_x, r_y;
    double hdg = angle360(headings[i]-180);
    projectPoint(hdg, ranges[i], c_x, c_y, r_x, r_y); 
    xstarts.push_back(r_x);
    ystarts.push_back(r_y);
  }

  
  // Calculate the ending points based on heading and range
  vector<double> xstops;
  vector<double> ystops;
  for(i=0; i<amt; i++) {
    double r_x, r_y;
    double hdg = angle360(headings[i]);
    projectPoint(hdg, (radius*1.5), c_x, c_y, r_x, r_y); 
    xstops.push_back(r_x);
    ystops.push_back(r_y);
  }

  // Calculate the required speeds to result in a collision at center
  vector<double> speeds;
  speeds.push_back(spd);
  double secs = 0;
  if(spd != 0)
    secs = ranges[0] / spd;
  for(i=1; i<amt; i++) {
    if(secs != 0)
      speeds.push_back(ranges[i] / secs);
    else
      speeds.push_back(0);
  }

  // Create the TrajectorySet
  TrajectorySet tset;
  for(i=0; i<amt; i++) {
    tset.addTrajectory(xstarts[i], ystarts[i], speeds[i], headings[i],
		       xstops[i], ystops[i]);
  }

  m_trajectory_set = tset;
}

