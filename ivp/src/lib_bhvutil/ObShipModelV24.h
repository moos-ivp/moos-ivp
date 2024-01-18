/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / MIT Cambridge MA            */
/*    FILE: ObShipModelV24.h                                     */
/*    DATE: Sep 6th, 2019                                        */
/*    DATE: Jul 31st, 2023 ObShipModelX with PlatModel           */
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

#ifndef OB_SHIP_MODELV24_HEADER
#define OB_SHIP_MODELV24_HEADER

#include <vector>
#include "XYPolygon.h"
#include "XYSeglr.h"
#include "PlatModel.h"

class ObShipModelV24
{
 public:
  ObShipModelV24(double osx=0, double osy=0, double osh=0, double osv=0);
  ~ObShipModelV24() {};

 public: // setters
  bool   setPose(double osx, double osy, double osh, double osv=0);
  bool   setPoseOSX(double osx);
  bool   setPoseOSY(double osy);
  bool   setPoseOSH(double osh);
  bool   setPoseOSV(double osv);

  void   setMinUtil(double);
  void   setMaxUtil(double);
  void   setMinMaxUtil(double, double);
  void   setOBuffRDegs(double);

  void   setPlatModel(PlatModel tm);

 public: // Setters that may generate health warnings
  std::string  setGutPoly(XYPolygon);
  std::string  setGutPoly(std::string);
  std::string  setPwtInnerDist(double);
  std::string  setPwtOuterDist(double);
  std::string  setCompletedDist(double);
  std::string  setMinUtilCPA(double);
  std::string  setMaxUtilCPA(double);
  std::string  setAllowableTTC(double);

  void   print(std::string key="") const;
  void   printBnds() const;

 public: // Setters for benchmark config
  void   useTurnCache(bool v=true) {m_turn_cache_enabled=v;}
  
 public: // getters (of things that can be set)
  double getOSX() const  {return(m_plat_model.getOSX());}
  double getOSY() const  {return(m_plat_model.getOSY());}
  double getOSH() const  {return(m_plat_model.getOSH());}
  double getOSV() const  {return(m_plat_model.getOSV());}

  double getPwtInnerDist() const    {return(m_pwt_inner_dist);}
  double getPwtOuterDist() const    {return(m_pwt_outer_dist);}
  double getMinUtilCPA() const      {return(m_min_util_cpa);}
  double getMaxUtilCPA() const      {return(m_max_util_cpa);}
  double getMinUtilCPAFlex() const  {return(m_min_util_cpa_flex);}
  double getMaxUtilCPAFlex() const  {return(m_max_util_cpa_flex);}

  double getAllowableTTC() const    {return(m_allowable_ttc);}
  double getCompletedDist() const   {return(m_completed_dist);}
  double getOBuffRDegs() const      {return(m_obuff_rdegs);}
  XYPolygon getGutPoly() const     {return(m_gut_poly);}
  PlatModel getPlatModel() const    {return(m_plat_model);}
  bool   isTurnCacheEnabled() const {return(m_turn_cache_enabled);}
  
 public: // analyzers (getters of things calculated)
  double getObcentX() const        {return(m_cx);}
  double getObcentY() const        {return(m_cy);}
  double getObcentBng() const      {return(m_obcent_bng);};
  double getObcentRelBng() const   {return(m_obcent_relbng);}

  XYPolygon getMidPoly() const {return(m_mid_poly);}
  XYPolygon getRimPoly() const {return(m_rim_poly);}
  std::string getPassingSide() const   {return(m_passing_side);}

  double getRange() const           {return(m_range);}
  double getRangeInOSH() const      {return(m_range_in_osh);}

  double getGutBngMinToPoly() const {return(m_gut_bng_min_to_poly);}
  double getGutBngMaxToPoly() const {return(m_gut_bng_max_to_poly);}
  double getRimBngMinToPoly() const {return(m_rim_bng_min_to_poly);}
  double getRimBngMaxToPoly() const {return(m_rim_bng_max_to_poly);}

  unsigned int getGutBngHitCount() const   {return(m_gut_bng_hit_count);}
  unsigned int getGutBngUnhitCount() const {return(m_gut_bng_unhit_count);}
  unsigned int getRimBngHitCount() const   {return(m_rim_bng_hit_count);}
  unsigned int getRimBngUnhitCount() const {return(m_rim_bng_unhit_count);}

  
  double getGutBngMinDistToPoly() const {return(m_gut_bng_min_dist_to_poly);}
  double getGutBngMaxDistToPoly() const {return(m_gut_bng_max_dist_to_poly);}
  double getRimBngMinDistToPoly() const {return(m_rim_bng_min_dist_to_poly);}
  double getRimBngMaxDistToPoly() const {return(m_rim_bng_max_dist_to_poly);}

  double getCPAInOSH() const {return(m_cpa_in_osh);}

  bool   paramIsSet(std::string) const;
  bool   ownshipInGutPoly() const;
  bool   ownshipInMidPoly() const;
  bool   ownshipInRimPoly() const;

  std::string getFailedExpandPolyStr(bool clear=true);

  std::string getObstacleLabel() const {return(m_gut_poly.get_label());}
  
  double getRangeRelevance();
  bool   isObstacleAft(double xbng=0) const;

  bool   isValid() const;

  double rayCPA(double hdg, double& ix, double& iy) const;
  double seglrCPA(double hdg, double& ix, double& iy,
		  double& stemdist, bool verbose=false) const;

  double evalHdgSpd(double hdg, double spd, bool verbose=false) const;

  void   setCachedVals(bool force=false);

  void   updateBngExtremes();
  
protected:
  bool updateDynamic();
  void fillTurnCache(const XYPolygon&);

  bool setBngMinMaxToGutPoly();
  bool setBngMinMaxToRimPoly();
  
 private: // Config (set) variables
  XYPolygon m_gut_poly;
  PlatModel m_plat_model;
  
  double m_min_util;
  double m_max_util;
  double m_min_util_cpa;
  double m_max_util_cpa;
  double m_min_util_cpa_flex;
  double m_max_util_cpa_flex;
  double m_pwt_inner_dist;
  double m_pwt_outer_dist;
  double m_allowable_ttc;
  double m_completed_dist;
  
  double m_obuff_rdegs;

  bool   m_turn_cache_enabled;
  bool   m_pause_update_dynamic;

  bool   m_stale_cache;
  
  std::set<std::string> m_set_params;
  
 private: // State (derived) variables
  XYPolygon m_mid_poly;
  XYPolygon m_rim_poly;

  unsigned int m_dynamic_updates;
  
  double m_cx;  // Obstacle center x,y
  double m_cy;

  double m_obcent_bng;
  double m_obcent_relbng;

  std::string m_failed_expand_poly_str;
  std::string m_passing_side;

  double m_range;
  double m_range_in_osh;
  double m_cpa_in_osh;

  // boundaries and distance caches. Used mainly if there
  // is a Refinery using the ObShipModel
  
private:

  double m_gut_bng_min_to_poly;
  double m_gut_bng_max_to_poly;
  double m_rim_bng_min_to_poly;
  double m_rim_bng_max_to_poly;

  unsigned int m_gut_bng_hit_count;   // If 0, good in all dirs
  unsigned int m_gut_bng_unhit_count; // If 0, bad in all dirs
  unsigned int m_rim_bng_hit_count;
  unsigned int m_rim_bng_unhit_count;
  
  double m_gut_bng_min_dist_to_poly;
  double m_gut_bng_max_dist_to_poly;

  double m_rim_bng_min_dist_to_poly;
  double m_rim_bng_max_dist_to_poly;

  // ToDo improvements:
  
  // 1) calc and cache the seglr for each hdg angle. Use in
  // both the bngMinMaxToRimPoly and bngMinMaxToGutPoly. If

  // 2) By calculating bngMinMaxToRimPoly first, then all angs
  // that are non-zero will also be non-zero for the gut poly

  // 3) In calculating seglr dist to the rim_poly, we can also
  // just calculate the seglr dist to the gut poly, and subtract
  // the rim buffer dist. Presumably, since the rim_poly is
  // grown from the gut_poly, it will have more vertices and thus
  // more calculations involved for calculating the seglr dist.

  // 4) outward cache sweep. a) calculate the seglr angle to the
  // center of the obstacle. b) sweep outward until a non-zero
  // cpa dist is detected in both directions (this is the basin).
  // c) continue sweeping outward in both directions until a
  // cpa_dist to the rim poly is non-zero. This range is the
  // the plateau(s). Rounding down, inward, this is the range of
  // headings we may ever care about. Even the headings that hit
  // the gut poly, we may still want to calculate the stem_dist
  // so we may apply the TTC criteria.
  // NOTE on outward cache sweep: If the center of the gut poly
  // is closer than the Dubins turn radius, that the refinery
  // can resort to just one plateau on the otherside of ownship.
  
  
};

#endif 
