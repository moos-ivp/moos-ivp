/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Viewingset.h                                         */
/*    DATE: July 5th, 2008                                       */
/*****************************************************************/

#ifndef VMARKER_SET_HEADER
#define VMARKER_SET_HEADER

#include <vector>
#include <string>

class VMarkerSet {
public:
  VMarkerSet();
  virtual ~VMarkerSet() {};

  bool addVMarker(std::string mtype, double xpos, double ypos,
		   double mscale, std::string label, std::string colors);

  bool setParam(std::string param, std::string value);
  bool setParam(std::string param, double);
  
  unsigned int size()          {return(m_marker_type.size());};
  double   getMarkerGScale()   {return(m_marker_scale_global);}
  bool     viewable(std::string s="all") {return(m_marker_viewable_all);};

  std::string getMarkerType(int ix);
  double      getMarkerXPos(int ix);
  double      getMarkerYPos(int ix);
  double      getMarkerScale(int ix);
  std::string getMarkerLabel(int ix);

  int            getMarkerColorCount(int ix);
  std::string    getMarkerColorString(int mix, int cix);
  std::vector<double> getMarkerColorVector(int mix, int cix);

protected:
  std::vector<std::string>  m_marker_type;
  std::vector<double>       m_marker_xpos;
  std::vector<double>       m_marker_ypos;
  std::vector<double>       m_marker_scale;
  std::vector<std::string>  m_marker_label;

  std::vector<std::vector<std::string> > m_marker_colors;
  std::vector<std::vector<std::vector<double> > > m_marker_color_vectors;

  double m_marker_scale_global;
  bool   m_marker_viewable_all;

};

#endif
