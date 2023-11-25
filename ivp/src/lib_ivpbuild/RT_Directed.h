/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RT_Directed.h                                        */
/*    DATE: Mar 20th, 2007                                       */
/*    NOTE: "RT_" stands for "Reflector Tool"                    */
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

#ifndef RT_DIRECTED_HEADER
#define RT_DIRECTED_HEADER

#include "PDMap.h"

class Regressor;

class RT_Directed {
 public:
  RT_Directed() {m_verbose=false;}
  virtual ~RT_Directed() {}

 public: 
  PDMap* create(PDMap*, const IvPBox&, const IvPBox&);

  void   setVerbose(bool v=true) {m_verbose=v;}
  
 private:
  bool m_verbose;
};

#endif





