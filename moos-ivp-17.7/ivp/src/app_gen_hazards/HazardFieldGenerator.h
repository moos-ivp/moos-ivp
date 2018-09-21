/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardFieldGenerator.h                               */
/*    DATE: Jan 27th, 2012                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef HAZARD_FIELD_GENERATOR_HEADER
#define HAZARD_FIELD_GENERATOR_HEADER

#include <vector>
#include <string>
#include <set>
#include "XYFieldGenerator.h"
#include "XYPolygon.h"
#include "XYFormatUtilsPoly.h"

class HazardFieldGenerator
{
 public:
  HazardFieldGenerator();
  virtual ~HazardFieldGenerator() {}

  bool   addPolygon(std::string s) {return(m_field_generator.addPolygon(s));}
  bool   addObjectSet(std::string);

  bool   setResemblanceExp(std::string str);
  bool   setAspectBase(std::string str);
  bool   setAspectRange(std::string str);
  bool   setAspectMinBase(std::string str);
  bool   setAspectMinRange(std::string str);
  bool   setAspectMaxBase(std::string str);
  bool   setAspectMaxRange(std::string str);

  bool   generate();

 protected: // local utility functions
  std::string generateRandomUniqueLabel();

 protected:
  bool generateObjectSet(unsigned int amt, std::string obj_type);

 protected: // Config variables
  std::vector<unsigned int> m_obj_set_amt;
  std::vector<std::string>  m_obj_set_type;
  double                    m_resemblance_exp;

  double                    m_aspect_base;
  double                    m_aspect_range;
  double                    m_aspect_min_base;
  double                    m_aspect_min_range;
  double                    m_aspect_max_base;
  double                    m_aspect_max_range;

  bool                      m_aspect_inplay;

 protected: // State variables
  XYFieldGenerator          m_field_generator;
  double                    m_pt_step;

  std::set<std::string>     m_rand_labels;
};

#endif 








