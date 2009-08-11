/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_VPlugPlots.cpp                             */
/*    DATE: August 9th, 2009                                     */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "Populator_VPlugPlots.h"
#include "MBUtils.h"
#include "FileBuffer.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: populateRawInfoFromALog()

bool Populator_VPlugPlots::populateRawInfoFromALog(string alog_file)
{
  FILE *f = fopen(alog_file.c_str(), "r");
  if(!f)
    return(false);
  fclose(f);

  vector<string> lines = fileBuffer(alog_file);

  unsigned int i, vsize = lines.size();
  for(i=0; i<vsize; i++) {
    lines[i] = findReplace(lines[i], '\t', ' ');
    lines[i] = compactConsecutive(lines[i], ' ');
    lines[i] = stripBlankEnds(lines[i]);
    
    string time = stripBlankEnds(biteString(lines[i],' '));
    string var  = stripBlankEnds(biteString(lines[i],' '));
    if((var == "VIEW_POINT")   ||  (var == "VIEW_SEGLIST") ||
       (var == "VIEW_POLYGON") ||  (var == "VIEW_CIRCLE")  ||
       (var == "GRID_CONFIG")  ||  (var == "GRID_DELTA")) {      
      string source = stripBlankEnds(biteString(lines[i],' '));
      string value  = stripBlankEnds(lines[i]);
      if((time!="")&&(var!="")&&(source!="")&&(value!="")) {
	m_visual_event_var.push_back(var);
	m_visual_event_val.push_back(value);
	m_visual_event_time.push_back(atof(time.c_str()));
      }
    }
  }
  return(true);
}

//---------------------------------------------------------------
// Procedure: populateVPlugPlotFromRawInfo()

bool Populator_VPlugPlots::populateVPlugPlotFromRawInfo()
{
  int i, vsize = m_visual_event_time.size();
  for(i=0; i<vsize; i++)
    m_vplug_plot.addEvent(m_visual_event_var[i], 
			  m_visual_event_val[i], 
			  m_visual_event_time[i]);
  return(true);
}

