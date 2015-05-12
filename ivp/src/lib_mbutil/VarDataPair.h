/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarDataPair.h                                        */
/*    DATE: Jul 4th 2005 Monday morning at Brueggers             */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef VAR_DATA_PAIR_HEADER
#define VAR_DATA_PAIR_HEADER

#include <string>

class VarDataPair
{
public:
  VarDataPair();

  VarDataPair(const std::string& varname, 
	      double varvalue);

  VarDataPair(const std::string& varname, 
	      const std::string& varvalue);

  VarDataPair(const std::string& varname, 
	      const std::string& varvalue, 
	      const std::string& typehint);

  virtual ~VarDataPair() {}
  
  void        set_key(const std::string& s)   {m_key=s;}
  void        set_ptype(const std::string& s) {m_ptype=s;}
  void        set_sdata(const std::string& s) {m_sdata=s; m_is_string=true;}
  void        set_ddata(double v)             {m_ddata=v; m_is_string=false;}

  std::string get_var()   const {return(m_var);}
  std::string get_sdata() const {return(m_sdata);}
  double      get_ddata() const {return(m_ddata);}
  bool        is_string() const {return(m_is_string);}
  bool        is_quoted() const {return(m_is_quoted);}
  std::string get_key()   const {return(m_key);}
  std::string get_ptype() const {return(m_ptype);}

  std::string getPrintable();

protected:
  std::string m_var;
  std::string m_sdata;
  double      m_ddata;
  bool        m_is_string;
  bool        m_is_quoted;
  std::string m_key;
  std::string m_ptype;
};

#endif 





