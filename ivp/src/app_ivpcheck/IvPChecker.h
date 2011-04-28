/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IvPChecker.h                                         */
/*    DATE: April 18th, 2011                                     */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef IVP_CHECKER_HEADER
#define IVP_CHECKER_HEADER

#include <vector>
#include "IvPFunction.h"
#include "QuadSet.h"

class IvPChecker
{
 public:
  IvPChecker() {};
  ~IvPChecker() {};
  
  bool checkIvP(const std::vector<std::string>& ipf_strings);
  void checkBrute(const std::vector<std::string>& ipf_strings);
  void checkCollectiveQuadSet(const QuadSet&);
  
 protected:
  void buildIvPDomain();
  void buildIvPFunctions(const std::vector<std::string>& ipf_strings);
  void clearIvPFunctions();

 protected:
  IvPDomain                  m_ivp_domain;
  std::vector<IvPFunction*>  m_ivp_functions;
};

#endif
