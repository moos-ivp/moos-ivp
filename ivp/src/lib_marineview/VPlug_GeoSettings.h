/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_GeoSettings.h                                  */
/*    DATE: Aug 8th, 2009 (Split from GeoShapes.h)               */
/*****************************************************************/

#ifndef VPLUG_GEO_SETTINGS_HEADER
#define VPLUG_GEO_SETTINGS_HEADER

#include <vector>
#include <map>
#include <string>
#include "ColorPack.h"

class VPlug_GeoSettings {
public:
  VPlug_GeoSettings();
  virtual ~VPlug_GeoSettings() {};

  bool   setParam(const std::string& param, std::string value);
  bool   setColorMapping(std::string);
  bool   setColorMapping(std::string, std::string);
  bool   setViewableMapping(std::string, std::string);
  bool   viewable(const std::string&, bool view_default=true);
  double geosize(const std::string&, double size_default=0);
  bool   setGSizeMapping(std::string, std::string, double max=0);
  
  ColorPack geocolor(const std::string&, 
		     std::string color_default="");

  std::vector<std::string> getParamReport() const;

protected:
  std::map<std::string, ColorPack>  m_color_map;
  std::map<std::string, bool>       m_viewable_map;
  std::map<std::string, double>     m_gsize_map;

  // The below is strictly for parameter reporting and not used
  // in drawing. Should be identical to the m_color_map except
  // in form (string vs. vector<double>) only.
  std::map<std::string, std::string>  m_color_string_map;
};

#endif
