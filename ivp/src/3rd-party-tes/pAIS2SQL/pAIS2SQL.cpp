// t. schneider tes@mit.edu 11.29.07
// laboratory for autonomous marine sensing systems
// massachusetts institute of technology 
// 
// this is pAIS2SQL.cpp 
//
// see the readme file within this directory for information
// pertaining to usage and purpose of this script.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this software.  If not, see <http://www.gnu.org/licenses/>.

#include "pAIS2SQL.h"

// define the blackout time for new results to be published to the mysql db
// (in seconds per vehicle)
#define BLACKOUT_TIME 1

using namespace std;

CpAIS2SQL::CpAIS2SQL()
{
  
}
CpAIS2SQL::~CpAIS2SQL()
{
  /* disconnect from server */
  mysql_close (conn);
}

// OnNewMail: called when new mail (previously registered for)
// has arrived.
bool CpAIS2SQL::OnNewMail(MOOSMSG_LIST &NewMail)
{

  MOOSMSG_LIST::reverse_iterator p;
  
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &msg = *p;
    
    string key = msg.m_sKey;
    string sdata = msg.m_sVal;

    // got a new AIS_REPORT
    if(MOOSStrCmp(key, "AIS_REPORT"))
      {

	bool ok = true;
	
	// adapted from pTransponderAIS
	string vname;         bool vname_set   = false;
	string vtype;         bool vtype_set   = false;
	string utc_time;      bool utc_time_set = false;
	string nav_x_val;     bool nav_x_set   = false;
	string nav_y_val;     bool nav_y_set   = false;
	string nav_lat_val;   bool nav_lat_set = false;
	string nav_long_val;  bool nav_long_set = false;
	string nav_spd_val;   bool nav_spd_set = false;
	string nav_hdg_val;   bool nav_hdg_set = false;
	string nav_dep_val;   bool nav_dep_set = false;

	vector<string> svector = parseString(sdata, ',');
	int vsize = svector.size();
	for(int i=0; i<vsize; i++) {
	  vector<string> svector2 = parseString(svector[i], '=');
	  if(svector2.size() == 2) {
	    string left = tolower(stripBlankEnds(svector2[0]));
	    string right = stripBlankEnds(svector2[1]);
	    
	    if(left=="name") {
	      vname = right;
	      vname_set = true;
	    }
	    if(left == "type") {
	      vtype = right;
	      vtype_set = true;
	    }
	    if(left == "utc_time") {
	      utc_time = right;
	      utc_time_set = true;
	    }
	    if(left == "x") {
	      nav_x_val = right;
	      nav_x_set = true;
	    }
	    if(left == "y") {
	      nav_y_val = right;
	      nav_y_set = true;
	    }
	    if(left == "lat") {
	      nav_lat_val = right; 
	      nav_lat_set = true;
	    }
	    if(left == "lon") {
	      nav_long_val = right;
	      nav_long_set = true;
	    }
	    if(left == "spd") {
	      nav_spd_val = right;
	      nav_spd_set = true;
	    }
	    if(left == "hdg") {
	      nav_hdg_val = right;
	      nav_hdg_set = true;
	    }
	    if(left == "depth") {
	      nav_dep_val = right;
	      nav_dep_set = true;
	    }
	  }
	}
	
	// determine vehicle id or add new vehicle
	string vid;
	
	MYSQL_RES *res_set;
	
	string query_veh = "SELECT vehicle_id FROM ge_vehicle WHERE ";
	query_veh += "lower(vehicle_name) = '" + tolower(vname);
	query_veh += "' AND lower(vehicle_type) = '" + tolower(vtype);
	query_veh += "'";
	
	MOOSTrace(query_veh);
	MOOSTrace("\n");
	
	if (mysql_query(conn, query_veh.c_str()) != 0)
	  {
	    print_error (conn, "mysql_query() failed");
	  }
	else
	  {
	    res_set = mysql_store_result(conn);
	    
	    if(res_set == NULL)
	      print_error(conn, "mysql_store_result() failed");
	    else
	      {
		
		if(mysql_num_rows(res_set) == 0)
		  {
		    //add new id
		    string query_veh_insert = "INSERT INTO ge_vehicle (vehicle_type, vehicle_name) ";
		    query_veh_insert += "VALUES ('" + tolower(vtype);
		    query_veh_insert += "', '" + tolower(vname);
		    query_veh_insert += "')";
		    
		    MOOSTrace(query_veh_insert);
		    MOOSTrace("\n");
		    
		    
		    if (mysql_query(conn, query_veh_insert.c_str()) != 0)
		      {
			print_error (conn, "insert failed");
			return false;
		      }	    
		    else
		      {
			vid = intToString(mysql_insert_id(conn));
		      }
		    
		  }
		else
		  {
		    //get veh id
		    MYSQL_ROW row;
		    row = mysql_fetch_row(res_set);
		    vid = row[0];
		  }
		
		MOOSTrace("pAIS2SQL: vehicle id is ");
		MOOSTrace(vid);
		MOOSTrace(".\n");
		
		mysql_free_result(res_set);
	      }
	  }
	

	// check the blackout time on this vehicle. if not enough time elapsed
	// return to stop wasting our time :) since we aren't going to publish
	// to the mysql db
	if (!check_blackout(vid, atof(utc_time.c_str())))
	  return true;

	
	string nlat;
	string nlong;
	
	if(!nav_lat_set || !nav_long_set)
	  {
	    if(!nav_x_set || !nav_y_set || !m_geo_ok)
	      ok = false;
	    else
	      {
		double dnlat;
		double dnlong;
		//get lat/long from geodesy		
		if(!m_geodesy.LocalGrid2LatLong(atof(nav_x_val.c_str()), atof(nav_y_val.c_str()), dnlat, dnlong))
		  return MOOSFail("pAIS2SQL: Geodesy conversion failed\n");
		else
		  {
		    nlat = doubleToString(dnlat);
		    nlong = doubleToString(dnlong);
		  }
	      }
	  }
	else
	  {
	    nlat = nav_lat_val;
	    nlong = nav_long_val;
	  }

	if(ok)
	  {
	    
	    string query_insert = \
	      "INSERT INTO ge_data (data_vehicleid, data_cruiseid, data_time, data_lat, data_long, data_heading, data_speed, data_depth) ";
	    query_insert += "VALUES ('"+vid;
	    query_insert += "', '"+m_cruise_id;
	    query_insert += "', '"+utc_time;
	    query_insert += "', '"+nlat;
	    query_insert += "', '"+nlong;
	    query_insert += "', '"+nav_hdg_val;
	    query_insert += "', '"+nav_spd_val;
	    query_insert += "', '"+nav_dep_val;
	    query_insert += "')";
	    
	    MOOSTrace(query_insert);
	    MOOSTrace("\n");
	    
	    if (mysql_query(conn, query_insert.c_str()) != 0)
	      {
		print_error (conn, "insert failed");
	      }
	  }
	else
	  {
	    MOOSTrace("pAIS2SQL: Warning: AIS_REPORT received lacking information.\n");
	  }
      }
  }
 

  return true;
}

// OnConnectToServer: called when connection to the MOOSDB is
// complete
bool CpAIS2SQL::OnConnectToServer()
{

  DoRegistrations();


  m_geo_ok = false;
  // look for latitude, longitude global variables
  double latOrigin, longOrigin;
  if (!m_MissionReader.GetValue("LatOrigin", latOrigin))
    {
      MOOSTrace("pAIS2SQL: Warning, no LatOrigin set in .moos file. Vehicle reports must included latitude information.\n");
    }
  else if (!m_MissionReader.GetValue("LongOrigin", longOrigin))
    {
      MOOSTrace("pAIS2SQL: Warning, no LatOrigin set in .moos file. Vehicle reports must included longitude information.\n");
    }
  // initialize m_geodesy
  else if (!m_geodesy.Initialise(latOrigin, longOrigin))
    {
      MOOSTrace("pAIS2SQL Geodesy init failed.\n");
    }
  else
    {
      m_geo_ok = true;
    }
  

      

  string cruise_name, cruise_location, cruise_start, cruise_end;


  m_port_num = -1;

  //parse ye ol' configuration file
  
  list<string> sParams;
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    
    list<string>::iterator p;
    for(p = sParams.begin(); p!=sParams.end(); p++) {
      
      string sLine    = *p;
      string sVarName = stripBlankEnds(MOOSChomp(sLine, "="));
      
      if(MOOSStrCmp(sVarName, "mysql_host"))
	m_host_name = stripBlankEnds(sLine);
      
      if(MOOSStrCmp(sVarName, "mysql_user"))
	m_user_name = stripBlankEnds(sLine);
      
      if(MOOSStrCmp(sVarName, "mysql_password"))
	m_password = stripBlankEnds(sLine);
      
      if(MOOSStrCmp(sVarName, "mysql_port"))
      	m_port_num = atoi(stripBlankEnds(sLine).c_str());
      
      if(MOOSStrCmp(sVarName, "mysql_db_name"))
	m_db_name = stripBlankEnds(sLine);
      
      if(MOOSStrCmp(sVarName, "cruise_start"))
	cruise_start = stripBlankEnds(sLine);
      
      if(MOOSStrCmp(sVarName, "cruise_end"))
	cruise_end = stripBlankEnds(sLine);
      
    }
  }

  m_MissionReader.GetValue("cruise_location", cruise_location);
  m_MissionReader.GetValue("cruise_name", cruise_name);
  
  if (m_host_name.size() == 0)
    return MOOSFail("pAIS2SQL: mandatory parameter 'mysql_host' not provided\n");
  
  if (m_user_name.size() == 0)
    return MOOSFail("pAIS2SQL: mandatory parameter 'mysql_user' not provided\n");
  
  if (m_password.size() == 0)
    return MOOSFail("pAIS2SQL: mandatory parameter 'mysql_password' not provided\n");
  
  if (m_port_num == -1)
    {
      MOOSTrace("pAIS2SQL: warning: no mysql_port specified, using default (3306)\n");
      m_port_num = 3306;
    }
  
  if (m_db_name.size() == 0)
    return MOOSFail("pAIS2SQL: mandatory parameter 'mysql_db_name' not provided\n");
  
  if (cruise_name.size() == 0)
    return MOOSFail("pAIS2SQL: mandatory parameter 'cruise_name' not provided\n");
  
  if (cruise_location.size() == 0)
    return MOOSFail("pAIS2SQL: mandatory parameter 'cruise_location' not provided\n");
  
  if (cruise_start.size() == 0)
    return MOOSFail("pAIS2SQL: mandatory parameter 'cruise_start' not provided\n");
  
  if (cruise_end.size() == 0)
    return MOOSFail("pAIS2SQL: mandatory parameter 'cruise_end' not provided\n");



  /* initialize connection handler */
  conn = mysql_init (NULL);
  if (conn == NULL)
    {
      fprintf (stderr, "pAIS2SQL: mysql_init() failed (probably out of memory)\n");
      exit (1);
    }
  /* connect to mysql server */
  if (mysql_real_connect (conn, m_host_name.c_str(), m_user_name.c_str(), m_password.c_str(),
			  m_db_name.c_str(), m_port_num, "", 0) == NULL)
    {
      fprintf (stderr, "pAIS2SQL: mysql_real_connect() failed\n");
      mysql_close (conn);
      exit (1);
    }

  /* select db */
  if (mysql_select_db(conn, m_db_name.c_str()) != 0)
    {
      fprintf (stderr, "pAIS2SQL: could not select database\n");
    }


  // determine cruise id or add new cruise
 
  MYSQL_RES *res_set;
  

  string query_cruise = "SELECT cruise_id FROM ge_cruise WHERE ";
  query_cruise += "lower(cruise_name) = '" + tolower(cruise_name);
  query_cruise += "' AND lower(cruise_location) = '" + tolower(cruise_location);
  query_cruise += "' AND FROM_UNIXTIME(cruise_start, '%Y.%m.%d') = '" + cruise_start;
  query_cruise += "' AND FROM_UNIXTIME(cruise_end, '%Y.%m.%d') = '" + cruise_end;
  query_cruise += "'";

  MOOSTrace(query_cruise);
  MOOSTrace("\n");

  if (mysql_query(conn, query_cruise.c_str()) != 0)
    {
      print_error (conn, "mysql_query() failed");
    }
  else
    {
      res_set = mysql_store_result(conn);
      
      if(res_set == NULL)
	print_error(conn, "mysql_store_result() failed");
      else
	{

	  if(mysql_num_rows(res_set) == 0)
	    {
	      //add new id
	      string query_cruise_insert = "INSERT INTO ge_cruise (cruise_name, cruise_location, cruise_start, cruise_end) ";
	      query_cruise_insert += "VALUES ('" + cruise_name;
	      query_cruise_insert += "', '" + cruise_location;
	      query_cruise_insert += "', UNIX_TIMESTAMP('" + cruise_start;
	      query_cruise_insert += "'), UNIX_TIMESTAMP('" + cruise_end;
	      query_cruise_insert += "'))";

	      MOOSTrace(query_cruise_insert);
	      MOOSTrace("\n");
	      
	      
	    if (mysql_query(conn, query_cruise_insert.c_str()) != 0)
	      {
		print_error (conn, "insert failed");
		return false;
	      }	    
	    else
	      {
		m_cruise_id = intToString(mysql_insert_id(conn));
	      }

	    }
	  else
	    {
	      //get cruise id
	      MYSQL_ROW row;
	      row = mysql_fetch_row(res_set);
	      m_cruise_id = row[0];
	    }
	  
	  MOOSTrace("pAIS2SQL: cruise_id is ");
	  MOOSTrace(m_cruise_id);
	  MOOSTrace(".\n");

	  mysql_free_result(res_set);
	}
    }


  return true;
}

// Iterate: called AppTick times per second
bool CpAIS2SQL::Iterate()
{ 
  return true;
}

// OnStartUp: called when program is run
bool CpAIS2SQL::OnStartUp()
{      
  DoRegistrations();

  return true;
}


// DoRegistrations: register for MOOS variables we 
// want to hear about (receive mail for)
void CpAIS2SQL::DoRegistrations()
{
  m_Comms.Register("AIS_REPORT", 0);

  return;
}


void CpAIS2SQL::print_error (MYSQL *conn, char *message)
{
  fprintf (stderr, "%s\n", message);
  if (conn != NULL)
    {
#if MYSQL_VERSION_ID >= 40101
      fprintf (stderr, "pAIS2SQL: Error %u (%s): %s\n",
	       mysql_errno (conn), mysql_sqlstate(conn), mysql_error (conn));
#else
      fprintf (stderr, "pAIS2SQL: Error %u: %s\n",
	       mysql_errno (conn), mysql_error (conn));
#endif
    }
}



// check_blackout: check to see if a report for the vehicle has
// been published to the mysql db in within the BLACKOUT_TIME interval
// zero means OK to publish to mysql db, nonzero means not OK
// this routine also updates the m_known_time and adds new vehicles as needed
bool CpAIS2SQL::check_blackout(string vid, double report_time)
{
  int i;
  
  for(i=0; i<m_known_vid.size(); i++)
    {
      if(m_known_vid[i] == vid)
	{
	  
	  if (m_known_time[i]+BLACKOUT_TIME <= report_time)
	    {
	      //do it
	      m_known_time[i] = report_time;
	      return true;
	    }
	  else
	    {
	      //not enough time elapsed
	      return false;
	    }
	}
    }
  
  m_known_vid.push_back(vid);
  m_known_time.push_back(report_time);
  return true;
}
