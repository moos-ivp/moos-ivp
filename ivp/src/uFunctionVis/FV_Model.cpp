/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FV_Model.cpp                                         */
/*    DATE: Sep 24th 2006                                        */
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

#include <iostream>
#include "MBUtils.h"
#include "FV_Model.h"
#include "ColorParse.h"
#include "BuildUtils.h"

#define GL_PI 3.1415926f

using namespace std;

//-------------------------------------------------------------
// Constructor

FV_Model::FV_Model()
{
  m_lock_ipf   = false;
  m_curr_iter  = 0;

  m_bundle_series_maxlen = 50;
  m_verbose    = true;

  m_nav_heading = 0;
}

//-------------------------------------------------------------
// Procedure: addIPF()

void FV_Model::addIPF(const string& ipf_str, const string& platform)
{
  m_ipf_mutex.Lock();

  if(!m_lock_ipf) {
  
    m_bundle_series.addIPF(ipf_str);
    m_bundle_series_platform = platform;

    unsigned int bs_size = m_bundle_series.size();
    if(bs_size > m_bundle_series_maxlen)
      m_bundle_series.popFront(1);

    if(m_curr_source == "") {
      vector<string> sources = m_bundle_series.getAllSources();
      if(sources.size() > 0)
	m_curr_source = sources[0];
    }

    m_curr_iter = m_bundle_series.getMaxIteration();
    
    if(m_curr_source == "") 
      m_curr_source = m_bundle_series.getFirstSource();
  }
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: modColorMap

void FV_Model::modColorMap(string str)
{
  m_ipf_mutex.Lock();
  m_color_map.setType(str);
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: modSource()

void FV_Model::modSource(std::string bhv_source)
{
  vector<string> sources = m_bundle_series.getAllSources();
  if(vectorContains(sources, bhv_source))
    m_curr_source = bhv_source;
}

//-------------------------------------------------------------
// Procedure: getCurrSource()

string FV_Model::getCurrSource()
{
  if(m_collective != "")
    return("Collective Function");
  else {
    if(m_curr_source != "")
      return(m_curr_source);
    else
      return("no source detected");
  }
}

//-------------------------------------------------------------
// Procedure: getCurrPieces()

string FV_Model::getCurrPieces()
{
  if(m_collective != "")
    return("n/a");
  else {
    unsigned int pcs;
    pcs = m_bundle_series.getPieces(m_curr_iter, m_curr_source);
    return(uintToString(pcs));
  }
}

//-------------------------------------------------------------
// Procedure: getCurrPriority()

string FV_Model::getCurrPriority()
{
  if(m_collective != "")
    return("n/a");
  else {
    double pwt;
    pwt = m_bundle_series.getPriority(m_curr_iter, m_curr_source);
    return(dstringCompact(doubleToString(pwt)));
  }
}

//-------------------------------------------------------------
// Procedure: getCurrDomain()

string FV_Model::getCurrDomain()
{
  if(m_collective != "")
    return("n/a");
  else {
    IvPDomain ivp_domain;
    ivp_domain = m_bundle_series.getDomain(m_curr_iter, m_curr_source);
    return(domainToString(ivp_domain, false));
  }
}

//-------------------------------------------------------------
// Procedure: getCurrPlatform()

string FV_Model::getCurrPlatform()
{
  return(m_bundle_series_platform);
}

//-------------------------------------------------------------
// Procedure: getQuadSet()

QuadSet FV_Model::getQuadSet(bool dense)
{
  m_ipf_mutex.Lock();

  QuadSet quadset;
  if(m_collective != "")
    quadset = m_bundle_series.getCollectiveQuadSet(m_curr_iter, m_collective);
  else
    quadset = m_bundle_series.getQuadSet(m_curr_iter, m_curr_source, dense);
  
  m_ipf_mutex.UnLock();

  quadset.normalize(0,200);
  quadset.applyColorMap(m_color_map);
  return(quadset);
}






