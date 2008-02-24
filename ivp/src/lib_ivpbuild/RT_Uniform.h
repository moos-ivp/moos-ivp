/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_Uniform.h                                         */
/*    DATE: Aug 26th 2005 (from OFR_AOF written long ago)        */
/*    NOTE: "RT_" stands for "Reflector Tool"                    */
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
#pragma warning(disable : 4503)
#endif
#ifndef RT_UNIFORM_HEADER
#define RT_UNIFORM_HEADER

#include "PDMap.h"
#include "PQueue.h"

class Regressor;

class RT_Uniform {
public:
  RT_Uniform(Regressor*);
  virtual ~RT_Uniform() {};
  
public: 
  PDMap*  create(const IvPBox*u, const IvPBox*, PQueue&);
  
 protected:
  Regressor* m_regressor;
};

#endif
