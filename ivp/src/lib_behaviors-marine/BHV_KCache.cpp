/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_KCache.cpp                                       */
/*    DATE: Apr 29th 2015 (Collier dedication, Phoenix Talk)     */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdlib>
#include "BHV_KCache.h"
#include "MBUtils.h"
#include "BuildUtils.h"
#include "XYSeglr.h"
#include "ZAIC_PEAK.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_KCache::BHV_KCache(IvPDomain gdomain) : IvPBehavior(gdomain)
{
  m_descriptor = "bhv_kcache";

  m_domain = subDomain(m_domain, "pitch");

  m_hdg_gap = 10.0;
  m_spd     = 1.0;

  m_seglr_vertex_size = -1;  // No preference - user viewer defaults
  m_seglr_edge_width  = -1;  // No preference - user viewer defaults
  
  m_seglr_vertex_color = ""; // No preference - user viewer defaults
  m_seglr_edge_color   = ""; // No preference - user viewer defaults
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_KCache::setParam(string param, string val) 
{
  if((param == "seglr_vertex_color") && isColor(val))
    m_seglr_vertex_color = val;

  else if((param == "seglr_edge_color") && isColor(val))
    m_seglr_edge_color = val;

  else if((param == "seglr_vertex_size") && isNumber(val))
    m_seglr_vertex_size = atof(val.c_str());

  else if((param == "seglr_edge_width") && isNumber(val))
    m_seglr_edge_width = atof(val.c_str());

  else if((param == "hdg_gap") && isNumber(val)) {
    int ival = atoi(val.c_str());
    if((360 % ival) != 0)
      return(false);
    m_hdg_gap = (double)(ival);
  }

  else if((param == "speed") && isNumber(val)) 
    m_spd = atof(val.c_str());  

  else
    return(false);

  return(true);      
}

//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_KCache::onRunState() 
{
  if(m_kcache)
    postMessage("KCACHE_SIZE", m_kcache->size());
  else
    postMessage("KCACHE_SIZE", 0);


  ZAIC_PEAK zaic(m_domain, "pitch");
  zaic.setSummit(0);
  zaic.setBaseWidth(4);
  zaic.setPeakWidth(5);
  zaic.setSummitDelta(20);

  IvPFunction *ipf = zaic.extractIvPFunction();
  if(ipf)
    ipf->setPWT(m_priority_wt);


  postVisuals();
  return(ipf);
}


//-----------------------------------------------------------
// Procedure: postVisuals()

void BHV_KCache::postVisuals()
{
  if(!m_kcache || m_kcache->empty())
    return;

  vector<Seglr> seglrs;
  
  for(double hdg=-180; hdg <= 180; hdg += m_hdg_gap) {
    Seglr seglr = m_kcache->getSeglrCurr(m_spd, hdg);
    if(seglr.size() != 0)
      seglrs.push_back(seglr);
  }

  for(unsigned int i=0; i<seglrs.size(); i++) {
    XYSeglr xyseglr(seglrs[i]);
    xyseglr.set_label("sg_" + uintToString(i));
    string spec = xyseglr.getSpec();
    postMessage("VIEW_SEGLR", spec);
  }
}





