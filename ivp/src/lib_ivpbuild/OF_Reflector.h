/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_Reflector.h                                       */
/*    DATE: Aug 29th 2005 (derived from OFR_AOF built long ago)  */
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
#ifndef OF_REFLECTOR_HEADER
#define OF_REFLECTOR_HEADER

#include "AOF.h"

class IvPFunction;
class PDMap;
class Regressor;
class RT_Uniform;
class RT_Focus;
class RT_Priority;

class OF_Reflector {
public:
  OF_Reflector(const AOF*, int degree=1);
  virtual ~OF_Reflector();

public: 
  int    createUniform(int amt, int qlev=0);
  int    createUniform(const IvPBox*, const IvPBox*b=0, int qlev=0);

  int    createPriority(int more_pcs, double thresh=0);
  int    createFocusRefine(IvPBox, IvPBox);

  int    create(const std::string);

  IvPFunction* extractOF(bool normalize=true);
 
  bool   setParam(std::string, std::string);
  
 protected:
  void initializePDMap();
  void clearPDMap();

protected:
  const AOF*   m_aof;
  IvPDomain    m_domain;
  PDMap*       m_pdmap;

  Regressor*   m_regressor;
  RT_Uniform*  m_rt_uniform;
  RT_Focus*    m_rt_focus;
  RT_Priority* m_rt_priority;

  IvPBox       m_uniform_piece;
  int          m_uniform_amount;
  int          m_smart_amount;
  int          m_smart_percent;
  bool         m_smart_peak;

  std::vector<IvPBox>  m_refine_regions;
  std::vector<IvPBox>  m_refine_pieces;
  std::vector<IvPBox>  m_refine_points;

};
#endif












