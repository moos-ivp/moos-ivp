/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RT_Smart.h                                           */
/*    DATE: Jan 20th, 2006                                       */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef RT_SMART_REFINE_HEADER
#define RT_SMART_REFINE_HEADER

#include "PDMap.h"
#include "PQueue.h"

class Regressor;
class RT_Smart {
public:
  RT_Smart(Regressor*);
  virtual ~RT_Smart() {}

 public: 
  PDMap* create(PDMap*, PQueue&, int more_pcs, double thresh=0);

  void   setVerbose(bool v=true) {m_verbose=v;}
  
 protected:
  Regressor* m_regressor;

 private:
  bool m_verbose;
};

#endif




















