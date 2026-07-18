/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SurveyGenerator.h                                    */
/*    DATE: August 15th, 2020                                    */
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

#ifndef SURVEY_GENERATOR_HEADER
#define SURVEY_GENERATOR_HEADER

#include <vector>
#include <string>
#include "XYPoint.h"
#include "XYSegList.h"

class SurveyGenerator 
{
 public:
  SurveyGenerator();
  ~SurveyGenerator() {};
  
  void generate();
  
  void setRegion(XYPolygon);
  void setLaneWidth(double);
  void setLaneAngle(double);
  void setEntryPoint(double x1, double y1);

  void setPackLanes(bool v) {m_pack_lanes=v;}
  void setDropLane(bool v)  {m_drop_lane=v;}
  void setCoreWidth(bool v) {m_core_width=v;}
  void setAutoShift(bool v) {m_auto_shift=v;}
  void setAutoDense(bool v) {m_auto_dense=v;}

  void togglePackLanes() {m_pack_lanes = !m_pack_lanes;}
  void toggleDropLane()  {m_drop_lane  = !m_drop_lane;}
  void toggleCoreWidth() {m_core_width = !m_core_width;}
  void toggleAutoShift() {m_auto_shift = !m_auto_shift;}
  void toggleAutoDense() {m_auto_dense = !m_auto_dense;}
  
  bool   getPackLanes()  const {return(m_pack_lanes);}
  bool   getDropLane()   const {return(m_drop_lane);}
  bool   getCoreWidth()  const {return(m_core_width);}
  bool   getAutoShift()  const {return(m_auto_shift);}
  bool   getAutoDense()  const {return(m_auto_dense);}
  double getLaneAngle()  const {return(m_lane_angle);}
  double getLaneWidth()  const {return(m_lane_width);}
  double getLaneWidthX() const {return(m_lane_widthx);}

  std::string  getRegionSpec() const {return(m_region.get_spec());}
  std::string  getSurveySpec() const {return(m_survey.get_spec());}
  double       getSurveyLen() const  {return(m_survey.length());}
  unsigned int getLaneCount() const;
  
  XYPolygon getRegion() const {return(m_region);}
  XYSegList getSurvey() const {return(m_survey);}
  XYPolygon getReHull() const {return(m_rehull);}

  unsigned int size() const {return(m_survey.size());}

  bool valid() const;

 protected:
  void generateBlock();
  void packLanes();
  void adjustFirstLane();
  void adjustLastLane();
  
 protected:

  double m_start_x;
  double m_start_y;
  bool   m_start_x_set;
  bool   m_start_y_set;

  double m_lane_angle;
  double m_lane_width;
  double m_lane_widthx;
  
  XYPolygon m_region;
  XYSegList m_survey;
  XYPolygon m_rehull;
  
  bool   m_pack_lanes;
  bool   m_drop_lane;
  bool   m_auto_shift;
  bool   m_auto_dense;
  bool   m_core_width;
};

#endif 

