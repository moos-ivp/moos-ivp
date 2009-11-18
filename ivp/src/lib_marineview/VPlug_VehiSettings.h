/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_VehiSettings.h                                 */
/*    DATE: Aug 12th 2008                                        */
/*          Aug 13th 2009 Broken out from VehicleSet class       */
/*          (Broken out from the Viewer class(es)                */
/*****************************************************************/

#ifndef MARINE_VEHICLE_SETTINGS_HEADER
#define MARINE_VEHICLE_SETTINGS_HEADER

#include <vector>
#include <string>
#include <map>
#include "ColorPack.h"

class VPlug_VehiSettings
{
 public:
  VPlug_VehiSettings();
  ~VPlug_VehiSettings() {};

  bool   setParam(std::string param, std::string value="");
  bool   setParam(std::string param, double value);

  int    getHistorySize()        const {return(m_history_size);};
  double getVehiclesShapeScale() const {return(m_vehicles_shape_scale);};
  double getTrailsLength()       const {return(m_trails_length);};
  double getTrailsPointSize()    const {return(m_trails_point_size);};

  ColorPack getColorTrails()  const {return(m_trails_color);};
  ColorPack getColorActiveVehicle() const
  {return(m_vehicles_active_color);};
  ColorPack getColorInactiveVehicle() const
  {return(m_vehicles_inactive_color);};
  ColorPack getColorVehicleName() const
  {return(m_vehicles_name_color);};

  std::string getVehiclesNameMode() const
    {return(m_vehicles_name_mode);};
  
  bool  isViewableVehicles()  const {return(m_vehicles_viewable);};
  bool  isViewableTrails()    const {return(m_trails_viewable);};
  bool  isViewableTrailsFuture() const 
  {return(m_trails_future_viewable);};
  bool  isViewableTrailsConnect() const 
  {return(m_trails_connect_viewable);};
  

 private:
  int    m_history_size;
  double m_vehicles_shape_scale;  
  double m_trails_length;         
  double m_trails_point_size;           

  bool   m_trails_viewable;      
  bool   m_trails_future_viewable;      
  bool   m_trails_connect_viewable;
  bool   m_vehicles_viewable; 

  std::string m_vehicles_name_mode; // off; names; names_mode; names_depth
  
  ColorPack m_trails_color; 
  ColorPack m_vehicles_active_color;
  ColorPack m_vehicles_inactive_color; 
  ColorPack m_vehicles_name_color;
};

#endif 


