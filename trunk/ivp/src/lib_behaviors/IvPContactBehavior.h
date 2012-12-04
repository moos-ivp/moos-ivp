/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_AvoidCollision.h                                 */
/*    DATE: Nov 18th 2006                                        */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/
 
#ifndef IVP_CONTACT_BEHAVIOR_HEADER
#define IVP_CONTACT_BEHAVIOR_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"
#include "LinearExtrapolator.h"
#include "XYSegList.h"

class IvPDomain;
class IvPContactBehavior : public IvPBehavior {
public:
  IvPContactBehavior(IvPDomain);
  ~IvPContactBehavior() {};

  bool  setParam(std::string, std::string);

 protected:
  bool  updatePlatformInfo();
  void  postViewableBearingLine();
  void  postErasableBearingLine();
  
 protected: // Configuration Parameters
  
  bool   m_on_no_contact_ok;  // true if no trouble reported on no contact
  bool   m_extrapolate;
  double m_decay_start;
  double m_decay_end;
  double m_time_on_leg;
  
  XYSegList                m_bearing_line;
  std::vector<std::string> m_bearing_line_colors;
  std::vector<double>      m_bearing_line_thresh;
  std::string              m_bearing_line_info;
  bool                     m_bearing_line_show;

 protected:  // State Variables
  
  double m_osx;   // Current ownship x position (meters) 
  double m_osy;   // Current ownship y position (meters) 
  double m_osh;   // Current ownship heading (degrees 0-359)
  double m_osv;   // Current ownship speed (meters) 

  double m_cnx;   // Current contact x position (meters) 
  double m_cny;   // Current contact y position (meters) 
  double m_cnh;   // Current contact heading (degrees 0-359)
  double m_cnv;   // Current contact speed (meters) 
  double m_cnutc; // UTC time of last contact report

  double m_contact_range; // Current range to contact (meters) 
  double m_relevance;

  LinearExtrapolator m_extrapolator;
};
#endif


