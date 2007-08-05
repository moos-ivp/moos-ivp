#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <stdexcept>
#include "BHV_StayInRange.h"
#include "GeometricUtils.h"
#include "IvPDomain.h"
#include "AOF_StayInRange.h"
#include "OF_Reflector.h"

using namespace std;

BHV_StayInRange::BHV_StayInRange(IvPDomain domain) : IvPBehavior(domain) {
  this->setParam("descriptor", "StayInRange");
  this->setParam("unifbox", "course=3, speed=2");
  this->setParam("gridbox", "course=9, speed=6");

  ID = "";
  state = CHILLIN;
  x = 0;
  y = 0;
  speed = 0;
  heading = 0;

  target = new Target();
  targetRange = 0;
  targetRangeMin = 0;
  targetRangeMax = 0;

  useHalfwayMark = true;
  useTargetPositioning = false;

  neighbourSearchRadius = 300;
  
  debug = false;
}

bool BHV_StayInRange::setParam(string param, string val) {
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

  if(param == "range_max") {
    targetRangeMax = atof(val.c_str());
    return true;
  }

  if(param == "use_halfway_mark") {
    if(val == "false")
      useHalfwayMark = false;

    return true;
  }

  if(param == "use_localization") {
    if(val == "true")
      useTargetPositioning = true;

    return true;
  }

  if(param == "neighbour_search_radius") {
    neighbourSearchRadius = atof(val.c_str());
    return true;
  }

  if(param == "debug") {
    if(val == "true")
      debug = true;
    return true;
  }

  return false;
}

IvPFunction *BHV_StayInRange::produceOF() {
  messages.clear();

  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return 0;
  }

  bool ok = true;

  x = info_buffer->dQuery(ID, xVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + xVariable + " not found.");
    cout<<"BHV_StayInRange, "<<ID<<": No x variable "<<xVariable<<" found"<<endl;
    return 0;
  }

  y = info_buffer->dQuery(ID, yVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + yVariable + " not found.");
    cout<<"BHV_StayInRange, "<<ID<<": No y variable "<<yVariable<<" found"<<endl;
    return 0;
  }

  heading = info_buffer->dQuery(ID, headingVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + headingVariable + " not found.");
    cout<<"BHV_StayInRange, "<<ID<<": No heading variable "<<headingVariable<<" found"<<endl;
    return 0;
  }
  heading = GeometricUtils::canonicalizeAngle(heading);

  speed = info_buffer->dQuery(ID, speedVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + speedVariable + " not found.");
    cout<<"BHV_StayInRange, "<<ID<<": No speed variable "<<speedVariable<<" found"<<endl;
    return 0;
  }

  if(useTargetPositioning) {
    localizeTarget();

//    if(amInearestTarget())
//      sendTargetData();
  }
  
  else {
    // chack if we have a number as a target ID
    if(atof((target->getID()).c_str()) != 0.0) {
      findNeighbours(neighbourSearchRadius);
    }

    string targetID = findTargetName();
 
    target->setX(info_buffer->dQuery(targetID, targetXVariable, &ok));
    if(!ok) {
      postEMessage(ID + " target " + targetID + " " + targetXVariable + " not found");
      cout<<"BHV_StayInRange, "<<ID<<": No "<<targetXVariable<<" found for target "<<targetID<<endl;
      return 0;
    }

    target->setY(info_buffer->dQuery(targetID, targetYVariable, &ok));
    if(!ok) {
      postEMessage(ID + " target " + targetID + " " + targetYVariable + " not found");
      cout<<"BHV_StayInRange, "<<ID<<": No "<<targetYVariable<<" found for target "<<targetID<<endl;
      return 0;
    }

    target->setSpeed(info_buffer->dQuery(targetID, targetSpeedVariable, &ok));
    if(!ok) {
      postEMessage(ID + " target " + targetID + " " + targetSpeedVariable + " not found");
      cout<<"BHV_StayInRange, "<<ID<<": No "<<targetSpeedVariable<<" found for target "<<targetID<<endl;
      return 0;
    }

    target->setHeading(GeometricUtils::canonicalizeAngle(
        info_buffer->dQuery(targetID, targetHeadingVariable, &ok)));
    if(!ok) {
      postEMessage(ID + " target " + targetID + " " + targetHeadingVariable + " not found");
      cout<<"BHV_StayInRange, "<<ID<<": No "<<targetHeadingVariable<<" found for target "<<targetID<<endl;
      return 0;
    }

    targetRange = GeometricUtils::distance(x, y, target->getX(),
        target->getY());
  }
  
  AOF_StayInRange *aof_stayInRange;

  if(!useHalfwayMark)
    if(target->isWithinRange(targetRange, targetRangeMin, targetRangeMax)) {
      state = CHILLIN;
      DEBUG;
      return 0;
    }

  // Maybe we are on the move to the halfway mark?
  if(isAtHalfwayMark() && useHalfwayMark) {
    // So we are in the intervall and have moved to the halfway mark, don't
    // bother to create objective function then
    cout<<"\nReached halfway mark!"<<endl;
    state = CHILLIN;
    DEBUG;
    return 0;
  }

  // If we are too close to the target we need to move away from it.
  // This is simply done by switching the target x,y with our.
  // TBD solved this by just inverting our heading.
  else if(target->isTooClose(targetRange, targetRangeMin) ||
      state == GUNNIN) {
    state = GUNNIN;
    aof_stayInRange = new AOF_StayInRange(
        domain, y, x, target->getHeading(), target->getSpeed(), target->getY(),
        target->getX());
  }

  // No, we are not too close, by are we too far away?
  else if(target->isTooFar(targetRange, targetRangeMax) ||
      state == CREEPING) {
    state = CREEPING;
    aof_stayInRange = new AOF_StayInRange(
        domain, target->getY(), target->getX(), target->getHeading(),
        target->getSpeed(), y, x);
  }

  // Default...
  // Should not need this but if something goes wrong we stop
  else {
    state = CHILLIN;
    DEBUG;
    return 0;
  }

  OF_Reflector *ofr_cutrange = new OF_Reflector(aof_stayInRange, 1);
  ofr_cutrange->create_uniform(unif_box, grid_box);
  IvPFunction *of = ofr_cutrange->extractOF();
  of->getPDMap()->normalize(0.0, 100.0);

  delete(aof_stayInRange);
  delete(ofr_cutrange);

  of->setDomainName(0, "course");
  of->setDomainName(1, "speed");
  of->setPWT(getPriority());

  DEBUG;
  return of;
}

double BHV_StayInRange::getPriority() {
  if(state == CHILLIN)
    return 0.0;

  if(targetRange > targetRangeMax)
    return priority_wt;

  return (priority_wt/(targetRangeMax - targetRangeMin))*
    (targetRangeMax - targetRange); 

  return 1.0;
}

bool BHV_StayInRange::isAtHalfwayMark() {
  double halfwayMark = targetRangeMin + 0.5*(targetRangeMax - targetRangeMin);

  // either we are to close or to far away from the halfway distance,
  // but we need to know what direction we are coming from
  if(targetRange > halfwayMark && state == GUNNIN)
    return true;

  if(targetRange < halfwayMark && state == CREEPING)
    return true;

  return false;
}


void BHV_StayInRange::findNeighbours(double rangeSearchRadious) {
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
        return;

      Neighbour newNeighbour;
      newNeighbour.ID = vehicle;
      newNeighbour.range = range;

      if(neighbours.empty())
        neighbours.push_back(newNeighbour);
      
      else {
        vector<Neighbour>::iterator neighbourIterator = neighbours.begin();
        int numberOfNeighbours = neighbours.size();
        
        for(int i = 0; i < numberOfNeighbours; i++){
          Neighbour currentNeighbour = neighbours.at(i);

          if(newNeighbour.range < currentNeighbour.range){
            neighbours.insert(neighbourIterator, newNeighbour);
            break;
          }
          else if(i == numberOfNeighbours - 1)
            neighbours.push_back(newNeighbour);

          neighbourIterator++;
        }
      }
    }
  }
}

bool BHV_StayInRange::amInearestTarget() {
  if(neighbours.size() < 1)
    return false;

  Neighbour n = neighbours.front();
  if(n.range < targetRange)
    return false;

  return true;
}

string BHV_StayInRange::findTargetName() {

  string targetID = target->getID();

  int targetNumber = (int) atof(targetID.c_str());
  
  // check if error, i.e we have a string as a name
  if(targetNumber == 0)
    return targetID;

  Neighbour n;
  try {
    n = neighbours.at(targetNumber - 1);
  }
  catch(out_of_range e) {
    cout<<"BHV_StayInRange, "<<ID<<": Could not find any neighbour at that index"<<endl;
    return targetID;
  }

  return n.ID;
}


// TBD this must be more generic
bool BHV_StayInRange::localizeTarget() {
  // the speed of the acoustic modem's sound waves in water (m/s).
  // speed of sound in water, 1482 m/s at 20°C, but also differ from salt to
  // sweet water. Set this to out discretion. It is assumed that all modems
  // operate under the same speed.
  double aComSpeed = 1482;

  // the differnet ping times from the kayaks
  bool ok1, ok2, ok3;
  double pingTime1 = info_buffer->dQuery("kayak1", "PING_TIME", &ok1);
  if(!ok1) {
    cout<<"PING_TIME for kayak1 not set"<<endl;
    return false;
  }
  double pingTime2 = info_buffer->dQuery("kayak2", "PING_TIME", &ok2);
  if(!ok2) {
    cout<<"PING_TIME for kayak2 not set"<<endl;
    return false;
  }
  double pingTime3 = info_buffer->dQuery("kayak3", "PING_TIME", &ok3);
  if(!ok3) {
    cout<<"PING_TIME for kayak3 not set"<<endl;
    return false;
  }

  // the radia of the spheres
  double r1 = aComSpeed*pingTime1;
  double r2 = aComSpeed*pingTime2;
  double r3 = aComSpeed*pingTime3;

  double rr1 = r1*r1;
  double rr2 = r2*r2;
  double rr3 = r3*r3;

  double x1 = info_buffer->dQuery("kayak1", "NAV_X");
  double y1 = info_buffer->dQuery("kayak1", "NAV_Y");
  double z1 = 0;

  double xx1 = x1*x1;
  double yy1 = y1*y1;
  double zz1 = z1*z1;

  double x2 = info_buffer->dQuery("kayak2", "NAV_X");
  double y2 = info_buffer->dQuery("kayak2", "NAV_Y");
  double z2 = 0;

  double xx2 = x2*x2;
  double yy2 = y2*y2;
  double zz2 = z2*z2;

  double x3 = info_buffer->dQuery("kayak3", "NAV_X");
  double y3 = info_buffer->dQuery("kayak3", "NAV_Y");
  double z3 = 0;

  double xx3 = x3*x3;
  double yy3 = y3*y3;
  double zz3 = z3*z3;

  // some necessary coefficients, check the maple file for more info
  double a = -0.5*(2*z1*x3+2*z2*x1-2*z2*x3-2*z1*x2-2*z3*x1+2*z3*x2)/
    (x3*y1-x3*y2-x2*y1+x1*y2-x1*y3+x2*y3);

  double aa = a*a;

  double b = -0.5*(-zz2*x1+yy3*x1-xx3*x2+xx3*x1+yy2*x3+zz1*x2-yy2*x1+
      xx2*x3-xx2*x1-zz1*x3-yy1*x3+yy1*x2-xx1*x3+xx1*x2-rr2*x3+rr3*x2-
      rr1*x2-rr3*x1+rr2*x1+rr1*x3-zz3*x2+zz3*x1+zz2*x3-yy3*x2)/
    (x3*y1-x3*y2-x2*y1+x1*y2-x1*y3+x2*y3);

  double bb = b*b;

  double c = 0.5*(2*z3*y2+2*z1*y3-2*z2*y3+2*z2*y1-2*z1*y2-2*z3*y1)/
    (y1*x3+y2*x1-y2*x3-y1*x2-y3*x1+y3*x2);

  double cc = c*c;
  
  double d = 0.5*(rr1*y3+rr2*y1-rr2*y3+xx1*y2+yy1*y2+zz1*y2+xx3*y1-xx3*
      y2+yy3*y1-yy3*y2+zz3*y1-zz3*y2-rr1*y2-rr3*y1+rr3*y2-xx1*y3-yy1*
      y3-zz1*y3-xx2*y1+xx2*y3-yy2*y1+yy2*y3-zz2*y1+zz2*y3)/
    (y1*x3+y2*x1-y2*x3-y1*x2-y3*x1+y3*x2);

  double dd = d*d;
  
  double targetZ1 = -(-z1-a*y1+c*d+a*b-c*x1-sqrt(2*cc*b*y1+2*aa*d*x1-xx1-yy1-
        dd+2*d*x1-bb+2*b*y1+2*z1*c*x1-2*z1*c*d+2*z1*a*y1-cc*zz1+cc*rr1-
        cc*yy1-cc*bb-aa*zz1+aa*rr1-aa*dd-aa*xx1-2*a*y1*c*d+2*a*y1*
        c*x1+2*c*d*a*b-2*a*b*c*x1-2*z1*a*b+rr1))/(cc+aa+1);

  double targetZ2 = -(-z1-a*y1+c*d+a*b-c*x1+sqrt(2*cc*b*y1+2*aa*d*x1-xx1-yy1-
        dd+2*d*x1-bb+2*b*y1+2*z1*c*x1-2*z1*c*d+2*z1*a*y1-cc*zz1+cc*rr1-
        cc*yy1-cc*bb-aa*zz1+aa*rr1-aa*dd-aa*xx1-2*a*y1*c*d+2*a*y1*
        c*x1+2*c*d*a*b-2*a*b*c*x1-2*z1*a*b+rr1))/(cc+aa+1);
  
  double targetX1 = -c*(-z1-a*y1+c*d+a*b-c*x1-sqrt(2*cc*b*y1+2*aa*d*x1-xx1-yy1-
        dd+2*d*x1-bb+2*b*y1+2*z1*c*x1-2*z1*c*d+2*z1*a*y1-cc*zz1+cc*rr1-
        cc*yy1-cc*bb-aa*zz1+aa*rr1-aa*dd-aa*xx1-2*a*y1*c*d+2*a*y1*
        c*x1+2*c*d*a*b-2*a*b*c*x1-2*z1*a*b+rr1))/(cc+aa+1)+d;

  double targetX2 = -c*(-z1-a*y1+c*d+a*b-c*x1+sqrt(2*cc*b*y1+2*aa*d*x1-xx1-yy1-
        dd+2*d*x1-bb+2*b*y1+2*z1*c*x1-2*z1*c*d+2*z1*a*y1-cc*zz1+cc*rr1-
        cc*yy1-cc*bb-aa*zz1+aa*rr1-aa*dd-aa*xx1-2*a*y1*c*d+2*a*y1*
        c*x1+2*c*d*a*b-2*a*b*c*x1-2*z1*a*b+rr1))/(cc+aa+1)+d;

  double targetY1 = -a*(-z1-a*y1+c*d+a*b-c*x1-sqrt(2*cc*b*y1+2*aa*d*x1-xx1-yy1-
        dd+2*d*x1-bb+2*b*y1+2*z1*c*x1-2*z1*c*d+2*z1*a*y1-cc*zz1+cc*rr1-
        cc*yy1-cc*bb-aa*zz1+aa*rr1-aa*dd-aa*xx1-2*a*y1*c*d+2*a*y1*
        c*x1+2*c*d*a*b-2*a*b*c*x1-2*z1*a*b+rr1))/(cc+aa+1)+b;

  double targetY2 = -a*(-z1-a*y1+c*d+a*b-c*x1+sqrt(2*cc*b*y1+2*aa*d*x1-xx1-yy1-
        dd+2*d*x1-bb+2*b*y1+2*z1*c*x1-2*z1*c*d+2*z1*a*y1-cc*zz1+cc*rr1-
        cc*yy1-cc*bb-aa*zz1+aa*rr1-aa*dd-aa*xx1-2*a*y1*c*d+2*a*y1*
        c*x1+2*c*d*a*b-2*a*b*c*x1-2*z1*a*b+rr1))/(cc+aa+1)+b;

  cout<<"targetX1 "<<targetX1<<endl;
  cout<<"targetX2 "<<targetX2<<endl;
  target->setX(targetX1);
  
  cout<<"targetY1 "<<targetY1<<endl;
  cout<<"targetY2 "<<targetY2<<endl;
  target->setY(targetY1);
  
  cout<<"targetZ1 "<<targetZ1<<endl;
  cout<<"targetZ2 "<<targetZ2<<endl;

  if(targetZ1 < 0)
    target->setZ(targetZ1);
  else
    target->setZ(targetZ2);
  
  return true;
}

void BHV_StayInRange::postDebugInfo() {
  string states[] = {"STOPPED", "CHASE", "ESCAPE"};

  postMessage("ID", ID);
  postMessage("x", x);
  postMessage("y", y);
  postMessage("speed", speed);
  postMessage("heading", heading);
  postMessage("state", states[state]);

  postMessage("targetID", target->getID());
  postMessage("targetX", target->getX());
  postMessage("targetY", target->getY());
  postMessage("targetZ", target->getZ());
  postMessage("targetSpeed", target->getSpeed());
  postMessage("targetHeading", target->getHeading());
  postMessage("targetRange", targetRange);

  postMessage("targetRangeMin", targetRangeMin);
  postMessage("targetRangeMax", targetRangeMax);

  bool ok;
  double pingtime = info_buffer->dQuery(ID, "PING_TIME", &ok);

  if(ok) {
    double pingRange1482 = 1482 * pingtime;
    double pingRange1533 = 1533 * pingtime;

    postMessage("pingRange1482", pingRange1482);
    postMessage("pingRange1533", pingRange1533);
  }
}
