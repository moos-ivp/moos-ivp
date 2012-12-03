/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_IPFBundleSeries.cpp                        */
/*    DATE: April 18th, 2011                                     */
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

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "IvPProblem.h"
#include "MBUtils.h"
#include "Populator_IPFBundleSeries.h"
#include "LogUtils.h"
#include "FunctionEncoder.h"
#include "DomainExpander.h"

using namespace std;

//--------------------------------------------------------
// Constructor

Populator_IPFBundleSeries::Populator_IPFBundleSeries()
{
  m_capture_all_iterations = true;

  m_capture_iteration_low = 0;
  m_capture_iteration_hgh = 0;
}

//--------------------------------------------------------
// Procedure: setIterationRange

void Populator_IPFBundleSeries::setIterationRange(unsigned int low,
						  unsigned int hgh)
{
  if(low > hgh)
    return;
  m_capture_iteration_low = low;
  m_capture_iteration_hgh = hgh;
  m_capture_all_iterations = false;
}

//--------------------------------------------------------
// Procedure: setIterationsAll

void Populator_IPFBundleSeries::setIterationsAll()
{
  m_capture_iteration_low = 0;
  m_capture_iteration_hgh = 0;
  m_capture_all_iterations = true;
}

//--------------------------------------------------------
// Procedure: readFile
//     Notes: 

bool Populator_IPFBundleSeries::readFile(string alogfile)
{
  FILE *file_in = fopen(alogfile.c_str(), "r");
  if(!file_in)
    return(false);
 
  bool done = false;
  while(!done) {
    string line_raw = getNextRawLine(file_in);
    
    bool   line_is_comment = false;
    if((line_raw.length() > 0) && (line_raw.at(0) == '%'))
      line_is_comment = true;

    if(line_raw == "eof") 
      done = true;
    else if(!line_is_comment) {
      string varname = getVarName(line_raw);
      string srcname = getSourceName(line_raw);
      string tstamp  = getTimeStamp(line_raw);
      double dtstamp = atof(tstamp.c_str());

      if(varname == "BHV_IPF") {
	string data = getDataEntry(line_raw);
	m_demuxer.addMuxPacket(data, dtstamp);
      }
    }
    
    string ipf_str = m_demuxer.getDemuxString();
    if(ipf_str != "") {
      if(m_capture_all_iterations)
	m_bundle_series.addIPF(ipf_str);
      else {
	string iter_str = StringToIvPContext(ipf_str);
	unsigned int this_iter = atoi(iter_str.c_str());
	if((this_iter >= m_capture_iteration_low) &&
	   (this_iter <= m_capture_iteration_hgh)) {
	  m_bundle_series.addIPF(ipf_str);
	}
      }
    }
  }

  fclose(file_in);

  return(true);
}



