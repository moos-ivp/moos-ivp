/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Alog2Range.h                                         */
/*    DATE: Feb 17th, 2011                                       */
/*****************************************************************/

#ifndef ALOG_TO_RANGE_HEADER
#define ALOG_TO_RANGE_HEADER

#include <string>
#include "LogPlot.h"

class Alog2Range
{
 public:
  Alog2Range();
  ~Alog2Range() {};

  bool handle(const std::string&);
  bool setReportType(std::string);
  bool setDelim(std::string);
  void setHeader(bool v)   {m_header=v;};
  void setPostNav(bool v)  {m_post_nav=v;};
  void setVerbose(bool v)  {m_verbose=v;};

  void printReport();
  
 protected:
  std::string  getVName(std::string);
  std::string  getRange(std::string);
  std::string  getID(std::string);
  std::string  getUTC(std::string);
  std::string  getRangeFromTimeID(std::string time, std::string id);

  void    determineMaxLens();
  void    determineUniqueBeaconIDs();
  void    determineUniqueTimeStamps();
  void    printReportFlat();
  void    printReportTable();
  
 protected: // Configuration Variables
  std::string  m_report_type;
  std::string  m_delim;
  bool         m_header;
  bool         m_post_nav;
  bool         m_verbose;

 protected: // State Variables
  std::vector<std::string> m_range_reports;
  std::vector<std::string> m_time_stamps;
  std::vector<std::string> m_unique_beacon_ids;
  std::vector<std::string> m_unique_time_stamps;

  LogPlot  m_lp_navx;
  LogPlot  m_lp_navy;
  LogPlot  m_lp_navh;

  unsigned int m_longest_moos_time;
  unsigned int m_longest_utc_time;
  unsigned int m_longest_vname;
  unsigned int m_longest_id;
  unsigned int m_longest_range;
};

#endif
