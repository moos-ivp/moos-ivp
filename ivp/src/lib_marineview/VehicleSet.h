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


  std::vector<double> getColor(const std::string& key) const;
  std::vector<std::string> getVehiNames() const;
  std::vector<std::string> getParamReport() const;

  std::string getActiveVehicle() const  {return(m_active_vehicle_name);};

  CPList     getVehiHist(const std::string&   s="active") const;   
  ObjectPose getObjectPose(const std::string& s="active") const;

  bool  hasVehiName(const std::string&) const;
  bool  isViewable(const std::string&) const;

  bool  getWeightedCenter(double& x, double&y) const;


 private:
  bool  updateVehiclePosition(const std::string& ais_report); 


 private:
  // Mapping from Vehicle Name to Vehicle Position
  std::map<std::string, ObjectPose>   m_pos_map;
  // Mapping from Vehicle Name to Vehicle Position History
  std::map<std::string, CPList>       m_hist_map;
  // Mapping from Vehicle Name to Vehicle Body Type
  std::map<std::string, std::string>  m_vbody_map;
  // Mapping from Vehicle Name to Time of last AIS report
  std::map<std::string, double>       m_ais_map;
  // Mapping from Vehicle Name to Vehicle Color
  std::map<std::string, std::vector<double> > m_vehi_color;

  std::map<std::string, std::string> m_param_report;

  std::string m_active_vehicle_name;

  int    m_history_size;
  double m_curr_time;

  // Rendering characteristics (colors, sizes etc)
  double m_vehicle_shape_scale;  
  double m_trails_length;         
  double m_trails_point_size;           
  double m_trails_gap;            
  bool   m_trails_viewable;      
  bool   m_trails_connect_viewable;
  bool   m_vehicles_viewable;
  bool   m_vehicle_names_viewable;
  
  std::vector<double> m_trails_color; 
  std::vector<double> m_active_vehicle_color;
  std::vector<double> m_inactive_vehicle_color; 
  std::vector<double> m_vehicle_name_color;
};

#endif 


