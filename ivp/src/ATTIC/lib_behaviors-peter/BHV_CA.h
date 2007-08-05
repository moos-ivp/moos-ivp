#ifndef __BHV_CA_h__
#define __BHV_CA_h__

#include <string>
#include <cmath>
#include "IvPBehavior.h"
#include "Target.h"

/**
 * Simple macro that uses debug flag from \c .bhv file to determine if to post
 * debug information into the MOOS community.
 */
#define DEBUG if(debug) postDebugInfo()

/**
 * Some usefull cellular automata macros
 */
#define NORTH_WEST left - 0.5*cubeLength, top + 0.5*cubeLength
#define NORTH left + 0.5*cubeLength, top + 0.5*cubeLength
#define NORTH_EAST right + 0.5*cubeLength, top + 0.5*cubeLength
#define EAST right + 0.5*cubeLength, bottom + 0.5*cubeLength
#define SOUTH_EAST right + 0.5*cubeLength, bottom - 0.5*cubeLength
#define SOUTH left + 0.5*cubeLength, bottom - 0.5*cubeLength
#define SOUTH_WEST left - 0.5*cubeLength, bottom - 0.5*cubeLength
#define WEST left - 0.5*cubeLength, bottom + 0.5*cubeLength
#define CURRENT left + 0.5*cubeLength, bottom + 0.5*cubeLength

#define MOVE_NORTH_WEST xp = left - 0.5*cubeLength; yp = top + 0.5*cubeLength
#define MOVE_NORTH xp = left + 0.5*cubeLength; yp = top + 0.5*cubeLength
#define MOVE_NORTH_EAST xp = right + 0.5*cubeLength; yp = top + 0.5*cubeLength
#define MOVE_EAST xp = right + 0.5*cubeLength; yp = bottom + 0.5*cubeLength
#define MOVE_SOUTH_EAST xp = right + 0.5*cubeLength; yp = bottom - 0.5*cubeLength
#define MOVE_SOUTH xp = left + 0.5*cubeLength; yp = bottom - 0.5*cubeLength
#define MOVE_SOUTH_WEST xp = left - 0.5*cubeLength; yp = bottom - 0.5*cubeLength
#define MOVE_WEST xp = left - 0.5*cubeLength; yp = bottom + 0.5*cubeLength
#define DONT_MOVE xp = left + 0.5*cubeLength; yp = bottom + 0.5*cubeLength

using namespace std;

/** 
 * Implementation of cellular automata rules found here
 * http://groups.csail.mit.edu/drl/publications/papers/ca.final.pdf.
 *
 * TBD more doc.
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
 * $Id: BHV_CA.h,v 1.1 2007/05/01 13:13:05 mikerb Exp $
 */
class BHV_CA: public IvPBehavior {
  public:
  /**
   * Constructor.
   *
   * @param domain an \em Interval \em Programming (IvP) domain.
   */
  BHV_CA(IvPDomain domain);

  /**
   * Destructor.
   */
  ~BHV_CA() {};
  
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
   * Calculets the priority for the behavior which in this cas is equal to that
   * of the priority weight specified in the moos file.
   *
   * @return the priority weight specified in the moos file.
   */
  double getPriority();

  /**
   * Checks if a cellular automata square is occupied. That is if there is any
   * one eles in that square except the target. This differs from
   * squareHasStationaryOccupant() by the fact that the occupant of this square
   * can be anywhere in it.
   *
   * @param xs the x coordinate of the square.
   * @param ys the y coordinate of the square.
   *
   * @return true if there is a vehicle in that cell, false otherwise.
   */
  bool squareIsOccupied(double xs, double ys);

  /**
   * Checks if a cellular automata square is occupied by a stationary target.
   * This is done by checking if there is someone, except the target, in the
   * center.
   * This differs from squareIsOccupied() by the fact that the occupant of this square
   * has to be in the center.
   *
   * @param xs the x coordinate of the square.
   * @param ys the y coordinate of the square.
   *
   * @return true if there is a vehicle in that cell, false otherwise.
   */
  bool squareHasStationaryOccupant(double xs, double ys);
  
  /**
   * Checks if a cellular automata square is locked, that is some is moving to
   * that square.
   * 
   * @param xs the x coordinate of the square.
   * @param ys the y coordinate of the square.
   *
   * @return true if so, false otherwise.
   */
  bool squareIsLocked(double xs, double ys);

  /**
   * Locks a cellular automata square so no one else can move to it.
   *
   * @param xs the x coordinate of the square.
   * @param ys the y coordinate of the square.
   */
  void lockSquare(double xs, double ys);

  /**
   * Counts the number of moving neighbours.
   *
   * @return the number of neighbours that are moving
   */
  int numberOfMovingNeighbours();
  
  bool moveNorth();

  bool moveNorthEast();

  bool moveEast();

  bool moveSouthEast();

  bool moveSouth();

  bool moveSouthWest();

  bool moveWest();

  bool moveNorthWest();
  
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
   * Length of the side of the cellular automata cube.
   */
  double cubeLength;
  
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
   * Flag that determines if to output debug information.
   */
  bool debug;

  /**
   * Neighbour information placeholder used by the neighbours vector.
   */
  struct Neighbour {
    /// The name or ID of the vehicle.
    string ID;
    /// Current range.
    double range;
    
    /**
     * The Current state of the vehicle.
     *
     * The vehicle can be in one of following states:
     * \par
     * STOPPED\n
     * MOVING
     */
    enum VehicleStates {
      STOPPED,
      MOVING
    } state;
    
    /// Clears ID and sets range to INFINITY.
    void clear() {
      ID = "";
      range = INFINITY;
    }
  };

  /**
   * Information about close by neighbours is stored here with the closest one
   * firts. See findNeighbours().
   */
  vector<Neighbour > neighbours;

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

  /**
   * Default epsilon for distances (in meters).
   */
  static const double EPSILON = 6;

  /**
   * Search radius in with to look for neighbours.
   */
  double neighbourSearchRadius;

  /**
   * Number of simultaneous moving neighbours allowed. That is, we will only
   * move if there are less moving neighbours then this number.
   */
  int simultaneousMoversAllowed;

  double left;
  double bottom;
  double right;
  double top;
  double xp, yp;
};
#endif
