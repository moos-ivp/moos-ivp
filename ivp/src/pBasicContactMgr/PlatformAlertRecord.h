/************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: PlatformAlterRecord.h                           */
/*    DATE: Apr 6th 2010                                    */
/************************************************************/

#ifndef PLATFORM_ALERT_RECORD_HEADER
#define PLATFORM_ALERT_RECORD_HEADER

#include <map>
#include <string>

class PlatformAlertRecord
{
 public:
  PlatformAlertRecord();
  ~PlatformAlertRecord() {};

  void print();

  void addAlertID(std::string str);
  void addVehicle(std::string str);

  void setValue(std::string vehicle, std::string alertid, bool);
  bool getValue(std::string vehicle, std::string alertid);

  bool containsVehicle(std::string vehicle);
  bool containsAlertID(std::string alertid);

  std::string getAlertedGroup(bool alerted);

  bool alertsPending();


 protected: 
  // map<vehicle, map<alertid, bool>>
  std::map<std::string, std::map<std::string, bool> > m_par;

  std::map<std::string, bool> m_temp_row;

  unsigned int m_rows;
  unsigned int m_cols;
};

#endif 
