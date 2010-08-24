/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_DropPoints.h                                   */
/*    DATE: June 13th, 2009                                      */
/*****************************************************************/

#ifndef VPLUG_DROPPOINTS_HEADER
#define VPLUG_DROPPOINTS_HEADER

#include <vector>
#include "XYPoint.h"
#include "ColorPack.h"

class VPlug_DropPoints {
public:
  VPlug_DropPoints();
  virtual ~VPlug_DropPoints() {};

  bool  setParam(const std::string& param, std::string value);
  void  addPoint(const XYPoint&);
  void  addPoint(const XYPoint&, const std::string& latlon_str, 
		 const std::string& localg, const std::string& native);
  void  deletePoint(const XYPoint&) {};
  bool  viewable()    {return(m_viewable_all);};

  XYPoint&    point(unsigned int i) {return(m_points[i]);};
  XYPoint     getPoint(unsigned int) const;
  std::string getCoordinates(unsigned int i) const;

  unsigned int size() {return(m_points.size());};


protected:
  std::vector<XYPoint>     m_points;
  std::vector<std::string> m_points_latlon;
  std::vector<std::string> m_points_localg;
  std::vector<std::string> m_points_native;
  bool                     m_viewable_all;
  int                      m_pfont_size;
  std::string              m_coord_mode;
  std::string              m_coord_color_latlon;
  std::string              m_coord_color_localg;
};

#endif
