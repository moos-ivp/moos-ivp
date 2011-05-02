/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: IC_Viewer.cpp                                       */
/*    DATE: Apr 19th 2011                                       */
/****************************************************************/

#include <iostream>
#include "IC_Viewer.h"
#include "ColorParse.h"
#include "BuildUtils.h"

using namespace std;

//--------------------------------------------------------------
// Constructor

IC_Viewer::IC_Viewer(int x, int y, int wid, int hgt, const char *l)
  : Common_IPFViewer(x, y, wid, hgt, l)
{
  m_clear_color.setColor("macbeige");
  m_collective  = false;
  m_curr_iter   = 0;
  m_check_brute = true;
}

//-------------------------------------------------------------
// Procedure: checkSolutions

void IC_Viewer::checkSolutions()
{
  cout << termColor("blue");
  cout << "Solution SUMMARY---------------------" << endl;
  
  m_curr_iter = m_bundle_series.getMinIteration();
  vector<string> ipf_strings = m_bundle_series.getIPFStrings(m_curr_iter);
  
  cout << "For Iteration: " << m_curr_iter << endl;
  cout << "Total # of IPF strings: " << ipf_strings.size() << endl;
  
  if(m_check_brute)
    m_ivp_checker.checkBrute(ipf_strings);

  cout << termColor("green");

  m_ivp_checker.checkIvP(ipf_strings);
  cout << termColor("magenta");
  
  QuadSet collective_quadset;
  collective_quadset = m_bundle_series.getCollectiveQuadSet(m_curr_iter);
  m_ivp_checker.checkCollectiveQuadSet(collective_quadset);
  cout << termColor() << endl;
}

//-------------------------------------------------------------
// Procedure: setIvPChecker

void IC_Viewer::setIvPChecker(IvPChecker ivp_checker)
{
  m_ivp_checker = ivp_checker;
}

//-------------------------------------------------------------
// Procedure: setIPF_BundleSeries

void IC_Viewer::setIPF_BundleSeries(IPF_BundleSeries bundle_series)
{
  m_bundle_series = bundle_series;
  vector<string> all_sources = m_bundle_series.getAllSources();
  if(all_sources.size() > 0)
    m_curr_source = all_sources[0];
  m_curr_iter = m_bundle_series.getMinIteration();
}

//-------------------------------------------------------------
// Procedure: resetQuadSet

void IC_Viewer::resetQuadSet()
{
  if(m_collective) {
    m_quadset = m_bundle_series.getCollectiveQuadSet(m_curr_iter);
    if(m_quadset.isEmpty())
      return;
    m_quadset.normalize(0,200);
    m_quadset.applyColorMap(m_color_map);
  }
  else {
    m_quadset = m_bundle_series.getQuadSet(m_curr_iter, m_curr_source);
    if(m_quadset.isEmpty())
      return;
    m_quadset.normalize(0,200);
    m_quadset.applyColorMap(m_color_map);
  }
}


//-------------------------------------------------------------
// Procedure: draw

void IC_Viewer::draw()
{
  Common_IPFViewer::draw();
  glPushMatrix();
  glRotatef(m_xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(m_zRot, 0.0f, 0.0f, 1.0f);
  
  bool result = Common_IPFViewer::drawIvPFunction();
  if(result) {
    if(m_draw_frame)
      drawFrame();
    drawOwnPoint();

    unsigned int max_crs_qix = m_quadset.getMaxPointQIX("course");
    unsigned int max_spd_qix = m_quadset.getMaxPointQIX("speed");
    drawMaxPoint(max_crs_qix, max_spd_qix);

  }
  glPopMatrix();
  
  glFlush();  
}

//-------------------------------------------------------------
// Procedure: incCurrFunction

void IC_Viewer::incCurrFunction()
{
  vector<string> sources = m_bundle_series.getAllSources();
  unsigned int i, vsize = sources.size();
  if(vsize == 0)
    return;
  
  unsigned int new_index = 0;
  for(i=0; i<vsize; i++) {
    if(sources[i] == m_curr_source) {
      if(i == (vsize-1))
	new_index = 0;
      else
	new_index = i+1;
    }
  }
  m_curr_source = sources[new_index];

  resetQuadSet();
  checkSolutions();
}


//-------------------------------------------------------------
// Procedure: decCurrFunction

void IC_Viewer::decCurrFunction()
{
  vector<string> sources = m_bundle_series.getAllSources();
  unsigned int i, vsize = sources.size();
  if(vsize == 0)
    return;
  
  unsigned int new_index = 0;
  for(i=0; i<vsize; i++) {
    if(sources[i] == m_curr_source) {
      if(i == 0)
	new_index = vsize-1;
      else
	new_index = i-1;
    }
  }
  m_curr_source = sources[new_index];

  resetQuadSet();
  checkSolutions();
}

//-------------------------------------------------------------
// Procedure: getCurrDescriptor()

string IC_Viewer::getCurrDescriptor()
{
  if(m_collective)
    return("Collective");
  else 
    return(m_curr_source);
}

//-------------------------------------------------------------
// Procedure: getCurrPriority()

string IC_Viewer::getCurrPriority()
{
  if(m_collective)
    return("0");
  else {
    double pwt = m_bundle_series.getPriority(m_curr_iter, m_curr_source);
    return(dstringCompact(doubleToString(pwt)));
  }
}

//-------------------------------------------------------------
// Procedure: getCurrDomain()

string IC_Viewer::getCurrDomain()
{
  IvPDomain ivp_domain;
  if(m_collective)
    ivp_domain = m_bundle_series.getDomain(m_curr_iter);
  else
    ivp_domain = m_bundle_series.getDomain(m_curr_iter, m_curr_source);

  return(domainToString(ivp_domain));
}

//-------------------------------------------------------------
// Procedure: getCurrPieces()

string IC_Viewer::getCurrPieces()
{
  if(m_collective)
    return("0");
  else {
    unsigned int pcs;
    pcs = m_bundle_series.getPieces(m_curr_iter, m_curr_source);
    return(dstringCompact(uintToString(pcs)));
  }
}

//-------------------------------------------------------------
// Procedure: getTotalFunctions()

string IC_Viewer::getTotalFunctions() 
{
  unsigned int bsize = m_bundle_series.getTotalFunctions(m_curr_iter);
  return(uintToString(bsize));
}


//-------------------------------------------------------------
// Procedure: setCollective

void IC_Viewer::setCollective(const string& val)
{
  bool prior_collective = m_collective;
  if(val == "true")
    m_collective = true;
  else if(val == "false")
    m_collective = false;
  else if(val == "toggle")
    m_collective = !m_collective;

  if(m_collective != prior_collective) {
    resetQuadSet();  
    checkSolutions();
  }

}

