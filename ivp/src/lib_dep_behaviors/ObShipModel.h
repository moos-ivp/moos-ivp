/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ObShipModel.h                                        */
/*    DATE: Sep 6th, 2019                                        */
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

#ifndef OB_SHIP_MODEL_HEADER
#define OB_SHIP_MODEL_HEADER

#include <vector>
#include "XYPolygon.h"

class ObShipModel
{
 public:
  ObShipModel();
  ~ObShipModel() {};

 public: // setters
  bool   setPose(double osx, double osy, double osh, double osv=-1);
  bool   setPoseOSX(double osx);
  bool   setPoseOSY(double osy);
  bool   setPoseOSH(double osh);
  bool   setPoseOSV(double osv);

 public: // Setters that may generate health warnings
  std::string  setObstacle(XYPolygon);
  std::string  setObstacle(std::string);
  std::string  setPwtInnerDist(double);
  std::string  setPwtOuterDist(double);
  std::string  setCompletedDist(double);
  std::string  setMinUtilCPA(double);
  std::string  setMaxUtilCPA(double);
  std::string  setAllowableTTC(double);

  void   print(std::string key="") const;

 public: // getters (of things that can be set)
  double getOSX() const            {return(m_osx);}
  double getOSY() const            {return(m_osy);}
  double getOSV() const            {return(m_osv);}
  double getOSH() const            {return(m_osh);}
  double getPwtInnerDist() const   {return(m_pwt_inner_dist);}
  double getPwtOuterDist() const   {return(m_pwt_outer_dist);}
  double getMinUtilCPA() const     {return(m_min_util_cpa);}
  double getMaxUtilCPA() const     {return(m_max_util_cpa);}
  double getMinUtilCPAFlex() const {return(m_min_util_cpa_flex);}
  double getMaxUtilCPAFlex() const {return(m_max_util_cpa_flex);}
  double getAllowableTTC() const   {return(m_allowable_ttc);}
  double getCompletedDist() const  {return(m_completed_dist);}
  XYPolygon getObstacle() const    {return(m_obstacle);}

 public: // analyzers (getters of things calculated)
  double getObcentX() const        {return(m_cx);}
  double getObcentY() const        {return(m_cy);}
  double getObcentBng() const      {return(m_obcent_bng);};
  double getObcentRelBng() const   {return(m_obcent_relbng);}

  XYPolygon getObstacleBuffMin() const {return(m_obstacle_buff_min);}
  XYPolygon getObstacleBuffMax() const {return(m_obstacle_buff_max);}
  std::string getPassingSide() const   {return(m_passing_side);}

  double getRange() const           {return(m_range);}
  double getRangeInOSH() const      {return(m_range_in_osh);}
  double getThetaW() const          {return(m_theta_w);}
  double getThetaB() const          {return(m_theta_b);}

  double getBngMinToPoly() const    {return(m_bng_min_to_poly);}
  double getBngMaxToPoly() const    {return(m_bng_max_to_poly);}
  double getCPABngMinToPoly() const {return(m_cpa_bng_min_to_poly);}
  double getCPABngMaxToPoly() const {return(m_cpa_bng_max_to_poly);}

  double getBngMinDistToPoly() const  {return(m_bng_min_dist_to_poly);}
  double getBngMaxDistToPoly() const  {return(m_bng_max_dist_to_poly);}
  double getCPABngMinDistToPoly() const {return(m_cpa_bng_min_dist_to_poly);}
  double getCPABngMaxDistToPoly() const {return(m_cpa_bng_max_dist_to_poly);}

  double getCPAInOSH() const {return(m_cpa_in_osh);}

  bool   paramIsSet(std::string) const;
  bool   ownshipInObstacle() const;
  bool   ownshipInObstacleBuffMin() const;
  bool   ownshipInObstacleBuffMax() const;
  bool   osHdgInPlatMajor(double osh) const;
  bool   osHdgInBasinMajor(double osh, bool verbose=false) const;
  bool   osSpdInPlatMinor(double osv) const;
  bool   osHdgSpdInBasinMinor(double osh, double osv) const;

  std::string getFailedExpandPolyStr(bool clear=true);

  std::string getObstacleLabel() const {return(m_obstacle.get_label());}
  
  double getRangeRelevance() const;
  bool   isObstacleAft(double xbng=0) const;
  bool   isValid() const;
  
 protected:
  bool   updateDynamic(); 
   
 private: // Config (set) variables
  double  m_osx;
  double  m_osy;
  double  m_osh;
  double  m_osv;

  double  m_min_util_cpa;
  double  m_max_util_cpa;
  double  m_min_util_cpa_flex;
  double  m_max_util_cpa_flex;
  double  m_pwt_inner_dist;
  double  m_pwt_outer_dist;
  double  m_allowable_ttc;
  double  m_completed_dist;
  
  XYPolygon m_obstacle;

  std::set<std::string> m_set_params;
  
 private: // State (derived) variables
  XYPolygon m_obstacle_buff_min;
  XYPolygon m_obstacle_buff_max;

  double m_cx;
  double m_cy;

  double m_obcent_bng;
  double m_obcent_relbng;

  std::string m_failed_expand_poly_str;
  
  std::string m_passing_side;

  double m_range;
  double m_range_in_osh;
  double m_cpa_in_osh;

  double m_osh180;
  double m_theta_w;
  double m_theta_b;  

  double m_bng_min_to_poly;
  double m_bng_max_to_poly;
  double m_cpa_bng_min_to_poly;
  double m_cpa_bng_max_to_poly;

  double m_bng_min_dist_to_poly;
  double m_bng_max_dist_to_poly;
  double m_cpa_bng_min_dist_to_poly;
  double m_cpa_bng_max_dist_to_poly;  
};

#endif 

