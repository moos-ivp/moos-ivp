/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VarDataPair.h                                        */
/*    DATE: Jul 4th 2005 Monday morning at Brueggers             */
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

#ifndef VAR_DATA_PAIR_HEADER
#define VAR_DATA_PAIR_HEADER

#include <string>
#include <vector>
#include <set>

class VarDataPair
{
public:
  VarDataPair();

  VarDataPair(std::string varname, double varvalue);

  VarDataPair(std::string varname, std::string varvalue);

  VarDataPair(std::string varname, std::string varvalue, std::string typehint);

  virtual ~VarDataPair() {}
  
  bool        set_var(std::string);
  bool        set_key(std::string);
  bool        set_ptype(std::string);
  bool        set_sdata(std::string, bool overwrite_ok=false);
  bool        set_ddata(double, bool overwrite_ok=false);
  bool        set_smart_data(std::string, bool overwrite_ok=false);

  void        set_post_tag(std::string s)   {m_post_tag=s;}
  
  std::string get_var()   const {return(m_var);}
  std::string get_sdata() const {return(m_sdata);}
  double      get_ddata() const {return(m_ddata);}
  bool        is_string() const {return(m_is_string);}
  bool        is_quoted() const {return(m_is_quoted);}
  std::string get_key()   const {return(m_key);}
  std::string get_ptype() const {return(m_ptype);}
  
  std::string get_post_tag() const {return(m_post_tag);}

  bool        is_solo_macro() const;
  
  bool        valid() const;
  int         why_invalid() const;

  bool        get_var_set()   const {return(m_var_set);}
  bool        get_sdata_set() const {return(m_sdata_set);}
  bool        get_ddata_set() const {return(m_ddata_set);}
  bool        get_ptype_set() const {return(m_ptype_set);}
  bool        get_key_set()   const {return(m_key_set);}
  
  std::string getPrintable() const;

  std::vector<std::string> getMacroVector() const;
  std::set<std::string>    getMacroSet() const;
  
protected:
  std::string m_var;
  std::string m_sdata;
  double      m_ddata;
  bool        m_is_string;
  bool        m_is_quoted;
  std::string m_key;
  std::string m_ptype;
 
  bool        m_var_set;
  bool        m_key_set;
  bool        m_ptype_set;
  bool        m_ddata_set;
  bool        m_sdata_set;

  // New in Release 20.X to support range related postings
  std::string m_post_tag;   
};

VarDataPair stringToVarDataPair(std::string);

#endif 









