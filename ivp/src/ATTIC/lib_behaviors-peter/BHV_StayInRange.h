#ifndef __BHV_StayInRange_h__
#define __BHV_StayInRange_h__

#include <string>
#include <cmath>
#include <vector>
#include "IvPBehavior.h"
#include "Target.h"

/**
 * Simple macro that uses debug flag from .bhv file to determine if to print 
 * debug information.
 */
#define DEBUG if(debug) postDebugInfo()

using namespace std;

/** 
 * Simple behavior that will keep the vehicle in a predefined range to
 * its target.
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
 * Created on: June 12 2005
 *
 * $Id: BHV_StayInRange.h,v 1.1 2007/05/01 13:13:05 mikerb Exp $
 */
class BHV_StayInRange: public IvPBehavior {
  public:
  /**
   * Constructor.
   *
   * @param domain an \em Interval \em Programming (IvP) domain.
   */
  BHV_StayInRange(IvPDomain domain);

  /**
   * Destructor.
   */
  ~BHV_StayInRange() {};
  
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

protected:
  
  /**
   * Calculets the priority for the behavior depending on the distance to the
   * target. If targetRange is grater then targetRangeMax then the priority is
   * set to the priority weight paramater -- priority_wt -- which is in the
   * range [0,100]. The priority then drops linearly when targetRange is in
   * [targetRangeMin,targetRangeMax] and is 0 for targetRange < targetRangeMin.
   *
   * @return the priority in the range of [0,priority_wt]
   */
  double getPriority();

  /**
   * Checks if if we have reached the halfway mark for the interval
   * [\a targetRangeMin \a targetRangeMax].
   *
   * @return true if we have passed the halfway mark, false otherwise.
   */
  bool isAtHalfwayMark();


  /**
   * Finds the all neighbours in the given range.
   *
   * All vehicle names registered in the moos environment are inspected and
   * neighbours are selected based on their name. For instance if all the
   * kayaks are named kayak1, kayak2, ... kayakn, the neighbours to kayak1 will
   * be those with name kayakn and range < rangeRadious.
   *
   * @param rangeRadious vehicle that have a range < rangeRadious will be looked
   * at.
   * 
   * TBD now only looking at vehicle with same name, might want to change that.
   * String comparison is also done by the first 3 chars...
   */
  void findNeighbours(double rangeRadious);
  
  /**
   * Used when useTargetRanging is set true. Utilizes the MOOSDB variable
   * PING_TIME set by pACom to compute the distance and 3D location of the
   * target. At least four sources are needed for this to work.\n
   * \n
   * Based on localization code from Carrick Detweiler. If something is not
   * working -- blame him.\n
   * \n
   * This method will set the value of targetRange and the coordinates variables
   * of the target object.
   *
   * @return false if the localization failed, true otherwise.
   */
   bool localizeTarget();
  
  /**
   * Checks if any of the neighbours are closer to the target.
   *
   * @return true if no one else is closer, false otherwise.
   */
   bool amInearestTarget();

  /**
   * Sets the target name according to what is specified in the moos file. If a
   * number is given then the target name is set to that corresponding
   * neighbour. For instance, if target = 1 in the moos file the first name in
   * the neighbour vector will be used, i.e the closes neighbour. If no valid
   * target name is found the name used in the moos file will be used. 
   *
   * @return the current target name.
   */
   string findTargetName();
   
  /**
   * Posts relevant information about the vehicle and it's targets as MOOS
   * variables.
   */
  void postDebugInfo();
  
private:
  /**
   * Unique vehicle ID.
   */
  string ID;

  /**
   * The Current state of the vehicle.
   *
   * The vehicle can be in one of following states:
   * \par
   * CHILLIN -- stopped \n
   * CREEPING -- moving towards target \n
   * GUNNIN -- moving away from target \n
   *
   * The different states are used for evaluating if the vehicle has moved to
   * the desired interval location.
   */
  enum VehicleState {
    CHILLIN,
    CREEPING, // moving toward target
    GUNNIN // moving away
  } state;

  /**
   * Neighbour information placeholder used by the neighbours vector.
   */
  struct Neighbour {
    /// The name or ID of the vehicle.
    string ID;
    /// Current range.
    double range;
    
    /// Clears ID and sets range to INFINITY.
    void clear() {
      ID = "";
      range = INFINITY;
    }
  };

  /**
   * Information about close by neighbours is stored here with the closest one
   * first. See findNeighbours().
   */
  vector<Neighbour > neighbours;
  
  /**
   * Vehicle global x coordinate.
   */
  double x;
  
  /**
   * Vehicle global y coordinate.
   */
  double y;
  
  /**
   * Vehicle speed.
   */
  double speed;
  
  /** 
   * Vehicle heading.
   */
  double heading;
  
  /**
   * The current target.
   */
  Target* target;

  /**
   * Current target range.
   */
  double targetRange;

  /**
   * Minimum range to keep to target.
   */
  double targetRangeMin;

  /**
   * Maximum range to keep from target.
   */
  double targetRangeMax;

  /**
   * Move vehicle to halfway mark when outside of target interval.
   */
  bool useHalfwayMark;

  /**
   * If set true the target position will be computed via the ping time.
   */
  bool useTargetPositioning;

  /**
   * Search radius in with to look for neighbours.
   */
  double neighbourSearchRadius;

  /**
   * Flag that determines if to output debug information.
   */
  bool debug;

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
