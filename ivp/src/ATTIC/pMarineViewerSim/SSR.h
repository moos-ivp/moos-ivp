/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SSR.H                                                */
/*    DATE: Mar 6th 2005 Sunday Brueggers (Lao-lao lai le)       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef SSR_HEADER
#define SSR_HEADER

#include <iostream>
#include <vector>
#include <string>

class SSR {
public:

  SSR() {};
  ~SSR() {};
  
  int  getCount() {return(h_nam.size());};

  std::string get_h_nam(int i) {return(h_nam[i]);};
  std::string get_h_dom(int i) {return(h_dom[i]);};
  std::string get_h_bhv(int i) {return(h_bhv[i]);};
  std::string get_h_lon(int i) {return(h_lon[i]);};
  std::string get_h_lat(int i) {return(h_lat[i]);};
  std::string get_h_crs(int i) {return(h_crs[i]);};
  std::string get_h_spd(int i) {return(h_spd[i]);};
  std::string get_h_pid(int i) {return(h_pid[i]);};
  std::string get_h_ver(int i) {return(h_ver[i]);};

public:
  std::vector <std::string>  h_nam;  // Unique identifier
  std::vector <std::string>  h_dom;  // IvPDomain
  std::vector <std::string>  h_bhv;  // Filename w/ behaviors
  std::vector <std::string>  h_lon;  // In meters
  std::vector <std::string>  h_lat;  // In meters
  std::vector <std::string>  h_crs;  // 0-359
  std::vector <std::string>  h_spd;  // meters/sec
  std::vector <std::string>  h_pid;  // PID Control params
  std::vector <std::string>  h_ver;  // Helm Quiet Flag
};
#endif





