/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_Convoy.h                                         */
/*    DATE: Jul 28th 2018                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef BHV_CONVOY_HEADER
#define BHV_CONVOY_HEADER

#include <string>
#include <list>
#include "IvPContactBehavior.h"

class IvPDomain;
class BHV_Convoy : public IvPContactBehavior {
public:
  BHV_Convoy(IvPDomain);
  ~BHV_Convoy() {}
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onSetParamComplete();
  void         onRunToIdleState();
  void         onIdleState();
  void         onHelmStart();

protected:
  double  getRelevance();
  double  getPriority();

  void    handleNewContactSpd(double);
  bool    handleNewContactPos(double, double);
  bool    checkDropOldestMark();

  void    postPoint(double x, double y, unsigned int id);
  void    erasePoint(unsigned int id);
  void    updateVisuals() {};

  void    updateCurrTrailRange();
  void    updateSetSpeed();
  
  IvPFunction *buildOF();
  
private:
  bool    pushNewPoint(double x, double y, double timestamp=0);
  bool    popOldPoint();

  double  getOldestX() const    {return(m_cn_mark_x.back());}
  double  getOldestY() const    {return(m_cn_mark_y.back());}
  double  getOldestTime() const {return(m_cn_mark_time.back());}
  unsigned int getOldestID() const {return(m_cn_mark_id.back());}
  
  double  getNewestX() const    {return(m_cn_mark_x.front());}
  double  getNewestY() const    {return(m_cn_mark_y.front());}
  double  getNewestTime() const {return(m_cn_mark_time.front());}
  unsigned int getNewestID() const {return(m_cn_mark_id.front());}
  
  unsigned int getQueueLen() const {return(m_cn_mark_x.size());}
  
protected: // State variables
  std::list<double> m_cn_mark_x;
  std::list<double> m_cn_mark_y;
  std::list<double> m_cn_mark_time;
  std::list<unsigned int> m_cn_mark_id;

  std::list<double> m_cn_spd_value;
  std::list<double> m_cn_spd_tstamp;
  
  unsigned int m_next_id;

  double m_wptx;
  double m_wpty;
  double m_set_speed;
  
  double m_cnv_avg_2sec;
  double m_cnv_avg_5sec;
    
  double m_total_length;
  
private: // Configuration parameters
  double m_radius;
  double m_nm_radius;

  double m_cruise_speed;
  double m_spd_max;
  double m_spd_faster;
  double m_spd_slower;
  
  double m_inter_mark_range;
  double m_max_mark_range;

  unsigned int m_marks_up_bound;

  double m_rng_estop;
  double m_rng_tgating;
  double m_rng_lagging;
  
};
#endif

