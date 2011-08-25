/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: IterBlockXMS.h                                       */
/*    DATE: Apr 28th 2008                                        */
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

#ifndef ITER_BLOCK_XMS_HEADER
#define ITER_BLOCK_XMS_HEADER

#include <vector>
#include <string>

class IterBlockXMS
{
public:
  IterBlockXMS() {m_iteration=-1;};
  virtual ~IterBlockXMS() {};
  
  void setIteration(int v)     
    {m_iteration=v;};

  void setVarNames(const std::vector<std::string>& val)
    {m_var_names = val;}
  void setVarVals(const std::vector<std::string>& val)
    {m_var_vals = val;}
  void setVarType(const std::vector<std::string>& val)
    {m_var_type = val;}
  void setVarSource(const std::vector<std::string>& val)
    {m_var_source = val;}
  void setVarTime(const std::vector<std::string>& val)
    {m_var_time = val;}
  void setVarCommunity(const std::vector<std::string>& val)
    {m_var_community = val;}
  void setVarCategory(const std::vector<std::string>& val)
    {m_var_category = val;}

  std::vector<std::string> getVarNames() const
    {return(m_var_names);};
  std::vector<std::string> getVarVals() const
    {return(m_var_vals);};
  std::vector<std::string> getVarType() const
    {return(m_var_type);};
  std::vector<std::string> getVarSource() const
    {return(m_var_source);};
  std::vector<std::string> getVarTime() const
    {return(m_var_time);};
  std::vector<std::string> getVarCommunity() const
    {return(m_var_community);};
  std::vector<std::string> getVarCategory() const
    {return(m_var_category);};
  
protected:
  std::vector<std::string> m_var_names;
  std::vector<std::string> m_var_vals;
  std::vector<std::string> m_var_type;
  std::vector<std::string> m_var_source;
  std::vector<std::string> m_var_time;
  std::vector<std::string> m_var_community;
  std::vector<std::string> m_var_category;

  int  m_iteration;
};


#endif 

