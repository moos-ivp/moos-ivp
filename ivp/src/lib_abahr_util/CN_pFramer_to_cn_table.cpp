//
// HISTORY:
// ----------------------------------------------------------------------------------------------------------------------
// 10/8/2006        abahr       created
// 17/8/2006	   abahr       changed style of cn matrix col 3-5 and col 9-11

#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "MOOSGeodesy.h"
#include "CN.h"
#include "MU.h"
using namespace std;


void CN_pFramer_to_cn_table(double my_x, double my_y, double my_cov_xx, double my_cov_xy, double my_cov_yx, double my_cov_yy, string new_frame, double owt, double speed_of_sound, double dfLatOrigin, double dfLongOrigin, gsl_matrix **cn, bool *cn_init, int max_length, bool debug)
{
	double NodeLatitude;
	double NodeLongitude;
	double NodeDepth;
	double NodeCEP;
	double SourcePlatformId;
	
	double dfYLocal;
	double dfXLocal;
	
	double ov_x, ov_y, ov_cov_xx, ov_cov_xy, ov_cov_yx, ov_cov_yy; 
	int ov_id;
	double range;
	
	// extract information from Framer string
	MU_get_from_comma_string(new_frame , "NodeLatitude", &NodeLatitude, debug );
	MU_get_from_comma_string(new_frame , "NodeLongitude", &NodeLongitude, debug );
	MU_get_from_comma_string(new_frame , "NodeDepth", &NodeDepth, debug);
	MU_get_from_comma_string(new_frame , "NodeCEP", &NodeCEP, debug);
	MU_get_from_comma_string(new_frame , "SourcePlatformId", &SourcePlatformId, debug);

	CMOOSGeodesy geodesy;

	if (!geodesy.Initialise(dfLatOrigin, dfLongOrigin))
	{
		MOOSTrace("Geodesy Init failed - FAIL\n");
	}
	
	geodesy.LatLong2LocalGrid(NodeLatitude, NodeLongitude, dfYLocal, dfXLocal);
	
	if(debug)
	{
		MOOSTrace("CN_pFramer_to_cn_table: The local coordinates are X=%6.4lf; Y=%6.4lf\n",dfYLocal, dfXLocal);
	}
		
	// compute entries into cn table
	
	// estimated position of other vehicle
	ov_x=dfXLocal;
	ov_y=dfYLocal;
	
	// covariance from CEP	
	ov_cov_xx=pow(NodeCEP,2);
	ov_cov_yy=pow(NodeCEP,2);
	
	ov_cov_xy=0;
	ov_cov_yx=0;
	
	// range from one-way travel time
	range=owt*speed_of_sound;
	
	// other vehicle's ID
	ov_id=int(SourcePlatformId);
	
	if(debug) MOOSTrace("\nCN_pFramer_to_cn_table: CN-Step 2: Adding new entry to CN table\n");
	if(debug)
    {
    	if(*cn_init)
    	{
    		MOOSTrace("CN_pFramer_to_cn_table: cn is intialized\n");
    	}
    	else
    	{
    		MOOSTrace("CN_pFramer_to_cn_table: cn is NOT initialized\n");
    	}
    }
	
	CN_add_to_cn_table(cn, cn_init, max_length, my_x, my_y, my_cov_xx, my_cov_xy, my_cov_yx, my_cov_yy, ov_x, ov_y, ov_cov_xx, ov_cov_xy, ov_cov_yx, ov_cov_yy, ov_id, range, debug);
}
