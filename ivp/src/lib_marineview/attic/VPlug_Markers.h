/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VPlug_Markers.h                                      */
/*    DATE: July 5th, 2008                                       */
/*****************************************************************/

#ifndef VMARKER_SET_HEADER
#define VMARKER_SET_HEADER

#include <vector>
#include <string>
#include "ColorPack.h"

class CMOOSGeodesy;
class VPlug_Markers {
public:
  VPlug_Markers();
  virtual ~VPlug_Markers() {};

  bool setParam(const std::string& param, std::string value);
  bool addVMarker(const std::string& description, 
		  CMOOSGeodesy& geodesy);
  
  unsigned int size()          {return(m_marker_type.size());};
  double   getMarkerGScale()   {return(m_marker_scale_global);}
  bool     viewable(const std::string& s="all");

  std::string getMarkerType(unsigned int ix) const;
  double      getMarkerXPos(unsigned int ix) const;
  double      getMarkerYPos(unsigned int ix) const;
  double      getMarkerWidth(unsigned int ix) const;
  std::string getMarkerLabel(unsigned int ix) const;
  bool        active(unsigned int ix) const;

  //int            getMarkerColorCount(int ix);
  std::string    getMarkerColorString(unsigned int mix, 
				      unsigned int cix) const;

  std::vector<ColorPack> getMarkerColorVectors(unsigned int mix) const;

  ColorPack getLabelColor() {return(m_label_color);};

  std::vector<std::string> getParamReport() const;

protected:
  void addVMarker(std::string mtype, double xpos, double ypos,
		  double mscale, std::string label, std::string colors,
		  bool active);

protected:
  std::vector<std::string>  m_marker_type;
  std::vector<double>       m_marker_xpos;
  std::vector<double>       m_marker_ypos;
  std::vector<double>       m_marker_width;
  std::vector<std::string>  m_marker_label;
  std::vector<bool>         m_marker_active;

  ColorPack  m_label_color_default;
  ColorPack  m_label_color;
  int        m_label_color_index;

  std::vector<std::vector<std::string> > m_marker_colors;
  std::vector<std::vector<ColorPack> >   m_marker_cpacks;
  

  double m_marker_scale_global;
  bool   m_markers_viewable;
  bool   m_marker_labels_viewable;
};

#endif
