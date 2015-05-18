/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ConvexHull.h                                         */
/*    DATE: Aug 26th 2014                                        */
/*****************************************************************/

#ifndef P_CONVEX_HULL_GENERATOR_HEADER
#define P_CONVEX_HULL_GENERATOR_HEADER

#include <list>
#include <vector>
#include "XYPoint.h"
#include "XYPolygon.h"

class ConvexHullGenerator
{
 public:
  ConvexHullGenerator() {}
  ~ConvexHullGenerator() {}

   void addPoint(double, double);
   void addPoint(double, double, std::string);

   XYPolygon generateConvexHull();
   XYPoint   getRootPoint() {return(m_root);}

 protected:
   void   findRoot();
   void   sortPoints();

 private: // Configuration variables
   std::vector<XYPoint>  m_original_pts;

 private: // State variables
   XYPoint               m_root;
   std::vector<XYPoint>  m_points;
   
   std::list<XYPoint>    m_stack;
};

#endif 
