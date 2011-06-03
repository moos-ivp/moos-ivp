/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVD_Table.h                                          */
/*    DATE: Feb 8th, 2009                                        */
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

#ifndef AVD_TABLE_HEADER
#define AVD_TABLE_HEADER

#include <vector>

class AVD_Table {
public:
  AVD_Table();
  ~AVD_Table() {};

  bool initialize(double max_heading_delta, double max_speed, 
		  unsigned int hdg_pts, unsigned int spd_pts, 
		  double default_radius);

  bool addMeasurement(double hdelta, double speed, double radius);

  double getRadius(double hdelta, double speed);

  unsigned int  tsize();

 protected:
  std::vector<std::vector<double> > m_table_value;
  std::vector<std::vector<bool> >   m_table_guess;

  double        m_max_heading_delta;
  double        m_max_speed;
  unsigned int  m_heading_points;
  unsigned int  m_speed_points;
  bool          m_initialized;
};

#endif

