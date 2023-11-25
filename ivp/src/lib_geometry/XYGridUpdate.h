/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYGridUpdate.h                                       */
/*    DATE: March 18th 2022                                      */
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

#ifndef XY_GRID_UPDATE_HEADER
#define XY_GRID_UPDATE_HEADER

#include <string>
#include <vector>

class XYGridUpdate {
public:
  XYGridUpdate(std::string grid_name="");
  virtual ~XYGridUpdate() {}
  
  void   setGridName(std::string s)    {m_grid_name=s;}

  void   setUpdateTypeDelta();
  void   setUpdateTypeReplace();
  void   setUpdateTypeAverage();
  
  bool   addUpdate(unsigned int, std::string, double);

  bool   valid() const;
  
  std::string  getGridName() const  {return(m_grid_name);}

  bool  isUpdateTypeDelta() const   {return(m_update_type_delta);}
  bool  isUpdateTypeReplace() const {return(m_update_type_replace);}
  bool  isUpdateTypeAverage() const {return(m_update_type_average);}

  unsigned int size() const {return(m_cell_ix.size());}

  unsigned int getCellIX(unsigned int) const;
  std::string  getCellVar(unsigned int) const;
  double       getCellVal(unsigned int) const;

  std::string  get_spec() const;
  
protected:
  std::string  m_grid_name;

  bool  m_update_type_delta;
  bool  m_update_type_replace;
  bool  m_update_type_average;

  std::vector<unsigned int> m_cell_ix;
  std::vector<std::string>  m_cell_var;					     
  std::vector<double>       m_cell_val;

};

XYGridUpdate stringToGridUpdate(std::string);
#endif






