/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_VehiSettings.h                                 */
/*    DATE: Aug 12th 2008                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef VPLUG_VEHICLE_SETTINGS_HEADER
#define VPLUG_VEHICLE_SETTINGS_HEADER

#include <vector>
#include <string>
#include <map>
#include "ColorPack.h"

class VPlug_VehiSettings
{
 public:
  VPlug_VehiSettings();
  ~VPlug_VehiSettings() {}

  bool   setParam(std::string param, std::string value="");
  bool   setParam(std::string param, double value);

  std::string strvalue(const std::string&);

  unsigned int getTrailsLength() const {return(m_trails_length);}

  double getVehiclesShapeScale() const {return(m_vehicles_shape_scale);}
  double getTrailsPointSize()    const {return(m_trails_point_size);}
  double getStaleReportThresh()  const {return(m_stale_report_thresh);}
  double getStaleRemoveThresh()  const {return(m_stale_remove_thresh);}

  ColorPack getColorTrails()  const         {return(m_trails_color);}
  ColorPack getColorActiveVehicle() const   {return(m_vehicles_active_color);}
  ColorPack getColorInactiveVehicle() const {return(m_vehicles_inactive_color);}
  ColorPack getColorVehicleName() const     {return(m_vehicles_name_color);}

  std::string getVehiclesNameMode() const   {return(m_vehicles_name_mode);}
  
  bool  isViewableVehicles()  const     {return(m_vehicles_viewable);}
  bool  isViewableTrails()    const     {return(m_trails_viewable);}
  bool  isViewableBearingLines() const  {return(m_bearing_lines_viewable);}
  bool  isViewableTrailsFuture() const  {return(m_trails_future_viewable);}
  bool  isViewableTrailsConnect() const {return(m_trails_connect_viewable);}

 private:
  unsigned int m_trails_length;         

  double m_vehicles_shape_scale;  
  double m_trails_point_size;           
  double m_stale_report_thresh;
  double m_stale_remove_thresh;

  bool   m_trails_viewable;      
  bool   m_trails_future_viewable;      
  bool   m_trails_connect_viewable;
  bool   m_vehicles_viewable; 
  bool   m_bearing_lines_viewable;

  std::string m_vehicles_name_mode; // off; names; names_mode; names_depth
  
  ColorPack m_trails_color; 
  ColorPack m_vehicles_active_color;
  ColorPack m_vehicles_inactive_color; 
  ColorPack m_vehicles_name_color;

  std::string  m_trails_color_orig; 
  std::string  m_vehicles_active_color_orig;
  std::string  m_vehicles_inactive_color_orig; 
  std::string  m_vehicles_name_color_orig;
};

#endif 











