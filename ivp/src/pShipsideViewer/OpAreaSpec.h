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

  bool setParam(std::string param, std::string value);

  double getXPos(int ix);
  double getYPos(int ix);
  double getLWidth(int ix);

  std::string getGroup(int ix);
  std::string getLabel(int ix);
  bool        getDashed(int ix);

  std::vector<double> getLColor(int ix);
  std::vector<double> getVColor(int ix);

  unsigned int size() {return(m_vertex_xpos.size());}

protected:
  void addVertex(double xpos, double ypos, double lwidth,
		 std::string group, std::string label, 
		 std::string lcolor, std::string vcolor, 
		 bool ldashed=false);

protected:
  std::vector<double>       m_vertex_xpos;
  std::vector<double>       m_vertex_ypos;
  std::vector<double>       m_vertex_lwidth;
  std::vector<std::string>  m_vertex_group;
  std::vector<std::string>  m_vertex_label;
  std::vector<bool>         m_vertex_dashed;

  std::vector<std::vector<double> > m_vertex_lcolor;
  std::vector<std::vector<double> > m_vertex_vcolor;

  bool m_viewable_labels;

};

#endif

