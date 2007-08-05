#ifndef __Target_h__
#define __Target_h__

#include <string>

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
 * Class to agregate all neccessary target information.
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
 * Created on: July 2 2005
 *
 * $Id: Target.h,v 1.1 2007/05/01 13:13:05 mikerb Exp $
 */
class Target {
  public:
  /**
   * Default constructor.
   */
  Target();

  /**
   * Constructor.
   */
  Target(string id);

  /**
   * Destructor.
   */
  ~Target();

  /**
   * Gets the target ID.
   *
   * @return the target ID.
   */
  string getID();

  /**
   * Sets the target ID.
   *
   * @param id the desired target ID to use.
   */
  void setID(string id);

  /**
   * Gets the target x coordinate.
   *
   * @return the x coordinate.
   */
  double getX() const;

  /**
   * Sets the target x coordinate.
   *
   * @param x the desired x coordinate to use.
   */
  void setX(double x);
  
  /**
   * Gets the target y coordinate.
   *
   * @return the y coordinate.
   */
  double getY() const;

  /**
   * Sets the target y coordinate.
   *
   * @param y the desired y coordinate to use.
   */
  void setY(double y);

  /**
   * Gets the target z coordinate.
   *
   * @return the z coordinate.
   */
  double getZ() const;

  /**
   * Sets the target z coordinate.
   *
   * @param z the desired y coordinate to use.
   */
  void setZ(double z);

  /**
   * Gets the target speed.
   *
   * @return the speed.
   */
  double getSpeed() const;

  /**
   * Sets the target speed.
   *
   * @param speed the desired speed to use.
   */
  void setSpeed(double speed);
  
  /**
   * Gets the target heading.
   *
   * @return the heading.
   */
  double getHeading() const;

  /**
   * Sets the target heading.
   *
   * @param heading the desired heading to use.
   */
  void setHeading(double heading);

  /**
   * Checks if range is less then the desired target range,
   * that is are we too close to the target and need to move away.
   *
   * @param targetRange the actual range to target in m.
   * @param desiredTargetRange the desired range to target in m.
   * 
   * @return true iff we are closer to the target range then the minimal range.
   */
  bool isTooClose(double targetRange, double desiredTargetRange) const;
  
  /**
   * Checks if the range is further away then the desired target range,
   * that is we are too far away from the target and need to move closer.
   *
   * @param targetRange the actual range to target in m.
   * @param desiredTargetRange the desired range to target in m.
   *
   * @return true iff we are further away to the target range then the maximal
   * range.
   */
  bool isTooFar(double targetRange, double desiredTargetRange) const;

  /**
   * Checks if the range is within 
   * [\a targetRangeMin \a targetRangeMax].
   * 
   * @param targetRange the actual range to target in m.
   * @param targetRangeMin the lower bound of the interval.
   * @param targetRangeMax the upper bound of the interval.
   *
   * @return true iff we are within target range else returns false.
   */
  bool isWithinRange(double targetRange, double targetRangeMin,
      double targetRangeMax) const;

  /**
   * Checks if the current position is to the right of the given coordinates and
   * the offset.
   *
   * @param x the x coordinate.
   * @param y the y coordinate.
   * @param offset the offset
   *
   * @return true if we are to the right of the given coordinates, false
   * otherwise.
   */
  bool isToTheRightOf(double x, double y, double offset);

  /**
   * Checks if the current position is to the left of the given coordinates and
   * the offset.
   *
   * @param x the x coordinate.
   * @param y the y coordinate.
   * @param offset the offset
   *
   * @return true if we are to the left of the given coordinates, false
   * otherwise.
   */
  bool isToTheLeftOf(double x, double y, double offset);

  /**
   * Checks if the current position is to the below or under the given
   * coordinates and the offset.
   *
   * @param x the x coordinate.
   * @param y the y coordinate.
   * @param offset the offset
   *
   * @return true if we are below of the given coordinates, false
   * otherwise.
   */

  bool isBelow(double x, double y, double offset);

  /**
   * Checks if the current position above or ahead the given coordinates and
   * the offset.
   *
   * @param x the x coordinate.
   * @param y the y coordinate.
   * @param offset the offset
   *
   * @return true if we are above the given coordinates, false
   * otherwise.
   */
  bool isAbove(double x, double y, double offset);
 
  /**
   * Check if we are moving away from the given coordinates.
   * 
   * @param x the x coordinate.
   * @param y the y coordinate.
   *
   * @return true if we are moving away, false otherwise.
   */
  bool isMovingAway(double x, double y);

  /**
   * Check if we are moving toward from the given coordinates.
   * 
   * @param x the x coordinate.
   * @param y the y coordinate.
   *
   * @return true if we are moving toward, false otherwise.
   */
  bool isMovingToward(double x, double y);

  /**
   * Check if we are moving north west from the given coordinates.
   * 
   * @return true if so, false otherwise.
   */
  bool isMovingNorthWest();

  /**
   * Check if we are moving north from the given coordinates.
   * 
   * @return true if so, false otherwise.
   */
  bool isMovingNorth();

  /**
   * Check if we are moving north east from the given coordinates.
   * 
   * @return true if so, false otherwise.
   */
  bool isMovingNorthEast();

  /**
   * Check if we are moving East from the given coordinates.
   * 
   * @return true if so, false otherwise.
   */
  bool isMovingEast();

  /**
   * Check if we are moving south east from the given coordinates.
   * 
   * @return true if so, false otherwise.
   */
  bool isMovingSouthEast();

  /**
   * Check if we are moving south from the given coordinates.
   * 
   * @return true if so, false otherwise.
   */
  bool isMovingSouth();

  /**
   * Check if we are moving south west from the given coordinates.
   * 
   * @return true if so, false otherwise.
   */
  bool isMovingSouthWest();

  /**
   * Check if we are moving west from the given coordinates.
   * 
   * @return true if so, false otherwise.
   */
  bool isMovingWest();

  /**
   * Cellular automata rule to check if there we are north-west of the asking
   * vehicle.
   *
   * @return true if so, false otherwise
   */
  bool isInNorthWestSquare(double x, double y, double l);

  /**
   * Cellular automata rule to check if there we are north of the asking
   * vehicle.
   *
   * @return true if so, false otherwise
   */
  bool isInNorthSquare(double x, double y, double l);

  /**
   * Cellular automata rule to check if there we are north-east of the asking
   * vehicle.
   *
   * @return true if so, false otherwise
   */
  bool isInNorthEastSquare(double x, double y, double l);

  /**
   * Cellular automata rule to check if there we are east of the asking
   * vehicle.
  i *
   * @return true if so, false otherwise
   */
  bool isInEastSquare(double x, double y, double l);

  /**
   * Cellular automata rule to check if there we are south-east of the asking
   * vehicle.
   *
   * @return true if so, false otherwise
   */
  bool isInSouthEastSquare(double x, double y, double l);

  /**
   * Cellular automata rule to check if there we are south of the asking
   * vehicle.
   *
   * @return true if so, false otherwise
   */
  bool isInSouthSquare(double x, double y, double l);

  /**
   * Cellular automata rule to check if there we are south-west of the asking
   * vehicle.
   *
   * @return true if so, false otherwise
   */
  bool isInSouthWestSquare(double x, double y, double l);

  /**
   * Cellular automata rule to check if there we are west of the asking
   * vehicle.
   *
   * @return true if so, false otherwise
   */
  bool isInWestSuare(double x, double y, double l);

  /**
   * Checks if we are northwest of the point (x,y) according to the cellular
   * automata rules.
   *
   * @return true if so, false otherwise
   */
  bool isNorthWestOf(double x, double y, double l);

  /**
   * Checks if we are north of the point (x,y) according to the cellular
   * automata rules.
   *
   * @return true if so, false otherwise
   */
  bool isNorthOf(double x, double y, double l);

  /**
   * Checks if we are northeast of the point (x,y) according to the cellular
   * automata rules.
   *
   * @return true if so, false otherwise
   */
  bool isNorthEastOf(double x, double y, double l);

  /**
   * Checks if we are east of the point (x,y) according to the cellular
   * automata rules.
   *
   * @return true if so, false otherwise
   */
  bool isEastOf(double x, double y, double l);

  /**
   * Checks if we are southeast of the point (x,y) according to the cellular
   * automata rules.
   *
   * @return true if so, false otherwise
   */
  bool isSouthEastOf(double x, double y, double l);

  /**
   * Checks if we are south of the point (x,y) according to the cellular
   * automata rules.
   *
   * @return true if so, false otherwise
   */
  bool isSouthOf(double x, double y, double l);

  /**
   * Checks if we are southwest of the point (x,y) according to the cellular
   * automata rules.
   * @return true if so, false otherwise
   */
  bool isSouthWestOf(double x, double y, double l);

  /**
   * Checks if we are west of the point (x,y) according to the cellular
   * automata rules.
   *
   * @return true if so, false otherwise
   */
  bool isWestOf(double x, double y, double l);
  
  private:
  /**
   * Target name.
   */
  string ID;

  /**
   * Target x coordinate.
   */
  double x;
  
  /**
   * Target y coordinate.
   */
  double y;

  /**
   * Target z coordinate.
   */
  double z;
  
  /**
   * Target speed.
   */
  double speed;
  
  /**
   * Target heading.
   */
  double heading; 
  };
#endif
