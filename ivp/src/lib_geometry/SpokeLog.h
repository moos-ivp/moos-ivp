/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: SpokeLog.h                                           */
/*    DATE: Dec 28th 2025                                        */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef SPOKE_LOG_HEADER
#define SPOKE_LOG_HEADER

#include <vector>
#include "SpokeLog.h"

class SpokeLog {
public:
  SpokeLog(std::string id="");
  ~SpokeLog() {}

public: // Group 1 Setters
  void setID(std::string s) {m_id=s;}
  void setDist(double dval) {m_dist_from_os=dval;}
  
public: // Group 1 Getters
  std::string getID() const {return(m_id);}
  double      getDist() const {return(m_dist_from_os);}
  
public: // Group 2 Setters
  void  setCPA(double v) {m_cpa=v;}
  void  setTimeCPA(double v) {m_time_cpa=v;}

  void  setPassesCN(bool bval) {m_passes_cn=bval;}
  void  setPassesCNPort(bool bval) {m_passes_cn_port=bval;}
  void  setPassesCNStar(bool bval) {m_passes_cn_star=bval;}
  void  setPassesCNPortDist(double v) {m_passes_cn_port_dist=v;}
  void  setPassesCNStarDist(double v) {m_passes_cn_star_dist=v;}
  void  setPassesCNSpd(double v) {m_passes_cn_spd=v;}

  void  setCrossesCN(bool bval) {m_crosses_cn=bval;}
  void  setCrossesCNBow(bool bval) {m_crosses_cn_bow=bval;}
  void  setCrossesCNStern(bool bval) {m_crosses_cn_stern=bval;}
  void  setCrossesCNBowDist(double v) {m_crosses_cn_bow_dist=v;}
  void  setCrossesCNSternDist(double v) {m_crosses_cn_stern_dist=v;}
  void  setCrossesCNSpd(double v) {m_crosses_cn_spd=v;}

public: // Group 2 Getters
  double cpa() const {return(m_cpa);}
  double timeCPA() const {return(m_time_cpa);}

  bool   passesCN() const {return(m_passes_cn);}
  bool   passesCNPort() const {return(m_passes_cn_port);}
  bool   passesCNStar() const {return(m_passes_cn_star);}
  double passesCNPortDist() const {return(m_passes_cn_port_dist);}
  double passesCNStarDist() const {return(m_passes_cn_star_dist);}
  double passesCNSpd() const {return(m_passes_cn_spd);}

  bool   crossesCN() const {return(m_crosses_cn);}
  bool   crossesCNBow() const {return(m_crosses_cn_bow);}
  bool   crossesCNStern() const {return(m_crosses_cn_stern);}
  double crossesCNBowDist() const {return(m_crosses_cn_bow_dist);}
  double crossesCNSternDist() const {return(m_crosses_cn_stern_dist);}
  double crossesCNSpd() const {return(m_crosses_cn_spd);}

public:
  void   print();
  
private: // Group 1 (intermediate values)
  std::string m_id;      
  double m_dist_from_os;

private: // Group 2 (primary values)
  double m_cpa;
  double m_time_cpa;

  bool   m_passes_cn;
  bool   m_passes_cn_port;
  bool   m_passes_cn_star;
  double m_passes_cn_port_dist;
  double m_passes_cn_star_dist;
  double m_passes_cn_spd;

  
  bool   m_crosses_cn;
  bool   m_crosses_cn_bow;
  bool   m_crosses_cn_stern;
  bool   m_crosses_cn_bow_dist;
  bool   m_crosses_cn_stern_dist;
  double m_crosses_cn_spd;
};

#endif

