/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: QuadSet.h                                           */
/*    DATE: July 4th 2006                                       */
/****************************************************************/

#ifndef QUAD_SET_HEADER
#define QUAD_SET_HEADER

#include <vector>
#include <string>
#include "Quad3D.h"
#include "IvPFunction.h"
#include "IvPDomain.h"
#include "FColorMap.h"

class QuadSet 
{
public:
  QuadSet();
  ~QuadSet() {};

  // Set/Apply Information
  void   applyIPF(IvPFunction *ipf, bool wrap=true);
  void   applyColorMap(const FColorMap&);
  void   applyColorMap(const FColorMap&, double low, double hgh);
  bool   addQuadSet(const QuadSet&);
  void   normalize(double, double);

  // Get Information
  bool         isEmpty() const               {return(m_quads.size()==0);};
  unsigned int size() const                  {return(m_quads.size());};
  Quad3D       getQuad(unsigned int i) const {return(m_quads[i]);};
  double       getMaxVal() const             {return(m_maxpt_val);};
  double       getMinVal() const             {return(m_minpt_val);};
  double       getMaxPoint(std::string) const;
  void         print() const;


  void         resetMinMaxVals();

protected:
  std::vector<Quad3D> m_quads;

  IvPDomain   m_ivp_domain;

  // Cache Min/Max Utility values (high/low of the function)
  double      m_maxpt_val;
  double      m_minpt_val;

  // Cache the location of the high/low of the function
  double      m_maxpt_dvar_a;
  double      m_maxpt_dvar_b;

  // Values of the objecive function can be made to snap to 
  // intervals. Can be used to alter the rendering.
  double      m_snap_val;
};
#endif
