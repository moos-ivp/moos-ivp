/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ProcessWatch.h                                       */
/*    DATE: May 27th 2007 (MINUS-07)                             */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef PROCESS_WATCH_HEADER
#define PROCESS_WATCH_HEADER

#include <string>
#include <vector>
#include "MOOSLib.h"

class ProcessWatch : public CMOOSApp
{
public:
  ProcessWatch() {};
  virtual ~ProcessWatch() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

protected:
  void addToWatchList(std::string);
  bool isAlive(std::string, bool=false);

protected:
  std::string               m_db_clients;
  std::string               m_awol_procs;

  std::vector<std::string>  m_watchlist;
  std::vector<bool>         m_isalive;
  std::vector<bool>         m_prefix_match;
};

#endif 

