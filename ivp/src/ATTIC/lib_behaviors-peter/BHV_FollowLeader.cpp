#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include "BHV_FollowLeader.h"
#include "GeometricUtils.h"
#include "IvPDomain.h"
#include "AOF_WPT2D.h"
#include "AOF_StayInRange.h"
#include "OF_Reflector.h"

using namespace std;

BHV_FollowLeader::BHV_FollowLeader(IvPDomain domain) : IvPBehavior(domain) {
  this->setParam("descriptor", "StayInRange");
  this->setParam("unifbox", "course=3, speed=2");
  this->setParam("gridbox", "course=9, speed=6");

  ID = "";
  x = 0;
  y = 0;
  speed = 0;
  heading = 0;
  
  target = new Target();

  targetRange = 0;
  desiredTargetRange = 10;

  debug = false;
}

bool BHV_FollowLeader::setParam(string param, string val) {
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

  if(param == "range") {
    desiredTargetRange = atof(val.c_str());
    return true;
  }

  if(param == "debug") {
    if(val == "true")
      debug = true;
    return true;
  }

  return false;    
}

IvPFunction* BHV_FollowLeader::produceOF() {
  // clear each time produceOF() is called
  messages.clear();
  
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    cout<<"toast!"<<endl;
    return 0;
  }

  bool ok = true;

  x = info_buffer->dQuery(ID, xVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + xVariable + " not found.");
    cout<<"No x variable found for "<<ID<<endl;
    return 0;
  }

  y = info_buffer->dQuery(ID, yVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + yVariable + " not found.");
    cout<<"No y variable found for "<<ID<<endl;
    return 0;
  }

  heading = info_buffer->dQuery(ID, headingVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + headingVariable + " not found.");
    cout<<"No heading variable found for "<<ID<<endl;
    return 0;
  }
  heading = GeometricUtils::canonicalizeAngle(heading);

  speed = info_buffer->dQuery(ID, speedVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + speedVariable + " not found.");
    cout<<"No speed variable found for "<<ID<<endl;
    return 0;
  }

  string targetID = target->getID();

  target->setX(info_buffer->dQuery(targetID, targetXVariable, &ok));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetXVariable + " not found");
    cout<<"No "<<targetXVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  target->setY(info_buffer->dQuery(targetID, targetYVariable, &ok));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetYVariable + " not found");
    cout<<"No "<<targetYVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  target->setSpeed(info_buffer->dQuery(targetID, targetSpeedVariable, &ok));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetSpeedVariable + " not found");
    cout<<"No "<<targetSpeedVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  target->setHeading(GeometricUtils::canonicalizeAngle(
        info_buffer->dQuery(targetID, targetHeadingVariable, &ok)));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetHeadingVariable + " not found");
    cout<<"No "<<targetHeadingVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  targetRange = GeometricUtils::distance(x, y, target->getX(),
      target->getY());

  DEBUG;

  if(targetRange < desiredTargetRange)
    return 0;
  
  AOF_StayInRange *aof_stayInRange = new AOF_StayInRange(
        domain,
        target->getY(),
        target->getX(),
        target->getHeading(),
        0,
        y,
        x);
  
  OF_Reflector *ofr_cutrange = new OF_Reflector(aof_stayInRange, 1);
  ofr_cutrange->create_uniform(unif_box, grid_box);
  IvPFunction *of = ofr_cutrange->extractOF();
  of->getPDMap()->normalize(0.0, 100.0);

  delete(aof_stayInRange);
  delete(ofr_cutrange);

  of->setDomainName(0, "course");
  of->setDomainName(1, "speed");

  of->setPWT(getRelevance() * priority_wt);
  
  return of;
}

// TBD add metric here
double BHV_FollowLeader::getRelevance() {
  return 1.0;
}

void BHV_FollowLeader::postDebugInfo() {
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
  
  postMessage("desiredTargetRange", desiredTargetRange);

  bool ok;
  double pingtime = info_buffer->dQuery(ID, "PING_TIME", &ok);
  
  if(ok) {
    double pingRange1482 = 1482 * pingtime;
    double pingRange1533 = 1533 * pingtime;
    postMessage("pingRange1482", pingRange1482);
    postMessage("pingRange1533", pingRange1533);
  }
}
