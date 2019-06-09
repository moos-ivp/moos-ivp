/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: FrontGridRender.cpp                                  */
/*    DATE: Arp 13th, 2012                                       */
/*****************************************************************/

#ifndef FRONT_GRID_RENDER_HEADER
#define FRONT_GRID_RENDER_HEADER

#include <map>
//#include "MOOSLib.h"
#include "MOOS/libMOOS/MOOSLib.h"
#include "CFrontSim.h"
#include "XYConvexGrid.h"
#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"

class FrontGridRender : public AppCastingMOOSApp
{
 public:
  FrontGridRender();
  ~FrontGridRender() {}

 protected:
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void RegisterVariables();
  
  bool handleCTDParameters(std::string, std::string s="truth");
  void updateGridTruth();
  void updateGridGuess();
  void postGridTruth(bool active=true);
  void postGridGuess(bool active=true);
  void printReport();
  bool buildReport();
 private: // Configuration variables
  //double m_term_report_interval;

 private: // State variables
  double       m_curr_time;
  unsigned int m_iterations;
  double       m_timewarp;
  double       m_last_grid_post_time;
  double       m_grid_post_interval;
  double       m_last_report_time;
  double       m_report_interval;

  unsigned int m_true_params_set;
  unsigned int m_guess_params_set;

  CFrontSim    m_frontsim_truth;
  CFrontSim    m_frontsim_guess;
  
  // Messages to be displayed to the terminal
  std::map<std::string, int> m_map_memos;
  
  XYConvexGrid m_grid_truth;
  XYConvexGrid m_grid_guess;

  bool         m_grid_truth_show;
  bool         m_grid_guess_show;
  bool         m_grid_truth_active;
  bool         m_grid_guess_active;

  std::vector<double>  m_grid_x;
  std::vector<double>  m_grid_y;

};

#endif 
