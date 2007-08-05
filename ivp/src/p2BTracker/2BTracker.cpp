/******************************************************************************
 *                                                                            *
 *  Module: 2BTracker                                                         *
 *  Author: Don Eickstedt, MIT/NUWC                                           * 
 *  Date:   21 Feb, 2007                                                      *
 *                                                                            *
 *  Description: This module is responsible for using local and remote bearing*
 *  measurements to estimate the position and velocity of a moving target.    *
 *  The module is specifically designed to handle delayed measurements        *
 *  received from the remote sensors.                                         * 
 *                                                                            * 
 ******************************************************************************/
#include <math.h>
#include <MOOSLib.h>
#include <MOOSGenLib.h>

#include "2BTracker.h"


/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::C2BTracker()                                          *
 *                                                                            *
 *  Description: The constructor for the C2BTracker class                     * 
 *                                                                            * 
 ******************************************************************************/
C2BTracker::C2BTracker()
{  
  filter_initialized = false;
  local_node = -1;
  silent = false;
  new_local_measurement = false;
  new_remote_measurement = false;
  Current_Contact.Ctype = NO_CONTACT;
  last_track_report_time = 0.0;
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::~C2BTracker()                                         *
 *                                                                            *
 *  Description: The destructor for the C2BTracker class                      * 
 *                                                                            * 
 ******************************************************************************/
C2BTracker::~C2BTracker()
{ 
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::Iterate()                                             *
 *                                                                            *
 *  Description: This method is called by the MOOSApp superclass on a         *
 *  regular schedule given by the apptick parameter in the .moos file         * 
 *                                                                            * 
 ******************************************************************************/
bool C2BTracker::Iterate()
{
  char buff[250];
  
  //if a new local measurement is available
  //push it on the queue
  if(new_local_measurement)
    {
      Local_Bearings.push_front(local_measurement);
      MOOSTrace("pushed new bearing in local queue, time = %lf\n",local_measurement.Meas_Time);
      new_local_measurement = false;     
    }

  //if a new remote measurement is available
  //find a solution
  if(new_remote_measurement)
    {
      Solve_Track(&remote_measurement);
      new_remote_measurement = false;
    }
          
  //if we have a track estimate from some previous time T
  //update the current estimate by running the state equations to T + dt  
  //assumes a constant velocity target of course
  if (Current_Contact.Ctype == RANGE_BEARING_HEADING_SPEED)
    {
      //calculate dt
      double current_time = MOOSTime();
      double elapsed = current_time-Current_Contact.Last_Solution_Time;

      //retrieve pos and velocities estimates at T
      double xd = Current_Contact.xdot;
      double yd = Current_Contact.ydot;
      double xp = Current_Contact.x_pos;
      double yp = Current_Contact.y_pos;

      //retrieve current AUV position
      double xnow = get_auv_x();
      double ynow = get_auv_y();

      //run state equations for position
      //assume constant velocity target
      double x_pos = xp + elapsed*xd;
      double y_pos = yp + elapsed*yd;     
      
      //calculate bearing based on CURRENT AUV position
      Current_Contact.bearing = 90.0-atan2((y_pos-ynow),(x_pos-xnow))*180.0/M_PI;
      if (Current_Contact.bearing < 0.0) 
	Current_Contact.bearing +=360.0;
      //calculate target speed
      Current_Contact.speed = sqrt(xd*xd + yd*yd);
      //calculate target heading
      Current_Contact.heading = 90.0-atan2(yd,xd)*180.0/M_PI;
      if (Current_Contact.heading < 0.0) 
	Current_Contact.heading +=360.0;
      //calculate target range based on CURRENT AUV position
      Current_Contact.range = sqrt(pow((Current_Contact.x_pos-xnow),2) + pow((Current_Contact.y_pos-ynow),2));

      if((current_time - last_track_report_time) > track_report_interval)
	{
	  //form TRACK_STATE string
	  sprintf(buff,"node=%d,state=%d,x=%.3f,y=%.3f,heading=%.2f,speed=%.2f,time=%.2f",local_node,2,x_pos,y_pos,Current_Contact.heading,Current_Contact.speed,current_time);

	  //send the TRACK_STAT message to the MOOS DB
	  m_Comms.Notify("TRACK_STAT",buff);  
  
	  //send target stats to MOOS DB
	  m_Comms.Notify("TRACKING","TRACKING");
	  m_Comms.Notify("TARGET_X",x_pos);
	  m_Comms.Notify("TARGET_Y",y_pos);
	  m_Comms.Notify("TARGET_HEADING",Current_Contact.heading);
	  m_Comms.Notify("TARGET_SPEED",Current_Contact.speed);
	  m_Comms.Notify("TARGET_RANGE",Current_Contact.range);
	  m_Comms.Notify("TARGET_BEARING",Current_Contact.bearing);
	  last_track_report_time = current_time;
	}
    }  
  
  //update the local measurement queue
  //this removes old measurements as necessary
  update_local_queue();
    
  return true;
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::OnStartUp()                                           *
 *                                                                            *
 *  Description: This method is called by the MOOSApp superclass when         *
 *  the p2BTracker  module begins execution                                   * 
 *                                                                            * 
 ******************************************************************************/
bool C2BTracker::OnStartUp()
{

    CMOOSApp::OnStartUp();
    
    //default xdot for Kalman filter initialization
    double xdot_init;
    if(m_MissionReader.GetConfigurationParam("init_filter_xdot",xdot_init))
    {
      xdot_config = xdot_init;
    }
    else
    {
        MOOSTrace("2BTracker::xdot estimate not set - FAIL\n");
        
        return false;
    }

    //default ydot for Kalman filter initialization
    double ydot_init;
    if(m_MissionReader.GetConfigurationParam("init_filter_ydot",ydot_init))
    {
      ydot_config = ydot_init;
    }
    else
    {
        MOOSTrace("2BTracker::ydot estimate not set - FAIL\n");
        
        return false;
    }    

    //length of the local measurement queu (in seconds)
    double length;
    if(m_MissionReader.GetConfigurationParam("queue_length",length))
    {
      queue_length = (int) length;
    }
    else
    {
        MOOSTrace("2BTracker::queue length not set - FAIL\n");
        
        return false;
    }    

    //length of the local measurement queu (in seconds)
    double interval;
    if(m_MissionReader.GetConfigurationParam("report_interval",interval))
    {
      track_report_interval = interval;
    }
    else
    {
        MOOSTrace("2BTracker::report interval not set - FAIL\n");
        
        return false;
    }    

    return true;
}


/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::OnNewMail(MOOSMSG_LIST &NewMail)                      *
 *                                                                            *
 *  Description: This method is called by the MOOSApp superclass when new     *
 *  mail is available.                                                        * 
 *                                                                            * 
 ******************************************************************************/

bool C2BTracker::OnNewMail(MOOSMSG_LIST &NewMail)
{
  CMOOSMsg Msg;
  Measurement *New_Measurement;

  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &Msg = *p;
 
    //bearing from our vehicle
    if(Msg.m_sKey == "BEARING_STAT")
      {
	string bstat = Msg.m_sVal;
        New_Measurement = new(Measurement);
	
	//we have a good bearing measurement
	if(decode(bstat,New_Measurement))
	  {
	    //if it is from the local node, put in queue
	    if(IsLocalNode(New_Measurement->node))
	      {
		local_measurement = *New_Measurement;
		new_local_measurement = true;
	      }	
	    else
	      {
		//bearing from remote node
		remote_measurement = *New_Measurement;
		new_remote_measurement = true;
	      }
	  }		
	delete(New_Measurement);
      }
    //local node ID
    else if(Msg.m_sKey == "VEHICLE_ID")
      {
	local_node = (int) Msg.m_dfVal;
        MOOSTrace("vehicle id = %d\n",local_node);
      }
    else if(Msg.m_sKey == "NAV_X")
      {
      	double x;
	x = Msg.m_dfVal;
	set_auv_x(x);
      }
    else if(Msg.m_sKey == "NAV_Y")
      {
 	double y;
	y = Msg.m_dfVal;
	set_auv_y(y);
      }
    else if (Msg.m_sKey == "PROSECUTE_XDOT")
      {
	xdot_config = Msg.m_dfVal;
      }
    else if (Msg.m_sKey == "PROSECUTE_YDOT")
      {
	ydot_config = Msg.m_dfVal;
      }

    else
      MOOSTrace("2BTracker::Unrecognized command: [%s]\n",Msg.m_sKey.c_str());
  }
   
    return UpdateMOOSVariables(NewMail);
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::set_auv_x(double)                                     *
 *                                                                            *
 *  Description: This method stores the value of the current vehicle x        *
 *  position                                                                  *      
 *                                                                            * 
 ******************************************************************************/
void C2BTracker::set_auv_x(double xpos)
{
  auv_x = xpos;
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::set_auv_y(double)                                     *
 *                                                                            *
 *  Description: This method stores the value of the current vehicle y        *
 *  position                                                                  *      
 *                                                                            * 
 ******************************************************************************/
void C2BTracker::set_auv_y(double ypos)
{
  auv_y = ypos;
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::get_auv_x(double)                                     *
 *                                                                            *
 *  Description: This method returns the value of the current vehicle x       *
 *  position                                                                  *      
 *                                                                            * 
 ******************************************************************************/
double C2BTracker::get_auv_x()
{
  return (auv_x);
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::get_auv_x(double)                                     *
 *                                                                            *
 *  Description: This method returns the value of the current vehicle y       *
 *  position                                                                  *      
 *                                                                            * 
 ******************************************************************************/
double C2BTracker::get_auv_y()
{
  return(auv_y);
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::OnConnectToServer()                                   *
 *                                                                            *
 *  Description: This method is called by the MOOSApp superclass when a       *
 *  connection to the MOOS DB is established.                                 * 
 *                                                                            * 
 ******************************************************************************/
bool C2BTracker::OnConnectToServer()
{
    
  m_Comms.Register("BEARING_STAT",0);
  m_Comms.Register("VEHICLE_ID",0);
  m_Comms.Register("NAV_X",0);
  m_Comms.Register("NAV_Y",0);

  return true;
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::decode(string, int, Measurement*)                     *
 *                                                                            *
 *  Description: This method decodes an incoming BEARING_STAT message         *
 *                                                                            * 
 ******************************************************************************/
bool C2BTracker::decode(string status, Measurement* MStruct)
{

  string bite;
  int node,track_state;

  //decode node
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  MStruct->node=atoi(bite.c_str());

  //decode bearing status 
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  track_state = atoi(bite.c_str());

  //check if we have a true bearing yet
  if (track_state != 2)
    return false;

  //decode bearing
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  MStruct->true_bearing =  atof(bite.c_str());
  
  //decode xp
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  MStruct->auv_x = atof(bite.c_str());
    
  //decode yp
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  MStruct->auv_y = atof(bite.c_str());

  //decode beam number for this measurement
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");

  //decode sigma for this measurement
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  MStruct->sigma = atof(bite.c_str());
  
  //decode measurement time
  MOOSChomp(status,"=");
  bite = MOOSChomp(status);
  MStruct->Meas_Time = atof(bite.c_str());

  MOOSTrace("Measurement from node %d,xp=%.3f,yp=%.3f,bear=%.2f,time=%.3lf\n",MStruct->node,MStruct->auv_x,MStruct->auv_y,MStruct->true_bearing,MStruct->Meas_Time);
  //MOOSTrace("sigma = %lf\n",MStruct->sigma);
 
  return(true);
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::IsLocalNode(int)                                      *
 *                                                                            *
 *  Description: This method compares a node id with the local node id and    *
 *  returns true if they are equal, else it returns false. An error message   *
 *  is generated if the local node id has not been set.                       *
 *                                                                            * 
 ******************************************************************************/
bool C2BTracker::IsLocalNode(int node_id)
{
  if (local_node == -1)
    {
      MOOSTrace("2BTracker -> Local node ID not set!\n");
      return false;
    }

  if (node_id == local_node)
    return true;
  else 
    return false;
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::reset()                                               *
 *                                                                            *
 *  Description: This method resets the tracker for a new target              *
 *                                                                            * 
 ******************************************************************************/
void C2BTracker::reset()
{
  filter_initialized = false;
  Current_Contact.Topt.reset();
  new_local_measurement = false;
  new_remote_measurement = false;
  Current_Contact.Ctype = NO_CONTACT;
  last_track_report_time = 0.0;
}



/******************************************************************************
 *                                                                            *
 *  Method: Measurement::Measurement()                                        *
 *                                                                            *
 *  Description: The constructor for the Measurement class                    * 
 *                                                                            * 
 ******************************************************************************/
Measurement::Measurement()
{
}

/******************************************************************************
 *                                                                            *
 *  Method: Measurement::~Measurement()                                       *
 *                                                                            *
 *  Description: The destructor for the Measurement class                     * 
 *                                                                            * 
 ******************************************************************************/
Measurement::~Measurement()
{
}


/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::Solve_Track(Measurement* remote_meas)                 *
 *                                                                            *
 *  Description: This method pairs a remote node measurement with a local node*
 *  measurement made at the same time and calls a Kalman filter to estimate   *
 *  the target position and velocity components                               *
 *                                                                            * 
 ******************************************************************************/
bool C2BTracker::Solve_Track(Measurement* remote_meas)
{
  double xdot,ydot,dt,xuf,yuf;
  Measurement* local_meas; 
  
  local_meas = new(Measurement);
  //MOOSTrace("in solve track\n");
  //find local bearing to pair with the remote one
  if (Get_Local_Measurement(local_meas,remote_meas))
    {

      MOOSTrace("Found local measurement pair\n");
      //check the aperture
      double aperture = get_aperture(local_meas,remote_meas);
      m_Comms.Notify("TRACK_APERTURE",aperture);

      if (fabs(90.0-aperture) < 60.0)
	{
	  //first time through
	  if(!filter_initialized)
	    {
	      unfiltered_position(local_meas,remote_meas,&xuf,&yuf);
	      initialize_filter(xuf,yuf,xdot_config,ydot_config);
	      Current_Contact.Last_Solution_Time = local_meas->Meas_Time;
	      dt = 0;
	    }
	  else
	    dt = local_meas->Meas_Time-Current_Contact.Last_Solution_Time;

	  double aperture = get_aperture(local_meas,remote_meas);
	  MOOSTrace("aperture = %lf\n",aperture);

	  //set measurement for filter
	  double  x1 = local_meas->auv_x;
	  double  y1 = local_meas->auv_y;
	  double  bear1 = atan(tan((90.0-local_meas->true_bearing)*M_PI/180.0));
	  double  x2 = remote_meas->auv_x;
	  double  y2 = remote_meas->auv_y;
	  double  bear2 = atan(tan((90.0-remote_meas->true_bearing)*M_PI/180.0));
	  double msigma1 = local_meas->sigma;
	  double msigma2 = remote_meas->sigma;

	  //set the measurement
	  MOOSTrace("Setting the measurement, dt = %.2f\n",dt);
	  Current_Contact.Topt.set_measurement(x1,y1,bear1,x2,y2,bear2,dt,msigma1,msigma2);  

	  //calculate the unfiltered position
	  unfiltered_position(local_meas,remote_meas,&xuf,&yuf);

	  //filter the measurements
	  MOOSTrace("Calling Kalman Loop\n");
	  Current_Contact.Topt.Kalman_Loop();

	  //get position and velocity solution
	  Current_Contact.x_pos = Current_Contact.Topt.xtpos;
	  Current_Contact.y_pos = Current_Contact.Topt.ytpos;
	  Current_Contact.xdot  = Current_Contact.Topt.xtdot;
	  Current_Contact.ydot  = Current_Contact.Topt.ytdot;
     
	  if (!silent)
	    MOOSTrace("Solution: xt = %f, yt = %f, xdot = %f, ydot = %f\n",Current_Contact.x_pos,Current_Contact.y_pos,Current_Contact.xdot,Current_Contact.ydot);
            
	  Current_Contact.Ctype = RANGE_BEARING_HEADING_SPEED;
	  Current_Contact.Last_Solution_Time = local_meas->Meas_Time;

	  //retrieve the estimate uncertainty
	  double sigma_xy,sigma_yx,sigmasq_x,sigmasq_y;
	  Current_Contact.Topt.estimate_covariance(&sigma_xy,&sigma_yx,&sigmasq_x,&sigmasq_y);

	  //report the position uncertainty
	  m_Comms.Notify("TARGET_SIGMA_XY",sigma_xy);
	  m_Comms.Notify("TARGET_SIGMASQ_X",sigmasq_x);
	  m_Comms.Notify("TARGET_SIGMASQ_Y",sigmasq_y);
      
	  //MOOSTrace("Set solution time to %lf\n",local_meas->Meas_Time);
	  delete(local_meas);
	  return true;
	}
      else
	{
	  //the aperture is too small to make a reliable solution
	  MOOSTrace("DENIED!, aperture = %lf\n",aperture);
	  delete(local_meas);
	  return false;
	}
    }
  else
    {
      //couldn't pair up local and remote measurements
      MOOSTrace("No local measurement pair found\n");
      delete(local_meas);
      return false;
    }
}

double C2BTracker::get_aperture(Measurement* local,Measurement* remote)
{
  double theta1 = local->true_bearing;
  double theta2 = remote->true_bearing;

  double diff = theta1-theta2;

  if (fabs(diff) < 180.0)
    return(fabs(diff));
  else
    return((-fabs(diff))+360.0);
}


/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::initialize_filter(Measurement*,Measurement*)          *
 *                                                                            *
 *  Description: This method initializes the Kalman filter state vector       *
 *  using an unfiltered triangulation of the initial two bearings.            *
 *                                                                            * 
 ******************************************************************************/
void C2BTracker::initialize_filter(double xinit, double yinit, double xdot_init, double ydot_init)
{
  Current_Contact.Topt.init_filter(xdot_init,xinit,ydot_init,yinit);
 
  if (!silent)
    MOOSTrace("Filter initialized to x= %f, y = %f, xdot = %f, ydot = %f\n",xinit,yinit,xdot_config,ydot_config);

   filter_initialized = true;
}

/**************************************************************************************
 *                                                                                    *
 *  Method: C2BTracker::unfiltered_position(Measurement*,Measurement*,double*,double*)*
 *                                                                                    *
 *  Description: This method calculates a target position using a triangulation of    *
 *  two (near) simultaneous bearing measurements                                      *
 *                                                                                    * 
 **************************************************************************************/
void C2BTracker::unfiltered_position(Measurement* local,Measurement* remote,double* ufx,double* ufy)
{
  double theta1 = tan((local->true_bearing)*M_PI/180.0);
  double v1posx = local->auv_x;
  double v1posy = local->auv_y;

  double theta2 = tan((remote->true_bearing)*M_PI/180.0);
  double v2posx = remote->auv_x;
  double v2posy = remote->auv_y;

  double xuf = ((theta1*v2posx) - (theta2*v1posx) + ((v1posy - v2posy)*theta1*theta2))/(theta1-theta2);
  double yuf = ((theta1*v1posy) - (theta2*v2posy) + v2posx - v1posx)/(theta1-theta2);

  *ufx = xuf;
  *ufy = yuf;

  MOOSTrace("Unfiltered position = %.3f,%.3f\n",xuf,yuf);
  m_Comms.Notify("UNFILTERED_X",xuf);
  m_Comms.Notify("UNFILTERED_Y",yuf);
 }

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::Get_Local_Measurement(Measurement*)                   *
 *                                                                            *
 *  Description: This method is responsible for returning the local           *
 *  measurement that is closest in time to the received remote measurement    *
 *                                                                            * 
 ******************************************************************************/
bool C2BTracker::Get_Local_Measurement(Measurement* local,Measurement* remote)
{
  int ival;

  int qsize,gg;

  qsize = Local_Bearings.size();

  //make sure we actually have local measurements
  if(qsize == 0)
    return false;

  //check the oldest local measurement
  //return false if remote measurement is too old
  if(remote->Meas_Time < Local_Bearings[qsize-1].Meas_Time)
    return false;
  
  //if there is only one measurement, this has to be the closest
  if (qsize == 1) 
    ival = 0;

  for (gg = 0;gg < qsize; gg++)
    if(Local_Bearings[gg].Meas_Time <= remote->Meas_Time)
      break;
  //MOOSTrace("gg = %d\n",gg);
 

  if(gg == 0)
    ival = 0;
  //determine which measurement the remote measurement is closest to
  else if (fabs(remote->Meas_Time-Local_Bearings[gg].Meas_Time) < fabs(remote->Meas_Time-Local_Bearings[gg-1].Meas_Time))
    ival = gg;    
  else
    ival = gg-1;

  if(!silent)
    {
      MOOSTrace("Size of local queue = %d\n",Local_Bearings.size());
      MOOSTrace("Local Measurement index found = %d\n",ival);
      MOOSTrace("local time = %lf, remote time = %lf\n",Local_Bearings[ival].Meas_Time,remote->Meas_Time);
    }

  //copy measurement values for return
  local->true_bearing = Local_Bearings[ival].true_bearing;
  local->Meas_Time = Local_Bearings[ival].Meas_Time;
  local->auv_x = Local_Bearings[ival].auv_x;
  local->auv_y = Local_Bearings[ival].auv_y;
  local->sigma = Local_Bearings[ival].sigma;

  return true;
}

/******************************************************************************
 *                                                                            *
 *  Method: C2BTracker::update_local_queue()                                  *
 *                                                                            *
 *  Description: This method is responsible for maintaining the length of the *
 *  local measurement queue                                                   *
 *                                                                            * 
 ******************************************************************************/
void C2BTracker::update_local_queue()
{

  int qsize;

  qsize = Local_Bearings.size();

  if(qsize > 0)
    {
      //MOOSTrace("Size of queue = %d\n",qsize);
      double current_time = MOOSTime();

      double first_meas_time = Local_Bearings[qsize-1].Meas_Time;
      // MOOSTrace("first meastime-update() = %lf\n",first_meas_time);
      double diff = current_time-first_meas_time;

      //MOOSTrace("diff = %f\n",diff);
      //if the first measurement in the queue is too old
      //pop it off
      if((current_time-first_meas_time) > queue_length)
	{
	  // MOOSTrace("popped queue\n");  
	  Local_Bearings.pop_back();  
	}
    }
  //if(!silent)
  //MOOSTrace("local queue updated\n");
}

Contact::Contact()
{
  id = 1;
}

Contact::Contact(int idx)
{
  id = idx;
}

Contact::~Contact()
{
}



