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

  MYSQL_RES *res_set;
  
  if (mysql_query(conn, "SHOW TABLES FROM ge_db") != 0)
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
	  
	  process_result_set(conn, res_set);
	  mysql_free_result(res_set);
	}
    }


  return true;
}

// OnConnectToServer: called when connection to the MOOSDB is
// complete
bool CpAIS2SQL::OnConnectToServer()
{

  DoRegistrations();

  // read from configuration file
  m_host_name = "18.251.3.46";      /* server host (default=localhost) */
  m_user_name = "sea";      /* username (default=login name) */
  m_password = "saline12";     /* password (default=none) */
  m_port_num = 3306;   /* port number (use built-in value) */
  m_db_name = "ge_testdb";        /* database name (default=none) */

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


void CpAIS2SQL::process_result_set (MYSQL *conn, MYSQL_RES *res_set)
{
  MYSQL_ROW row;
  unsigned int i;
  while ((row = mysql_fetch_row (res_set)) != NULL)
    {
      for (i = 0; i < mysql_num_fields (res_set); i++)
	{
	  if (i > 0)
	    fputc ('\t', stdout);
	  printf ("%s", row[i] != NULL ? row[i] : "NULL");
	}
      fputc ('\n', stdout);
    }
  if (mysql_errno (conn) != 0)
    print_error (conn, "mysql_fetch_row() failed");
  else
    printf ("%lu rows returned\n",
	    (unsigned long) mysql_num_rows (res_set));
}

