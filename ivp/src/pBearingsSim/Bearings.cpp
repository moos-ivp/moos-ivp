#include <MOOSLib.h>
#include <MOOSGenLib.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

#include "Bearings.h"

using namespace std;

CBearings::CBearings()
{
  target = false;
  last = 0.0;
  state = 0;
  mu = 0.0;
  tracking_on = true;
}

CBearings::~CBearings()
{
    
}


bool CBearings::Iterate()
{
  if(tracking_on)
    {
 if (target){
   current = MOOSTime();
   if (current-last >= 1.0) {
     if (state == 0){
       head_save = hp;
       if(head_save < 0.0)
	 head_save = head_save + 360;
       state = 1;
       char buff[250];
       sprintf(buff,"state=%d,bearing=%.16f,xp=%.16f,yp=%.16f,beamno=%d,time=%.16f",state,0.0,0.0,0.0,0,current); 
       string rStr = buff;
       m_Comms.Notify("BEARING_STAT",rStr);
     }
     else if (state == 1){
       double currh = hp;
       if (currh < 0.0)
	 currh = currh + 360.0;
       
       head_diff = currh-head_save;
       
       if (head_diff < 0.0){
	 if(head_diff > -180)
	   head_diff = fabs(head_diff);
	     else
       	 head_diff = 360.0+head_diff;
       }
       else if (head_diff >180.0)
        head_diff = 360.0-head_diff;
       cout << "current = " << currh << "head_save = " << head_save << "head_diff = " << head_diff << endl; 
       if ((fabs(head_diff) >= 70.0))
	 state = 2;
       cout << "state = 1, turn = " << head_diff << " degrees" << endl;
       m_Comms.Notify("TRACKING","AMBIGUOUS"); 
       m_Comms.Notify("TRACKING_SIGNAL",1);
     }
     else if (state == 2){
       double dev = gauss_dev(mu,sigma);
       bear = (90.0 - atan2((yt-yp),(xt-xp))*180.0/M_PI)+dev;
       //bear = (90.0 - atan((yt-yp)/(xt-xp))*180.0/M_PI)+dev;
       cout << "dev = " << dev <<endl;
       char buff[250];
       sprintf(buff,"state=%d,bearing=%.16f,xp=%.16f,yp=%.16f,beamno=%d,time=%.16f",state,bear,xp,yp,0,current);
       string rStr = buff;       
       m_Comms.Notify("BEARING_STAT",rStr);
       m_Comms.Notify("BEARING_STATE",2);
       cout << "state = 2, target bearing = " << bear << endl;
     }
     last = current;
   }
 }
    
 else
   {
   current = MOOSTime();
   char buff[250];
   sprintf(buff,"state=%d,bearing=%.16f,xp=%.16f,yp=%.16f,beamno=%d,time=%.16f",0,0.0,0.0,0.0,0,current); 
   string rStr = buff;
   cout << "state = 0" << endl;
   m_Comms.Notify("BEARING_STAT",rStr);
   m_Comms.Notify("TRACKING_SIGNAL",0);
   m_Comms.Notify("TRACKING","NO_TRACK");  
   }
    }   
    else
   {
   current = MOOSTime();
   char buff[250];
   sprintf(buff,"state=%d,bearing=%.16f,xp=%.16f,yp=%.16f,beamno=%d,time=%.16f",0,0.0,0.0,0.0,0,current); 
   string rStr = buff;
   cout << "state = 0" << endl;
   m_Comms.Notify("BEARING_STAT",rStr);
   m_Comms.Notify("TRACKING_SIGNAL",0);
   m_Comms.Notify("TRACKING","NO_TRACK");  
   }
    
    return true;
}

bool CBearings::OnConnectToServer()
{
  m_Comms.Register("TARGET_XPOS",0);
  m_Comms.Register("TARGET_YPOS",0);
  m_Comms.Register("NAV_X",0);
  m_Comms.Register("NAV_Y",0);
  m_Comms.Register("NAV_YAW",0);
  m_Comms.Register("TRACK_CONTROL",0);

return true;
}



bool CBearings::OnStartUp()
{

    CMOOSApp::OnStartUp();

    double gdsigma;
    if(m_MissionReader.GetValue("sigma",gdsigma))
    {
      sigma = (double) gdsigma;
    }
    else
    {
        MOOSTrace("pBearings:: sigma not set - FAIL\n");
        
        return false;
    }

   
}



bool CBearings::OnNewMail(MOOSMSG_LIST &NewMail)
{
   CMOOSMsg Msg;

  //NewMail.sort();
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &Msg = *p;
    
    if(Msg.m_sKey == "TARGET_XPOS")
      {
	xt = Msg.m_dfVal;
        target = true;
      }
    else if(Msg.m_sKey == "TARGET_YPOS")
      {	
	yt = Msg.m_dfVal;
        target = true;
      }
    else if(Msg.m_sKey == "NAV_Y")
      {	
	yp = Msg.m_dfVal;
      }
    else if(Msg.m_sKey == "NAV_X")
      {	
	xp = Msg.m_dfVal;
      }
    else if(Msg.m_sKey == "NAV_YAW")
      {	
	hp = -Msg.m_dfVal*180/M_PI;
      }
    else if(Msg.m_sKey == "TRACK_CONTROL")
      {	
        string mess = Msg.m_sVal;
	if (MOOSStrCmp(mess,"OFF")){
	  MOOSTrace("turned tracking off\n");
	   tracking_on = false;
	}
      }

    else
      MOOSTrace("Unrecognized command: [%s]\n",Msg.m_sKey.c_str());
  }
  

  //  return UpdateMOOSVariables(NewMail);
}

bool CBearings::PublishData()
{  
 
    return PublishFreshMOOSVariables();
}



double CBearings::gauss_dev(double mu, double sigma)
{
  static double V1,V2,S;
  static int phase = 0;
  double X;

  if (phase ==0){
    do{
      double U1 = (double)rand()/RAND_MAX;
      double U2 = (double)rand()/RAND_MAX;
      
      V1 = 2*U1-1;
      V2 = 2*U2-1;
      S = V1*V1 + V2*V2;
    } while(S >=1 || S == 0);

    X = mu + (V1*sqrt(-2*log(S)/S)) * sqrt(sigma);
  }
  else
    X = mu + (V2*sqrt(-2*log(S)/S)) * sqrt(sigma);

  phase = 1 - phase;

  return X;

}
    

  

