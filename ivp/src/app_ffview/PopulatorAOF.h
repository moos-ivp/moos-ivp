/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PopulatorAOF.h                                       */
/*    DATE: Jan 14th 2006                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef POPULATOR_AOF_HEADER
#define POPULATOR_AOF_HEADER

#include <string>
#include "IvPDomain.h"
#include "AOF.h"

class PopulatorAOF
{
public:
  PopulatorAOF()  {define_mode=0; aof=0;};
  ~PopulatorAOF() {};

  AOF* populate(std::string filename);

protected:
  bool handleLine(std::string);
  bool handleEntry(std::string, std::string, std::string);

  bool buildDomain(std::string);
  
protected:
  int        define_mode;
  AOF*       aof;
  IvPDomain  domain;
};
#endif





