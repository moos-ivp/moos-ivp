/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XYCommsPulse.h                                       */
/*    DATE: Dec 5th 2011                                         */
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

#ifndef XY_COMMS_PULSE_HEADER
#define XY_COMMS_PULSE_HEADER

#include <string>
#include <vector>
#include "XYObject.h"

class XYCommsPulse : public XYObject {
public:
  XYCommsPulse();
  XYCommsPulse(double sx, double sy, double tx, double ty);
  virtual ~XYCommsPulse() {}

  void   set_sx(double v)          {m_sx=v; m_sx_set=true;}
  void   set_sy(double v)          {m_sy=v; m_sy_set=true;}
  void   set_tx(double v)          {m_tx=v; m_tx_set=true;}
  void   set_ty(double v)          {m_ty=v; m_ty_set=true;}
  void   set_beam_width(double);
  void   set_duration(double);
  void   set_fill(double);
  void   set_pulse_type(std::string);
  
  double get_sx()    const        {return(m_sx);}
  double get_sy()    const        {return(m_sy);}
  double get_tx()    const        {return(m_sx);}
  double get_ty()    const        {return(m_sy);}
  double get_beam_width()  const  {return(m_beam_width);}
  double get_duration() const     {return(m_duration);}
  double get_fill() const         {return(m_fill);}
  double get_fill(double tstamp) const;

  bool   valid() const;

  std::string get_pulse_type() const {return(m_pulse_type);}

  std::string get_spec(std::string s="") const;

  std::vector<double> get_triangle(double timestamp) const;

 protected:
  void initialize();

protected:
  double    m_sx;
  double    m_sy;
  double    m_tx;
  double    m_ty;
  double    m_beam_width;
  double    m_duration;

  bool      m_sx_set;
  bool      m_sy_set;
  bool      m_tx_set;
  bool      m_ty_set;

  double    m_fill;

  std::string m_pulse_type;
};
#endif









