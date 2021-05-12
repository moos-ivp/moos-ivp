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
  void updateGridGuess1();
  void updateGridGuess2();
  void postGridTruth(bool active=true);
  void postGridGuess1(bool active=true);
  void postGridGuess2(bool active=true);
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
  unsigned int m_guess_params_set1;
  unsigned int m_guess_params_set2;

  CFrontSim    m_frontsim_truth;
  CFrontSim    m_frontsim_guess1;
  CFrontSim    m_frontsim_guess2;
  
  // Messages to be displayed to the terminal
  std::map<std::string, int> m_map_memos;
  
  XYConvexGrid m_grid_truth;
  XYConvexGrid m_grid_guess1;
  XYConvexGrid m_grid_guess2;

  bool         m_grid_truth_show;
  bool         m_grid_guess1_show;
  bool         m_grid_guess2_show;

  bool         m_grid_truth_active;
  bool         m_grid_guess1_active;
  bool         m_grid_guess2_active;

  std::string m_vname1;
  std::string m_vname2;
  
  std::vector<double>  m_grid_x;
  std::vector<double>  m_grid_y;

};

#endif 
