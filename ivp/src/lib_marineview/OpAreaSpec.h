/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OpAreaSpec.h                                         */
/*    DATE: July 6th, 2008                                       */
/*****************************************************************/

#ifndef OP_AREA_SPEC_HEADER
#define OP_AREA_SPEC_HEADER

#include <vector>
#include <string>

class CMOOSGeodesy;
class OpAreaSpec {
public:
  OpAreaSpec();
  virtual ~OpAreaSpec() {};

  bool addVertex(const std::string& description, 
		 CMOOSGeodesy& m_geodesy);

  bool setParam(const std::string& param, std::string value);

  unsigned int size() {return(m_vertex_xpos.size());}
  
  bool viewable(const std::string& s="all");
  double getLineShade()   {return(m_line_shade);};
  double getLabelShade()  {return(m_label_shade);};

  double getXPos(unsigned int ix) const;
  double getYPos(unsigned int ix) const;
  double getLWidth(unsigned int ix) const;

  std::string getGroup(unsigned int ix) const;
  std::string getLabel(unsigned int ix) const;
  bool        getDashed(unsigned int ix) const;
  bool        getLooped(unsigned int ix) const;

  std::vector<double> getLColor(unsigned int ix) const;
  std::vector<double> getVColor(unsigned int ix) const;

  std::vector<double> getDatumColor() const
    {return(m_datum_color);};

  double getDatumSize() const {return(m_datum_size);};
  
protected:
  void addVertex(double xpos, double ypos, double lwidth,
		 std::string group, std::string label, 
		 std::string lcolor, std::string vcolor, 
		 bool looped=false, bool ldashed=false);

protected:
  std::vector<double>       m_vertex_xpos;
  std::vector<double>       m_vertex_ypos;
  std::vector<double>       m_vertex_lwidth;
  std::vector<std::string>  m_vertex_group;
  std::vector<std::string>  m_vertex_label;
  std::vector<bool>         m_vertex_dashed;
  std::vector<bool>         m_vertex_looped;

  std::vector<std::vector<double> > m_vertex_lcolor;
  std::vector<std::vector<double> > m_vertex_vcolor;

  bool   m_viewable_labels;
  bool   m_viewable_all;
  double m_line_shade;
  double m_label_shade;

  // Datum member variables
  bool   m_datum_viewable;
  double m_datum_size;
  std::vector<double> m_datum_color;
};

#endif

