#include <MOOSLib.h>
#include <MOOSGenLib.h>
#include <math.h>
#include<time.h>
#include <iostream>

#include "1BTracker.h"

using namespace std;


C1BTracker::C1BTracker()
{
  tstate = 0;
  Mnum = 0;
  total_time = 0;
  solution = false;
  last_btime = 0.0;
  meas_count = 0;
  vehicleID = -1;
}

C1BTracker::~C1BTracker()
{
   
}

void C1BTracker::reset()
{
  CList.reset();
  tstate = 0;
  Mnum = 0;
  total_time = 0;
  solution = false;
  last_btime = 0.0;
  trange = 0.0;  
  nmeas = 0;
}

bool C1BTracker::Iterate()
{  
  char buff[500];
  CINFO  Temp_Contact;
  
  //if we're tracking at least one target
  //hard-wired for one target
  if (CList.get_num_contacts() > 0)
    {
      //GET THE CONTACT INFO
      CList.get_Cinfo(0,&Temp_Contact);

      if(Temp_Contact.num_meas > init_meas)
        m_Comms.Notify("CLOSE_RANGE","TRUE");
      else
      	m_Comms.Notify("CLOSE_RANGE","FALSE");

      nmeas = Temp_Contact.num_meas;   
      //if we have a full solution
      if (Temp_Contact.Ctype == RANGE_BEARING_HEADING_SPEED)
	{ 
	  double cx    = Temp_Contact.x_pos;
	  double cy    = Temp_Contact.y_pos;
	  double ch    = Temp_Contact.heading;
	  double cs    = Temp_Contact.speed;
	  double range = Temp_Contact.range;
	  double stime = Temp_Contact.Last_Meas_Time;
	  
	  
	  m_Comms.Notify("TRACKING","TRACKING");
	  m_Comms.Notify("TARGET_X",cx);
	  m_Comms.Notify("TARGET_Y",cy);
	  m_Comms.Notify("TARGET_HEADING",ch);
	  m_Comms.Notify("TARGET_SPEED",cs);	   

	  //send the TRACK_STAT message to the MOOS DB
	  sprintf(buff,"node=%d,state=%d,x=%.16f,y=%.16f,heading=%.16f,speed=%.16f,time=%.2f",vehicleID,2,cx,cy,ch,cs,stime); 	   
	  m_Comms.Notify("TRACK_STAT",buff);  
	}
      else if (Temp_Contact.Ctype == AMBIGUOUS_BEARING)
	{ 
	  //send the TRACK_STAT message to the MOOS DB
	  sprintf(buff,"node=%d,state=%d,x=%.16f,y=%.16f,mnum=%d",vehicleID,1,0.0,0.0,Temp_Contact.num_meas); 
	  m_Comms.Notify("TRACK_STAT",buff);  
	}      
    }  
  return true;
}

bool C1BTracker::OnStartUp()
{

  //CMOOSApp::OnStartUp();
        
    double th;
    
    double dfmeas;
    if(m_MissionReader.GetValue("init_meas",dfmeas))
    {
      init_meas = (int) dfmeas;
    }
    else
    {
        MOOSTrace("1BTracker:: inital measurements not set - FAIL\n");
        
        return false;
    }

    double meas;
    if(m_MissionReader.GetValue("max_measurements",meas))
    {
      //Topt.set_range(range);
      max_meas = (int)meas;
    }
    else
    {
        MOOSTrace("1BTracker:: max_measurements not set - FAIL\n");
        
        return false;
    }

    double len;
    if(m_MissionReader.GetValue("Z_length",len))
    {
      
      CList.Z_length = (int) len;
    }
    else
    {
        MOOSTrace("1BTracker:: Z_length not set - FAIL\n");
        
        return false;
    }

    double theta;
    if(m_MissionReader.GetValue("sampling_theta",theta))
    {
      
      dtheta = (double) theta;
    }
    else
    {
        MOOSTrace("1BTracker:: Z_length not set - FAIL\n");
        
        return false;
    }
   
 
    return true;
}



bool C1BTracker::OnNewMail(MOOSMSG_LIST &NewMail)
{
  CMOOSMsg Msg;
  Measurement *New_Measurement;
  int cidx;
  double sint;

  //NewMail.sort();
  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) 
    {

    CMOOSMsg &Msg = *p;
    
    if(Msg.m_sKey == "BEARING_STAT")
      {

       New_Measurement = new(Measurement);
       string bstat = Msg.m_sVal;
       decode(bstat,New_Measurement);
       	
       int bearing_state = New_Measurement->state;
       if (bearing_state != 0)
	 {
	   
	   if(meas_count > 0)
	     {
	       sint =((tan(dtheta*M_PI/180.0)*prosecute_target_range)/auv_speed);
	       MOOSTrace("Interval = %f\n",sint);  
	     }
	   else
	     sint = 0;
	   
	   double curr_time = MOOSTime();
           //we don't use every bearing measurement because they are redundant
	   if(((curr_time - last_btime) > sint))
	    {
	     MOOSTrace("New measurement\n");
	     cidx = CList.associate_bearing(New_Measurement);
	     cout << "cidx = " << cidx << endl;
	     CList.update_contact(cidx,New_Measurement);
             meas_count++;
	     if(bearing_state == 2)
	       last_btime = curr_time;
	     else
	       last_btime = 0;
	    }
	   
	 }
         else
	    {
	      int num_contacts = CList.get_num_contacts();
              MOOSTrace("num_Contacts = %d\n",num_contacts);
	      //if we have lost contact, reset
	      if (CList.get_num_contacts() > 0)
	        {
	        CList.reset();
                meas_count = 0;
	        MOOSTrace("resetting in newmail\n");
	        m_Comms.Notify("CLOSE_RANGE","FALSE");
		}
	    }
		 
	  delete New_Measurement;	  	      
      }
    else if (Msg.m_sKey == "NAV_SPEED")
      {
	auv_speed = Msg.m_dfVal;
      }
    else if (Msg.m_sKey == "PROSECUTE_XDOT")
      {
	CList.prosecute_xdot = Msg.m_dfVal;
      }
    else if (Msg.m_sKey == "PROSECUTE_YDOT")
      {
	CList.prosecute_ydot = Msg.m_dfVal;
      }
    else if (Msg.m_sKey == "PROSECUTE_TARGET_X")
      {
	CList.prosecute_target_x = Msg.m_dfVal;
      }
    else if (Msg.m_sKey == "PROSECUTE_TARGET_Y")
      {
	CList.prosecute_target_y = Msg.m_dfVal;
      }
    else if (Msg.m_sKey == "PROSECUTE_RANGE")
      {
	prosecute_target_range=Msg.m_dfVal;
	//MOOSTrace("Target Range = %f\n",prosecute_target_range);
      }
    else if (Msg.m_sKey == "VEHICLE_ID")
      {
	vehicleID = (int)Msg.m_dfVal;
      }
    else
      MOOSTrace("Unrecognized command: [%s]\n",Msg.m_sKey.c_str());
    }
  

    return UpdateMOOSVariables(NewMail);
}


bool C1BTracker::OnConnectToServer()
{
    
  
  m_Comms.Register("BEARING_STAT",0);
  m_Comms.Register("NAV_SPEED",0);
  m_Comms.Register("PROSECUTE_XDOT",0);
  m_Comms.Register("PROSECUTE_YDOT",0);
  m_Comms.Register("PROSECUTE_TARGET_X",0);
  m_Comms.Register("PROSECUTE_TARGET_Y",0);
  m_Comms.Register("PROSECUTE_RANGE",0);
  m_Comms.Register("VEHICLE_ID",0);
  
  m_Comms.Notify("CLOSE_RANGE","FALSE");
 
    return true;
}


bool C1BTracker::decode(string status, Measurement* MStruct)
{

  string bite;
  //MOOSTrace("string = %s\n",status.c_str());

  //decode bearing status 
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  //bstate = atoi(bite.c_str());
  MStruct->state = atoi(bite.c_str());

  cout << "DECODING ->state = " << MStruct->state << endl;

  //decode bearing
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  //convert to rads and normal angle for opt routine
  MStruct->true_bearing = (90.0-atof(bite.c_str()))*M_PI/180.0;

  //decode xp
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  //convert to rads and normal angle for opt routine
  MStruct->auv_x = atof(bite.c_str());
  //cout << "auv_x = " << MStruct->auv_x << endl;

  //decode yp
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  //convert to rads and normal angle for opt routine
  MStruct->auv_y = atof(bite.c_str());
  //cout << "auv_y = " << MStruct->auv_y << endl;

  //decode beam number for this measurement
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  MStruct->beam = atoi(bite.c_str());

  //decode sigma for this measurement
  MOOSChomp(status,"=");
  bite = MOOSChomp(status,",");
  

  //decode measurement time
  MOOSChomp(status,"=");
  bite = MOOSChomp(status);
  MStruct->Meas_Time = atof(bite.c_str());
  //MOOSTrace("meas time = %f\n",MStruct->Meas_Time); 
  return(true);
}


Measurement::Measurement()
{
}

Measurement::~Measurement()
{
}

Contact_List::Contact_List()
{
  num_contacts = 0;
  skipped = 0; 
 }

Contact_List::~Contact_List()
{
}

void Contact_List::set_num_contacts(int num)
{
  num_contacts = num;
}


Contact::Contact()
{
  num_meas = 0;
  id = 1;
  total_time = 0.0;
}

Contact::Contact(int idx)
{
  id = idx;
  num_meas = 0;
  total_time = 0.0;
}

Contact::~Contact()
{
}

void Contact_List::reset()
{
  MOOSTrace("in CList reset()\n");
  int num_c = get_num_contacts();

  for (int jj = 0; jj < num_c;jj++)
      Contacts[jj].Topt.reset(); 

  Contacts.pop_back();
  skipped = 0;

  num_contacts = 0;
}

int Contact_List::associate_bearing(Measurement* Meas)
{

  Contact* New_Contact;
  double new_bearing;
  int beamno;
  int cid;

  new_bearing = Meas->true_bearing;
  beamno = Meas->beam;
 

  //right now hard-wired for a single contact
  //in the future add an algorithm to track multiple
  //contacts and associate a contact with each bearing
  

  //add a contact
  if (num_contacts == 0)
    {
      cid = 0;
       New_Contact = new Contact(0);
      //New_Contact = new(Contact);
      //add a contact to our list
      Contacts.push_back(*New_Contact);
      num_contacts++;
      //delete(New_Contact);
      //Contacts[cid].Topt.set_range(range_guess);  
      Contacts[cid].Topt.set_Z_length(Z_length);
      Contacts[cid].Ctype = AMBIGUOUS_BEARING;
      MOOSTrace("Added contact!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %d\n",cid);

    }
  
  cid = 0;
 
  return (0);
}

int Contact_List::get_num_contacts()
{
  return num_contacts;
}

void Contact_List::update_contact(int cid, Measurement* Meas)
{
  int id_num;

  Measurement* New_measurement; 

  New_measurement = Meas;

  id_num = cid;
  MOOSTrace("updating contact, state = %d, skipped = %d\n",Meas->state,skipped);
  //tracking bearing
  if((Meas->state == 2))
    {
      if (skipped >=1)
	{
	  Add_Measurement(id_num,New_measurement); 
	  SolveTrack(cid,New_measurement->auv_x,New_measurement->auv_y);
	}
      else
	skipped++;
    }
  else
    {  
       
      Contacts[cid].Last_Meas_Time = New_measurement->Meas_Time;
    }
}

bool Contact_List::Add_Measurement(int cid,Measurement* Meas)
{
  double dt,x,y;
  bool silent = false;
  
  if (Contacts[cid].num_meas == 0)
    {
      Contacts[cid].start_time = Meas->Meas_Time;
      dt = 0.0;
    }
  else if (Contacts[cid].num_meas > 0)  
    dt = Meas->Meas_Time - Contacts[cid].start_time;
 
  Contacts[cid].total_time = dt;

  cout << "total time = " <<  Contacts[cid].total_time << endl;
  x = Meas->auv_x;
  y = Meas->auv_y;

  if(!silent)
    MOOSTrace("Setting Measurement #:%d: x = %f, y = %f, bear = %f, dt = %f\n",Contacts[cid].num_meas+1,x,y,Meas->true_bearing,dt);

  
  Contacts[cid].Topt.set_measurement(x,y, Meas->true_bearing,dt);

  Contacts[cid].Last_Meas_Time = Meas->Meas_Time;
  Contacts[cid].num_meas += 1;

  return true;
}


bool Contact_List::SolveTrack(int track_id, double xnow, double ynow)
{
  status stat;
  bool silent = false;
  double xzero,yzero,xdot,ydot,x_pos_check,y_pos_check,solver_range;

  //if we've taken enough measurements
  //make this a parameter
  if(Contacts[track_id].num_meas >= 55)
    {
      //initialize tracker algorithminitial guess based on prosecute information     
      Contacts[track_id].Topt.set_guess(prosecute_xdot,prosecute_ydot,prosecute_target_x,prosecute_target_y);

      //try to get a solution
      stat = Contacts[track_id].Topt.get_track();

      //if the tracker algorithm converged
      if (stat == success)
	{
	  //this is the solution the tracker algorithm found
	  xzero = Contacts[track_id].Topt.xzero;
	  yzero = Contacts[track_id].Topt.yzero;
	  xdot  = Contacts[track_id].Topt.xdot;
	  ydot  = Contacts[track_id].Topt.ydot;
	  
	  //based on the solution what is the target range?
	  x_pos_check = xzero + Contacts[track_id].total_time*xdot;
	  y_pos_check = yzero + Contacts[track_id].total_time*ydot;
	  solver_range = sqrt(pow((x_pos_check-xnow),2) + pow((y_pos_check-ynow),2));

	  //is the solution within the detection range?
	  //make this a parameter
	  if(solver_range < 5000.0)
	  {
	    //position
	    Contacts[track_id].x_pos = xzero + Contacts[track_id].total_time*xdot;
	    Contacts[track_id].y_pos = yzero + Contacts[track_id].total_time*ydot;
	    //bearing to target
	    Contacts[track_id].bearing = 90.0-atan2((Contacts[track_id].y_pos-ynow),(Contacts[track_id].x_pos-xnow))*180.0/M_PI;
	    //speed of target
	    Contacts[track_id].speed = sqrt(xdot*xdot + ydot*ydot);
	    //target heading
	    Contacts[track_id].heading = 90.0-atan2(ydot,xdot)*180.0/M_PI;
	  
	    //indicate we have a full solution
	    Contacts[track_id].Ctype = RANGE_BEARING_HEADING_SPEED;
	  
	    if (!silent)
	      MOOSTrace("Good solution: x = %f, y = %f, heading = %f, speed = %f\n",Contacts[track_id].x_pos,Contacts[track_id].y_pos,Contacts[track_id].heading,Contacts[track_id].speed);
	    //target range
	    Contacts[track_id].range = sqrt(pow((Contacts[track_id].x_pos-xnow),2) + pow((Contacts[track_id].y_pos-ynow),2));
          }
	}
      else
	{
	  if (!silent)
	    MOOSTrace("No Solution\n");
	}  
    }

  return true;
}


void Contact::set_range_guess(double range)
{
  range_guess = range;
}

void Contact_List::get_Cinfo(int cid, CINFO *this_contact)
{
  time_t the_time;
  struct tm *tm_ptr;
  double Mtime;
  
  this_contact->id = Contacts[cid].id;
  this_contact->x_pos = Contacts[cid].x_pos;
  this_contact->y_pos = Contacts[cid].y_pos;
  this_contact->num_meas = Contacts[cid].num_meas;
  this_contact->range = Contacts[cid].range;
  this_contact->bearing = Contacts[cid].bearing;
  this_contact->heading = Contacts[cid].heading;
  this_contact->speed = Contacts[cid].speed;
  
  Mtime = Contacts[cid].Last_Meas_Time;
  the_time = (time_t) Mtime;
  tm_ptr = gmtime(&the_time);
  
  this_contact->hours   = tm_ptr->tm_hour; 
  this_contact->minutes = tm_ptr->tm_min;
  this_contact->seconds = tm_ptr->tm_sec;
  this_contact->Ctype   = Contacts[cid].Ctype;
}


CINFO::CINFO()
{
}

CINFO::~CINFO()
{
}


