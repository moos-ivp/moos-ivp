/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WormHoleSet.h                                        */
/*    DATE: Jan 21st 2021                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef WORM_HOLE_SET_HEADER
#define WORM_HOLE_SET_HEADER

#include <string>
#include <vector>
#include "WormHole.h"

class WormHoleSet
{
public:
  WormHoleSet();
  ~WormHoleSet() {}

 public: // Setters
  bool   addWormHoleConfig(std::string);
  void   setTunnelTime(double);
  void   setTransparency(double);
  
 public: // Getters
  WormHole  getWormHole(unsigned int);
  WormHole  getWormHole(std::string);

  double getTransparency() const {return(m_transparency);}
  double getTunnelTime() const   {return(m_tunnel_time);}
  
  unsigned int size() const {return(m_worm_holes.size());}

  std::vector<std::string> getConfigSummary() const;

 public: // Application  

  bool apply(double curr_time, double osx, double osy,
	     double& newx, double& newy);

 protected:
  std::string findWormHoleEntry(double osx, double osy);
  
  
 protected: // Config vars
  std::vector<WormHole>    m_worm_holes;
  std::vector<std::string> m_worm_hole_tags;
  double      m_tunnel_time;

 protected: // state vars (for particular vehicle)
  std::string m_worm_hole_state;
  std::string m_worm_hole_tag;
  double      m_worm_hole_tstamp;
  double      m_transparency;

  double      m_inx;
  double      m_iny;
};

#endif 





