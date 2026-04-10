/*****************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: OpField.h                                       */
/*    DATE: Sep 25th, 2023                                  */
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

#ifndef OP_FIELD_HEADER
#define OP_FIELD_HEADER

#include <string> 
#include <map>
#include "XYPoint.h"
#include "XYPolygon.h"
#include "XYSegList.h"

class OpField
{
public:
  OpField() ;
  ~OpField() {};

public: 
  bool config(std::string);

  bool addPoint(std::string alias, std::string ptstr);
  bool addPoint(std::string alias, XYPoint pt);
  bool setPoint(std::string alias, std::string pstr);
  bool setPoint(std::string alias, XYPoint pt);
  
  bool addPoly(std::string alias, std::string polystr);
  bool addPoly(std::string alias, XYPolygon poly);
  bool setPoly(std::string alias, std::string polystr);
  bool setPoly(std::string alias, XYPolygon poly);
  
  std::vector<std::string> getPtAliases() const;
  XYPoint getPoint(std::string alias) const;
  std::string getColor(std::string alias) const;

  bool hasKey(std::string) const;
  bool hasKeyPt(std::string) const;
  bool hasKeyPoly(std::string) const;
  bool hasKeySegl(std::string) const;
  
  unsigned int size() const;
  unsigned int ptSize() const {return(m_map_pts.size());}
  unsigned int polySize() const {return(m_map_polys.size());}
  unsigned int seglSize() const {return(m_map_segls.size());}

  bool merge(OpField);

  void simplify(bool keep_shorter=true);
  
protected:

  std::map<std::string, XYPoint>   m_map_pts;
  std::map<std::string, XYPolygon> m_map_polys;
  std::map<std::string, XYSegList> m_map_segls;

  std::map<std::string, std::string> m_map_colors;

};

#endif 
