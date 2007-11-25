/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: EchoVar.cpp                                          */
/*    DATE: July 22 2006                                         */
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

#ifndef TRANSLATOR_HEADER
#define TRANSLATOR_HEADER

#include <vector>
#include <string>
#include "MOOSLib.h"

class EchoVar : public CMOOSApp
{
public:
  EchoVar() {};
  virtual ~EchoVar() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();
  void registerVariables();

protected:
  bool addMapping(std::string, std::string);
  bool noCycles();
  
  std::vector<std::string> expand(std::vector<std::string> v);

protected:
  std::vector<std::string>  m_var_source;
  std::vector<std::string>  m_var_target;
  std::vector<std::string>  m_unique_sources;
};

#endif










