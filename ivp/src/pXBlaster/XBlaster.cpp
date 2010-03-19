/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: XBlaster.cpp                                    */
/*    DATE: Feb 20th, 2010                                  */
/************************************************************/

#include <iterator>
#include "XBlaster.h"

using namespace std;

//---------------------------------------------------------
// Constructor

XBlaster::XBlaster()
{
  m_iterations = 0;
  m_tally_sent = 0;

  m_var_sizes = 6;
  m_string_sizes = 100;
  m_posts_per_iter = 1;
}

//---------------------------------------------------------
// Procedure: Iterate()

bool XBlaster::Iterate()
{
  m_iterations++;

  unsigned int i;
  for(i=0; i<m_posts_per_iter; i++) {
    string random_str = randomString(m_string_sizes, true);
    string random_var = randomString(m_var_sizes, false);
    m_Comms.Notify(random_var, random_str);
    m_tally_sent++;
  }  
  m_Comms.Notify("BLAST_TALLY", m_tally_sent);
  m_Comms.Notify("BLAST_ITER", m_iterations);

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//      Note: happens before connection is open

bool XBlaster::OnStartUp()
{
  STRING_LIST sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  m_MissionReader.GetConfiguration(GetAppName(), sParams);
    
  STRING_LIST::iterator p;
  for(p = sParams.begin();p!=sParams.end();p++) {
    string line  = *p;
    string left  = MOOSChomp(line, "=");
    string right = line;
    double dval  = atof(right.c_str());

    if((left == "var_sizes") && (dval > 0)) 
      m_var_sizes = (unsigned int)(dval);
    if((left == "string_sizes") && (dval > 0)) 
      m_string_sizes = (unsigned int)(dval);
    if((left == "posts_per_iter") && (dval > 0)) 
      m_posts_per_iter = (unsigned int)(dval);
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: randomString()

string XBlaster::randomString(unsigned int len, bool lower)
{
  string rstr(len, ' ');

  int base = 97; // 'a'
  if(!lower)
    base = 65;   // 'A'

  unsigned int i;
  for(i=0; i<len; i++)
    rstr.at(i) = (base + (rand() % 26));
  
  return(rstr);
}

