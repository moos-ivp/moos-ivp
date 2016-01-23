/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_KCache.h                                   */
/*    DATE: April 23rd 2015                                      */
/*****************************************************************/

#ifndef POPULATOR_KCACHE_HEADER
#define POPULATOR_KCACHE_HEADER

#include <string>
#include "KCache.h"
#include "AVK_Table.h"

class Populator_KCache {

public:
  Populator_KCache() {};
  virtual ~Populator_KCache() {};

  bool populate(std::string filename);

  KCache getKCache() const;

protected:
  
  AVK_Table m_avk_table;

};

#endif
