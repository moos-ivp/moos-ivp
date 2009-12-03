/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: EFlipper.h                                           */
/*    DATE: April 4th, 2009                                      */
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

#ifndef EFLIPPER_HEADER
#define EFLIPPER_HEADER

#include <vector>
#include <string>
#include <map>

class EFlipper
{
public:
  EFlipper();
  virtual ~EFlipper() {};

  bool setParam(std::string param, std::string value);
  bool valid();
  void print();
  
  std::string  getKey()       {return(m_key);};
  std::string  getSourceVar() {return(m_source_variable);};
  std::string  getDestVar()   {return(m_dest_variable);};

  std::string  flip(std::string);

protected:
  std::string  m_key;
  std::string  m_source_variable;
  std::string  m_dest_variable; 
  std::string  m_source_separator;
  std::string  m_dest_separator;

  // The "component" mapping, e.g. "x" -> "xpos"
  std::map<std::string, std::string> m_cmap;

  // The "filter" mapping, e.g. "key" -> "foobar"
  std::map<std::string, std::string> m_fmap;
};
#endif


