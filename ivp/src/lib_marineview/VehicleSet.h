/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VehicleSet.h                                         */
/*    DATE: Aug 12th 2008                                        */
/*          (Broken out from the Viewer class(es)                */
/*****************************************************************/

#ifndef MARINE_VEHICLE_DATASET_HEADER
#define MARINE_VEHICLE_DATASET_HEADER

#include <vector>
#include <string>
#include <map>
#include "ObjectPose.h"
#include "ColoredPoint.h"
#include "ColorPack.h"

class VehicleSet
{
 public:
  VehicleSet();
  ~VehicleSet() {};
    
  bool  setParam(std::string param, std::string value="");
  bool  setParam(std::string param, double value);

  //------ General Purpose Information retrieval --------------
  bool  getDoubleInfo(const std::string& vname, 
		      const std::string& info_type, 
		      double& result) const;
  
  bool  getStringInfo(const std::string& vname, 
		      const std::string& info_type, 
		      std::string& result) const;

  //------ Simplified Information retrieval --------------
  std::string getStringInfo(const std::string& vname, 
			    const std::string& info_type) const;
  double      getDoubleInfo(const std::string& vname, 
			    const std::string& info_type) const;

  //------ Very Simplified Information retrieval --------------
  std::string getStringInfo(const std::string& info_type) const;
  double      getDoubleInfo(const std::string& info_type) const;

  std::vector<std::string> getVehiNames() const;

  std::string getActiveVehicle() const  {return(m_vehicles_active_name);};
  std::string getCenterVehicle() const  {return(m_vehicles_center_name);};

  CPList     getVehiHist(const std::string&   s="active") const;   
  ObjectPose getObjectPose(const std::string& s="active") const;

  bool  hasVehiName(const std::string&) const;
  bool  getWeightedCenter(double& x, double&y) const;

 private:
  bool  updateVehiclePosition(const std::string& ais_report); 

 private:
  // Mapping from Vehicle Name to Vehicle Position
  std::map<std::string, ObjectPose>   m_pos_map;
  // Mapping from Vehicle Name to Vehicle Length
  std::map<std::string, double>       m_vlen_map;
  // Mapping from Vehicle Name to Vehicle Position History
  std::map<std::string, CPList>       m_hist_map;
  // Mapping from Vehicle Name to Vehicle Body Type
  std::map<std::string, std::string>  m_vbody_map;
  // Mapping from Vehicle Name to Vehicle Helm Mode (in AIS_REPORT)
  std::map<std::string, std::string>  m_vmode_map;
  // Mapping from Vehicle Name to Time of last AIS report
  std::map<std::string, double>       m_ais_map;

  std::string m_vehicles_active_name;
  std::string m_vehicles_center_name;

  int    m_history_size;
  double m_curr_time;

  std::vector<std::string> m_node_report_vars;
};

#endif 


