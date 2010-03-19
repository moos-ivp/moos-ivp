/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_BehaviorSet.cpp                            */
/*    DATE:                                                      */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <iostream>
#include "Populator_BehaviorSet2.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

Populator_BehaviorSet2::Populator_BehaviorSet2(IvPDomain g_domain,
					     InfoBuffer *g_buffer)
   : Populator_BehaviorSet(g_domain, g_buffer)
{
}

#if 0
//----------------------------------------------------------
void Populator_BehaviorSet2::loadEnvVarDirectories(std::string envVar, 
						   bool verbose) {
  bhv_factory.loadEnvVarDirectories(envVar, verbose);
}
#endif

//----------------------------------------------------------
// Procedure: initializeBehavior

IvPBehavior* Populator_BehaviorSet2::initializeBehavior(string bhv_name)
{
  //IvPBehavior* bhv = Populator_BehaviorSet::initializeBehavior(bhv_name);
  IvPBehavior* bhv = m_bfactory_static.newBehavior(bhv_name);
  if(bhv != NULL) {
    cerr << "    InitializeBehavior: ";
    cerr << "found static behavior " << bhv_name << endl;
    return(bhv);
  }
  
  if(m_bfactory_dynamic.isKnownBehavior(bhv_name)) {
    cerr << "    InitializeBehavior: ";
    cerr << "found dynamic behavior " << bhv_name << endl;
    return(m_bfactory_dynamic.newBehavior(bhv_name, m_domain));
  }

  cerr << "    InitializeBehavior: ";
  cerr << "couldn't find behavior " << bhv_name << endl;

  return(0);
}
