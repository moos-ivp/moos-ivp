/************************************************************/
/*    NAME: Jacob Gerlach                                   */
/*    ORGN: MIT                                             */
/*    FILE: TurnSequence.cpp                                */
/*    DATE: 6/13/14                                         */
/************************************************************/

#include "TurnSequence.h"
#include <vector>
#include <iostream>
#include "stdlib.h" //atof
#include "GeomUtils.h"
#include "AngleUtils.h"
#include <cmath> //hypot
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h" //MOOSFormat

using namespace std;

//---------------------------------------------------------
// Constructor

TurnSequence::TurnSequence()
{
  m_DeltaHeading   = 0;
  m_InitialSpeed   = 0;
  m_PtPrecision    = 2;
  m_InitialHeading = 0;
  m_FinalHeading   = 0;
  m_TurnDuration   = 0;
  m_TurnDirection  = 1;
  m_ActualDelta    = 0;
}

//---------------------------------------------------------
// Destructor

TurnSequence::~TurnSequence()
{
}

//---------------------------------------------------------
// Setters

void TurnSequence::setSpeed(double speed)
{
  m_InitialSpeed=speed;
}

//---------------------------------------------------------
// 
// Precision in the XYSegList is an unsigned int, so protect against
// accidentally setting an undesired high accuracy

void TurnSequence::setPtPrecision(int prec)
{
  if(prec>=0)
    m_PtPrecision=prec;
}

//TODO: domain checking/bounding on heading?
void TurnSequence::setDeltaHeading(double delta)
{
  m_DeltaHeading=delta;
}

void TurnSequence::setActualDelta(double delta)
{
  m_ActualDelta=delta;
}

void TurnSequence::setInitialHeading(double head)
{
  //TODO: domain checking?
  m_InitialHeading=head;
}

void TurnSequence::setFinalHeading(double head)
{
  //TODO: domain checking?
  m_FinalHeading=head;
}

void TurnSequence::setTurnDuration(double dur)
{
  if(dur>0)
    m_TurnDuration=dur;
}

void TurnSequence::setTurnDirection(int dir)
{
  m_TurnDirection=dir;
}

void TurnSequence::setPositionData(XYSegList points)
{
  m_PositionData=points;
}

//TODO: Should the string spec either report normalization
//(true/false) or by convention only spec a normalized turn? Does it
//matter? What happens if you try to normalize an already-normalized
//turn?

string TurnSequence::getSpec() const
{
  string pts = m_PositionData.get_spec_pts(m_PtPrecision);

#if 0
  string spec=MOOSFormat("dh=%.f,dh_act=%.f,i_hdg=%.f,spd=%.1f,dur=%.1f,",
                         m_DeltaHeading,m_ActualDelta,m_InitialHeading,
                         m_InitialSpeed,m_TurnDuration);
#endif
#if 1
  string spec= "dh=" + doubleToString(m_DeltaHeading,0);
  spec += ",dh_act=" + doubleToString(m_ActualDelta,0);
  spec += ",i_hdg=" + doubleToString(m_InitialHeading,0);
  spec += ",spd=" + doubleToString(m_InitialSpeed,1);
  spec += ",dur=" + doubleToString(m_TurnDuration,1) + ",";
#endif

  spec+=pts;
  // spec += "dh=";
  // spec += doubleToString(m_DeltaHeading,0);
  // spec += ",";
  // spec += "dh_act=";
  // spec += doubleToString(m_ActualDelta,0);
  // spec += ",";
  // spec += "i_hdg=";
  // spec += doubleToString(m_InitialHeading,0);
  // // spec += "f_head=";
  // // spec += doubleToString(m_FinalHeading,0);
  // spec += ",";
  // spec += "spd=";
  // spec += doubleToString(m_InitialSpeed,1);
  // spec += ",";
  // spec += "dur=";
  // spec += doubleToString(m_TurnDuration,1);
  // spec += ",";
  // spec += m_PositionData.get_spec_pts(m_PtPrecision);
  return(spec);
}

//Create the points section of a turn sequence from the string spec for
//an XYSegList
void TurnSequence::storePtsFromSpec(string pts)
{
  // string pts=spec;
  // biteString(pts,'=');
  
  pts=stripBraces(pts);
  
  vector<string> spts = parseString(pts,':');
  
  for(unsigned int i=0;i<spts.size();i++){
    string stri = spts[i];
    string sx = biteString(stri,',');
    
    double ix, iy;
    
    ix = atof(sx.c_str());
    iy = atof(stri.c_str());
    
    m_PositionData.add_vertex(ix,iy);
  }
}

//---------------------------------------------------------------
// Procedure: addPoint()

void TurnSequence::addPoint(double x, double y)
{
  m_PositionData.add_vertex(x,y);
}

//---------------------------------------------------------------
// Procedure: storeTurnFromSpec()
//      Note: Create a turn sequence from a string spec representation. 
//            String spec has the following format: 
//            dh=0,spd=0.0,dur=10.1,pts={0,-1:1,-1:2,-1:2,0}

bool TurnSequence::storeTurnFromSpec(string spec)
{
  // vector<string> pSpec=parseStringQ(spec,',');
  
  // // Validation: pSpec should have one element each for dh, spd, dur,
  // // and pts
  // if(pSpec.size()!=4)
  //   return(false);
  // for(int i=0;i<3;i++)
  //   biteString(pSpec[i],'=');
  
  m_PositionData.clear();
  
  string dh, spd, dur, pts, ihd, fhd, act;
  dh = tokStringParse(spec,"dh",',','=');
  spd = tokStringParse(spec,"spd",',','=');
  dur = tokStringParse(spec,"dur",',','=');
  pts = tokStringParse(spec,"pts",',','=');
  ihd = tokStringParse(spec,"i_hdg",',','=');
  // fhd = tokStringParse(spec,"f_head",',','=');
  act = tokStringParse(spec,"dh_act",',','=');

  m_DeltaHeading=atof(dh.c_str());
  m_InitialSpeed=atof(spd.c_str());
  m_TurnDuration=atof(dur.c_str());
  m_InitialHeading=atof(ihd.c_str());
  // m_FinalHeading=atof(fhd.c_str());
  m_ActualDelta=atof(act.c_str());

  storePtsFromSpec(pts);

  //TODO: more restrictive error checking?
  if(m_DeltaHeading==0 ||
     m_InitialSpeed==0 ||
     m_PositionData.size() < 2)
    return(false);
  
  return(true);
}

//---------------------------------------------------------------
// Procedure: projectTurn()
//   Purpose: Returns a seglist representation of this turn translated 
//            and rotated to correspond to the input arguments. valid 
//            choices for direction are:
//            -1 (turn to the left), 
//            1 (turn to the right), and
//            0 (special case for normalizing input data).

//TODO: since it returns a new XYSL instead of modifying, this really
//ought to be const. The use of "reflectTurn" prevents const, even
//though it's reversed at the end. Maybe refactor at some point to
//clean this up

XYSegList TurnSequence::projectTurn(double startX, double startY, 
            double startHead, int direction)
{
  //First reflect the turn if it will be to the left
  //TODO: This seems inefficient.
  if(direction==-1)
    reflectTurn();
  
  XYSegList newTurn;
  double x_offset, y_offset;

  unsigned int i, vsize = m_PositionData.size();
  
  for(i=0;i<vsize;i++) {
    double ix=m_PositionData.get_vx(i);
    double iy=m_PositionData.get_vy(i);
    
    //Store start of turn for translating other pts
    if(i==0) {
      x_offset=ix-startX;
      y_offset=iy-startY;
      newTurn.add_vertex(startX,startY);
    }
    else {
      //Translate to a turn started at startX/Y
      double px=ix-x_offset;
      double py=iy-y_offset;
      
      //XYSegList::rotate rotates about the centroid, which isn't what we
      //want here. Need to use the first point rather than the centroid.
      //Following rotation code modified from rotate_pt from XYSegList.cpp.
      
      double deg=startHead;
      
      double curr_dist  = hypot(px-startX, py-startY);
      double curr_angle = relAng(startX, startY, px, py);
      //TODO: I'm not positive that angle360 makes sense here,
      //particularly in the case where the wrap from 0 to 359 happens
      //partway through a turn. However, projectPoint applies angle360
      //to the angle argument, so that happens no matter what.
      
      //If we're normalizing a turn, we want to cancel out the initial
      //heading.
      double new_angle = 0;  // init added by mikerb 3/27/15
      if(direction==0)
        new_angle = -m_InitialHeading + curr_angle;
      //TODO: I want to require that initial heading and x/y are 0
      //when not normalizing a turn, but need to think through how to
      //do that WRT constructor values and string specs
      else if(direction==-1 || direction == 1)
        // new_angle = angle360(curr_angle + direction * deg);
        new_angle = angle360(deg + curr_angle);
      else
        {/*TODO: Any way to gracefully report this to user?*/}
      
      double nx, ny;
      projectPoint(new_angle, curr_dist, startX, startY, nx, ny);
      newTurn.add_vertex(nx,ny);
    }
  }
  
  //Restore the normal orientation if the turn was flipped in order to
  //turn left.
  if(direction==-1)
    reflectTurn();

  return(newTurn);
}

//---------------------------------------------------------------
// Procedure: normalizeTurn()
//            Translate the current TurnSequence to start at the origin, 
//            driving north, and turn to the right

void TurnSequence::normalizeTurn(bool keep_dir)
{
  XYSegList normalizedPoints;
  normalizedPoints=projectTurn(0,0,0,0);
  m_PositionData=normalizedPoints;
  
  if(!keep_dir){
    if(m_TurnDirection==-1){
      reflectTurn();
      m_TurnDirection=1;
      // m_DeltaHeading*=-1;
    }
  }
}

//---------------------------------------------------------------
// Procedure: normalizeTurn()
//            Reflect a normalized (i.e. initiated while driving north) 
//            turn across the y axis

void TurnSequence::reflectTurn()
{
  unsigned int i, vsize = m_PositionData.size();
  XYSegList reflectedPts;
  
  for(i=0;i<vsize;i++){
    double ix=m_PositionData.get_vx(i);
    double iy=m_PositionData.get_vy(i);
    // cout << "Initial point: (" << ix <<","<<iy<<"), reflected to (" << -ix <<"," << iy << ")" << endl;
    ix*=-1;
    reflectedPts.add_vertex(ix,iy);
  }
  
  m_PositionData=reflectedPts;
}

//---------------------------------------------------------------
// Procedure: getSegListSpec()

string TurnSequence::getSegListSpec() const
{
  string spec = m_PositionData.get_spec_pts(m_PtPrecision);
  return(spec);
}

//---------------------------------------------------------------
// Procedure: setDirFromSignedHead()

void TurnSequence::setDirFromSignedHead()
{
  if(m_ActualDelta<0)
    m_TurnDirection=-1;
  else
    m_TurnDirection=1;
}

//---------------------------------------------------------------
// Procedure: customProject()

void TurnSequence::customProject(double startX, double startY, 
         double startHead, int direction)
{
  int dir=direction;
  if(direction==0)
    dir=m_TurnDirection;
  XYSegList newPoints = projectTurn(startX, startY, startHead, dir);
  m_PositionData = newPoints;
}

//---------------------------------------------------------------
// Procedure: calcPointwiseHeading()

vector<double> TurnSequence::calcPointwiseHeading()
{
  vector<double> head;
  for(unsigned int i=1;i<m_PositionData.size();i++){
    double ax=m_PositionData.get_vx(i-1);
    double ay=m_PositionData.get_vy(i-1);
    double bx=m_PositionData.get_vx(i);
    double by=m_PositionData.get_vy(i);
    double ihead = relAng(ax,ay,bx,by);
    // cout << ihead << endl;
    head.push_back(ihead);
  }
  
  return(head);
}

//---------------------------------------------------------------
// Procedure: calcPointwiseHeading()
//            Assuming a normalized turn to the right for now. Not robust.

void TurnSequence::truncToHeadDiff(double diff)
{
  double final_head = m_DeltaHeading - diff;
  vector<double> heading = calcPointwiseHeading();
  unsigned int i;
  for(i=1;i<heading.size();i++){
    //Temporary measure to deal with noisy data
    if(heading[i]>130)
      continue;
    if(heading[i]>final_head)
      break;
  }
  
  XYSegList newpts=m_PositionData;
  
  unsigned int size=newpts.size();
  
  while(size>i){
    newpts.delete_vertex(size-1);
    size=newpts.size();
  }
  
  m_PositionData=newpts;
}
    
    
//---------------------------------------------------------------
// Procedure: removeDups()

void TurnSequence::removeDups()
{
  unsigned int i=0;
  while(i<m_PositionData.size()){
    //Starting from i+1 means only looking further down in the vector
    //for duplicates, which should be fine since the previous runs at
    //lower i would have checked i+1 to see if it was a dupe.
    for(unsigned int j=i+1;j<m_PositionData.size();j++){
      double ix=m_PositionData.get_vx(i);
      double iy=m_PositionData.get_vy(i);
      double jx=m_PositionData.get_vx(j);
      double jy=m_PositionData.get_vy(j);
      
      if(ix==jx && iy == jy){
  m_PositionData.delete_vertex(j);
  break; //Leave the for loop
      }
    }
    //I think that by putting this here, I make the assumption that
    //there will never be more than one duplicate. If I reach this
    //point, I either executed the break statement after deleting a
    //duplicate or I finished the for loop.
    i++;
  }
}

  
