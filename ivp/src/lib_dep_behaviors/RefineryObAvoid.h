/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RefineryObAvoid.h                                    */
/*    DATE: September 5th, 2019                                  */
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
 
#ifndef REFINERY_OBAVD_HEADER
#define REFINERY_OBAVD_HEADER

#include <vector>
#include "IvPBox.h"
#include "IvPDomain.h"
#include "XYPolygon.h"
#include "ObShipModel.h"

class RefineryObAvoid {
 public:
  RefineryObAvoid(IvPDomain);
  ~RefineryObAvoid() {}

  void setRefineRegions(ObShipModel);

  std::vector<IvPBox> getPlateaus() const {return(m_plat_regions);}
  std::vector<IvPBox> getBasins() const   {return(m_basin_regions);}
      
  void setVerbose(bool v=true) {m_verbose=v;}

 protected:
  void setRefineRegionsAll();
  void setRefineRegionsMajor();
  void setRefineRegionsMinor();
  
 protected: // general utilities
  double getSpdSnappedLow(double) const;
  double getSpdSnappedHigh(double) const;
  double getSpdSnappedProx(double) const;

  double getHdgSnappedLow(double) const;
  double getHdgSnappedHigh(double) const;
  double getHdgSnappedProx(double) const;
  
  void addPlatRegion(IvPBox, double plat=1);
  void addPlatRegions(std::vector<IvPBox>, double plat=1);
  void addBasinRegions(std::vector<IvPBox>, double plat=-2);
  
 protected: 
  bool        m_verbose;  
  IvPDomain   m_domain;
  ObShipModel m_obship_model;

 protected: 
  unsigned int m_crs_ix;
  unsigned int m_spd_ix;
  bool         m_initialized_domain;

 protected: 
  std::vector<IvPBox> m_plat_regions;
  std::vector<IvPBox> m_basin_regions;
};

#endif

