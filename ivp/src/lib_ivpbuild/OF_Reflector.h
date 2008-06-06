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
#include "PQueue.h"

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
  int    create(const std::string);
  int    create(int unif_amt=-1, int smart_amt=-1, double thresh=-1);

  IvPFunction* extractOF(bool normalize=true);
  
  std::string getUniformPieceStr() const {return(m_uniform_piece_str);};
 
  bool   setParam(std::string);
  bool   setParam(std::string, std::string);
  bool   setParam(std::string, int);
  
 protected:
  void   initializePDMap();
  void   clearPDMap();

protected:
  const AOF*   m_aof;
  IvPDomain    m_domain;
  PDMap*       m_pdmap;

  Regressor*   m_regressor;
  RT_Uniform*  m_rt_uniform;
  RT_Focus*    m_rt_focus;
  RT_Priority* m_rt_priority;
  PQueue       m_pqueue;
  
  IvPBox       m_uniform_piece;
  IvPBox       m_uniform_grid;
  int          m_uniform_amount;
  int          m_smart_amount;
  int          m_smart_percent;
  double       m_smart_thresh;
  bool         m_auto_peak;

  std::vector<IvPBox>  m_refine_regions;
  std::vector<IvPBox>  m_refine_pieces;
  std::vector<IvPBox>  m_refine_points;

  std::string  m_uniform_piece_str;
};
#endif












