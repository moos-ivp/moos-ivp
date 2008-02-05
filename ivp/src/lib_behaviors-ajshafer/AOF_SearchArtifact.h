/*****************************************************************/
/*    NAME: Andrew Shafer, Mike Benjamin                         */
/*    ORGN: MIT                                                  */
/*    FILE: AOF_SearchArtifact.h                                 */
/*    DATE: NOV 28, 2007                                         */
/*****************************************************************/
 
#ifndef AOF_SEARCH_ARTIFACT_HEADER
#define AOF_SEARCH_ARTIFACT_HEADER

#include "AOF.h"
#include "IvPDomain.h"
#include "XYArtifactGrid.h"
#include "SearchPassValue.h"

class AOF_SearchArtifact: public AOF {
public:
	AOF_SearchArtifact(IvPDomain, const XYArtifactGrid*);
	~AOF_SearchArtifact() {};

public: // virtual functions
	double evalBox(const IvPBox*) const; // virtual defined
	bool   setParam(const std::string&, double);
	bool   setParam(const std::string&, const std::string&);
	bool   initialize();
	void   fillCache();

protected:
	const XYArtifactGrid *search_grid;

	SearchPassValue pass_value; // Used to get the "utility" from the time spend in a grid

	// Own ship values
	double  os_lat;
	double  os_lon;
	double  time_horizon;

	bool    os_lat_set;
	bool    os_lon_set;

	int     crs_ix;  // Index of "course" variable in IvPDomain
	int     spd_ix;  // Index of "speed" variable in IvPDomain
	
	// Each first level vector is a discrete course direction
	// Each sub vector holds the index to searchgrid cells that are contained
	// in the time horizon based on top-speed
	std::vector<std::vector<int> > ray_cache;
};

#endif






