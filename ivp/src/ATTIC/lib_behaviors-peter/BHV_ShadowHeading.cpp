#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include "BHV_ShadowHeading.h"
#include "GeometricUtils.h"
#include "IvPDomain.h"

using namespace std;

BHV_ShadowHeading::BHV_ShadowHeading(IvPDomain gdomain) : IvPBehavior(gdomain) {
  this->setParam("descriptor", "ShadowHeading");
  
  ID = "";
  heading = 0;
  
  target = new Target();
  
  targetRangeMax = 50;
  targetRangeMin = 10;  
}

bool BHV_ShadowHeading::setParam(string param, string val) {
  if(IvPBehavior::setParamCommon(param, val))
    return true;

  if(param == "ID") {
    ID = val;
    return true;
  }
  
  if(param == "target") {
    target->setID(val);
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

  if(param == "x_variable") {
    xVariable = val;
    return true;
  }

  if(param == "y_variable") {
    yVariable = val;
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

  if(param == "target_heading_variable") {
    targetHeadingVariable = val;
    return true;
  }

  cout<<"Oops, I believe you forgot to set a variable"<<endl;
  return false;
}


IvPFunction *BHV_ShadowHeading::produceOF() {
  // clear each time produceOF() is called
  messages.clear();

  bool ok = true;
  
  x = info_buffer->dQuery(ID, xVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + xVariable + " not found.");
    cout<<"No x variable found for "<<ID<<endl;
    return(0);
  }
  
  y = info_buffer->dQuery(ID, yVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + yVariable + " not found.");
    cout<<"No y variable found for "<<ID<<endl;
    return(0);
  }

  heading = info_buffer->dQuery(ID, headingVariable, &ok);
  if(!ok) {
    postEMessage(ID + " " + headingVariable + " not found.");
    cout<<"No heading variable found for "<<ID<<endl;
    return(0);
  }
  heading = GeometricUtils::canonicalizeAngle(heading);

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

  target->setHeading(GeometricUtils::canonicalizeAngle(
        info_buffer->dQuery(targetID, targetHeadingVariable, &ok)));
  if(!ok) {
    postEMessage(ID + " target " + targetID + " " + targetHeadingVariable + " not found");
    cout<<"No "<<targetHeadingVariable<<" found for target "<<targetID<<endl;
    return 0;
  }

  targetRange = GeometricUtils::distance(x, y, target->getX(),
      target->getY());


  // if target is too far away we won't bother to create the objective function
  if(target->isTooFar(targetRange, targetRangeMax))
    return 0;

  // used to index the possible courses
  int domainIndex = (int) floor(target->getHeading() + 0.5);
  // just the possition of course variable in the domain
  int courseIndex = domain.getIndex("course");
  // number of possible course settings.
  // this should be 360.
  int coursePoints = domain.get_dpoints(courseIndex);

  // doesn't really matter what this is it gets normalized to [0, 100] if we use
  // normalize further down...
  double maxUtility = 100;

  // when it's no longer interesting to look at other course headings
  // alter this if you want a more narrow range but then you have to add more
  // pieces. So leave this unchanged if you don't want to screw things up
  int targetCourseRange = 180;

  double slope = maxUtility / targetCourseRange;

  // create a 1D box with degree 1
  IvPBox universe = IvPBox(1, 1);
  // set range
  universe.setPTS(0, 0, coursePoints - 1);

  IvPBox *piece1 = new IvPBox(1, 1);
  IvPBox *piece2 = new IvPBox(1, 1);
  IvPBox *piece3 = new IvPBox(1, 1);

  // two cases to consider when chopping up the decision space
  // if we have a targetCourse < 180 (or targetCourseRange) we have to add the
  // last piece so we cover the possible courses from the "other side" of 360
  //          ^    
  //      100-|piece1.
  //          |     / \
  //          |    /   \ piece2
  //          |   /     \
  // Utility  |  /       \     piece3
  //          | /         \      .
  //          |/           \    /|
  //          |             \  / |
  //          |______|_______\/__|_______
  //          0      |--180--|  360
  //                 |
  //             domainIndex
  //            (targetCourse)
  //
  // the weight values comes from Box.cpp where we are creating linear functions
  // in the form of
  // 
  //   y = wt(0)*domainIndex + wt(1)
  //   
  // where y is [0, maxUtility]
  if(domainIndex < targetCourseRange) {
    piece1->setPTS(0, 0, domainIndex);
    piece1->wt(0) = slope;
    piece1->wt(1) = maxUtility - slope*domainIndex;

    piece2->setPTS(0, domainIndex + 1, domainIndex + targetCourseRange);
    piece2->wt(0) = -slope;
    piece2->wt(1) = maxUtility + slope*(domainIndex + 1);

    piece3->setPTS(0, domainIndex + targetCourseRange + 1, coursePoints - 1);
    piece3->wt(0) = slope;
    piece3->wt(1) = -slope*(domainIndex + targetCourseRange + 1);
  }

  //          ^    
  //      100-|            .
  //          |    piece2 / \
  //          |          /   \
  //          |         /     \
  // Utility  | piece1 /       \ piece3
  //          |\      /         \
  //          | \    /           |
  //          |  \  /            |
  //          |___\/_______|_____|_______
  //          0    |--180--|  360
  //                       |
  //                  domainIndex
  //                 (targetCourse)
  // you do the math...
  else {
    piece1->setPTS(0, 0, domainIndex - targetCourseRange);
    piece1->wt(0) = -slope;
    piece1->wt(1) = slope*(domainIndex - targetCourseRange);

    piece2->setPTS(0, domainIndex - targetCourseRange + 1, domainIndex);
    piece2->wt(0) = slope;
    piece2->wt(1) = maxUtility - slope*domainIndex;

    piece3->setPTS(0, domainIndex + 1, coursePoints - 1);
    piece3->wt(0) = -slope;
    piece3->wt(1) = maxUtility + slope*(domainIndex + 1);
  }

  PDMap *pdmap = new PDMap(3, universe, 1);
  pdmap->bx(0) = piece1;
  pdmap->bx(1) = piece2;
  pdmap->bx(2) = piece3;
  pdmap->setGelBox(&universe);
  pdmap->normalize(0.0, 100.0);
  pdmap->updateGrid();

  IvPFunction *of = new IvPFunction(pdmap);
  of->setDomainName(0, "course");
  of->setPWT(getPriority());

  return of;
}

double BHV_ShadowHeading::getPriority() {
  return priority_wt;
/*
  if(targetRange > targetRangeMax)
    return 0;

  if(targetRange < targetRangeMin)
    return priority_wt;

  return (priority_wt/(targetRangeMax - targetRangeMin))*
    (targetRangeMax - targetRange);
*/
}
