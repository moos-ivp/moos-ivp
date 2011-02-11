/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYGrid.h                                             */
/*    DATE: Aug 27th 2005                                        */
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

#ifndef XY_GRID_HEADER
#define XY_GRID_HEADER

#include <string>
#include <vector>
#include "XYSquare.h"
#include "XYPolygon.h"

class XYGrid {
public:
  XYGrid();
  virtual ~XYGrid() {};

  bool      initialize(std::string);

  XYSquare  getElement(unsigned int index) const;
  void      setVal(unsigned int index, double val);
  double    getVal(unsigned int index) const;

  XYSquare  getSBound() const  {return(bounding_square);};
  XYPolygon getPBound() const  {return(bounding_poly);};
  double    getMinVal() const  {return(min_val);};
  double    getMaxVal() const  {return(max_val);};
  int       size() const       {return(elements.size());};

  void      setUtil(unsigned int ix, double val);
  void      setUtilRange(double, double);
  double    getUtil(unsigned int ix)  const;

  double    getMinUtil()     const {return(min_util);};
  double    getMaxUtil()     const {return(max_util);};
  double    getMinUtilPoss() const {return(min_util_possible);};
  double    getMaxUtilPoss() const {return(max_util_possible);};
  
  void      handleSegment(double, double, double, double);
  void      resetFromMin();

  bool      ptIntersect(double, double) const;
  bool      ptIntersectBound(double, double) const;
  bool      segIntersectBound(double, double, double, double) const;
  bool      processDelta(const std::string&);
  void      processDelta(const std::string&, const std::string&);

  std::string getConfigString() const {return(config_string);};
  std::string getLabel() const {return(label);};

protected:
  bool     initialize(XYPolygon, const XYSquare&, double);
  bool     initialize(const XYSquare&, const XYSquare&, double);
  void     clear();
    
protected:
  std::vector<XYSquare> elements;
  std::vector<double>   values;
  std::vector<double>   utilities;
  std::string           config_string;
  std::string           label;
  XYSquare              bounding_square;
  XYPolygon             bounding_poly;
  
  double min_val;
  double max_val;

  double min_util;
  double max_util;
  double min_util_possible;
  double max_util_possible;
};
#endif









