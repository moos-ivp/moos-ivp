/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_BehaviorSet.h                              */
/*    DATE: Jan 27 2005 NICOP funding approved by D.Wagner       */
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

#ifndef POPULATOR_BEHAVIORSET2_HEADER
#define POPULATOR_BEHAVIORSET2_HEADER

#include "BehaviorFactory.h"
#include "Populator_BehaviorSet.h"

/**
   This class introduces dynamically loaded behaviors.  When a behavior name is
   encountered in a behavior file, the superclass (Populator_BehaviorSet) is
   given an opportunity to instantiate that behavior.  This will generally work
   only when the behavior is one of those that's statically linked to the 
   application.

   If the behavior name isn't recognized by the superclass, then this class
   will try to instantiate the behavior based on behavior libraries (shared 
   objects) that this class has dynamically loaded.
*/
class Populator_BehaviorSet2 : public Populator_BehaviorSet {
public:
  Populator_BehaviorSet2(IvPDomain, InfoBuffer*);
  ~Populator_BehaviorSet2() {};

  void load_behavior_libs(std::string dirname);

  void loadEnvVarDirectories(std::string envVar, bool verbose);

protected:
  virtual IvPBehavior *initializeBehavior(std::string);

//   void loadBehaviors(const std::string & lib_directory);

protected:
  BehaviorFactory bhv_factory;
};
#endif
