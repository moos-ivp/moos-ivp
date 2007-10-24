/*****************************************************************/
/*    NAME: Paul Newman - Michael Benjamin                       */
/*    ORGN: NAVSEA Newport RI and Oxford                         */
/*    FILE: IvPBehaviorPlus.cpp                                  */
/*    DATE: Mar 30, 2007                                         */
/*****************************************************************/

#ifndef IVP_BEHAVIOR_PLUS_HEADER
#define IVP_BEHAVIOR_PLUS_HEADER

#include <string>
#include "IvPDomain.h"
#include "IvPBehavior.h"
#include "AOF_Plus.h"

class IvPBehaviorPlus : public IvPBehavior
{
  IvPBehaviorPlus(IvPDomain);
  
  static double _EvalCB(double *pActions, 
			int     nActions, 
			void*   pMeVoid)
    {
      IvPBehaviorPlus* pMe = (IvPBehaviorPlus*)(pMeVoid);
      return(pMe->EvaluateThis(pActions, nActions));
    };

  
  virtual  double       EvaluateThis(double*, int) = 0;
  virtual  IvPFunction* produceOF();
  virtual  bool         OnPreEvaluate() {return(true);};
  
  bool SetDimension(int n, const std::string& str);
  bool setParam(std::string s1,std::string s2);
  bool SetSubDomain(std::string sDomain);
  
  AOF_Plus* m_pAOF;
};

#endif
