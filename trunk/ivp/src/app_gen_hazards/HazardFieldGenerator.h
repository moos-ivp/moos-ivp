/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HazardFieldGenerator.h                               */
/*    DATE: Jan 27th, 2012                                       */
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
  HazardFieldGenerator() {m_pt_step=1; m_count=0; m_exp=0;};
  virtual ~HazardFieldGenerator() {};

  bool   addPolygon(std::string s) {return(m_field_generator.addPolygon(s));};
  bool   addObjectSet(std::string);

  void   setExp(std::string str);

  bool   generate();
  
  std::string generateRandomUniqueLabel();

 protected:
  bool generateObjectSet(unsigned int amt, std::string obj_type);

 protected: // Config variables
  std::vector<unsigned int> m_obj_set_amt;
  std::vector<std::string>  m_obj_set_type;

  XYFieldGenerator          m_field_generator;
  double                    m_pt_step;
  unsigned int              m_count;

  double                    m_exp;

  std::set<std::string>     m_rand_labels;


};

#endif 

