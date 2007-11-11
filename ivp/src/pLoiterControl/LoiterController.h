/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LoiterController.h                                   */
/*    DATE: Feb 9th 2006                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef LOITER_CONTROLLER_HEADER
#define LOITER_CONTROLLER_HEADER

#include "MOOSLib.h"

class LoiterController : public CMOOSApp
{
public:
  LoiterController();
  virtual ~LoiterController() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  bool performMapping();
  bool postMapping();

protected:
  std::vector<std::string> generateMapping();

protected:
  double  last_mapping_time;
  double  wait_period;
  bool    mapping_request;
  bool    m_paused;
  double  m_paused_time_total;
  double  m_paused_time_start;
  int     m_post_counter;


  std::vector<std::string> vehicles;
  std::vector<std::string> regions;

  std::vector<int> curr_mapping;
};

#endif




