/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WormHole.h                                           */
/*    DATE: Jan 21st 2021                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef WORM_HOLE_HEADER
#define WORM_HOLE_HEADER

#include <string>
#include <vector>
#include <map>
#include "XYPolygon.h"

class WormHole
{
public:
  WormHole(std::string tag="");
  ~WormHole() {}

 public: // Setters
  bool   setMadridPoly(XYPolygon);
  bool   setWeberPoly(XYPolygon);  

  bool   setMadridPoly(double ctrx, double ctry_y);
  bool   setWeberPoly(double ctrx, double ctry_y);

  void   setTag(std::string s)       {m_tag=s;} 
  void   setDelay(double dval)       {m_delay = dval;}
  void   setIDChange(bool bval)      {m_id_change = bval;}
  void   setConnectionType(std::string);

 public: // Getters
  XYPolygon getMadridPoly() const  {return(m_madrid_poly);}
  XYPolygon getWeberPoly() const   {return(m_weber_poly);}

  std::vector<XYPolygon> getPolys() const;
  
  std::string getTag() const  {return(m_tag);}

  std::vector<std::string> getConfigSummary() const;

  std::string getConnectionType() const {return(m_connection);}
  
 public:
  void crossPositionWeberToMadrid(double wx, double wy, double& mx, double& my);
  void crossPositionMadridToWeber(double mx, double my, double& wx, double& wy);
  
 protected:
  void getCrossPosition(XYPolygon, XYPolygon, double, double, double&, double&);
  
protected:
  std::string m_tag;

  // Antipodes: Weber New Zealand, Madrid Spain
  XYPolygon m_madrid_poly;
  XYPolygon m_weber_poly;

  std::string m_connection;

  double      m_delay;
  bool        m_id_change;
};

#endif 





