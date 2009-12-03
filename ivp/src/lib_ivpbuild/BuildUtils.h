/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BuildUtils.h                                         */
/*    DATE: May 15th, 2003                                       */
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

#ifndef BUILD_UTIL_HEADER
#define BUILD_UTIL_HEADER

#include <string>
#include "PDMap.h"
#include "BoxSet.h"

IvPBox  genUnifBox(const IvPDomain&, int);
IvPBox  makeRand(const IvPBox&); 
IvPBox  makeRand(const IvPDomain&);
bool    containedWithinBox(const IvPBox&, const IvPBox&);
bool    intersectDomain(const IvPDomain&, const IvPDomain&);

IvPDomain   unionDomain(const IvPDomain&, const IvPDomain&);
IvPDomain   subDomain(const IvPDomain&, std::string);
IvPDomain   boxToDomain(const IvPBox&);
IvPDomain   stringToDomain(const std::string&);
IvPBox      domainToBox(const IvPDomain&);
IvPBox      stringToBox(const std::string&);
std::string domainToString(const IvPDomain&);

std::string domainAndBoxToString(const IvPBox&, const IvPDomain&);

BoxSet*  makeUniformDistro(const IvPBox&, const IvPBox&, int=1);
BoxSet*  subtractBox(const IvPBox&, const IvPBox&);
IvPBox*  cutBox(IvPBox*, int);
IvPBox*  quarterBox(IvPBox*, int, bool);

//--------------------------------------------------------------------
IvPBox  stringToPointBox(const std::string&, const IvPDomain&, 
			 const char gsep=',', const char lsep=':'); 

IvPBox  stringDiscreteToPointBox(const std::string&, const IvPDomain&, 
				 const char gsep=',', const char lsep=':'); 

IvPBox  stringNativeToPointBox(const std::string&, const IvPDomain&, 
			       const char gsep=',', const char lsep=':'); 

//--------------------------------------------------------------------
IvPBox  stringToRegionBox(const std::string&, const IvPDomain&, 
			  const char gsep=',', const char lsep=':'); 

IvPBox  stringNativeToRegionBox(const std::string&, const IvPDomain&, 
				const char gsep=',', const char lsep=':'); 

IvPBox  stringDiscreteToRegionBox(const std::string&, const IvPDomain&, 
				  const char gsep=',', const char lsep=':');

#endif










