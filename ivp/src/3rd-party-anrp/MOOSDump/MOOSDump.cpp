#include "MOOSDump.h"

bool MOOSDump::OnNewMail(MOOSMSG_LIST &NewMail)
{
  CMOOSMsg Msg;
  MOOSTrace("replying with double data");
  return(true); 
}


bool MOOSDump::OnConnectToServer()
{
  //register for variables here....
  
  //a good spot to figure out what to subscribe to
  //perhaps by looking at mission file..
  
  //for example here we want to be told every time
  //the data named by m_sWatchFor is written to
  //if we wanted to be told at most 10 HZ we would use 
  //0.1 instead of 0...
  
  return(true);
}

bool MOOSDump::Iterate()
{    
  MOOSMSG_LIST InMail;   
  if(m_Comms.ServerRequest("ALL",InMail)) {
    MOOSMSG_LIST::iterator p;
    printf("%i vars\n", InMail.size());
    
    for(p = InMail.begin(); p != InMail.end(); p++) {
      CMOOSMsg &m = *p;
      
      printf("%-20s ", m.m_sKey.c_str());
      if(m.m_cDataType == MOOS_STRING) { 
	printf("\"%s\"", m.m_sVal.c_str());
      } else if(m.m_cDataType == MOOS_NOT_SET) {
	printf("<undef>");
      } else if(m.m_cDataType == MOOS_DOUBLE) {
	printf("%f", m.m_dfVal);
      } else 
	printf("!?");
      printf("\n");		
    }
    //exit(0)
    return(true);
  }
}

bool MOOSDump::OnStartUp()
{
  //do start up things here...
  //for example read from mission file...
  return(true);
}
