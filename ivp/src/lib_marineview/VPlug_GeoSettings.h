/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoSettings.h                                  */
/*    DATE: Aug 8th, 2009 (Split from GeoShapes.h)               */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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
};

#endif


