/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CurrentField.h                                       */
/*    DATE: Oct 20th 2010                                        */
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

#ifndef CURRENT_FIELD_HEADER
#define CURRENT_FIELD_HEADER

#include <string>
#include <vector>
#include "XYVector.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

class CurrentField
{
public:
  CurrentField();
  ~CurrentField() {}

  bool populate(std::string filename);
  void addVector(const XYVector&, bool marked=false);
  void getLocalForce(double x, double y, double& fx, double& fy) const;
  void setRadius(double radius);
  bool initGeodesy(double datum_lat, double datum_lon);
  void print();

  bool deleteVector(unsigned int ix);
  bool unmarkVector(unsigned int ix);
  void markupVector(unsigned int ix);
  void markVector(unsigned int ix);

  bool isMarked(unsigned int ix);
  void deleteMarkedVectors();

  void modVector(unsigned int ix, std::string param, double val);
  void modMarkedVectors(std::string param, double val);
  bool unmarkAllVectors();
  bool markAllVectors();
  void applySnap(double);

  XYVector getVector(unsigned int) const;
  bool     getVMarked(unsigned int) const;
  double   getXPos(unsigned int) const;
  double   getYPos(unsigned int) const;
  double   getForce(unsigned int) const;
  double   getDirection(unsigned int) const;
  bool     hasActiveVertex() const;

  unsigned int selectVector(double x, double y, double& dist);

  unsigned int getActiveIX() {return(m_active_ix);}
  unsigned int size()        {return(m_vectors.size());}
  std::string getName()      {return(m_field_name);}
  double getRadius()         {return(m_radius);}

  std::vector<XYVector> getVectors();
  std::vector<XYVector> getVectorsMarked();
  std::vector<XYVector> getVectorsUnMarked();

  std::vector<std::string> getListing();

 protected:
  bool   handleLine(std::string);
  void   applyRenderHints();
  void   applyRenderHint(std::string, std::string);

protected:
  std::vector<XYVector> m_vectors;
  std::vector<bool>     m_vmarked;
  std::string           m_field_name;
  double                m_radius;
  bool                  m_active_vertex;
  unsigned int          m_active_ix;
  CMOOSGeodesy          m_geodesy;

  std::vector<std::string> m_render_hints;

};

#endif 









