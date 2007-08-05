#ifndef __BHV_ShadowHeading_h__
#define __BHV_ShadowHeading_h__

#include <string.h>
#include "IvPBehavior.h"
#include "Target.h"

using namespace std;

/** 
 * This behavior shadows a acquires a given target's heading and adjusts its own
 * to match that.
 *
 * It differs from the original behavior (BHV_Shadow by M. R. Benjamin) by
 * only applying the target's heading and not the speed.
 *
 * It uses three pieces in a range of 180 deg to cover the possible heading
 * decision space. See BHV_ShadowHeading.cpp for more info.
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
 * @author Peter Osagie
 *
 * @version $Revision: 1.1 $
 *
 * @date $Date: 2007/05/01 13:13:05 $
 *
 * Contact: osagie@mit.edu
 *
 * Created on: May 10 2005 by M. Benjamin \n
 * Revised on June 20 2005 by Peter Osagie
 *
 * $Id: BHV_ShadowHeading.h,v 1.1 2007/05/01 13:13:05 mikerb Exp $
 */
class BHV_ShadowHeading: public IvPBehavior {
  public:
  /**
   * Constructor.
   * @param domain an \em Interval \em Programming (IvP) domain.
   */
  BHV_ShadowHeading(IvPDomain domain);

  /**
   * Destructor.
   */
  ~BHV_ShadowHeading() {};
  
  /**
   * Creates an objective function.
   *
   * @note All state updates are done here.
   */  
  IvPFunction* produceOF();

  /**
   * Reads parameters from .bhv file.
   *
   * Is called repeatedly until all parameters are read within the block.
   *
   * @param param the current parameter parsed the .bhv file.
   * @param val the string value of that parameter.
   *
   * @return true if the parameter was found and successfully set, false
   * otherwise.
   */  
  bool setParam(string param, string val);
  
private:
  /**
   * Calculates the priority fo the behavior using the priority weight and the
   * current distance to the target.
   *
   * @return the priority.
   */
  double getPriority();

  /**
   * Unique vehicle ID.
   */
  string ID;
  
  /**
   * Vehicle global x coordinate.
   */
  double x;
  
  /**
   * Vehicle global y coordinate.
   */
  double y;

  /**
   * Vehicle heading.
   */
  double heading;
  
  /**
   * The current target.
   */
  Target* target;

  /**
   * Range to target.
   */
  double targetRange;

  /**
   * Minimum range to target where the behavior is active at highest priority.
   * The utility output for the behavior will have a maximim (equal to the priority
   * weight) in the range [0, targetRangeMin] and decrease linearly in the range 
   * [targetRangeMin,targetRangeMax].
   */
  double targetRangeMin;
  
  /**
   * The behavior disregard any target that has a range grater then this.
   * The utility output for the behavior will be 0 for any targetRange grater
   * then this value, decrease linearly in the range 
   * [targetRangeMin,targetRangeMax] and have a maximum (equal to the priority
   * weight) in the range [0, targetRangeMin].
   */
  double targetRangeMax;

  /**
   * x variable name specified in the configuration block for the behavior.
   */
  string xVariable;
  
  /**
   * y variable name specified in the configuration block for the behavior.
   */
  string yVariable;

  /**
   * speed variable name specified in the configuration block for the behavior.
   */
  string speedVariable;

  /**
   * heading variable name specified in the configuration block for the behavior.
   */
  string headingVariable;

  /**
   * Target x variable name specified in the configuration block for the behavior.
   */
  string targetXVariable;

  /**
   * Target y variable name specified in the configuration block for the behavior.
   */
  string targetYVariable;

  /**
   * Target speed variable name specified in the configuration block for the behavior.
   */
  string targetSpeedVariable;

  /**
   * Target heading variable name specified in the configuration block for the behavior.
   */
  string targetHeadingVariable;
};
#endif
