/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: GPUtils.h                                            */
/*    DATE: Aug 07, 2026                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/
 
#ifndef GP_UTILS_HEADER
#define GP_UTILS_HEADER

#include <vector>
#include "XYSeglr.h"
#include "XYSegList.h"
#include "XYPolygon.h"

// Decently potentially useful functions. Created before another
// approach was decided on. Leaving them around in case useful in
// the future. Largely untested. 

double   distSegInPoly(double x1, double y1, double x2, double y2,
		       const XYPolygon& poly);

double   distSegListInPoly(const XYSegList& segl,
			   const XYPolygon& poly);

double   distRayInPoly(double rx, double ry, double ray_angle,
		       const XYPolygon& poly);

double   distSeglrInPoly(const XYSeglr& seglr,
			 const XYPolygon& poly);

#endif
