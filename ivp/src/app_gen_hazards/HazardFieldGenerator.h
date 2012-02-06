/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: HazardFieldGenerator.h                               */
/*   DATE: Jan 27th, 2012                                       */
/****************************************************************/

#ifndef HAZARD_FIELD_GENERATOR_HEADER
#define HAZARD_FIELD_GENERATOR_HEADER

#include <vector>
#include <string>
#include "XYFieldGenerator.h"
#include "XYPolygon.h"
#include "XYFormatUtilsPoly.h"

class HazardFieldGenerator
{
 public:
  HazardFieldGenerator() {};
  virtual ~HazardFieldGenerator() {};

  bool addPolygon(std::string s) {return(m_field_generator.addPolygon(s));};
  bool addObjectSet(std::string);

  bool generate();

 protected:
  bool generateObjectSet(unsigned int amt, std::string obj_type);

 protected: // Config variables
  std::vector<unsigned int> m_obj_set_amt;
  std::vector<std::string>  m_obj_set_type;

  XYFieldGenerator          m_field_generator;

};

#endif 
