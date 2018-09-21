/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VehicleSet.h                                         */
/*    DATE: Aug 12th 2008                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef MARINE_VEHICLE_DATASET_HEADER
#define MARINE_VEHICLE_DATASET_HEADER

#include <vector>
#include <string>
#include <map>
#include "NodeRecord.h"
#include "ColoredPoint.h"
#include "ColorPack.h"
#include "BearingLine.h"

class VehicleSet
{
 public:
  VehicleSet();
  ~VehicleSet() {}
    
  bool   setParam(std::string param, std::string value="");
  bool   setParam(std::string param, double value);

  bool   handleNodeReport(double, std::string report_str, std::string& whynot);
  bool   handleNodeReport(std::string report_str, std::string& whynot);

  void   clear(const std::string& vname="");
  double getXMin() const {return(m_xmin);}
  double getXMax() const {return(m_xmax);}
  double getYMin() const {return(m_ymin);}
  double getYMax() const {return(m_ymax);}

  NodeRecord getNodeRecord(const std::string& vname) const;

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

  std::string getActiveVehicle() const  {return(m_vehicles_active_name);}
  std::string getCenterVehicle() const  {return(m_vehicles_center_name);}

  std::string getClosestVehicle(double x, double y) const;
  
  CPList      getVehiHist(const std::string&   s="active") const;   
  BearingLine getBearingLine(const std::string& s="active") const;

  bool  hasVehiName(const std::string&) const;
  bool  getWeightedCenter(double& x, double&y) const;

  void  print() const;

 private:
  bool  updateVehicleBearingLine(const std::string& bearing_line); 

 private:
  // Mapping from Vehicle Name to Vehicle Position
  std::map<std::string, NodeRecord>   m_rec_map;
  // Mapping from Vehicle Name to Local Receive time
  std::map<std::string, double>       m_map_node_local_time;
  // Mapping from Vehicle Name to Vehicle Position History
  std::map<std::string, CPList>       m_hist_map;

  // Mapping from Vehicle Name to Bearing Lines
  std::map<std::string, BearingLine> m_bearing_map;

  std::string m_vehicles_active_name;
  std::string m_vehicles_center_name;

  unsigned int m_history_size;
  double       m_curr_time;

  double  m_xmin;
  double  m_xmax;
  double  m_ymin;
  double  m_ymax;
};

#endif 







