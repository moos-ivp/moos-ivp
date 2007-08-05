#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include "BHV_CA.h"
#include "GeometricUtils.h"
#include "IvPDomain.h"
#include "AOF_WPT2D.h"
#include "OF_Reflector.h"
#include "IvPFunction.h"

using namespace std;

BHV_CA::BHV_CA(IvPDomain domain) : IvPBehavior(domain) {
  this->setParam("descriptor", "bhv_CA");
  this->setParam("unifbox", "course=3, speed=2");
  this->setParam("gridbox", "course=6, speed=4");

  // TBD figure out better default values
  ID = "";
  x = 0;
  y = 0;
  speed = 0;
  heading = 0;

  cubeLength = 0;

  simultaneousMoversAllowed = 1;

  target = new Target();
  targetRange = 0;
  targetRangeMin = 0;

  debug = false;
}

  bool BHV_CA::setParam(string param, string val) {
    if(IvPBehavior::setParamCommon(param, val))
      return true;

    if(param == "ID") {
      ID = val;
      return true;
    }

    if(param == "speed") {
      speed = atof(val.c_str());
      return true;
    }

    if(param == "target") {
      target->setID(val);
      return true;
    }

    if(param == "x_variable") {
      xVariable = val;
      return true;
    }

    if(param == "y_variable") {
      yVariable = val;
      return true;
    }

    if(param == "speed_variable") {
      speedVariable = val;
      return true;
    }

    if(param == "heading_variable") {
      headingVariable = val;
      return true;
    }

    if(param == "target_x_variable") {
      targetXVariable = val;
      return true;
    }

    if(param == "target_y_variable") {
      targetYVariable = val;
      return true;
    }

    if(param == "target_speed_variable") {
      targetSpeedVariable = val;
      return true;
    }

    if(param == "target_heading_variable") {
      targetHeadingVariable = val;
      return true;
    }

    if(param == "range_min") {
      targetRangeMin = atof(val.c_str());
      return true;
    }

    if(param == "neighbour_search_radius") {
      neighbourSearchRadius = atof(val.c_str());
      return true;
    }

    if(param == "cube_length"){
      cubeLength = atof(val.c_str());
      return true;
    }

    if(param == "simultaneous_movers"){
      simultaneousMoversAllowed = (int) atof(val.c_str());
      return true;
    }

    if(param == "debug") {
      if(val == "true")
        debug = true;
      return true;
    }

    return true;
  }

IvPFunction* BHV_CA::produceOF() {
  // clear each time produceOF() is called
  messages.clear();

  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }

  bool ok = true;

  x = info_buffer->dQuery(ID, xVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + xVariable + " not found.");
    cout<<"BHV_CA, "<<ID<<": No x variable "<<xVariable<<" found"<<endl;
    return 0;
  }

  y = info_buffer->dQuery(ID, yVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + yVariable + " not found.");
    cout<<"BHV_CA, "<<ID<<": No y variable "<<yVariable<<" found"<<endl;
    return 0;
  }

  heading = info_buffer->dQuery(ID, headingVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + headingVariable + " not found.");
    cout<<"BHV_CA, "<<ID<<": No heading variable "<<headingVariable<<" found"<<endl;
    return 0;
  }
  heading = GeometricUtils::canonicalizeAngle(heading);

  //  speed = info_buffer->dQuery(ID, speedVariable, &ok);
  //  if(!ok) {
  //    post_emsg(ID + " " + speedVariable + " not found.");
  //    cout<<"BHV_CA, "<<ID<<": No speed variable "<<speedVariable<<" found"<<endl;
  //    return 0;
  //  }

  string targetID = target->getID();

  target->setX(info_buffer->dQuery(targetID, targetXVariable, &ok));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetXVariable + " not found");
    cout<<"BHV_CA, "<<ID<<": No "<<targetXVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  target->setY(info_buffer->dQuery(targetID, targetYVariable, &ok));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetYVariable + " not found");
    cout<<"BHV_CA, "<<ID<<": No "<<targetYVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  target->setSpeed(info_buffer->dQuery(targetID, targetSpeedVariable, &ok));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetSpeedVariable + " not found");
    cout<<"BHV_CA, "<<ID<<": No "<<targetSpeedVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  target->setHeading(GeometricUtils::canonicalizeAngle(
        info_buffer->dQuery(targetID, targetHeadingVariable, &ok)));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetHeadingVariable + " not found");
    cout<<"BHV_CA, "<<ID<<": No "<<targetHeadingVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  targetRange = GeometricUtils::distance(x, y, target->getX(),
      target->getY());

  AOF_WPT2D *aof_wpt2D;

  findNeighbours(neighbourSearchRadius);

  double a = floor(x/cubeLength);
  double b = floor(y/cubeLength);

  left = a*cubeLength;
  bottom = b*cubeLength;
  right = left + cubeLength;
  top = bottom + cubeLength;

  // keep moving to last computed location
  if(xp != 0 && yp != 0 && (abs(xp - x) > EPSILON || abs(yp - y) > EPSILON)) {
    xp = xp;
//    DONT_MOVE;
  }

  // check if target is too close
  else if(targetRange < targetRangeMin) {
    xp = xp;
//    DONT_MOVE;
  }

  // check if someone is moving already
  else if(numberOfMovingNeighbours() >= simultaneousMoversAllowed) {
//    xp = xp;
    DONT_MOVE;
  }

  // or get a new set of coordinates to go to
  else {
    if(target->isNorthWestOf(x, y, cubeLength)) {
      if(moveNorthWest()) {
        MOVE_NORTH_WEST;
      }

      else if(moveNorth()) {
        MOVE_NORTH;
      }

      else if(moveWest()) {
        MOVE_WEST;
      }

      else if(moveNorthEast()) {
        MOVE_NORTH_EAST;
      }

      else {
        DONT_MOVE;
      }
    }

    else if(target->isNorthOf(x, y, cubeLength)) {
      if(moveNorth()) {
        MOVE_NORTH;
      }

      else if(moveNorthWest()) {
        MOVE_NORTH_WEST;
      }

      else if(moveNorthEast()) {
        MOVE_NORTH_EAST;
      }

      else {
        DONT_MOVE;
      }
    }

    else if(target->isNorthEastOf(x, y, cubeLength)) {
      if(moveNorthEast()) {
        MOVE_NORTH_EAST;
      }

      else if(moveNorth()) {
        MOVE_NORTH;
      }

      else if(moveEast()) {
        MOVE_EAST;
      }

      else if(moveNorthWest()) {
        MOVE_NORTH_WEST;
      }

      else {
        DONT_MOVE;
      }
    }

    else if(target->isEastOf(x, y, cubeLength)) {
      if(moveEast()) {
        MOVE_EAST;
      }

      else if(moveNorthEast()) {
        MOVE_NORTH_EAST;
      }

      else if(moveSouthEast()) {
        MOVE_SOUTH_EAST;
      }

      else {
        DONT_MOVE;
      }
    }

    else if(target->isSouthEastOf(x, y, cubeLength)) {
      if(moveSouthEast()) {
        MOVE_SOUTH_EAST;
      }

      else if(moveEast()) {
        MOVE_EAST;
      }

      else if(moveSouth()) {
        MOVE_SOUTH;
      }

      else if(moveSouthWest()) {
        MOVE_SOUTH_WEST;
      }

      else {
        DONT_MOVE;
      }
    }

    else if(target->isSouthOf(x, y, cubeLength)) {
      if(moveSouth()) {
        MOVE_SOUTH;
      }

      else if(moveSouthEast()) {
        MOVE_SOUTH_EAST;
      }

      else if(moveSouthWest()) {
        MOVE_SOUTH_WEST;
      }

      else {
        DONT_MOVE;
      }
    }

    else if(target->isSouthWestOf(x, y, cubeLength)) {
      if(moveSouthWest()) {
        MOVE_SOUTH_WEST;
      }

      else if(moveWest()) {
        MOVE_WEST;
      }

      else if(moveSouth()) {
        MOVE_SOUTH;
      }

      else if(moveSouthEast()) {
        MOVE_SOUTH_EAST;
      }

      else {
        DONT_MOVE;
      }
    }

    else if(target->isWestOf(x, y, cubeLength)) {
      if(moveWest()) {
        MOVE_WEST;
      }

      else if(moveNorthWest()) {
        MOVE_NORTH_WEST;
      }

      else if(moveSouthWest()) {
        MOVE_SOUTH_WEST;
      }

      else {
        DONT_MOVE;
      }
    }
  }

  // do nothing if we are at our location
  if(abs(xp - x) < EPSILON && abs(yp - y) < EPSILON) {
    postMessage("state", 0);
    return 0;
  }

  lockSquare(xp, yp);

  postMessage("state", 1);

  aof_wpt2D = new AOF_WPT2D(domain, speed, y, x, yp, xp);

  OF_Reflector *ofr_wpt2D = new OF_Reflector(aof_wpt2D, 1);
  ofr_wpt2D->create_uniform(unif_box, grid_box);
  IvPFunction *of = ofr_wpt2D->extractOF();

  delete(aof_wpt2D);
  delete(ofr_wpt2D);

  of->setDomainName(0, "course");
  of->setDomainName(1, "speed");
  of->setPWT(getPriority());
  of->getPDMap()->normalize(0.0, 100.0);

  DEBUG;
  return of;
}

  bool BHV_CA::moveNorthWest() {
    if(!target->isMovingWest() &&
       !target->isMovingNorthWest() &&
       !target->isMovingNorth() &&
       !target->isMovingNorthEast() &&
       !target->isMovingSouthWest())
      return false;

    if(squareIsOccupied(NORTH_WEST) || squareIsLocked(NORTH_WEST))
      return false;

    if((squareHasStationaryOccupant(NORTH) &&
       !squareIsOccupied(EAST) &&
       !squareIsOccupied(SOUTH) &&
       !squareIsOccupied(WEST)) ||
       (squareHasStationaryOccupant(NORTH) &&
       squareHasStationaryOccupant(NORTH_EAST) &&
       !squareIsOccupied(SOUTH) &&
       !squareIsOccupied(WEST)))
      return true;

    else if((squareHasStationaryOccupant(WEST) &&
            !squareIsOccupied(NORTH) &&
            !squareIsOccupied(EAST) &&
            !squareIsOccupied(SOUTH)) ||
            (squareHasStationaryOccupant(SOUTH_WEST) &&
            squareHasStationaryOccupant(WEST) &&
            !squareIsOccupied(NORTH) &&
            !squareIsOccupied(EAST)))
      return true;

    return false;
  }

  bool BHV_CA::moveNorth() {
    if(!target->isMovingNorthWest() &&
       !target->isMovingNorth() &&
       !target->isMovingNorthEast() &&
       !target->isMovingWest() &&
       !target->isMovingEast())
      return false;

    if(squareIsOccupied(NORTH) || squareIsLocked(NORTH))
      return false;

    // check if squares north west or north east are occupied but noth both
    if(squareHasStationaryOccupant(NORTH_EAST) &&
       squareHasStationaryOccupant(EAST) &&
       !squareIsOccupied(WEST) &&
       !squareIsOccupied(SOUTH))
      return true;

    else if(squareHasStationaryOccupant(NORTH_WEST) &&
            squareHasStationaryOccupant(WEST) &&
            !squareIsOccupied(EAST) &&
            !squareIsOccupied(SOUTH))
      return true;

    return false;
  }

  bool BHV_CA::moveNorthEast() {
    if(!target->isMovingNorth() &&
       !target->isMovingNorthEast() &&
       !target->isMovingEast() &&
       !target->isMovingNorthWest() &&
       !target->isMovingSouthEast())
      return false;

    if(squareIsOccupied(NORTH_EAST) || squareIsLocked(NORTH_EAST))
      return false;

    // check if squares north or east are occupied but noth both
    if((squareHasStationaryOccupant(EAST) &&
       !squareIsOccupied(NORTH) &&
       !squareIsOccupied(SOUTH) &&
       !squareIsOccupied(WEST)) ||
       (squareHasStationaryOccupant(EAST) &&
       squareHasStationaryOccupant(SOUTH_EAST) &&
       !squareIsOccupied(WEST) &&
       !squareIsOccupied(NORTH)))
      return true;

    else if((squareHasStationaryOccupant(NORTH) &&
            !squareIsOccupied(EAST) &&
            !squareIsOccupied(SOUTH) &&
            !squareIsOccupied(WEST)) ||
            squareHasStationaryOccupant(NORTH_WEST) &&
            squareHasStationaryOccupant(NORTH) &&
            !squareIsOccupied(EAST) &&
            !squareIsOccupied(SOUTH))
      return true;

    return false;
  }

  bool BHV_CA::moveEast() {
    if(!target->isMovingNorthEast() &&
       !target->isMovingEast() &&
       !target->isMovingSouthEast() &&
       !target->isMovingNorth() &&
       !target->isMovingSouth())
      return false;

    if(squareIsOccupied(EAST) || squareIsLocked(EAST))
      return false;

    // check squares north east and south east are occupied but noth both
    if((squareHasStationaryOccupant(NORTH) &&
       squareHasStationaryOccupant(NORTH_EAST) &&
       !squareIsOccupied(SOUTH) &&
       !squareIsOccupied(WEST)) ||
       (squareHasStationaryOccupant(EAST) &&
       squareHasStationaryOccupant(SOUTH_EAST) &&
       !squareIsOccupied(WEST) &&
       !squareIsOccupied(NORTH)))
      return true;

    else if(squareHasStationaryOccupant(SOUTH) &&
            squareHasStationaryOccupant(SOUTH_EAST) &&
            !squareIsOccupied(WEST) &&
            !squareIsOccupied(SOUTH))
      return true;

    return false;
  }

  bool BHV_CA::moveSouthEast() {
    if(!target->isMovingEast() &&
       !target->isMovingSouthEast() &&
       !target->isMovingSouth() &&
       !target->isMovingNorthEast() &&
       !target->isMovingSouthWest())
      return false;

    if(squareIsOccupied(SOUTH_EAST) || squareIsLocked(SOUTH_EAST))
      return false;

    // check squares east and south are occupied but noth both
    if((squareHasStationaryOccupant(EAST) &&
       !squareIsOccupied(SOUTH) &&
       !squareIsOccupied(WEST) &&
       !squareIsOccupied(NORTH)) ||
       (squareHasStationaryOccupant(NORTH_EAST) &&
       squareHasStationaryOccupant(EAST) &&
       !squareIsOccupied(SOUTH) &&
       !squareIsOccupied(WEST)))
      return true;

    else if((squareHasStationaryOccupant(SOUTH) &&
            !squareIsOccupied(NORTH) &&
            !squareIsOccupied(EAST) &&
            !squareIsOccupied(WEST)) ||
            (squareHasStationaryOccupant(SOUTH) &&
            squareHasStationaryOccupant(SOUTH_WEST) &&
            !squareIsOccupied(NORTH) &&
            !squareIsOccupied(EAST)))
      return true;

    return false;
  }

  bool BHV_CA::moveSouth() {
    if(!target->isMovingSouthWest() &&
       !target->isMovingSouth() &&
       !target->isMovingSouthEast() &&
       !target->isMovingWest() &&
       !target->isMovingEast())
      return false;

    if(squareIsOccupied(SOUTH) || squareIsLocked(SOUTH))
      return false;

    // check squares west and east are occupied but noth both
    if(squareHasStationaryOccupant(EAST) &&
       squareHasStationaryOccupant(SOUTH_EAST) &&
       !squareIsOccupied(WEST) &&
       !squareIsOccupied(NORTH))
      return true;

    else if(squareHasStationaryOccupant(SOUTH_WEST) &&
            squareHasStationaryOccupant(WEST) &&
            !squareIsOccupied(NORTH) &&
            !squareIsOccupied(EAST))
      return true;

    return false;
  }

  bool BHV_CA::moveSouthWest() {
    if(!target->isMovingWest() &&
       !target->isMovingSouthWest() &&
       !target->isMovingSouth() &&
       !target->isMovingNorthWest() &&
       !target->isMovingSouthEast())
      return false;

    if(squareIsOccupied(SOUTH_WEST) || squareIsLocked(SOUTH_WEST))
      return false;

    // check squares west and south are occupied but noth both
    if((squareHasStationaryOccupant(SOUTH) &&
       !squareIsOccupied(WEST) &&
       !squareIsOccupied(NORTH) &&
       !squareIsOccupied(EAST)) ||
       (squareHasStationaryOccupant(SOUTH_EAST) &&
       squareHasStationaryOccupant(SOUTH) &&
       !squareIsOccupied(WEST) &&
       !squareIsOccupied(NORTH)))
      return true;

    else if((squareHasStationaryOccupant(WEST) &&
            !squareIsOccupied(NORTH) &&
            !squareIsOccupied(EAST) &&
            !squareIsOccupied(SOUTH)) ||
            squareHasStationaryOccupant(WEST) &&
            squareHasStationaryOccupant(NORTH_WEST) &&
            !squareIsOccupied(EAST) &&
            !squareIsOccupied(SOUTH))
      return true;

    return false;
  }

  bool BHV_CA::moveWest() {
    if(!target->isMovingSouthWest() &&
       !target->isMovingWest() &&
       !target->isMovingNorthWest() &&
       !target->isMovingNorth() &&
       !target->isMovingSouth())
      return false;

    if(squareIsOccupied(WEST) || squareIsLocked(WEST))
      return false;

    // check squares north west and south west are occupied but noth both
    if(squareHasStationaryOccupant(NORTH_WEST) &&
       squareHasStationaryOccupant(NORTH) &&
       !squareIsOccupied(EAST) &&
       !squareIsOccupied(SOUTH))
      return true;

    else if(squareHasStationaryOccupant(SOUTH) &&
            squareHasStationaryOccupant(SOUTH_WEST) &&
            !squareIsOccupied(NORTH) &&
            !squareIsOccupied(EAST))
      return true;

    return false;
  }

double BHV_CA::getPriority() {
  return priority_wt;
}

void BHV_CA::findNeighbours(double rangeSearchRadious) {
  neighbours.clear();

  vector<string> vehicleNames = info_buffer->getVehicleNames();

  // finding and storing neighbour info
  for(vector<string>::iterator vehicleIterator = vehicleNames.begin();
      vehicleIterator != vehicleNames.end(); vehicleIterator++ ) {
    // TBD fix this, now only looking at first 3 letters
    string vehicle = *vehicleIterator;

    if(strncmp(vehicle.c_str(), ID.c_str(), 3) == 0 && vehicle != ID) {
      double range = GeometricUtils::distance(x, y,
          info_buffer->dQuery(vehicle, "x"),
          info_buffer->dQuery(vehicle, "y"));

      if(range > rangeSearchRadious)
        break;

      Neighbour newNeighbour;
      newNeighbour.ID = vehicle;
      newNeighbour.range = range;
      int vState = (int) info_buffer->dQuery(vehicle, "state");
      switch(vState) {
        case 0:
          newNeighbour.state = Neighbour::STOPPED;
          break;
        case 1:
          newNeighbour.state = Neighbour::MOVING;
          break;
        default:
          newNeighbour.state = Neighbour::STOPPED;
      }

      neighbours.push_back(newNeighbour);
    }
  }
}

bool BHV_CA::squareIsOccupied(double xs, double ys) {
  double l = xs - 0.5*cubeLength - EPSILON;
  double r = xs + 0.5*cubeLength + EPSILON;
  double t = ys + 0.5*cubeLength + EPSILON;
  double b = ys - 0.5*cubeLength - EPSILON;

  for(int i = 0; i < neighbours.size(); i++) {
    Neighbour neighbour = neighbours.at(i);

    double neighbourX = info_buffer->dQuery(neighbour.ID, "x");
    double neighbourY = info_buffer->dQuery(neighbour.ID, "y");

    if(l <= neighbourX && neighbourX <= r)
      if(b <= neighbourY && neighbourY <= t)
        return true;
  }

  return false;
}


bool BHV_CA::squareHasStationaryOccupant(double xs, double ys) {
  double l = xs - 0.5*cubeLength - EPSILON;
  double r = xs + 0.5*cubeLength + EPSILON;
  double t = ys + 0.5*cubeLength + EPSILON;
  double b = ys - 0.5*cubeLength - EPSILON;

  for(int i = 0; i < neighbours.size(); i++) {
    Neighbour neighbour = neighbours.at(i);

    double neighbourX = info_buffer->dQuery(neighbour.ID, "x");
    double neighbourY = info_buffer->dQuery(neighbour.ID, "y");

    if(l <= neighbourX && neighbourX <= r)
      if(b <= neighbourY && neighbourY <= t)
        if(neighbour.state == Neighbour::STOPPED)
          return true;
  }

  return false;
}

bool BHV_CA::squareIsLocked(double xs, double ys) {
  for(int i = 0; i < neighbours.size(); i++) {
    Neighbour neighbour = neighbours.at(i);

    double lockedSquareX = info_buffer->dQuery(neighbour.ID, "lockedSquareX");
    double lockedSquareY = info_buffer->dQuery(neighbour.ID, "lockedSquareY");

    if(xs == lockedSquareX && ys == lockedSquareY)
      return true;
  }

  return false;

}

void BHV_CA::lockSquare(double xs, double ys) {
  postMessage("lockedSquareX", xs);
  postMessage("lockedSquareY", ys);
}

int BHV_CA::numberOfMovingNeighbours() {
  int count = 0;

  for(int i = 0; i < neighbours.size(); i++) {
    Neighbour neighbour = neighbours.at(i);

    if(neighbour.state == Neighbour::MOVING)
      count++;
  }
  return count;
}

void BHV_CA::postDebugInfo() {
  postMessage("ID", ID);
  postMessage("x", x);
  postMessage("y", y);
  postMessage("speed", speed);
  postMessage("heading", heading);

  postMessage("targetID", target->getID());
  postMessage("targetX", target->getX());
  postMessage("targetY", target->getY());
  postMessage("targetZ", target->getZ());
  postMessage("targetSpeed", target->getSpeed());
  postMessage("targetHeading", target->getHeading());
  postMessage("targetRange", targetRange);

  postMessage("targetRangeMin", targetRangeMin);

  bool ok;
  double pingtime = info_buffer->dQuery(ID, "PING_TIME", &ok);

  if(ok) {
    double pingRange1482 = 1482 * pingtime;
    double pingRange1533 = 1533 * pingtime;
    postMessage("pingRange1482", pingRange1482);
    postMessage("pingRange1533", pingRange1533);
  }
}
