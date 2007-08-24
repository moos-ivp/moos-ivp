/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PopulatorTSet.h                                      */
/*    DATE: August 17th, 2007                                    */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef POPULATOR_TSET_HEADER
#define POPULATOR_TSET_HEADER

#include <string>
#include <vector>

class PopulatorTSet
{
public:
  PopulatorTSet() {};
  virtual ~PopulatorTSet() {};
  
  std::vector<TrajectorySet> populate(std::string filename);

protected:
  bool handleLine(const std::string);

protected:
  std::vector<TrajectorySet> m_vtset;

};

#endif




