/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BehaviorRegistry.h                                   */
/*    DATE: May 11th 2011                                        */
/*****************************************************************/

#ifndef BEHAVIOR_REGISTRY_HEADER
#define BEHAVIOR_REGISTRY_HEADER

#include <string>
#include <vector>
#include "IvPFunction.h"

class BehaviorRegistry {
public:
  BehaviorRegistry();
  virtual ~BehaviorRegistry() {};
  
  bool addIPF(IvPFunction*, unsigned int, std::string="");
  
  unsigned int size() const    {return(m_keys.size());};
  bool         isEmpty() const {return(m_keys.empty());};

  IvPFunction *getIPF(unsigned int index) const;
  IvPFunction *getIPF(std::string key) const;

 protected:
  std::vector<std::string>  m_keys;
  std::vector<IvPFunction*> m_ipfs;
  std::vector<unsigned int> m_iter;

  bool  m_keys_unique;
};

#endif 
