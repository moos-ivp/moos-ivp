/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Fixation.h                                           */
/*    DATE: Oct 27th, 2006                                       */
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

#ifndef FIXATION_VARIABLE_HEADER
#define FIXATION_VARIABLE_HEADER

#include <string>

class Fixation {
public:
  Fixation(std::string);
  ~Fixation() {};

  bool         isValid();
  std::string  getVarName()        {return(m_varname);}; 
  std::string  getSortType()       {return(m_sort_type);};  
  std::string  getDefaultType()    {return(m_default_type);};
  std::string  getDefaultString()  {return(m_string_default);};
  double       getDefaultDouble()  {return(m_double_default);};

protected:
  
  std::string  m_varname;
  std::string  m_sort_type;
  std::string  m_default_type;					
  std::string  m_string_default;
  double       m_double_default;
};

#endif
















