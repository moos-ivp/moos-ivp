// $Header: /home/cvsroot/project-plus/src/pNaFCon/pNaFCon.h,v 1.5 2007-08-17 12:00:02 arjunab Exp $
// (c) 2005 
// CpNaFCon.h: an interface/translator for communications
////////////////////////////////////////////////

#ifndef __CpNaFCon_h__
#define __CpNaFCon_h__

#include "CommServer.h"
#include "NMEAMessage.h"
#include "CClientTCPSocket.h"
#include "CDatagramCtl.h"
#include "CUDPSocket.h"
#include "MOOSGeodesy.h"
#include "MOOSLib.h"
#include <string>

class CpNaFCon : public CMOOSApp
{
public:
	CpNaFCon();
	virtual ~CpNaFCon();

	bool OnNewMail(MOOSMSG_LIST &NewMail);
	bool Iterate();
	bool OnConnectToServer();
	bool OnStartUp();
	
	double AbortTime;
	double PollingMaxDuration;
	bool MessageFlag,abort_set;
	
	int modem_count;

	int first_deploy;
	double DeployX;
	double DeployY;
	double Op_radius;
	double LastPolled;
	double SincePolled;

	string Nav_x;
	string Nav_y;
	string Nav_z;
	string Nav_yaw;
	string Nav_speed;
	string Nav_pitch;
	string Nav_roll;
	string sVehicleId;
	string sCollVehicleId;
	string MissionState;
	string Mis_Type;
	string Bstat_bearing;
	string Bstat_x;
	string Bstat_y;
	string Bstat_time;
	string Tstat_course;
	string Tstat_speed;
	string Tstat_x;
	string Tstat_y;
	string TimeToSlowdown;
	string TimeToSpeedup;
	string SensorCmdType;
	string sProsecuteMission;
	int VirtualTarget;
	string BearingStatState;
	string CollaborationMode;


	double Init_time;
	double Init_head;
	double Init_speed;
	double Init_X;
	double Init_Y;
	double Init_xdot; 
	double Init_ydot; 
	double Old_bstat_time;
	
	double last_time; 	
	double target_x; 	
	double target_y; 	
	double AUV_y; 	
	double AUV_x; 	
	double Contact_vrt_rpt_time; 	
	double Sensor_rpt_time; 	
	double Contact_rpt_time; 	
	double Track_rpt_time; 	
	double Noise_rpt_time; 	
	double rpt_delay; 	
	int  TrackNumber; 	
	string  stracknumber; 	
protected:
	// insert local vars here

        CMOOSGeodesy m_Geodesy;

	void Translate(CMOOSMsg sentence);
	string Verbose;
};

#endif /* __CpNaFCon_h__ */
