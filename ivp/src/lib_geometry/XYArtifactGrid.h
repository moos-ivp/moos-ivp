/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYArtifactGrid.h                                     */
/*    DATE: 3 JUL 2007                                           */
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

#ifndef XYARTIFACTGRID_HEADER
#define XYARTIFACTGRID_HEADER

#include "XYSquare.h"
#include "XYPolygon.h"
#include <vector>
#include <string>

typedef std::vector<std::string> ArtVec;
typedef unsigned int uint;

class XYArtifactGrid {
public:
  XYArtifactGrid();
  virtual ~XYArtifactGrid() {}
  
  bool      initialize(std::string); 
  
  const std::vector<XYSquare> getElements() const {return elements;}
  XYSquare  getElement(unsigned int index) const; 
  
  void      setClearance(unsigned int index, double val); 
  double    getClearance(unsigned int index) const; 
  double    getAvgClearance() const;
  
  void      setArtVec(unsigned int index, const ArtVec &); 
  ArtVec    getArtVec(unsigned int index) const; 
  uint      getDetectedArts() const;
  
  
  XYSquare  getSBound() const  {return(bounding_square);} 
  XYPolygon getPBound() const  {return(bounding_poly);}  // Used by bhv_SearchArtifact
  uint      size() const       {return(elements.size());} 
  
  bool      containsPoint(double, double) const; 
  bool      ptIntersectBound(double, double) const; 
  bool      segIntersectBound(double, double, double, double) const; 
  bool      processDelta(const std::string&); 
  
  std::string getConfigString() const {return(config_string);} 
  std::string getLabel() const {return(label);} 
  
 protected:
  bool     initialize(const XYPolygon&, const XYSquare&); 
  bool     initialize(const XYSquare&, const XYSquare&); 
  void     clear(); 
  
 protected:
  std::vector<double>   clearances;
  std::vector<ArtVec>   artifacts;
  std::vector<XYSquare> elements;
  
  std::string           config_string;
  std::string           label;
  XYSquare              bounding_square;
  XYPolygon             bounding_poly;
};

#endif









