#ifndef __BHV_FollowLeader_h__
#define __BHV_FollowLeader_h__

#include <string>
#include "IvPBehavior.h"
#include "Target.h"

/**
 * Simple macro that uses debug flag from \c .bhv file to determine if to post
 * debug information into the MOOS community.
 */
#define DEBUG if(debug) postDebugInfo()

/**
 * Default pi.
 */
#ifndef PI
#define PI 3.14159
#endif

/**
 * Converts degrees to radians.
 */
#define toRadians(angle) angle*PI/180

using namespace std;

/** 
 * Simple behavior that will follow its target.
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
 * Created on: July 1 2005
 *
 * $Id: BHV_FollowLeader.h,v 1.1 2007/05/01 13:13:05 mikerb Exp $
 */
class BHV_FollowLeader: public IvPBehavior {
  public:
  /**
   * Constructor.
   *
   * @param domain an \em Interval \em Programming (IvP) domain.
   */
  BHV_FollowLeader(IvPDomain domain);

  /**
   * Destructor.
   */
  ~BHV_FollowLeader() {};
  
  /**
   * Creates an objective function.
   */  
  IvPFunction* produceOF();

  /**
   * Reads parameters from \c .bhv file.
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

  /**
   * TBD more doc.
   */
  double getRelevance();

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
   * Current range to target.
   */
  double targetRange;
  
  /**
   * The desired range to keep to the target.
   */
  double desiredTargetRange;

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
