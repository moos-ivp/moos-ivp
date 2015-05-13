/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: OF_Reflector.h                                       */
/*    DATE: Aug 29th 2005 (derived from OFR_AOF built long ago)  */
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
#ifndef OF_REFLECTOR_HEADER
#define OF_REFLECTOR_HEADER

#include <string>
#include <vector>
#include "AOF.h"
#include "PQueue.h"

class IvPFunction;
class PDMap;
class Regressor;
class RT_Uniform;
class RT_Directed;
class RT_Smart;
class RT_AutoPeak;

class OF_Reflector {
public:
  OF_Reflector(const AOF*, int degree=1);
  virtual ~OF_Reflector();

 public: 
  int    create(const std::string);
  int    create(int unif_amt=-1, int smart_amt=-1, double thresh=-1);

  // extractOF is deprecated, supported for now, use extractIvPFunction
  IvPFunction* extractOF(bool normalize=true);
  IvPFunction* extractIvPFunction(bool normalize=true)
  {return(extractOF(normalize));}
  
  std::string getUniformPieceStr() const {return(m_uniform_piece_str);}
  std::string getWarnings() const        {return(m_warnings);}

  bool   stateOK() const    {return(m_warnings=="");}
  bool   setParam(std::string);
  bool   setParam(std::string, std::string);
  bool   setParam(std::string, double);
  bool   setParam(std::string, IvPBox);
    
  // Added by mikerb May1614
  unsigned int getMessageCnt() const;
  std::string  getMessage(unsigned int) const;
  

 protected:
  void   initializePDMap();
  void   clearPDMap();
  void   addWarning(std::string);

 protected:
  const AOF*   m_aof;
  IvPDomain    m_domain;
  PDMap*       m_pdmap;

  Regressor*   m_regressor;
  RT_Uniform*  m_rt_uniform;
  RT_Directed* m_rt_directed;
  RT_Smart*    m_rt_smart;
  RT_AutoPeak* m_rt_autopeak;
  PQueue       m_pqueue;
  
  IvPBox       m_uniform_piece;
  IvPBox       m_uniform_grid;
  int          m_uniform_amount;
  int          m_smart_amount;
  int          m_smart_percent;
  double       m_smart_thresh;
  bool         m_auto_peak;
  int          m_auto_peak_max_pcs;

  std::vector<IvPBox>  m_refine_regions;
  std::vector<IvPBox>  m_refine_pieces;
  std::vector<IvPBox>  m_refine_points;

  std::string  m_uniform_piece_str;

  std::string m_warnings;
};
#endif
















