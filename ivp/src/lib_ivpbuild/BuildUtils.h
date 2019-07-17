/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BuildUtils.h                                         */
/*    DATE: May 15th, 2003                                       */
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

IvPDomain   unionDomain(const IvPDomain&, const IvPDomain&B);
IvPDomain   subDomain(const IvPDomain&, std::string);
IvPDomain   boxToDomain(const IvPBox&);
IvPDomain   stringToDomain(const std::string&);
IvPBox      domainToBox(const IvPDomain&);
IvPBox      stringToBox(const std::string&);
std::string domainToString(const IvPDomain&, bool=true);

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

//--------------------------------------------------------------------
// Heading/Speed Box Utilities

IvPBox  buildBoxHdgAll(IvPDomain, double smin, double smax);

std::vector<IvPBox>  buildBoxesSpdAll(IvPDomain, double hmin, double hmax);

std::vector<IvPBox>  buildBoxesHdgSpd(IvPDomain, double hmin, double hmax,
				      double smin, double smax);


//--------------------------------------------------------------------
// Utility added by mikerb April 2018 for testing Refineries

std::vector<IvPBox> getPointBoxes(IvPBox);

#endif

