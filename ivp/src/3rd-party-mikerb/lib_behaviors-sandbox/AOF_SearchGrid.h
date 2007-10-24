/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AOF_SearchGrid.h                                     */
/*    DATE: Dec 13th, 2006                                       */
/*****************************************************************/
 
#ifndef AOF_SEARCH_GRID_HEADER
#define AOF_SEARCH_GRID_HEADER

#include "AOF.h"
#include "IvPDomain.h"
#include "XYGrid.h"
#include "Timutif.h"

class AOF_SearchGrid: public AOF {
 public:
  AOF_SearchGrid(IvPDomain, const XYGrid*);
  ~AOF_SearchGrid() {};

public: // virtual functions
  double evalBox(const IvPBox*) const; // virtual defined
  bool   setParam(const std::string&, double);
  bool   setParam(const std::string&, const std::string&);
  bool   initialize();
  void   fillCache();

protected:
  const XYGrid *search_grid;
  
  Timutif timutif;

  double  os_lat;
  double  os_lon;
  double  time_horizon;

  bool    os_lat_set;
  bool    os_lon_set;

  int     crs_ix;  // Index of "course" variable in IvPDomain
  int     spd_ix;  // Index of "speed" variable in IvPDomain

  std::vector<std::vector<int> > ray_cache;
};

#endif






