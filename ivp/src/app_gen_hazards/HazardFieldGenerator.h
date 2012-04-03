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
