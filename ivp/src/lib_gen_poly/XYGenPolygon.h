/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: XYGenPolygon.h                                       */
/*    DATE: Oct 16th, 2020                                       */
/*    DATE: Nov 27th, 2025                                       */
/*    DATE: Aug 7th, 2026                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/
 
#ifndef XY_GEN_POLYGON_HEADER
#define XY_GEN_POLYGON_HEADER

#include <vector>
#include "XYSeglr.h"
#include "XYSegList.h"
#include "XYPolygon.h"

class XYGenPolygon {
 public:
  XYGenPolygon() {}
  ~XYGenPolygon() {}

  bool   setGenPoly(XYSegList);
  bool   setGenPoly(XYSegList, std::vector<XYPolygon> polys);
  
  bool   contains(double px, double py) const;
  double dist_to_poly(double px, double py) const;
  double dist_to_poly(double x1, double y1, double x2, double y2) const;
  bool   line_intersects(double x1, double y1, double x2, double y2) const;

  void   shift_vert(double amt);
  void   shift_horz(double amt);
  double area() const;
  void   clear();
  void   clearCoverPolys();
  bool   valid() const {return(m_segl_border.size() != 0);}

  
  unsigned int sizeSegl() const  {return(m_segl_border.size());}
  unsigned int sizeCover() const {return(m_cover_polys.size());}
  
  std::vector<XYPolygon> getCoverPolys() const {return(m_cover_polys);}
  XYSegList getSegList() const {return(m_segl_border);}
  
  std::string get_spec() const;
  std::string get_spec_bdr() const;

  unsigned int getBorderPtsSize() const  {return(m_segl_border.size());}
  unsigned int getCoverPolyCount() const {return(m_cover_polys.size());}

  unsigned int getSeglSize() const  {return(m_segl_border.size());}
  unsigned int getPolyCount() const {return(m_cover_polys.size());}

public:
  double   distPtToExitGP(double px, double py) const;
  double   distSegToExitGP(double x1, double y1, double x2, double y2,
			   bool& exited) const;
  double   distSeglToExitGP(const XYSegList&, bool& exited) const;
  double   distRayToExitGP(double rx, double ry, double ray_angle) const;

  double   distSeglrToExitGP(const XYSeglr& seglr) const;  
  
  double   distPtToEnterGP(double px, double py) const;
  double   distRayToEnterGP(double px, double py, double ray_angle) const;

  double   cpaSegToGP(double x1, double y1, double x2, double y2) const;
  double   cpaSeglToGP(const XYSegList&, bool verbose=false) const;
  double   cpaSeglrToGP(const XYSeglr&, double dist, bool verbose=false) const;
  
protected:
  XYSegList m_segl_border; 

  std::vector<XYPolygon> m_cover_polys;
};

XYGenPolygon stringToGenPoly(std::string);

#endif
