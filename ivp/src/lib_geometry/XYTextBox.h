/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYTextBox.h                                          */
/*    DATE: Aug 31st, 2023                                       */
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
 
#ifndef XY_TEXT_BOX_HEADER
#define XY_TEXT_BOX_HEADER

#include <string>
#include "XYObject.h"

class XYTextBox : public XYObject {
 public:
  XYTextBox();
  XYTextBox(double x, double y, std::string label="");

  virtual ~XYTextBox() {}
  
  void clear();
  
  void setXY(double x, double y);
  void setFSize(int fsize);
  void setMColor(std::string);

  void setX(double v)  {m_x=v; m_x_set=true;}
  void setY(double v)  {m_y=v; m_y_set=true;}
  
  void setMsg(std::string);
  void addMsg(std::string);
  
public:
  void shiftX(double val) {m_x += val;}
  void shiftY(double val) {m_y += val;}
  void applySnap(double snapval);

public:
  double getX() const     {return(m_x);}
  double getY() const     {return(m_y);}
  double x() const        {return(m_x);}
  double y() const        {return(m_y);}
  int    getFSize() const {return(m_fsize);}
  bool   valid() const;

  std::string getMsg(unsigned int i=0) const;
  std::string getMColor() const {return(m_mcolor);}
  
  std::string get_spec(std::string s="") const;
  std::string get_spec_inactive() const;

  unsigned int size() const {return(m_msgs.size());}
  
protected:
  double m_x;
  double m_y;
  bool   m_x_set;
  bool   m_y_set;
  int    m_fsize;

  std::string m_mcolor;
  
  std::vector<std::string> m_msgs;
};

XYTextBox stringToTextBox(std::string);

#endif

