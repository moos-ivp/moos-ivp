/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: TMOOSClient.cpp                                      */
/*   DATE: Dec 2nd 2011                                         */
/****************************************************************/

#include <iterator>
#include "TMOOSClient.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

TMOOSClient::TMOOSClient()
{
  m_counter = 0;
  m_postvar = "UMC";
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool TMOOSClient::OnNewMail(MOOSMSG_LIST &NewMail)
{
  MOOSMSG_LIST::iterator p;
	
#if 0	
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.GetKey();
    double dval  = msg.GetDouble();
    string sval  = msg.GetString(); 
    double mtime = msg.GetTime();
    bool   mdbl  = msg.IsDouble();
    bool   mstr  = msg.IsString();
    string msrc  = msg.GetSource();
//    if(key == "UMC_CHANNEL")
//      handleNewFOOBAR(sval);
  }
#endif

  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool TMOOSClient::OnConnectToServer()
{
  RegisterVariables();  

  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void TMOOSClient::RegisterVariables()
{
  m_Comms.Register("UMC_CHANNEL", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool TMOOSClient::Iterate()
{
  unsigned int i;
  for(i=0; i<m_posts_per_iter; i++) {
    m_counter++;
    m_Comms.Notify(m_postvar, m_counter);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool TMOOSClient::OnStartUp()
{
  // happens before connection is open
	
  RegisterVariables();
  m_Comms.Notify("UMC_CHANNEL", m_postvar);
  return(true);
}

