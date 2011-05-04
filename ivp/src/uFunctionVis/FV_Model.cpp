/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_Model.cpp                                        */
/*    DATE: Sep 24th 2006                                       */
/****************************************************************/

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
  m_collective = false;
  m_lock_ipf   = false;
  m_curr_iter  = 0;

  m_bundle_series_maxlen = 50;
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

void FV_Model::modColorMap(const string &str)
{
  m_ipf_mutex.Lock();
  m_color_map.setType(str);
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: modSource()

void FV_Model::modSource(bool increment)
{
  if(m_collective)
    return;

  m_ipf_mutex.Lock();

  cout << "Old source: " << m_curr_source << endl;

  vector<string> sources = m_bundle_series.getAllSources();
  unsigned int i, vsize = sources.size();
  if(vsize == 0) {
    m_curr_source = "";
    return;
  }

  cout << "Sources size: " << vsize << endl;
  string new_source = sources[0];
  for(i=0; i<vsize; i++) {
    if(m_curr_source == sources[i]) {
      if(increment) {
	if(i == (vsize-1))
	  new_source = sources[0];
	else
	  new_source = sources[i+1];
      }
      else {
	if(i == 0)
	  new_source = sources[(vsize-1)];
	else
	  new_source = sources[i-1];
      }
    }
  }
  m_curr_source = new_source;

  cout << "New source: " << m_curr_source << endl;
  m_ipf_mutex.UnLock();
}

//-------------------------------------------------------------
// Procedure: getCurrSource()

string FV_Model::getCurrSource()
{
  if(m_collective)
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
  if(m_collective)
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
  if(m_collective)
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
  if(m_collective)
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

QuadSet FV_Model::getQuadSet()
{
  m_ipf_mutex.Lock();

  QuadSet quadset;
  if(m_collective)
    quadset = m_bundle_series.getCollectiveQuadSet(m_curr_iter);
  else
    quadset = m_bundle_series.getQuadSet(m_curr_iter, m_curr_source);
  
  m_ipf_mutex.UnLock();

  quadset.normalize(0,200);
  quadset.applyColorMap(m_color_map);
  return(quadset);
}

