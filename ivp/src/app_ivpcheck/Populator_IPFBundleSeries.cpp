/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_IPFBundleSeries.cpp                        */
/*    DATE: April 18th, 2011                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
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

      if(varname == "BHV_IPF") {
	string data = getDataEntry(line_raw);
	m_demuxer.addMuxPacket(data);
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
