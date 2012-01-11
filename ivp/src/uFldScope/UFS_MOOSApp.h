/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: UFS_MOOSApp.h                                        */
/*   DATE: Nov 23rd 2011                                        */
/****************************************************************/

#ifndef UFS_MOOS_APP_HEADER
#define UFS_MOOS_APP_HEADER

#include <set>
#include <vector>
#include <map>
#include "MOOSLib.h"
#include "UFS_Config.h"
#include "VarDataPair.h"

class UFS_MOOSApp : public CMOOSApp
{
 public:
  UFS_MOOSApp();
  virtual ~UFS_MOOSApp() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  void handleCommand(char);

 protected:
  void registerVariables();
  bool addScope(std::string);
  bool addLayout(std::string);

  bool addPosting(std::string moosvar, std::string msgval);
  std::string getPosting(std::string moosvar, std::string keyval);

  void makeReportRaw();
  void printReport() const;
  void printHelp() const;

  bool configInLayout(const UFS_Config&) const;

  void outputRawReport() const;
  void outputRawColInfo() const;

 protected: // Config Variables
  // A representation of all the info the user desires to show
  std::vector<UFS_Config>            m_config;
  // A map from MOOS variable to the key indicating the vname
  // For example: NODE_REPORT -> NAME
  std::map<std::string, std::string> m_map_varkeys;

  std::vector<std::vector<std::string> > m_layouts;

 protected: // State Variables
  std::vector<std::string> m_postings_moosvar;  // e.g. "NODE_REPORT"
  std::vector<std::string> m_postings_keyval;   // e.g. "henry"
  std::vector<std::string> m_postings_msgval;   // e.g. "X=2,y=44,TIME..."


  // A 2D report structure - one vehicle per row, one fld per column
  std::vector<std::vector<std::string> > m_raw_report;
  
  // A Vector indicating the longest string for each column in the 
  // raw report table.
  std::vector<unsigned int> m_raw_report_maxlens;

  unsigned int m_total_reports;
  
  // Mode States
  std::string  m_refresh_mode;
  bool         m_update_requested;
  bool         m_layout_applied;
  unsigned int m_layout_index;

};
#endif 
