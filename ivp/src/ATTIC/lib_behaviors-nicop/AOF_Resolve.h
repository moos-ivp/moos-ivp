/***********************************************************/
/*    NAME: M.Benjamin / P.Newman                          */
/*    FILE: AOF_Resolve.h                                  */
/*    DATE: Sep 7th 2005                                   */
/***********************************************************/
 
#ifndef AOF_RESOLVE_HEADER
#define AOF_RESOLVE_HEADER

#include "AOF.h"
#include "IvPDomain.h"
#include "XYGrid.h"
#include "Resolver.h"

class AOF_Resolve: public AOF {
 public:
  AOF_Resolve(IvPDomain, CResolver*);
  ~AOF_Resolve() {};

public: // virtuals defined
  double evalBox(const IvPBox*) const;
  bool   setParam(const std::string&, double);
  bool   initialize();
	
	

protected:
  double os_lat;   // Ownship Lat position
  double os_lon;   // Ownship Lon position

  bool   os_lat_set;
  bool   os_lon_set;

  CResolver* resolver;

  int    crs_ix;  // Index of "course" variable in IvPDomain
  int    spd_ix;  // Index of "speed" variable in IvPDomain
  int    tol_ix;  // Index of "tol" variable in IvPDomain
};

#endif
