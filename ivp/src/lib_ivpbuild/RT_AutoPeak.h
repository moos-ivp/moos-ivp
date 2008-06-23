/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_AutoPeak.h                                        */
/*    DATE: June 21, 2008                                        */
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
#ifndef RT_AUTOPEAK_HEADER
#define RT_AUTOPEAK_HEADER

#include "PDMap.h"
#include "PQueue.h"

class Regressor;
class RT_AutoPeak {
public:
  RT_AutoPeak(Regressor*);
  virtual ~RT_AutoPeak() {};

public: 
  PDMap* create(PDMap*);

protected:
  Regressor* m_regressor;
};

#endif












