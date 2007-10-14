// $Header: /home/cvsroot/project-plus/src/pNaFCon/pNaFCon.cpp,v 1.3 2007-10-05 22:19:46 arjunab Exp $
// (c) 2006
// CpNaFCon.h: An interface/translator for communications
// between MOOS and the pHelm 
////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <cctype>
#include <stdio.h>
#include <list>
#include <sys/time.h>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "pNaFCon.h"
#include "MBUtils.h"

using namespace std;

CpNaFCon::CpNaFCon()
{
  MOOSTrace("====================================\n");
  MOOSTrace("\n");
  MOOSTrace("             pNaFCon\n");
  MOOSTrace("\n");
  MOOSTrace("        V 1.0 8/23/2006 \n");
  MOOSTrace("        Modified 8/08/2007 \n");
  MOOSTrace("\n");
  MOOSTrace("(c) Copyright, 2007 Arjuna Balasuriya MIT\n");
  MOOSTrace("====================================\n");

  dep_state = "FALSE";
  dep_mission = "FALSE";
  prose_state = "FALSE";
  close_range = "FALSE";
  track_tracking = "FALSE";
  MessageFlag = 0;
  abort_set = false;
  modem_count = 0;
  Nav_x = "0";
  Nav_y = "0";
  Nav_z = "0";
  Nav_yaw = "0";
  Nav_speed = "0";
  Nav_pitch = "0";
  Nav_roll = "0";
  TimeToSlowdown = "0";
  TimeToSpeedup = "0";
  VirtualTarget = 0;
  last_time = 0;
  target_x = 0;
  target_y = 0;
  SensorCmdType = "0";

  TrackNumber = 0;
  LastPolled = 0;
  SincePolled = 0;
  PollingMaxDuration = 0;
  CollaborationMode = "0";
}

CpNaFCon::~CpNaFCon()
{
       Contact_rpt_time = 0.0;
       Contact_vrt_rpt_time = 0.0;
       Sensor_rpt_time = 0.0;
       Track_rpt_time = 0.0;
}

bool CpNaFCon::OnNewMail(MOOSMSG_LIST &NewMail)
{
  std::stringstream ss;
  std::stringstream ss1;
  std::stringstream ss2;
  std::stringstream ss3;
  std::stringstream ss4;
  std::stringstream ss5;
  std::stringstream ss6;
  std::stringstream ss7;
  std::stringstream ss8;
  std::stringstream ss9;
  std::stringstream ss10;
  std::stringstream ss11;
  string sCCLMsg;
  CMOOSMsg Msg;
  NMEAMessage Sentence;
  int counter = 0; 
  double TimeNow = MOOSTime();
  string sTime = doubleToString(TimeNow,4);
  string bearing_stat;
  string track_stat;
  string trackstate;
  string bearingstate;
  string bearingid;
  string btrdata;

   if(m_Comms.PeekMail(NewMail,"NAFCON_MESSAGES",Msg))
   {
        // Get data coming in from the modem.
             MOOSTrace("Got new acoustic data\n");
             MOOSTrace("\n");
	     //Process the CCL Message
	     Translate(Msg);
   }
   if(m_Comms.PeekMail(NewMail,"NAV_LAT",Msg))
   {
	    ss1 <<  Msg.m_dfVal;
	    ss1 >> Nav_x;
	    counter++; 
   } 
   if(m_Comms.PeekMail(NewMail,"NAV_LONG",Msg))
   {
	    ss2 <<  Msg.m_dfVal;
	    ss2 >> Nav_y;
	    counter++; 
   } 

   if(m_Comms.PeekMail(NewMail,"NAV_DEPTH",Msg))
   {
	    ss3 <<  Msg.m_dfVal;
            ss3 >> Nav_z; 
	    counter++; 
   }
   if(m_Comms.PeekMail(NewMail,"NAV_HEADING",Msg))
   {
	    ss4 <<  Msg.m_dfVal;
	    ss4 >> Nav_yaw;
	    double double_nav_yaw = atof(Nav_yaw.c_str()); 
	    while((double_nav_yaw) < 0)
	      double_nav_yaw += 360;
	    
	    Nav_yaw = doubleToString(double_nav_yaw);
	    
	    counter++; 
   }
   if(m_Comms.PeekMail(NewMail,"NAV_SPEED",Msg))
   {
	    ss5 <<  Msg.m_dfVal;
	    ss5 >> Nav_speed; 
	    counter++; 
   }
   if(m_Comms.PeekMail(NewMail,"NAV_PITCH",Msg))
   {
	    ss6 <<  Msg.m_dfVal;
	    ss6 >> Nav_pitch; 
	    counter++; 
   }
   if(m_Comms.PeekMail(NewMail,"NAV_ROLL",Msg))
   {
	    ss7 <<  Msg.m_dfVal;
	    ss7 >> Nav_roll; 
	    counter++; 
   }

   if(m_Comms.PeekMail(NewMail,"COLLABORATION_MODE",Msg))
   {
	   if(Msg.m_sVal == "OFF")
		CollaborationMode = "0";

	   if(Msg.m_sVal == "SYNCH")
		CollaborationMode = "1";
	   
	   if(Msg.m_sVal == "COLLABORATING")
		CollaborationMode = "2";
	    counter++; 
   }

   if(m_Comms.PeekMail(NewMail,"TIME_TO_SLOWDOWN",Msg))
   {
            ss8 <<  Msg.m_dfVal;
            ss8 >>  TimeToSlowdown;
   }
   if(m_Comms.PeekMail(NewMail,"TIME_TO_SPEEDUP",Msg))
   {
            ss9 <<  Msg.m_dfVal;
            ss9 >>  TimeToSpeedup;
   }
   if(m_Comms.PeekMail(NewMail,"NAV_X",Msg))
   {
            ss10 <<  Msg.m_dfVal;
            ss10 >>  AUV_x;
   }
   if(m_Comms.PeekMail(NewMail,"NAV_Y",Msg))
   {
            ss11 <<  Msg.m_dfVal;
            ss11 >>  AUV_y;
   }

   if(m_Comms.PeekMail(NewMail,"LAST_POLLED_TIME",Msg))
   {
	LastPolled = Msg.m_dfVal;
	MOOSTrace("Last Polled Updated\n");
	MOOSTrace("Last Polled Time %f\n",LastPolled);
   }  

   if(m_Comms.PeekMail(NewMail,"POLLING_ADDRESS",Msg))
   {
	string polling_add = Msg.m_sVal;

	sCollVehicleId = Msg.m_sVal;

	MOOSTrace("Polling address %d\n",atoi(polling_add.c_str()));
   }  

   if(m_Comms.PeekMail(NewMail,"DEPLOY_STATE",Msg))
   {
	dep_state = Msg.m_sVal;
   }
   if(m_Comms.PeekMail(NewMail,"DEPLOY_MISSION",Msg))
   {
	dep_mission = Msg.m_sVal;
   }
   if(m_Comms.PeekMail(NewMail,"PROSECUTE_STATE",Msg))
   {
	prose_state = Msg.m_sVal;
   }
   if(m_Comms.PeekMail(NewMail,"CLOSE_RANGE",Msg))
   {
	close_range = Msg.m_sVal;
   }
   if(m_Comms.PeekMail(NewMail,"TRACKING",Msg))
   {
	track_tracking = Msg.m_sVal;
   }

   //Send a Contact Report
   if(m_Comms.PeekMail(NewMail,"BEARING_STAT",Msg))
   {
	    bearing_stat =  Msg.m_sVal;
	    MOOSChomp(bearing_stat,"node=");
	    bearingid = MOOSChomp(bearing_stat,",");
	    MOOSChomp(bearing_stat,"state=");
	    bearingstate = MOOSChomp(bearing_stat,",");
	    MOOSChomp(bearing_stat,"bearing=");
	    Bstat_bearing = MOOSChomp(bearing_stat,",");

	    MOOSChomp(bearing_stat,"xp=");
	    Bstat_x = MOOSChomp(bearing_stat,",");

	    MOOSChomp(bearing_stat,"yp=");
	    Bstat_y = MOOSChomp(bearing_stat,",");

	    MOOSChomp(bearing_stat,"sigma=");
	    string Bstat_sigma = MOOSChomp(bearing_stat,",");

	    double sigma_dbl = atof(Bstat_sigma.c_str());
	    if(sigma_dbl > 127) sigma_dbl = 127.0;
	    if(sigma_dbl < 0) sigma_dbl = 0.0;
 
	    MOOSChomp(bearing_stat,"time=");
	    Bstat_time = MOOSChomp(bearing_stat);

	//Contact bearing = relative bearing
	double abs_cont_bearing = atof(Bstat_bearing.c_str());
	double abs_sensor_heading = atof(Nav_yaw.c_str());
	double relative_bearing = 0.0;

	if(abs_sensor_heading > abs_cont_bearing)
	  relative_bearing = 360 - abs_sensor_heading + abs_cont_bearing;

	if(abs_sensor_heading == abs_cont_bearing)
	  relative_bearing = 0;

	if(abs_sensor_heading < abs_cont_bearing)
	  relative_bearing = abs_cont_bearing - abs_sensor_heading;


	if(bearingstate == "0")
	{

	}
	else
	if((bearingstate == "2") && (atoi(bearingid.c_str()) == atoi(sVehicleId.c_str())))
	{


 	double sensorLat;
	double sensorLon;	

	if(m_Geodesy.LocalGrid2LatLong(atof(Bstat_x.c_str()),atof(Bstat_y.c_str()),sensorLat,sensorLon))
	{
		MOOSTrace("Sensor Lat = %.3f, Sensor Lon = %.3f\n",sensorLat,sensorLon);
	}

	if(MOOSTime() > Contact_rpt_time + rpt_delay)
	{	

		MOOSTrace("Relative Bearing = %f\n",relative_bearing);

     		string ContactMessage = "MessageType=SENSOR_CONTACT,SensorReportType=1,SourcePlatformId="+sVehicleId+",DestinationPlatformId="+sCollVehicleId+","
                "ContactTimestamp="+Bstat_time+",SensorHeading="+Nav_yaw+",SensorPitch="+Nav_pitch+",SensorRoll="+Nav_roll+",SensorLatitude="+doubleToString(sensorLat,4)+","
                "SensorLongitude="+doubleToString(sensorLon,4)+",SensorDepth="+Nav_z+",SensorCEP=0,ContactBearing="+doubleToString(relative_bearing,1)+",ContactBearingUncertainty="+doubleToString(sigma_dbl,1)+","
                "ContactBearingRate=0,ContactBearingRateUncertainty=0,ContactElevation=0,ContactElevationUncertainty=0,"
                "ContactSpectralLevel1=0,ContactFrequency1=0,ContactBandwidth1=0,ContactSpectralLevel2=0,"
                "ContactFrequency2=0,ContactBandwidth2=0";


  //      string ContactMessage = "MessageType=SENSOR_CONTACT,SensorReportType=1,SourcePlatformId="+sVehicleId+",DestinationPlatformId="+sCollVehicleId+","
  //              "ContactTimestamp="+Bstat_time+",SensorHeading="+Nav_yaw+",SensorPitch="+Nav_pitch+",SensorRoll="+Nav_roll+",SensorLatitude="+Nav_x+","
  //              "SensorLongitude="+Nav_y+",SensorDepth="+Nav_z+",SensorCEP=0,ContactBearing="+doubleToString(trkheading,4)+",ContactBearingUncertainty=0,"
  //              "ContactBearingRate=0,ContactBearingRateUncertainty=0,ContactElevation=0,ContactElevationUncertainty=0,"
  //              "ContactSpectralLevel1=0,ContactFrequency1=0,ContactBandwidth1=0,ContactSpectralLevel2=0,"
  //              "ContactFrequency2=0,ContactBandwidth2=0";


	    if(((dep_state == "DEPLOY")&&(dep_mission == "0")) || ((prose_state == "PROSECUTE")&&(close_range == "TRUE")&&(track_tracking == "TRACKING")))
	    { 
	      SetMOOSVar("Plusnetmessages", ContactMessage, TimeNow);
              Contact_rpt_time = MOOSTime();
	    }
	}
	}	
   }

   if(m_Comms.PeekMail(NewMail,"BTR_DATA",Msg))
   {
	btrdata = Msg.m_sVal;

	//Data Extraction

	int beam_num;
	double beam_val[50];

	vector<string> beam_data = parseString(btrdata,',');
	int numberofbeams = beam_data.size();

 	for(beam_num = 0; beam_num < numberofbeams; beam_num ++)
	{
		beam_val[beam_num] = atof(beam_data[beam_num].c_str()); 
	}	

	
        string NoiseMessage = "MessageType=SENSOR_NOISE,SensorReportType=3,SourcePlatformId="+sVehicleId+",DestinationPlatformId="+sCollVehicleId+","
                "Timestamp="+doubleToString(MOOSTime(),4)+",SensorLatitude="+Nav_x+",SensorLongitude="+Nav_y+",SensorDepth="+Nav_z+",BearingSector=2,"
                "VerticalAngle=2,FrequencyBand=1,AmbientSpectralLevel0=0,AmbientSpectralLevel1=1,"
                "AmbientSpectralLevel2=2,AmbientSpectralLevel3=3,AmbientSpectralLevel4=4,AmbientSpectralLevel5=5,"
                "AmbientSpectralLevel6=6,AmbientSpectralLevel7=7,AmbientSpectralLevel8=8,AmbientSpectralLevel9=9,"
                "AmbientSpectralLevel10=10,AmbientSpectralLevel11=11,AmbientSpectralLevel12=12,"
                "AmbientSpectralLevel13=13,AmbientSpectralLevel14=14,AmbientSpectralLevel15=15,"
                "AmbientSpectralLevel16=16,AmbientSpectralLevel17=17,AmbientSpectralLevel18=18,"
                "AmbientSpectralLevel19=19,AmbientSpectralLevel20=20,AmbientSpectralLevel21=21";

	if(MOOSTime() > Noise_rpt_time + 120)
	{
//	     SetMOOSVar("Plusnetmessages", NoiseMessage, TimeNow);
	     Noise_rpt_time = MOOSTime();	
	}

   }	

  //Send a Track report
   if(m_Comms.PeekMail(NewMail,"TRACK_STAT",Msg))
   {
	    track_stat =  Msg.m_sVal;

	    MOOSChomp(track_stat,"node=");
	    string track_srcid = MOOSChomp(track_stat,",");
 
	    MOOSChomp(track_stat,"state=");
	    trackstate = MOOSChomp(track_stat,",");
	MOOSTrace("Node = %s & state = %s\n",track_srcid.c_str(),trackstate.c_str()); 

	if((trackstate == "2") && (atoi(track_srcid.c_str()) == atoi(sVehicleId.c_str())))
//	if(trackstate == "2")
	{ 
	    MOOSChomp(track_stat,"x=");
	    Tstat_x = MOOSChomp(track_stat,",");
	    MOOSChomp(track_stat,"y=");
	    Tstat_y = MOOSChomp(track_stat,",");
	    MOOSChomp(track_stat,"heading=");
	    Tstat_course = MOOSChomp(track_stat,",");
	    MOOSChomp(track_stat,"speed=");
	    Tstat_speed = MOOSChomp(track_stat,",");
	    MOOSChomp(track_stat,"time=");
	    string Tstat_time = track_stat;

	if(MOOSTime() > Track_rpt_time + rpt_delay)
	{
//        string TrackMessage = "MessageType=SENSOR_TRACK,SensorReportType=2,SourcePlatformId="+sVehicleId+",DestinationPlatformId="+sCollVehicleId+","
 //               "TrackTimestamp="+Tstat_time+",PlatformID="+sVehicleId+",TrackNumber=0,TrackLatitude="+Tstat_x+",TrackLongitude="+Tstat_y+",TrackCEP=0,"
  //              "TrackDepth=0,TrackDepthUncertainty=0,TrackHeading="+Tstat_course+",TrackHeadingUncertainty=0,TrackSpeed="+Tstat_speed+","
   //             "TrackSpeedUncertainty=0,DepthClassification=0,TrackClassification=0,TrackSpectralLevel1=0,"
    //            "TrackFrequency1=0,TrackBandwidth1=0,TrackSpectralLevel2=0,TrackFrequency2=0,TrackBandwidth2=0";

 	double TrackLat;
	double TrackLon;	

	if(m_Geodesy.LocalGrid2LatLong(atof(Tstat_x.c_str()),atof(Tstat_y.c_str()),TrackLat,TrackLon))
	{
		MOOSTrace("Track Lat = %.3f, Track Lon = %.3f\n",TrackLat,TrackLon);
	}
        string TrackMessage = "MessageType=SENSOR_TRACK,SensorReportType=2,SourcePlatformId="+sVehicleId+",DestinationPlatformId="+sCollVehicleId+","
                "TrackTimestamp="+Tstat_time+",PlatformID="+sVehicleId+",TrackNumber="+stracknumber+",TrackLatitude="+doubleToString(TrackLat,4)+",TrackLongitude="+doubleToString(TrackLon,4)+",TrackCEP=0,"
                "TrackDepth=0,TrackDepthUncertainty=0,TrackHeading="+Tstat_course+",TrackHeadingUncertainty=0,TrackSpeed="+Tstat_speed+","
                "TrackSpeedUncertainty=0,DepthClassification=0,TrackClassification=0,TrackSpectralLevel1=0,"
                "TrackFrequency1=0,TrackBandwidth1=0,TrackSpectralLevel2=0,TrackFrequency2=0,TrackBandwidth2=0";
	SetMOOSVar("Plusnetmessages", TrackMessage, TimeNow);
	Track_rpt_time = MOOSTime();
	}
	
   }
} 
	return true;
}

bool CpNaFCon::OnConnectToServer()
{
	m_Comms.Register("NAFCON_MESSAGES",0);
	m_Comms.Register("NAV_LAT",0);
	m_Comms.Register("NAV_LONG",0);
	m_Comms.Register("NAV_DEPTH",0);
	m_Comms.Register("NAV_HEADING",0);
	m_Comms.Register("NAV_SPEED",0);
	m_Comms.Register("NAV_PITCH",0);
	m_Comms.Register("NAV_ROLL",0);
	m_Comms.Register("BEARING_STAT",0);
	m_Comms.Register("TRACK_STAT",0);
	m_Comms.Register("NAV_X",0);
	m_Comms.Register("NAV_Y",0);
	m_Comms.Register("TIME_TO_SLOWDOWN",0);
	m_Comms.Register("TIME_TO_SPEEDUP",0);
	m_Comms.Register("BTR_DATA",0);
	m_Comms.Register("LAST_POLLED_TIME",0);
	m_Comms.Register("POLLING_ADDRESS",0);
	m_Comms.Register("COLLABORATION_MODE",0);
	m_Comms.Register("DEPLOY_STATE",0);
	m_Comms.Register("DEPLOY_MISSION",0);
	m_Comms.Register("PROSECUTE_STATE",0);
	m_Comms.Register("CLOSE_RANGE",0);
	m_Comms.Register("TRACKING",0);
	
	string sVal;
	string sVehicle;
	double dfLatOrigin;
	double dfLongOrigin;
  	double TimeNow = MOOSTime();
	Old_bstat_time = 0.0;

	MissionState = "2"; 
	Mis_Type = "2";
	first_deploy = 0;

	MOOSTrace("\n");
	if(m_MissionReader.GetValue("LatOrigin",sVal))
	{
		dfLatOrigin = atof(sVal.c_str());
		MOOSTrace("  LatOrigin  = %10.5f deg.\n",dfLatOrigin);
    	}
  	else
    	{
      		MOOSTrace("LatOrigin not set - FAIL\n");
      		return false;
    	}

  	if(m_MissionReader.GetValue("LongOrigin",sVal))
    	{
      		dfLongOrigin = atof(sVal.c_str());
      		MOOSTrace("  LongOrigin = %10.5f deg.\n",dfLongOrigin);
    	}
  	else
    	{
      		MOOSTrace("LongOrigin not set - FAIL\n");
      		return false;
    	}

  	if(m_MissionReader.GetValue("DestinationPlatformId",sVehicleId))
    	{
      		MOOSTrace("  Vehicle Id extracted =  %s\n",sVehicleId.c_str());
    	}
  	else
    	{
      		MOOSTrace("Vehicle Id for Macrura = 4\n");
      		return false;
    	}


	if(m_MissionReader.GetValue("CollaboratingPlatformId",sCollVehicleId))
    	{
      		MOOSTrace(" Collaborating Vehicle Id extracted =  %s\n",sCollVehicleId.c_str());
    	}
  	else
    	{
      		MOOSTrace("No collaborating id\n");
      		return false;
    	}


  	if(m_MissionReader.GetValue("ProsecuteMission",sProsecuteMission))
    	{
      		MOOSTrace(" Prosecute Mission =  %s\n",sProsecuteMission.c_str());
		SetMOOSVar("ProsecuteMission", sProsecuteMission, TimeNow);
		
    	}
  	else
    	{
      		MOOSTrace("Error reading Prosecute Mission Type\n");
      		return false;
    	}

  	// We do this here, as iGPS won't be running.
  	if(!m_Geodesy.Initialise(dfLatOrigin,dfLongOrigin))
    	{
      		MOOSTrace("Geodesy Init failed - FAIL\n");
      		return false;
    	}

	return true;
}

bool CpNaFCon::Iterate()
{

  	double dfTimeNow = MOOSTime();
	double distancefromDeploy;
	double xinmeters;
	double yinmeters;

	SincePolled = MOOSTime() - LastPolled;
	m_Comms.Notify("SINCE_POLLED",SincePolled, MOOSTime());
 
	if(MessageFlag == 1)
     	{	

	  //Previous Setup for total Deploy/Prosecute duration
	  //		if(dfTimeNow > AbortTime)
	  //	{
	  //	  if(!abort_set)
	  //	    {
/*
	      		SetMOOSVar("DeployState", "ABORT", dfTimeNow);
	      		SetMOOSVar("ProsecuteState", "ABORT", dfTimeNow);
			Mis_Type = "2";	
			MissionState = "2";
			abort_set = true;
*/
	  //	    }
	  //	}

	  //Modified abort time based on polling durations
	  if(SincePolled > PollingMaxDuration)
	    {
	  	  if(!abort_set)
	  	    {

	      		SetMOOSVar("DeployState", "ABORT", dfTimeNow);
	      		SetMOOSVar("ProsecuteState", "ABORT", dfTimeNow);
			Mis_Type = "2";	
			MissionState = "2";
			abort_set = true;

	  	    }

	    }
		
		if(m_Geodesy.LatLong2LocalUTM(atof(Nav_x.c_str()),atof(Nav_y.c_str()),yinmeters,xinmeters))
		{
			distancefromDeploy = sqrt((xinmeters - DeployX)*(xinmeters - DeployX)+(yinmeters - DeployY)*(yinmeters - DeployY)); 	
			SetMOOSVar("DiatanceAway", distancefromDeploy, dfTimeNow);
		}
		
		
	}


	if(MOOSTime()> Sensor_rpt_time + rpt_delay)
	{

//        string StatusMessage = "MessageType=SENSOR_STATUS,SensorReportType=0,SourcePlatformId="+sVehicleId+",DestinationPlatformId="+sCollVehicleId+",Timestamp="+doubleToString(dfTimeNow,8)+",NodeLatitude="+Nav_x+",NodeLongitude="+Nav_y+",NodeDepth="+Nav_z+",NodeCEP=0,NodeHeading="+Nav_yaw+",NodeSpeed="+Nav_speed+",MissionState="+MissionState+",MissionType="+Mis_Type+",LastGPSTimestamp=0,PowerLife=0,SensorHealth=0,RecorderState=1,RecorderLife=0,NodeSpecificInfo0="+TimeToSlowdown+",NodeSpecificInfo1="+TimeToSpeedup+",NodeSpecificInfo2="+SensorCmdType+",NodeSpecificInfo3=0,NodeSpecificInfo4="+sVehicleId+",NodeSpecificInfo5=0";
        string StatusMessage = "MessageType=SENSOR_STATUS,SensorReportType=0,SourcePlatformId="+sVehicleId+",DestinationPlatformId="+sCollVehicleId+",Timestamp="+doubleToString(dfTimeNow,8)+",NodeLatitude="+Nav_x+",NodeLongitude="+Nav_y+",NodeDepth="+Nav_z+",NodeCEP=0,NodeHeading="+Nav_yaw+",NodeSpeed="+Nav_speed+",MissionState="+MissionState+",MissionType="+Mis_Type+",LastGPSTimestamp=0,PowerLife=0,SensorHealth=0,RecorderState=1,RecorderLife=0,NodeSpecificInfo0="+TimeToSlowdown+",NodeSpecificInfo1="+TimeToSpeedup+",NodeSpecificInfo2="+SensorCmdType+",NodeSpecificInfo3="+CollaborationMode+",NodeSpecificInfo4="+sVehicleId+",NodeSpecificInfo5=0";

        SetMOOSVar("Plusnetmessages", StatusMessage, dfTimeNow);
        Sensor_rpt_time = MOOSTime();

	}


        if(VirtualTarget==1)
        {

                double elapsed = MOOSTime() - Init_time;
                Init_time = MOOSTime();

                target_x = target_x + Init_xdot*elapsed;
                target_y = target_y + Init_ydot*elapsed;

		double trkheading = 90.0 - atan2(target_y - AUV_y, target_x - AUV_x)*180.0/M_PI;
		if(trkheading < 0.0) trkheading += 360.0;
 

	if(MOOSTime() > Contact_vrt_rpt_time + rpt_delay)
	{

        string ContactMessage = "MessageType=SENSOR_CONTACT,SensorReportType=1,SourcePlatformId="+sVehicleId+",DestinationPlatformId="+sCollVehicleId+","
                "ContactTimestamp="+Bstat_time+",SensorHeading="+Nav_yaw+",SensorPitch="+Nav_pitch+",SensorRoll="+Nav_roll+",SensorLatitude="+Nav_x+","
                "SensorLongitude="+Nav_y+",SensorDepth="+Nav_z+",SensorCEP=0,ContactBearing="+doubleToString(trkheading,4)+",ContactBearingUncertainty=0,"
                "ContactBearingRate=0,ContactBearingRateUncertainty=0,ContactElevation=0,ContactElevationUncertainty=0,"
                "ContactSpectralLevel1=0,ContactFrequency1=0,ContactBandwidth1=0,ContactSpectralLevel2=0,"
                "ContactFrequency2=0,ContactBandwidth2=0";

	SetMOOSVar("Plusnetmessages", ContactMessage, dfTimeNow);


       Contact_vrt_rpt_time = MOOSTime();

	}	
        }


	
	PublishFreshMOOSVariables();
	return true;
}

bool CpNaFCon::OnStartUp()
{
  	double dfTimeNow = MOOSTime();
	string vehicleId;


	//DEPLOY_STATE (FALSE, DEPLOY, ABORT, RETURN)
	  AddMOOSVariable("DeployState",	"",	"DEPLOY_STATE",       0);
	//DEPLOY_MISSION(0=continuous DCL, 1=low-power DCL, 2=Off, 3=Return to base, 4=Yoyo, 5=ZigZag) 
	  AddMOOSVariable("DeployMission",	"",	"DEPLOY_MISSION",       0);
	//DEPLOY_RADIUS = operating radius (tens of meters),10bit unsigned int range 0-1023m 
	  AddMOOSVariable("DeployRadius",	"",	"DEPLOY_RADIUS",	0);
	//SENSOR_DEPTH_DEPLOY
	  AddMOOSVariable("DeployDepth",	"",	"SENSOR_DEPTH_DEPLOY",       0);
	//SENSOR_DEPTH_RETURN 
	  AddMOOSVariable("ReturnDepth",	"",	"SENSOR_DEPTH_RETURN",       0);
	//SENSOR_DEPTH_ABORT 
	  AddMOOSVariable("AbortDepth",	"",	"SENSOR_DEPTH_ABORT",       0);
	//SENSOR_DEPLOY = "deploy lat, deploy lon"
	  AddMOOSVariable("SensorDeploy",	"",	"SENSOR_DEPLOY",       0);
	//added for kayaks
	  AddMOOSVariable("SensorProsecute",	"",	"SENSOR_Prosecute",       0);



	//ABORT Lat
	  AddMOOSVariable("AbortLat",	"",	"ABORT_LAT",	0);
	//ABORT Lon
	  AddMOOSVariable("AbortLon",	"",	"ABORT_LON",	0);

	//SENSOR_ABORT = "abort lat, abort lon" 
	  AddMOOSVariable("SensorAbort",	"",	"SENSOR_ABORT",       0);
	//SENSOR_RETURN = "return lat, return lon" 
	  AddMOOSVariable("SensorReturn",	"",	"SENSOR_RETURN",       0);
	//SENSOR_RACETRACK  
	  AddMOOSVariable("SensorRaceTrack",	"",	"SENSOR_RACETRACK",       0);
	//  SENSOR_DEPTH_RACETRACK 
	  AddMOOSVariable("RaceTrackDepth",	"",	"SENSOR_DEPTH_RACETRACK",       0);
	//SENSOR_OFF  
	  AddMOOSVariable("SensorOff",	"",	"SENSOR_OFF",       0);
	//  SENSOR_DEPTH_OFF 
	  AddMOOSVariable("OffDepth",	"",	"SENSOR_DEPTH_OFF",       0);
	//Timeout in DEPLOY and PROSECUTE
	  AddMOOSVariable("AbortTime",	"",	"ABORT_TIME",       0);
	//PROSECUTE_STATE (FALSE, PROSECUTE, ABORT)
	  AddMOOSVariable("ProsecuteState",	"",	"PROSECUTE_STATE",       0);
	//TARGET_STATE = "lat, lon, depth, heading"
	  AddMOOSVariable("TargetState",	"",	"TARGET_STATE",       0);
	//YOYO_DISTANCE = DEPLOY_RADIUS (tens of meters), 10bit unsigned int range 0-1023m 
	  AddMOOSVariable("yoyodistance",	"",	"YOYO_DISTANCE",       0);
	//YOYO_HEADING = DEPLOY DCL FOV Start Heading, units 0.175867 (range 0-2047) 11bit unsigned int  
	  AddMOOSVariable("yoyoheading",	"",	"YOYO_HEADING",       0);
	//YOYO_UPPER = DEPLOY DCL FOV End Heading, units 0.175867 (range 0-2047) 11bit unsigned int  
	  AddMOOSVariable("yoyoupper",	"",	"YOYO_UPPER",       0);
	//YOYO_LOWER = DEPLOY DCL Search rate, units minutes (range 1-255) 8bit unsigned int  
	  AddMOOSVariable("yoyolower",	"",	"YOYO_LOWER",       0);
	//ZIGZAG_DISTANCE = DEPLOY_RADIUS (tens of meters), 10bit unsigned int range 0-1023m 
	  AddMOOSVariable("zigzagdistance",	"",	"ZIGZAG_DISTANCE",       0);
	//ZIGZAG_HEADING = DEPLOY DCL FOV Start Heading, units 0.175867 (range 0-2047) 11bit unsigned int  
	  AddMOOSVariable("zigzagheading",	"",	"ZIGZAG_HEADING",       0);
	//ZIGZAG_STATUS = points = zigzag:x,y,angle,length,period,amplitude
	  AddMOOSVariable("zigzagstatus",	"",	"ZIGZAG_STATUS",       0);
	  AddMOOSVariable("Plusnetmessages",	"",	"PLUSNET_MESSAGES",       0);
	  AddMOOSVariable("ModemCommand",	"",	"MICROMODEM_COMMAND",       0);
	  AddMOOSVariable("DistanceAway",	"",	"DISTANCE_FROM_DEPLOY",       0);
//	  AddMOOSVariable("DeployON",	"",	"DEPLOY_ON",       0);
	  AddMOOSVariable("VehicleID",	"",	"VEHICLE_ID",       0);
	  AddMOOSVariable("Clb_msg",	"",	"COLLABORATOR_MESSAGE",       0);
	  AddMOOSVariable("ProsecuteMission",	"",	"PROSECUTE_MISSION",       0);

	SetMOOSVar("Clb_msg", "0", dfTimeNow);
// Prosecute Mission is read from MOOS block
//	SetMOOSVar("ProsecuteMission", "0", dfTimeNow);
	SetMOOSVar("ProsecuteMission", sProsecuteMission, dfTimeNow);
	m_MissionReader.GetValue("DestinationPlatformId",vehicleId);
	m_MissionReader.GetValue("Report_Delay",rpt_delay);
	SetMOOSVar("VehicleID", atoi(vehicleId.c_str()), dfTimeNow);
	SetMOOSVar("DeployState", "FALSE", dfTimeNow);
	SetMOOSVar("DeployMission", "15", dfTimeNow);
	SetMOOSVar("DeployRadius", 0.0, dfTimeNow);//dpe
	SetMOOSVar("DeployDepth", "depth=0.0", dfTimeNow);
	SetMOOSVar("ReturnDepth", "depth=0.0", dfTimeNow);
	SetMOOSVar("AbortDepth", "depth=0.0", dfTimeNow);
	SetMOOSVar("SensorDeploy", "points=radial:0.0,0.0,100,6", dfTimeNow);
	
	// added for kayaks
	SetMOOSVar("SensorProsecute", "points=radial:0.0,0.0,100,6", dfTimeNow);

	SetMOOSVar("SensorAbort", "points=0.0,0.0", dfTimeNow);
	SetMOOSVar("SensorReturn", "points=0.0,0.0", dfTimeNow);
	SetMOOSVar("SensorRaceTrack", "deploy_location=0,0#heading=0", dfTimeNow);
	SetMOOSVar("AbortTime", 0.0, dfTimeNow);
	SetMOOSVar("ProsecuteState", "FALSE", dfTimeNow);
	SetMOOSVar("TargetState", "0,0,0,0,0,0", dfTimeNow);
	SetMOOSVar("yoyodistance", 0.0, dfTimeNow); //dpe
	SetMOOSVar("yoyoheading", 0.0, dfTimeNow); //dpe
	SetMOOSVar("yoyoupper", 0.0, dfTimeNow); //dpe
	SetMOOSVar("yoyolower", 0.0, dfTimeNow); //dpe
	SetMOOSVar("zigzagdistance", 0.0, dfTimeNow);//dpe
	SetMOOSVar("zigzagheading", 0.0, dfTimeNow);//dpe
	SetMOOSVar("zigzagstatus","points=zigzag:0,0,0,0,0,0",dfTimeNow); 
//	SetMOOSVar("DeployON",0,dfTimeNow); 
        m_Comms.Notify ("BEARING_STAT","0");

	return true;
}

void CpNaFCon::Translate(CMOOSMsg sentence)
{
	vector<string> mfields;
	string ss;
	string sId;
	string slat;
	string slon;
	string sdepth;
	string sablat;
	string sablon;
	string sabdepth;
	string smistype;
	string sopradius;
	string sduration;
	string ssdeploy;
	string ssprosecute;
	string sabort;
	string sreturn;
	string destiId;
	string clbId;
	string trackId;
	string stargetHead;
	string stargetState;
	string stargetSpeed;
	string stargetTime;
	string stargetId;
	string stargetSPL1;
	string stargetFRQ1;
	string stargetBW1;
	string sracetrack;
	string ssensoroff;


	string sdclstartheading;
	string sdclendheading;
	string sdclsearchrate;
	string szigzagstate;
	string orbit_radius;
	string orbit_points;
	string zigzagperiod;
	string zigzagampli;
	string PlatformID;
 
	string SourceID; 
	string SrcBstatTime;
	string SrcNavYaw;
	string SrcNavPitch;
	string SrcNavRoll;
	string SrcNavX;
	string SrcNavY;
	string SrcNavZ;
	string SrcBstatBearing;
	double contact_x,contact_y;

	string TrackTime;
	string TrackLat;
	string TrackLon;
	string TrackCourse;
	string TrackSpeed;
	double track_x,track_y;
	

	bool match;
  	double dfTimeNow = MOOSTime();
	string misstrduration;
	double latcoord;
	double loncoord;
	double localNorth;
	double localEast;
	double depth;

	double node_x;
	double node_y;	
	string node_speed;
	string node_heading;
	string node_time;
	string node_lat;
	string node_long;
	string node_remote_colla;

	string msg;
	char buff[200];
	char buff2[200];
	char buff3[200];
	
	//Get the Destination ID from the MOOS file
	  m_MissionReader.GetValue("DestinationPlatformId",destiId);
	//Get the Collaborating vehicle ID from the MOOS file
	  m_MissionReader.GetValue("CollaboratingPlatformId",clbId);
	//Get the TargetID for Contact report from the MOOS file
	  m_MissionReader.GetValue("TrackId",trackId);
	//Get the Orbit radius
	  m_MissionReader.GetValue("Orbit_radius",orbit_radius);
	//Get the Orbit points
	  m_MissionReader.GetValue("Orbit_points",orbit_points);
	//Get the ZigZag Period
	  m_MissionReader.GetValue("ZigZag_Period",zigzagperiod);
	//Get the ZigZag Amplitude
	  m_MissionReader.GetValue("ZigZag_Amplitude",zigzagampli);

	MOOSTrace("%f\n",dfTimeNow);
	MOOSTrace("\n");
	
//	mfields = parseString(sentence.m_sVal.c_str(),',');
//	MOOSTrace("%s\n",mfields[0].c_str());

	match = tokParse(sentence.m_sVal.c_str(),"MessageType",',','=',ss); 

	if(match){
		if(ss == "SENSOR_STATUS"){
			MOOSTrace("MessageType = SENSOR_STATUS\n");
	       
//		         match = tokParse(sentence.m_sVal.c_str(),"SourcePlatformId",',','=',SourceID);
		         match = tokParse(sentence.m_sVal.c_str(),"NodeSpecificInfo4",',','=',SourceID);
		         match = tokParse(sentence.m_sVal.c_str(),"Timestamp",',','=',node_time);
		         match = tokParse(sentence.m_sVal.c_str(),"NodeLatitude",',','=',node_lat);
		         match = tokParse(sentence.m_sVal.c_str(),"NodeLongitude",',','=',node_long);
		         match = tokParse(sentence.m_sVal.c_str(),"NodeHeading",',','=',node_heading);
		         match = tokParse(sentence.m_sVal.c_str(),"NodeSpeed",',','=',node_speed);
		         match = tokParse(sentence.m_sVal.c_str(),"NodeSpecificInfo3",',','=',node_remote_colla);
		
			if(node_remote_colla == "0")
				m_Comms.Notify("REMOTE_COLLABORATION","OFF");

			if(node_remote_colla == "1")
				m_Comms.Notify("REMOTE_COLLABORATION","SYNCH");
	
			if(node_remote_colla == "2")
				m_Comms.Notify("REMOTE_COLLABORATION","COLLABORATING");

			if(m_Geodesy.LatLong2LocalUTM(atof(node_lat.c_str()),atof(node_long.c_str()),node_y,node_x))
			{
				MOOSTrace("Status Lat = %.3f, Long = %.3f\n",atof(node_lat.c_str()), atof(node_long.c_str()));
				MOOSTrace("Status x = %.3f, y = %.3f\n",node_x, node_y);
			}

			sprintf(buff2,"node=%d,x=%.3f,y=%.3f,heading=%.3f,speed=%.3f,time=%.6f",atoi(SourceID.c_str()),node_x,node_y,atof(node_heading.c_str()),atof(node_speed.c_str()),atof(node_time.c_str()));
                	   msg = buff2;
                	   m_Comms.Notify ("COMMUNITY_STAT",msg);
			//Under construction (COLLABORATING_MESSAGE = sourceID)
			SetMOOSVar("Clb_msg", sentence.m_sVal.c_str(), dfTimeNow);
			 
		}
		if(ss == "SENSOR_CONTACT"){
			MOOSTrace("MessageType = SENSOR_CONTACT\n");
			MOOSTrace("Message = %s\n",sentence.m_sVal.c_str());
	                match = tokParse(sentence.m_sVal.c_str(),"SourcePlatformId",',','=',SourceID);
	                match = tokParse(sentence.m_sVal.c_str(),"ContactTimestamp",',','=',SrcBstatTime);
	                match = tokParse(sentence.m_sVal.c_str(),"SensorHeading",',','=',SrcNavYaw);
	                match = tokParse(sentence.m_sVal.c_str(),"SensorPitch",',','=',SrcNavPitch);
	                match = tokParse(sentence.m_sVal.c_str(),"SensorRoll",',','=',SrcNavRoll);
	                match = tokParse(sentence.m_sVal.c_str(),"SensorLatitude",',','=',SrcNavX);
	                match = tokParse(sentence.m_sVal.c_str(),"SensorLongitude",',','=',SrcNavY);
	                match = tokParse(sentence.m_sVal.c_str(),"SensorDepth",',','=',SrcNavZ);
	                match = tokParse(sentence.m_sVal.c_str(),"ContactBearing",',','=',SrcBstatBearing);

			if(m_Geodesy.LatLong2LocalUTM(atof(SrcNavX.c_str()),atof(SrcNavY.c_str()),contact_y,contact_x))
			{
				MOOSTrace("Contact Lat = %.3f, Long = %.3f\n",atof(SrcNavX.c_str()), atof(SrcNavY.c_str()));
				MOOSTrace("Contact x = %.3f, y = %.3f\n",contact_x, contact_y);
			}

			if(((atof(SrcBstatTime.c_str()) - Old_bstat_time) != 0.0) && (atoi(SourceID.c_str()) == atoi(sCollVehicleId.c_str())))
			{
			   sprintf(buff,"node=%d,state=2,bearing=%.4f,xp=%.3f,yp=%.3f,beamno=0,sigma=0,time=%.6f",atoi(SourceID.c_str()),atof(SrcBstatBearing.c_str()),contact_x,contact_y,atof(SrcBstatTime.c_str()));
                	   msg = buff;
                	   m_Comms.Notify ("BEARING_STAT",msg);
			   Old_bstat_time = atof(SrcBstatTime.c_str());
			}

			SetMOOSVar("Clb_msg", sentence.m_sVal.c_str(), dfTimeNow);
		}
		if(ss == "SENSOR_TRACK"){
			MOOSTrace("MessageType = SENSOR_TRACK\n");
			MOOSTrace("Message = %s\n",sentence.m_sVal.c_str());
	                match = tokParse(sentence.m_sVal.c_str(),"SourcePlatformId",',','=',SourceID);
	                match = tokParse(sentence.m_sVal.c_str(),"TrackTimestamp",',','=',TrackTime);
	                match = tokParse(sentence.m_sVal.c_str(),"TrackLatitude",',','=',TrackLat);
	                match = tokParse(sentence.m_sVal.c_str(),"TrackLongitude",',','=',TrackLon);
	                match = tokParse(sentence.m_sVal.c_str(),"TrackHeading",',','=',TrackCourse);
	                match = tokParse(sentence.m_sVal.c_str(),"TrackSpeed",',','=',TrackSpeed);
			
			if(m_Geodesy.LatLong2LocalUTM(atof(TrackLat.c_str()),atof(TrackLon.c_str()),track_y,track_x))
			{
				MOOSTrace("Track Lat = %.3f, Long = %.3f\n",atof(TrackLat.c_str()), atof(TrackLon.c_str()));
				MOOSTrace("Track x = %.3f, y = %.3f\n",track_x, track_y);
			}

			   sprintf(buff3,"node=%d,state=2,x=%.16f,y=%.16f,heading=%.16f,speed=%.16f,time=%.16f",atoi(SourceID.c_str()),track_x,track_y,atof(TrackCourse.c_str()),atof(TrackSpeed.c_str()),atof(SrcBstatTime.c_str()));

			msg = buff3;
                	m_Comms.Notify ("TRACK_STAT",msg);

			SetMOOSVar("Clb_msg", sentence.m_sVal.c_str(), dfTimeNow);
		}
		if(ss == "SENSOR_NOISE"){
			MOOSTrace("MessageType = SENSOR_NOISE\n");
			SetMOOSVar("Clb_msg", sentence.m_sVal.c_str(), dfTimeNow);
		}

		//When SENSOR_DEPLOY
		if(ss == "SENSOR_DEPLOY"){
			SetMOOSVar("Clb_msg", sentence.m_sVal.c_str(), dfTimeNow);
	              match = tokParse(sentence.m_sVal.c_str(),"DestinationPlatformId",',','=',sId); 

		      //Check for Platform ID = destiId 
		      if(sId == destiId)
		      {
			MessageFlag = 1;
			first_deploy = 1; 
			LastPolled = MOOSTime();

                        VirtualTarget = 0;
			//For continuous DCL track ID is 255
			char trnumber[10];
			sprintf(trnumber,"255");
			stracknumber = trnumber;


//		      	SetMOOSVar("DeployON", first_deploy, dfTimeNow);
			
			//Mission State = 1
			MissionState = "0";

			//Set Abort to false
//			abort_set = false;

			//DEPLOY STATE
		      	SetMOOSVar("DeployState", "DEPLOY", dfTimeNow);
			//PROSECUTE STATE
			SetMOOSVar("ProsecuteState", "FALSE", dfTimeNow); //dpe
			SetMOOSVar("ProsecuteMission", sProsecuteMission, dfTimeNow);
			//stop surfacing
			m_Comms.Notify("GOTO_SURFACE", "FALSE", dfTimeNow); //dpe

			//Set prosecute_mission = 0
//			Prosecute mission is read from the MOOS block
//			SensorCmdType = "0";
//			SetMOOSVar("ProsecuteMission", SensorCmdType, dfTimeNow);

	              	match = tokParse(sentence.m_sVal.c_str(),"DeployLatitude",',','=',slat); 
	              	match = tokParse(sentence.m_sVal.c_str(),"DeployLongitude",',','=',slon); 
	              	match = tokParse(sentence.m_sVal.c_str(),"DeployDepth",',','=',sdepth); 
	              	match = tokParse(sentence.m_sVal.c_str(),"MissionType",',','=',smistype); 
	              	match = tokParse(sentence.m_sVal.c_str(),"OperatingRadius",',','=',sopradius); 

//			if(atoi(smistype.c_str()) > 4)
//			{
//				Mis_Type = "4";
//			}else
//			{
//				Mis_Type = smistype;
//			}

			Mis_Type = smistype;

			Op_radius = atof(sopradius.c_str())*10;

			//DEPLOY MISSION
			SetMOOSVar("DeployMission",smistype,dfTimeNow);

			//Converting Lat/Long to Local coordinates
			latcoord = atof(slat.c_str());
			loncoord = atof(slon.c_str());
			depth = atof(sdepth.c_str());
			sdepth = "depth="+sdepth; 
//			if(m_Geodesy.LatLong2LocalGrid(latcoord,loncoord,localNorth,localEast))
			if(m_Geodesy.LatLong2LocalUTM(latcoord,loncoord,localNorth,localEast))
			{
				MOOSTrace("Lat = %0.2f, LongE = %0.2f\n",latcoord,loncoord);
				MOOSTrace("LocalN = %0.2f, LocalE = %0.2f\n",localNorth,localEast);
			 	DeployX = localEast;
				DeployY = localNorth;	
			}

//			ssdeploy = doubleToString(localNorth,2)+","+doubleToString(localEast,2);
//			ssdeploy = "points="+doubleToString(localEast,9)+","+doubleToString(localNorth,9);
                        if(smistype == "5") //dpe
			  ssdeploy = "points="+doubleToString(localEast,9)+","+doubleToString(localNorth,9); //dpe
			else //dpe
			   ssdeploy = "polygon=radial:"+doubleToString(localEast,9)+","+doubleToString(localNorth,9)+","+orbit_radius+","+orbit_points;

			//SENSOR DEPLOY
		      	SetMOOSVar("SensorDeploy", ssdeploy, dfTimeNow);

			//DEPLOY DEPTH
			SetMOOSVar("DeployDepth",sdepth,dfTimeNow);
			 
			//If Mission type = 3 :Return To Base
			if(smistype == "3")
			{
				//DEPLOY STATE
		      		SetMOOSVar("DeployState", "RETURN", dfTimeNow);
				sreturn = "points="+doubleToString(localEast,9)+","+doubleToString(localNorth,9);

				//SENSOR RETURN
		      		SetMOOSVar("SensorReturn", sreturn, dfTimeNow);

				//RETURN DEPTH
				SetMOOSVar("ReturnDepth",sdepth,dfTimeNow);
			}
				
	              	match = tokParse(sentence.m_sVal.c_str(),"DeployDuration",',','=',sduration); 
	              	match = tokParse(sentence.m_sVal.c_str(),"DCLFOVStartHeading",',','=',sdclstartheading); 
	              	match = tokParse(sentence.m_sVal.c_str(),"DCLFOVEndHeading",',','=',sdclendheading); 
	              	match = tokParse(sentence.m_sVal.c_str(),"DCLSearchRate",',','=',sdclsearchrate); 

			//If Mission type = 4 : Race Track
			if(smistype == "4")
			{
				//DEPLOY STATE
		      		SetMOOSVar("DeployState", "DEPLOY", dfTimeNow);
				sracetrack = "points="+doubleToString(localEast,9)+","+doubleToString(localNorth,9)+"#heading="+sdclstartheading;

				//SENSOR RACETRACK 
		      		SetMOOSVar("SensorRaceTrack", sracetrack, dfTimeNow);

				//RACETRACK DEPTH
				SetMOOSVar("RaceTrackDepth",sdepth,dfTimeNow);
			}

			//If Mission type = 2 : Sensor Off 
			if(smistype == "2")
			{
				//DEPLOY STATE
		      		SetMOOSVar("DeployState", "OFF", dfTimeNow);
				ssensoroff = "points="+doubleToString(localEast,9)+","+doubleToString(localNorth,9);

				//SENSOR OFF 
		      		SetMOOSVar("SensorOff", ssensoroff, dfTimeNow);

				//OFF DEPTH
				SetMOOSVar("OffDepth",sdepth,dfTimeNow);
			}

			//DEPLOY RADIUS
                        double dfopradius = atof(sopradius.c_str())*10;  //dpe
			SetMOOSVar("DeployRadius",dfopradius,dfTimeNow); //dpe


			//If Mission type = 5 : Yoyo mission
			if(smistype == "5")
			{
			  
			  double dfdclstartheading = atof(sdclstartheading.c_str());  //dpe
			  double dfdclendheading = atof(sdclendheading.c_str());  //dpe
			  double dfdclsearchrate = atof(sdclsearchrate.c_str());  //dpe
			  
			  SetMOOSVar("yoyodistance",dfopradius,dfTimeNow); //dpe
			  SetMOOSVar("yoyoheading",dfdclstartheading,dfTimeNow); //dpe 
			  SetMOOSVar("yoyoupper",dfdclendheading,dfTimeNow); //dpe
			  SetMOOSVar("yoyolower",dfdclsearchrate,dfTimeNow); //dpe 
			}

			//If Mission type = 6 : Zigzag mission
			if(smistype == "6")
			{
			  double dfzigzagheading = atof(sdclstartheading.c_str());
			  double dfzigzagdistance = atof(sopradius.c_str())*10; 
				SetMOOSVar("zigzagdistance",dfzigzagdistance,dfTimeNow); 
				SetMOOSVar("zigzagheading",dfzigzagheading,dfTimeNow); 
				
				szigzagstate = "points=zigzag:"+doubleToString(localEast,9)+","+doubleToString(localNorth,9)+","+sdclstartheading+","+doubleToString(dfzigzagdistance,2)+","+zigzagperiod+","+zigzagampli;
				SetMOOSVar("zigzagstatus",szigzagstate,dfTimeNow);  
			}

			PollingMaxDuration = atof(sduration.c_str())*600.0;

			AbortTime = atof(sduration.c_str())*600.0+dfTimeNow;

			//ABORT TIME
		      	SetMOOSVar("AbortTime", AbortTime, dfTimeNow);
                        abort_set = false; //dpe

	              	match = tokParse(sentence.m_sVal.c_str(),"AbortLatitude",',','=',sablat); 
	              	match = tokParse(sentence.m_sVal.c_str(),"AbortLongitude",',','=',sablon); 
	              	match = tokParse(sentence.m_sVal.c_str(),"AbortDepth",',','=',sabdepth); 

			//Converting Lat/Long to Local coordinates
			latcoord = atof(sablat.c_str());
			loncoord = atof(sablon.c_str());
//			if(m_Geodesy.LatLong2LocalGrid(latcoord,loncoord,localNorth,localEast))
			if(m_Geodesy.LatLong2LocalUTM(latcoord,loncoord,localNorth,localEast))
			{
				MOOSTrace("Lat = %0.2f, LongE = %0.2f\n",latcoord,loncoord);
				MOOSTrace("LocalN = %0.2f, LocalE = %0.2f\n",localNorth,localEast);
			}
			 
//			sabort = doubleToString(localNorth,2)+","+doubleToString(localEast,2);
			sabort = "points="+doubleToString(localEast,9)+","+doubleToString(localNorth,9);

			//SENSOR ABORT
		      	SetMOOSVar("SensorAbort", sabort, dfTimeNow);

			sabdepth = "depth="+sabdepth; 
			//ABORT DEPTH
			SetMOOSVar("AbortDepth",sabdepth,dfTimeNow);

		      }    	
		}
		//When SENSOR_PROSECUTE 
		//Check for Platform ID = destiId 

		if(ss == "SENSOR_PROSECUTE"){
	              match = tokParse(sentence.m_sVal.c_str(),"DestinationPlatformId",',','=',sId); 
		      if((sId == destiId) && (first_deploy == 1))
		      { 

//			Mis_Type = "PROSECUTE";
			LastPolled = MOOSTime();
		
				
	              	match = tokParse(sentence.m_sVal.c_str(),"SensorCommandType",',','=',SensorCmdType); 
// 			Prosecute Mission type is read fro the MOOS block 
//			SetMOOSVar("ProsecuteMission", SensorCmdType, dfTimeNow);

	              	match = tokParse(sentence.m_sVal.c_str(),"TrackNumber",',','=',stargetId); 
	              	match = tokParse(sentence.m_sVal.c_str(),"PlatformID",',','=',PlatformID); 

			TrackNumber = atoi(stargetId.c_str());

			char trnumber[10];
			sprintf(trnumber,"%d",TrackNumber);
		
			stracknumber = trnumber;
	
	              	match = tokParse(sentence.m_sVal.c_str(),"TargetLatitude",',','=',slat); 
	              	match = tokParse(sentence.m_sVal.c_str(),"TargetLongitude",',','=',slon); 
	              	match = tokParse(sentence.m_sVal.c_str(),"TargetDepth",',','=',sdepth); 
	              	match = tokParse(sentence.m_sVal.c_str(),"TargetHeading",',','=',stargetHead); 
	              	match = tokParse(sentence.m_sVal.c_str(),"TargetSpeed",',','=',stargetSpeed); 
	              	match = tokParse(sentence.m_sVal.c_str(),"TargetTimestamp",',','=',stargetTime); 

	              	match = tokParse(sentence.m_sVal.c_str(),"TargetSpectralLevel1",',','=',stargetSPL1); 
			double targetSPL = atof(stargetSPL1.c_str());

        		m_Comms.Notify ("TARGET_SPL",targetSPL);
			
	              	match = tokParse(sentence.m_sVal.c_str(),"TargetFrequency1",',','=',stargetFRQ1); 
			double tgt_frq1 = atof(stargetFRQ1.c_str())*10.0;
		
			//Modified on 5/9/07	
        		m_Comms.Notify ("TARGET_FREQ",tgt_frq1);

	              	match = tokParse(sentence.m_sVal.c_str(),"TargetBandwidth1",',','=',stargetBW1); 
			double twospower = 15.0 - atof(stargetBW1.c_str());

			double bandwidthHertz = pow(2,twospower);

			MOOSTrace("Bandwidth received = %s\n",stargetBW1.c_str()); 
			MOOSTrace("Twospower = %f, Bandwidth = %f\n",twospower,bandwidthHertz); 

        		m_Comms.Notify ("TARGET_BW",bandwidthHertz);


			//Converting Lat/Long to Local coordinates
			latcoord = atof(slat.c_str());
			loncoord = atof(slon.c_str());
//			if(m_Geodesy.LatLong2LocalGrid(latcoord,loncoord,localNorth,localEast))
			if(m_Geodesy.LatLong2LocalUTM(latcoord,loncoord,localNorth,localEast))
			{
				MOOSTrace("Lat = %0.2f, LongE = %0.2f\n",latcoord,loncoord);
				MOOSTrace("LocalN = %0.2f, LocalE = %0.2f\n",localNorth,localEast);
			}
			//CONTACT REPORT
			if(stargetId == trackId)
//			if(PlatformID == trackId)
			{

                                //Send the Contact Report
                                MOOSTrace("Request for a Contact Report\n\n");
                                VirtualTarget = 1;

                                Init_time = atof(stargetTime.c_str());
                                Init_head = atof(stargetHead.c_str());
                                Init_speed = atof(stargetSpeed.c_str());
                                Init_X = localEast;
                                Init_Y = localNorth;

                                Init_xdot = Init_speed*sin(Init_head*M_PI/180.0);
                                Init_ydot = Init_speed*cos(Init_head*M_PI/180.0); 
			}	
			else
			{
			MessageFlag = 1;

                                VirtualTarget = 0;
			//Set Abort to false 
			abort_set = false;
			MissionState = "1";

			//No definition given for Mission Mode during Prosecute
			Mis_Type = "0";

			//PROSECUTE STATE
		      	SetMOOSVar("ProsecuteState", "PROSECUTE", dfTimeNow);
			
			ssprosecute = "polygon=radial:"+doubleToString(localEast,9)+","+doubleToString(localNorth,9)+",80,6";

			//SENSOR DEPLOY
		      	SetMOOSVar("SensorProsecute", ssprosecute, dfTimeNow);



			SetMOOSVar("ProsecuteMission", sProsecuteMission, dfTimeNow);
			SetMOOSVar("DeployState", "FALSE", dfTimeNow);
			//stop surfacing
			m_Comms.Notify("GOTO_SURFACE", "FALSE", dfTimeNow); //dpe
	              	match = tokParse(sentence.m_sVal.c_str(),"ProsecuteDuration",',','=',sduration); 

			PollingMaxDuration = atof(sduration.c_str())*600.0;

			AbortTime = atof(sduration.c_str())*600.0+dfTimeNow;

			//ABORT TIME
		      	SetMOOSVar("AbortTime", AbortTime, dfTimeNow);


//			stargetState = doubleToString(localNorth,2)+","+doubleToString(localEast,2)+","+sdepth+","+stargetHead+","+stargetSpeed+","+stargetTime+","+stargetId; 
			stargetState = doubleToString(localEast,9)+","+doubleToString(localNorth,9)+","+sdepth+","+stargetHead+","+stargetSpeed+","+stargetTime+","+stargetId; 

			//TARGET STATE 
		      	SetMOOSVar("TargetState",stargetState , dfTimeNow);

	              	match = tokParse(sentence.m_sVal.c_str(),"AbortLatitude",',','=',sablat); 
	              	match = tokParse(sentence.m_sVal.c_str(),"AbortLongitude",',','=',sablon); 
	              	match = tokParse(sentence.m_sVal.c_str(),"AbortDepth",',','=',sabdepth); 

			SetMOOSVar("AbortLat",sablat,dfTimeNow);
			SetMOOSVar("AbortLon",sablon,dfTimeNow);

			//Converting Lat/Long to Local coordinates
			latcoord = atof(sablat.c_str());
			loncoord = atof(sablon.c_str());
//			if(m_Geodesy.LatLong2LocalGrid(latcoord,loncoord,localNorth,localEast))
			if(m_Geodesy.LatLong2LocalUTM(latcoord,loncoord,localNorth,localEast))
			{
				MOOSTrace("Lat = %0.2f, LongE = %0.2f\n",latcoord,loncoord);
				MOOSTrace("LocalN = %0.2f, LocalE = %0.2f\n",localNorth,localEast);
			}
			 
//			sabort = doubleToString(localNorth,2)+","+doubleToString(localEast,2);
			sabort = "points="+doubleToString(localEast,9)+","+doubleToString(localNorth,9);

			//SENSOR ABORT
		      	SetMOOSVar("SensorAbort", sabort, dfTimeNow);
			abort_set = false; //dpe

			sabdepth = "depth="+sabdepth; 
			//ABORT DEPTH 
			SetMOOSVar("AbortDepth",sabdepth,dfTimeNow);
			}
		      }    	
		}
		if(ss == "SENSOR_RECORDER"){
			MOOSTrace("MessageType = SENSOR_RECORDER\n");
		}
		if(ss == "HUXLEY"){
			MOOSTrace("MessageType = HUXLEY\n");
		}
		if(ss == "XRAY_STATUS"){
			MOOSTrace("MessageType = XRAY_STATUS\n");
		}
		if(ss == "XRAY_BEHAVIOR"){
			MOOSTrace("MessageType = XRAY_BEHAVIOR\n");
		}
	}
 
}
 
vector<string> parseString(const string& string_str, char separator)
{
  const char *str = string_str.c_str();

  char buff[strlen(str)+1];

  vector<string> rvector;

  while(str[0] != '\0') {
    int i=0;
    while((str[i]!=separator)&&(str[i]!='\0'))
      i++;
    strncpy(buff, str, i);
    buff[i] = '\0';
    string nstr = buff;

    rvector.push_back(nstr);
    str += i;
    if(str[0]==separator)
      str++;
  }
  return(rvector);
}

bool tokParse(const string& str, const string& left,
               char gsep, char lsep, string& rstr)
{
  rstr = "error";
  vector<string> svector1 = parseString(str, gsep);
  for(int i=0; i<svector1.size(); i++) {
    vector<string> svector2 = parseString(svector1[i], lsep);
    if(svector2.size() != 2)
      return(false);
    if(svector2[0] == left) {
      rstr = svector2[1];
      return(true);
    }
  }
  return(false);
}

string doubleToString(double val, int digits)
{
  char buffAUX[10] = "%.5f\0";
  if((digits>0)&&(digits<7))
    buffAUX[2] = digits+48;

  char buff[100];
  sprintf(buff, buffAUX, val);

  string str = buff;
  return(str);
}

