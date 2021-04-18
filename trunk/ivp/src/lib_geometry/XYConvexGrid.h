/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYGrid.h                                             */
/*    DATE: Aug 27th 2005                                        */
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

#ifndef XY_CONVEX_GRID_HEADER
#define XY_CONVEX_GRID_HEADER

#include <string>
#include <vector>
#include "XYObject.h"
#include "XYSquare.h"
#include "XYPolygon.h"

class XYConvexGrid : public XYObject {
public:
  XYConvexGrid() {}
  ~XYConvexGrid() {}

  bool      initialize(const XYPolygon&, double cell_size, double init_val);
  bool      initialize(const XYPolygon&, double cell_size,
		       std::vector<std::string> cell_vars, 
		       std::vector<double> cell_init_vals);

  bool      ptIntersect(unsigned int ix, double x, double y) const;

  double    segIntersect(unsigned int ix, 
			 double x1, double y1,
			 double x2, double y2);

  unsigned int size() const    {return(m_elements.size());}

  XYSquare     getElement(unsigned int index) const;
  XYSquare     getSBound() const  {return(m_bounding_square);}
  bool         ptIntersect(double, double) const;
  bool         ptIntersectBound(double, double) const;
  bool         segIntersectBound(double, double, double, double) const;

  bool         hasCellVar(const std::string&) const;
  unsigned int getCellVarIX(const std::string&) const;
  unsigned int getCellVarCnt() const {return(m_cell_vars.size());}
  std::string  getConfigStr() const;
  std::string  get_spec() const;

  void    setVal(unsigned int ix, double val, unsigned int cix=0);
  void    incVal(unsigned int ix, double val, unsigned int cix=0);
  void    setMinLimit(double, unsigned int cix=0);
  void    setMaxLimit(double, unsigned int cix=0);

  double      getVal(unsigned int ix, unsigned int cix=0) const;
  std::string getVar(unsigned int cix=0) const;
  double      getMin(unsigned int cix=0) const;
  double      getMax(unsigned int cix=0) const;
  double      getInitVal(unsigned int cix=0) const;
  double      getMinLimit(unsigned int cix=0) const;
  double      getMaxLimit(unsigned int cix=0) const;
  bool        cellVarMinLimited(unsigned int cix=0) const;
  bool        cellVarMaxLimited(unsigned int cix=0) const;

  double  getCellSize() const {return(m_config_cell_size);}
  
  void    reset();
  void    reset(const std::string& cell_var);
  void    print() const; 

protected:
  bool    initialize(const XYSquare&, const XYSquare&);
    
 protected: // Config variables
  XYPolygon m_config_poly;
  double    m_config_cell_size;

  // Index is per cell variable 
  std::vector<std::string>    m_cell_vars;
  std::vector<double>         m_cell_init_vals;

  std::vector<double>         m_cell_max_limit;
  std::vector<double>         m_cell_min_limit;
  std::vector<bool>           m_cell_max_limited;
  std::vector<bool>           m_cell_min_limited;


 protected: // State variables
  std::vector<XYSquare> m_elements;
  XYSquare              m_bounding_square;
  
  // Outer IX: per grid element. Inner IX: per cellvar
  std::vector<std::vector<double> >  m_cell_vals;

  // Index is per cell variable
  std::vector<double>                m_cell_max_sofar;
  std::vector<double>                m_cell_min_sofar;
  std::vector<bool>                  m_cell_minmax_noted;
};

#endif
