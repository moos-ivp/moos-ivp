#ifndef __AOF_StayInRange_h__
#define __AOF_StayInRange_h__

#include "IvPDomain.h"
#include "AOF.h"
#include "CPAEngine.h"

/**
 * Objective function (OF) generator for BHV_StayInRange. 
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * @author Michael R. Benjamin
 * @author Peter Osagie
 *
 * @version $Revision: 1.1 $
 *
 * @date $Date: 2007/05/01 13:13:05 $
 *
 * Contact: osagie@mit.edu
 *
 * Created June 14 2005
 *
 * $Id: AOF_StayInRange.h,v 1.1 2007/05/01 13:13:05 mikerb Exp $
 */
class AOF_StayInRange: public AOF {
  public:
  /**
   * Constructor.
   * @param domain an \em Interval \em Programming (IvP) domain.
   * @param gcnlat given contact latitude position.
   * @param gcnlon given contact longitude position.
   * @param gcncrs given contact course.
   * @param gcnspd given contact speed.
   * @param goslat given ownship latitude position.
   * @param goslon given ownship latitude position.
   */
  AOF_StayInRange(IvPDomain domain, double gcnlat, double gcnlon,
      double gcncrs, double gcnspd, double goslat, double goslon);
  
  /**
   * Destructor.
   */
  ~AOF_StayInRange();

  /**
   * Evaluates a given <tt> \<Course, Speed\> </tt> tuple given by a 2D ptBox.
   *
   * @param box an IvP 2D box.
   *
   * @return TBD
   */
  double evalBox(const IvPBox* box) const;
  /**
   * Depricated, use evalBox(Box* box) instead.
   *
   * @return TBD.
   */
  double evalBox(int*) {};
  
  protected:
  /**
   * TBD more doc.
   *
   * @param val TBD
   *
   * @return TBD
   */
  double metric(double val) const;

  /**
   * The smallest possible course change that can be made.
   */
  double courseDelta;

  /**
   * The lowest possible course value.
   */  
  double courseBase;

  /**
   * Total number of course points.
   */
  int coursePts;

  /**
   * The smallest possible speed change that can be made.
   */
  double speedDelta;
  
  /**
   * The lowest possible speed value.
   */
  double speedBase;

  /**
   * Total number of speed points.
   */
  int speedPts;

  /**
   * TBD more doc.
   */
  double tolDelta;
  
  /**
   * TBD more doc.
   */
  double tolBase;

  /**
   * TBD more doc.
   */
  int tolPts;
  
  /**
   * TBD more doc.
   */
   CPAEngine *cpa_engine;
};
#endif
