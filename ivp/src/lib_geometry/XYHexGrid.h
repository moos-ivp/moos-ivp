/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: XYHexGrid.h                                          */
/*    DATE: Oct 30th 2005                                        */
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

#ifndef XY_HEXGRID_HEADER
#define XY_HEXGRID_HEADER

#include <string>
#include <vector>
#include "XYSquare.h"
#include "XYHexagon.h"

class XYHexGrid {
public:
  XYHexGrid() {};
  virtual ~XYHexGrid() {};

  bool     initialize(std::string);

  XYHexagon getElement(unsigned int index) const;
  int       size() const      {return(elements.size());};

  std::string getConfigString() const {return(config_string);};
  std::string getLabel() const {return(label);};

protected:
  bool     initialize(XYPolygon, const XYSquare&);
  bool     initialize(const XYSquare&, const XYSquare&);
  void     clear() {};
    
protected:
  std::vector<XYHexagon> elements;
  std::string            config_string;
  std::string            label;
  XYSquare               bounding_square;
};
#endif









