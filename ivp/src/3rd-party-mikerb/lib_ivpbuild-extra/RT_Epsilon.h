/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: RT_Epsilon.h                                         */
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
#ifndef RT_EPSILON_HEADER
#define RT_EPSILON_HEADER

#include "AOF.h"
#include "IvPFunction.h"

class PDMap;
class Regressor;

class RT_Epsilon {
public:
  RT_Epsilon(const AOF*, Regressor*);
  virtual ~RT_Epsilon() {};

public:
  PDMap* create(PDMap*, double epsilon);
  
protected:
  IvPBox* carve_one(const IvPBox*, double);
  IvPBox* carve_two(const IvPBox*, double);
  bool uniform_check(const IvPBox *g_box, double g_val);
  
protected:
  const AOF* m_aof;
  PDMap*     m_pdmap;
  Regressor* m_regressor;
};

#endif











