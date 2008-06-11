/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_BehaviorSet2.h                             */
/*    DATE: May 2008                                             */
/*****************************************************************/

#ifndef POPULATOR_BEHAVIORSET2_HEADER
#define POPULATOR_BEHAVIORSET2_HEADER

#include "BehaviorFactory.h"
#include "Populator_BehaviorSet.h"

//  This class introduces dynamically loaded behaviors.  When a 
//  behavior name is encountered in a behavior file, the superclass 
//  (Populator_BehaviorSet) is given an opportunity to instantiate 
//  that behavior.  This will generally work only when the behavior 
//  is one of those that's statically linked to the application.

//  If the behavior name isn't recognized by the superclass, then 
//  this class will try to instantiate the behavior based on behavior 
//  libraries (shared objects) that this class has dynamically loaded.

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
