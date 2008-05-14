/***********************************************************/
/*    NAME: Henrik Schmidt                                  */
/*    FILE: BHV_MinimizeNoise.cpp                  */
/*    DATE: July 03 2005                                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include "BHV_MinimizeNoise.h"
#include "AOF_MinimizeNoise.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_MinimizeNoise::BHV_MinimizeNoise(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)BHV_MinimizeNoise");
  this->setParam("build_info", "uniform_piece=course:3");
  this->setParam("build_info", "uniform_grid=course:9");

  m_domain = subDomain(m_domain,"course");

  m_contact_name = "";

  addInfoVars("NAV_X,NAV_Y,NAV_SPEED,NAV_HEADING");
  addInfoVars("NOISE_ROSETTE");

}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_MinimizeNoise::setParam(string param, string val) 
{

  IvPBehavior::setParamCommon(param, val);

  if((param == "them") || (param == "contact")) 
    {
      m_contact_name = toupper(val);
      updateContactList(); 
      return(true);
    }  

  return true;
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_MinimizeNoise::onRunState() 
{
  if(m_contact_name == "") 
    {
      postEMessage("contact ID not set.");
      return(0);
    }
  else
    {
      updateContactList();
    }

  // Set m_osx, m_osy, m_osh, m_osv, m_cnx, m_cny, m_cnh, m_cnv
  if(!updateInfoIn())
    return(0);

  double relevance = getRelevance();

  if(relevance <= 0 || m_noise.size() <= 0)
    return(0);
  
  AOF_MinimizeNoise aof_minnoi(m_domain);
  aof_minnoi.setParam("osY", m_osy);
  aof_minnoi.setParam("osX", m_osx);
  aof_minnoi.setParam("target_x",m_cnx );
  aof_minnoi.setParam("target_y",m_cny );

  aof_minnoi.resetNoise();
  for (int i=0; i<m_noise.size();i++)
    aof_minnoi.setNoiseBin(m_bearing[i],m_noise[i]);

  OF_Reflector reflector(&aof_minnoi,1);
    
  reflector.create(m_build_info);
   
  IvPFunction *of = reflector.extractOF();
  
  //  of->getPDMap()->normalize(0.0, 100.0);
  of->setPWT(relevance*m_priority_wt);
    
return(of);
}
  
//-----------------------------------------------------------------
// Procedure: updateContactList
//   Purpose: Maintain a list of known unique contact names, and subscrinbe to NAV info

void BHV_MinimizeNoise::updateContactList()
{
  int i;
  
  m_contact_name = toupper(m_contact_name);

  for(i=0; i<m_contact_list.size(); i++) {
    if(m_contact_list[i] == m_contact_name) {
      return;
    }
  }
  
  m_contact_list.push_back(m_contact_name);
  addInfoVars(m_contact_name+"_NAV_X");
  addInfoVars(m_contact_name+"_NAV_Y");
  addInfoVars(m_contact_name+"_NAV_SPEED");
  addInfoVars(m_contact_name+"_NAV_HEADING");
  addInfoVars(m_contact_name+"_NAV_UTC");

}

//-----------------------------------------------------------
// Procedure: updateInfoIn()
//   Purpose: Update info need by the behavior from the info_buffer.
//            Error or warning messages can be posted.
//   Returns: true if no vital info is missing from the info_buffer.
//            false otherwise.
//      Note: By posting an EMessage, this sets the state_ok member
//            variable to false which will communicate the gravity
//            of the situation to the helm.

bool BHV_MinimizeNoise::updateInfoIn()
{
  bool ok1, ok2, ok3, ok4, ok5, ok6, ok7, ok8, ok9, ok10;
 
  m_osx = getBufferDoubleVal("NAV_X", ok1);
  m_osy = getBufferDoubleVal("NAV_Y", ok2);
  m_osh = getBufferDoubleVal("NAV_HEADING", ok3);
  m_osv = getBufferDoubleVal("NAV_SPEED", ok4);

  m_cnx = getBufferDoubleVal(m_contact_name+"_NAV_X", ok5);
  m_cny = getBufferDoubleVal(m_contact_name+"_NAV_Y", ok6);
  m_cnh = getBufferDoubleVal(m_contact_name+"_NAV_HEADING", ok7);
  m_cnv = getBufferDoubleVal(m_contact_name+"_NAV_SPEED", ok8);
  m_cnutc = getBufferDoubleVal(m_contact_name+"_NAV_UTC", ok9);

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || 
     !ok6 || !ok7 || !ok8 || !ok9)
    return(false);

  // Get noise rosette
  string rosette = getBufferStringVal("NOISE_ROSETTE",ok10);
  num_bins = parseNoise(rosette);  

  double curr_time = getBufferCurrTime();
  // double mark_time = getBufferTimeVal(m_contact_name+"_NAV_X");

  if(!m_extrapolate)
    return(true);

  // if(mark_time == 0)
    m_extrapolator.setPosition(m_cnx, m_cny, m_cnv, m_cnh, m_cnutc);

  // Even if mark_time is zero and thus "fresh", still derive the 
  // the contact position from the extrapolator since the UTC time
  // of the position in this report may still be substantially 
  // behind the current own-platform UTC time.

  double new_cnx, new_cny;
  bool ok = m_extrapolator.getPosition(new_cnx, new_cny, curr_time);

  if(ok) {
    m_cnx = new_cnx;
    m_cny = new_cny;
    return(true);
  }
  else {
    postWMessage("Incomplete Linear Extrapolation");
    return(false);
  }
}

double BHV_MinimizeNoise::getRelevance()
{
  bool silent = true;

  //always fully relevant
  return(1.0);
}

int BHV_MinimizeNoise::parseNoise(string status)
{
  vector<string> svector;

  m_noise.clear();
  m_bearing.clear();

  // Parse the noise rosette  
  svector = parseString(status, ',');
  double delta = 360.0/svector.size();

  for(unsigned int i=0; i<svector.size(); i++) 
    {
      string svalue  = stripBlankEnds(svector[i]);
      double noise = atof(svalue.c_str());
      double bearing = i*delta; 
      m_bearing.push_back(bearing);
      m_noise.push_back(noise);
    }
  
  return( m_noise.size() );
}



