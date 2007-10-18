#include "SamplingControl.h"

#include <iostream>
#include <sstream>

using namespace std;

/**************************************************************************
*   SamplingControl::SamplingControl()                                    *
*                                                                         *
*   Constructor for the SamplingControl Object                            *
***************************************************************************/
SamplingControl::SamplingControl()
{
  on_station = false;
  Mission_Mission = 0;
  winch_state = 0;
  ctd_deploy = false;
}

/**************************************************************************
*   SamplingControl::~SamplingControl()                                   *
*                                                                         *
*   Destructor for the SamplingControl Object                             *
***************************************************************************/

SamplingControl::~SamplingControl()
{
}

bool SamplingControl::OnStartUp()
{
  string sVal;

  //latitude origin of local coordinate system
  if (m_MissionReader.GetValue("LatOrigin", sVal)) 
    {
	dfLatOrigin = atof(sVal.c_str());
    } 
  else 
    {
	MOOSTrace("LatOrigin not set - FAIL\n");
	return false;
    }
  //longitude origin of local coordinate system	
  if (m_MissionReader.GetValue("LongOrigin", sVal)) 
    {
	dfLongOrigin = atof(sVal.c_str());
    } 
  else 
    {
	MOOSTrace("LongOrigin not set - FAIL\n");
	return false;
    }
  //initialize the coordinate system
  if (!m_Geodesy.Initialise(dfLatOrigin, dfLongOrigin)) 
    {
	MOOSTrace("Geodesy Init failed - FAIL\n");
	return false;
    }
  
  double Loiter_X, Loiter_Y;
  //default loiter point x
  if (m_MissionReader.GetValue("Loiter_X", sVal)) 
    {
	Loiter_X = atof(sVal.c_str());
    } 
  else 
    {
	MOOSTrace("Loiter_X not set - FAIL\n");
	return false;
    }
  //default loiter point y
  if (m_MissionReader.GetValue("Loiter_Y", sVal)) 
    {
	Loiter_Y = atof(sVal.c_str());
    } 
  else 
    {
	MOOSTrace("Loiter_Y not set - FAIL\n");
	return false;
    }
  //set loiter point in MOOS DB
  char buff[200];
  sprintf(buff,"points=%f,%f",Loiter_X,Loiter_Y);
  m_Comms.Notify("SENSOR_LOITER",buff);
  loiter_str = string(buff);

  return(true);
}

bool SamplingControl::OnConnectToServer()
{  
  m_Comms.Register("ON_STATION",0);
  m_Comms.Register("STATE",0);
  m_Comms.Register("SENSOR_LAT",0);
  m_Comms.Register("SENSOR_LONG",0);
  m_Comms.Register("SENSOR_DURATION",0);
  m_Comms.Register("SENSOR_DISTANCE",0);
  m_Comms.Register("SENSOR_DEPTH",0);
  m_Comms.Register("SENSOR_MISSION",0);
  m_Comms.Register("SENSOR_INTERVAL",0);
  m_Comms.Register("SENSOR_HEADING",0);
  m_Comms.Register("WINCH_STATE",0);
  m_Comms.Register("SENSOR_AMPLITUDE",0);
  m_Comms.Register("SENSOR_PERIOD",0);
  m_Comms.Register("CTD_DEPLOY",0);
  m_Comms.Register("CTD_DEPTH",0);
  return(true);
}


bool SamplingControl::Iterate()
{
  //drift with CTD down
  if(Mission_Mission == 1)
    {
      if(!mission_initialized)
	{
	  double Mission_X,Mission_Y;
	  if (m_Geodesy.LatLong2LocalGrid(Mission_Lat,Mission_Long,Mission_Y,Mission_X)) 
	    {
	       char buff[200];
	       sprintf(buff,"points=%f,%f",Mission_X,Mission_Y);
	       m_Comms.Notify("SENSOR_DEPLOY",buff);
               sprintf(buff,"duration=%f",Mission_Duration);
	       m_Comms.Notify("DRIFT_INTERVAL",buff);
	       m_Comms.Notify("STATE","MISSION_ONE");
	       m_Comms.Notify("ON_STATION", "FALSE");
               mission_initialized = 1;
	    }
	}
    } 	    
 //station keep with CTD raise/lower
  else if(Mission_Mission == 2)
    {
      if(!mission_initialized)
	{
	  double Mission_X,Mission_Y;
	  if (m_Geodesy.LatLong2LocalGrid(Mission_Lat,Mission_Long,Mission_Y,Mission_X)) 
	    {
	       char buff[200];
	       sprintf(buff,"points=%f,%f",Mission_X,Mission_Y);
	       m_Comms.Notify("SENSOR_DEPLOY",buff);
	       sprintf(buff,"duration=%f",Mission_Duration);
	       m_Comms.Notify("STATION_INTERVAL",buff);
	       m_Comms.Notify("ON_STATION", "FALSE");
	       m_Comms.Notify("STATE","MISSION_TWO");
               mission_initialized = 1;
               last_time = 0;	      
	    }
	}
      //on station and winch is in up position
      if((ctd_deploy)&&(winch_state==0))
	{
	  double current_time = MOOSTime();
	  //if time for next cast
	  if ((current_time - last_time) > Mission_Interval)
	    {
	      winch_pos(Mission_Depth);
	      m_Comms.Notify("CTD_LOG","TRUE");
	      winch_state = 1;
	      last_time = current_time;
	    }
	}
      //winch is being lowered
      if((winch_state==1)&&(ctd_deploy))
	{
	  //check to see if it is at depth
	  if (winch_down())
	    {
	      winch_pos(1.0);
	      m_Comms.Notify("CTD_LOG","FALSE");
              winch_state = 2;
	    }
	}
      //winch is being raised
      if((winch_state==2)&&(ctd_deploy))
	{
	  //check to see if it is at depth
	  if (winch_up())
	    {
              winch_state = 0;
	    }
	}
 
    }
  //zigzag sampling
  else if(Mission_Mission == 3)
    {
      if(!mission_initialized)
	{
	  double Mission_X,Mission_Y;
	  if (m_Geodesy.LatLong2LocalGrid(Mission_Lat,Mission_Long,Mission_Y,Mission_X)) 
	    {
	       char buff[300];
	       sprintf(buff,"points=%f,%f",Mission_X,Mission_Y);
	       m_Comms.Notify("SENSOR_DEPLOY",buff);
               sprintf(buff,"points=zigzag:%f,%f,%f,%f,%f,%f,",Mission_X,Mission_Y,Mission_Heading,Mission_Distance,period,amplitude);
	       m_Comms.Notify("STATION_ZIGZAG",buff);
	       sprintf(buff,"duration=%f",Mission_Interval);
               m_Comms.Notify("STATION_INTERVAL",buff);
	       m_Comms.Notify("ON_STATION", "FALSE");
	       m_Comms.Notify("STATE","MISSION_THREE");
               mission_initialized = 1;
               winch_state = 0;   
	    }
	}
      //on station and winch is in up position
      if((on_station)&&(winch_state==0)&&(ctd_deploy))
	{
	      winch_pos(Mission_Depth);
	      m_Comms.Notify("CTD_LOG","TRUE");
	      winch_state = 1;
	}
      //winch is being lowered
      if((winch_state==1)&&(on_station)&&(ctd_deploy))
	{
	  //check to see if it is at depth
	  if (winch_down())
	    {
	      winch_pos(1.0);
	      m_Comms.Notify("CTD_LOG","FALSE");
              winch_state = 2;
	    }
	}
      //winch is being raised
      if((winch_state==2)&&(on_station)&&(ctd_deploy))
	{
	  //check to see if it is at depth
	  if (winch_up())
	    {
              winch_state = 0;
	      m_Comms.Notify("ON_STATION","FALSE");
	    }
	}
      
    }
  //station keep with CTD down
  else if(Mission_Mission == 4)
    {
      if(!mission_initialized)
	{
	  double Mission_X,Mission_Y;
	  if (m_Geodesy.LatLong2LocalGrid(Mission_Lat,Mission_Long,Mission_Y,Mission_X)) 
	    {
	       char buff[200];
	       sprintf(buff,"points=%f,%f",Mission_X,Mission_Y);
	       m_Comms.Notify("SENSOR_DEPLOY",buff);
	       sprintf(buff,"duration=%f",Mission_Duration);
	       m_Comms.Notify("STATION_INTERVAL",buff);
	       m_Comms.Notify("STATE","MISSION_FOUR");
	       m_Comms.Notify("ON_STATION", "FALSE");
               mission_initialized = 1;
	    }
	}
    } 	       

  return(true);
}


//-------------------------------------------------------------
// Procedure: OnNewMail

bool SamplingControl::OnNewMail(MOOSMSG_LIST &NewMail)

{  
  MOOSMSG_LIST::iterator p;

  for(p = NewMail.begin(); p != NewMail.end(); p++) {
    CMOOSMsg &Msg = *p;

      if(Msg.m_sKey == "SENSOR_LAT")
	{	
	  Mission_Lat = Msg.m_dfVal;        
	}      
      else if(Msg.m_sKey == "SENSOR_LONG")
	{	
	  Mission_Long = Msg.m_dfVal;        
	}  
      else if(Msg.m_sKey == "SENSOR_DURATION")
	{	
	  Mission_Duration = Msg.m_dfVal;        
	} 
      else if(Msg.m_sKey == "SENSOR_AMPLITUDE")
	{	
	  amplitude = Msg.m_dfVal;        
	}
      else if(Msg.m_sKey == "SENSOR_PERIOD")
	{	
	  period = Msg.m_dfVal;       
	}      
      else if(Msg.m_sKey == "SENSOR_DEPTH")
	{	
	  Mission_Depth = Msg.m_dfVal;        
	} 
      else if(Msg.m_sKey == "SENSOR_DISTANCE")
	{	
	  Mission_Distance = Msg.m_dfVal;        
	} 
      else if(Msg.m_sKey == "SENSOR_MISSION")
	{	
	  Mission_Mission = (int) Msg.m_dfVal;         
	} 
      else if(Msg.m_sKey == "SENSOR_INTERVAL")
	{	
	  Mission_Interval = (int) Msg.m_dfVal;         
	} 
      else if(Msg.m_sKey == "SENSOR_HEADING")
	{	
	  Mission_Heading = Msg.m_dfVal;         
	} 
      else if(Msg.m_sKey == "CTD_DEPTH")
	{	
	  ctd_depth = Msg.m_dfVal;         
	  if (ctd_depth < 4)
	    m_Comms.Notify("CTD_UP","TRUE");
	  else
	    m_Comms.Notify("CTD_UP", "FALSE");
	} 
       else if(Msg.m_sKey == "ON_STATION")
	{	
	  string sStation = Msg.m_sVal;
          if (sStation == "TRUE")
	    on_station = true;
	  else
	    on_station = false;
	    
	} 
      else if(Msg.m_sKey == "STATE")
	{	
	  //if a mission is complete, make sure winch is up
	  string sState = Msg.m_sVal;
          if (sState == "MISSION_COMPLETE")
	    {
	      winch_pos(1.0);	          
	      Mission_Mission = 0;
              mission_initialized = false;
              m_Comms.Notify("CTD_LOG","FALSE");

	    }
	}   
      else if(Msg.m_sKey == "WINCH_STATE")
	{
	  string wState = Msg.m_sVal;
	  if(wState=="WINCH_UP")
	    winch_state=WINCH_UP;
	  else if(wState=="WINCH_DOWN")
	    winch_state=WINCH_DOWN;
	}
      else if(Msg.m_sKey == "CTD_DEPLOY")
	{
	  if(MOOSStrCmp(Msg.m_sVal, "true"))
	    {
	      if(Mission_Mission == 1 || Mission_Mission == 4)
		{
		  
		  m_Comms.Notify("SENSOR_LOITER", loiter_str);
 
		  winch_pos(Mission_Depth);
		  m_Comms.Notify("CTD_LOG","TRUE");	    
		}
	      ctd_deploy = true;
	    }
	  else
	    {
	      winch_pos(0.0);
	      ctd_deploy = false;
	    }
	}
      else
       MOOSTrace("Unrecognized command: [%s]\n",Msg.m_sKey.c_str());
  }
  
  return(true);
}

/**************************************************************************
*   SamplingControl::winch_pos(double depth)                              *
*                                                                         *
*   Request a depth for the CTD                                           *
***************************************************************************/
void SamplingControl::winch_pos(double depth)
{
  //hack because winch controller throws current limit
  system("printf 'S\r' > /dev/ttyUSB.A20e1uVp");

  m_Comms.Notify("WINCH_DESIRED_DEPTH",depth);
}

/**************************************************************************
*   SamplingControl::winch_down()                                         *
*                                                                         *
*   Check to see if the CTD is at the requested depth                     *
***************************************************************************/
bool SamplingControl::winch_down()
{
  if(fabs(ctd_depth-Mission_Depth) <= 3.0)
    return true;
  else 
    return false;
}

/**************************************************************************
*   SamplingControl::winch_up()                                           *
*                                                                         *
*   Check to see if the CTD is up                                         *
***************************************************************************/
bool SamplingControl::winch_up()
{
  if(ctd_depth <= 4.0)
    return true;
  else 
    return false;
}
