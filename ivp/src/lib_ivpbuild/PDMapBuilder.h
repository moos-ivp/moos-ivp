/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PDMapBuilder.h                                       */
/*    DATE: May 24th, 2016                                       */
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

#ifndef PDMAP_BUILDER_HEADER
#define PDMAP_BUILDER_HEADER

#include <vector>
#include <string>
#include "IvPBox.h"
#include "PDMap.h"

class PDMapBuilder {
public:
  PDMapBuilder() {};
  virtual ~PDMapBuilder() {clearBldPieces();}

  void   setIvPDomain(IvPDomain);
  void   setDomainVals(std::vector<unsigned int>);
  void   setRangeVals(std::vector<double>);
  PDMap* getPDMap();
  
  bool   hasWarnings();
  void   clearWarnings();
  std::string  getWarnings();

 protected:

  bool   preprocess();
  void   clearBldPieces();
  void   addWarning(std::string);

  IvPBox* buildBox(unsigned int ix_low, double val_low,
		   unsigned int ix_hgh, double val_hgh);

protected:  
  std::vector<unsigned int> m_dom_vals;
  std::vector<double>       m_rng_vals;
  std::vector<std::string>  m_warnings;
  std::vector<IvPBox*>      m_bld_pieces;

  IvPDomain m_ivp_domain;
};
#endif









