/************************************************************/
/*    NAME: Kyle Woerner                                    */
/*    ORGN: MIT						    */
/*    FILE: TurnPoly.cpp                                    */
/*    DATE: 01 Sep 2014                                     */
/************************************************************/

#include "TurnPoly.h"
#include <vector>
#include <iostream>
#include "stdlib.h" //atof
#include <cmath> //hypot
#include "CurveFitting.h"

using namespace std;

//---------------------------------------------------------
// Constructor
TurnPoly::TurnPoly(){
    
    m_t_cutoff = 0;
    m_speed = 0;
    m_delta_hdg = 0;
    
    m_x_end = 0;
    m_y_end = 0;

    m_x_domain = 0;
    m_y_domain = 0;

    m_x_precision=0.8;       // x-precision for piecewise linear approximation
    m_point_precision = 0.1; // 0.1 indicates precision to decimeters

    m_rotation_angle = 0;


    m_simplify_by_angle = true;
    m_pwl_ang_tol = 10;
    m_max_pwl_segments = 8;
    m_tail_tolerance = 5;
    
    m_final_ray_is_delta_hdg = true;

    // arc variables
    m_radius = 0;
    m_x_center = 0;
    m_y_center = 0;
    m_theta_start = 0;
    m_theta_end = 0;
}

//---------------------------------------------------------
// Destructor
TurnPoly::~TurnPoly(){}
//---------------------------------------------------------
// Askers
bool TurnPoly::isProper() const{
  if(m_speed > 0){
    if(m_t_cutoff > 0){
      if(m_delta_hdg != 0){
	if(isPoly()){
	  if((m_x_end != 0) && (m_y_end != 0)){
	    if(m_point_precision > 0){
	      return true;
	    }
	  }
	}
	else if(isCircular()){
	  if(m_radius > 0){
	    return true;
	  }
	}
      }
    }
  }      
  return false;
}
bool TurnPoly::isPoly() const{
  if((m_poly_type=="poly") && (m_poly_coefficients.size() > 1)){
    return true;
  }
  else if(m_poly_type == "arc"){
    return false;
  }
  else{ // throw error??
    return 0;
  }
}
bool TurnPoly::isCircular() const{
    if(m_poly_type=="arc"){
        return true;
    }
    else if(m_poly_type == "poly"){
        return false;
    }
    else{ // throw error??
        return 0;
    }    
}

//---------------------------------------------------------
// Setters

bool TurnPoly::setSpeed(double spd){
  if(spd > 0){
    m_speed=spd;
    return true;
  }
  return false;
}

bool TurnPoly::setProjectPWLAngleTolerance(double din){
  if(din <= 0){
    return false;
  }
  m_pwl_ang_tol = din;
  return true;
}

bool TurnPoly::setMaxPWLSegments(unsigned int din){
  if(din <= 0){
    return false;
  }
  m_max_pwl_segments = din;
  return true;
}

bool TurnPoly::setLinearTailAngleTolerance(double din){
  if(din <= 0){
    return false;
  }
  m_tail_tolerance = din;
  return true;
}


void TurnPoly::setDeltaHeading(double delta){
  m_delta_hdg=angle360(delta);
}

bool TurnPoly::setPolyCoefficients(double tm_a4, double tm_a3, double tm_a2, double tm_a1, double tm_a0){
  vector<double> poly_coefficients;
  m_poly_type = "poly";
  poly_coefficients.push_back(tm_a0);
  poly_coefficients.push_back(tm_a1);
  poly_coefficients.push_back(tm_a2);
  poly_coefficients.push_back(tm_a3);
  poly_coefficients.push_back(tm_a4);
  return setPolyCoefficients(poly_coefficients); 
}
bool TurnPoly::setPolyCoefficients(vector<double> v_in){
  if(v_in.size() > 0){
    m_poly_coefficients = v_in;
    m_poly_type="poly";
    if(m_x_end != 0){
      setYEnd();
    }
    return true;
  }
  return false;
}
bool TurnPoly::setTCutoff(double txc){
  if(txc > 0){
    m_t_cutoff = txc;
    return true;
  }
  return false;
}
bool TurnPoly::setRotationAngle(double ang){
  if(ang < 85){
    if(ang > 5){
      m_rotation_angle = ang;
      setNFacingXYDomain();
      return true;
    }
  }
  return false;
}

bool TurnPoly::setXPrecision(double din){
  if(din > 0){
    m_x_precision = din;
    return true;
  }
  return false;
}
bool TurnPoly::setPtPrecision(double din){
  if(din > 0){
    m_point_precision = din;
    return true;
  }
  return false;
}
bool TurnPoly::setXYEnd(double xin, double yin){
  if(setXEnd(xin)){
    if(setYEnd(yin)){
      return true;
    }
  }
  return false;  
}
bool TurnPoly::getNFacingXYDomain(double& xout, double& yout){
  if(m_rotation_angle == 0){
    return false;
  }
  if((m_x_domain != 0 )){
    if(m_y_domain != 0){
      xout = m_x_domain;
      yout = m_y_domain;
      return true;
    }
    if(setYEnd()){
      xout = m_x_domain;
      yout = m_y_domain;
      return true;
    }
    return false;
  }
  return false;
}
bool TurnPoly::setNFacingXYDomain(){
  if(m_rotation_angle == 0){
    return false;
  }
  double xin = m_x_end;
  double yin = m_y_end;
  if((xin == 0) || (yin == 0)){
    return false;
  }
  
  m_x_domain =  xin * cos(radToDegrees(m_rotation_angle))
    + yin * sin(radToDegrees(m_rotation_angle));

  if(getPolyDegree() > 1){
    m_y_domain = evalPolyAtX(m_x_end);
    return true;
  }
  
  m_y_domain = -xin * sin(radToDegrees(m_rotation_angle))
    + yin * cos(radToDegrees(m_rotation_angle));

  return true;
}
bool TurnPoly::setXEnd(double din){
  if(din<0){
    return false;
  }
  m_x_end = din;
  if(getPolyDegree() > 1){
    m_y_end = evalPolyAtX(m_x_end);
  }
  return true;
}
bool TurnPoly::setYEnd(double din){
  if(m_x_end != 0){
    m_y_end = din;
    setNFacingXYDomain();
    return true;
  }
  return false;
}
bool TurnPoly::setYEnd(){
  if(m_x_end != 0){
    m_y_end=evalPolyAtX(m_x_end);
    return true;
  }
  return false;
}
      



bool TurnPoly::setXStart(double din){
  if(din<0){
    return false;
  }
  m_x_start = din;
  return true;
}
bool TurnPoly::setYStart(double din){
  if(m_x_start != -1){
    m_y_start = din;
    return true;
  }
  return false;
}


//---------------------------------------------------------
// Getters

vector<double> TurnPoly::getPolyCoefficients() const {
  return m_poly_coefficients;
}

XYSegList TurnPoly::projectPWLinear(){
  // options to simplify from discrete are: 1) angle tolerance to
  // simplify, and 2) max number of segments

  XYSegList xysl_out;
  xysl_out.add_vertex(0,0);
  XYSegList xysl = getNFaceDisc(0.1);
  
  for(unsigned int i = 0; i < xysl.size(); i++){
    xysl_out.add_vertex(xysl.get_vx(i),xysl.get_vy(i));
  }
  
  if(m_simplify_by_angle){    // angle tolerance
    xysl_out = simplifyLinearRegions(xysl_out, m_pwl_ang_tol);  
  }
  else{    // max number of segments
    unsigned int xysl_size = xysl_out.size();
    for(unsigned int count = 0; count < m_max_pwl_segments; count ++){
      xysl_out = simplifyLinearRegions(xysl_out, m_pwl_ang_tol);
      xysl_size = xysl_out.size();
      if(xysl_size <= m_max_pwl_segments){
        break;
      }
      else{
        cout << "ang_tol was " << doubleToString(m_pwl_ang_tol,2) << endl;
        m_pwl_ang_tol *= round(100 * (xysl_size) / (m_max_pwl_segments))/100;
        cout << "adjusted ang_tol to " << doubleToString(m_pwl_ang_tol,2) << endl;
      }
    }
  }
  
  updateRelativeTimes(xysl_out);
  return xysl_out;
}
XYSegList TurnPoly::projectPWLinearWithDynamicResolution(){
  // XYSL projectPWLinear() takes the current TurnPoly and discretizes
  // it for a piece-wise linear projection.  The output XYSegList is
  // returned without rotation (i.e., it will plot on top of the
  // continuous TurnPoly function) NOTE: for a NORTH-facing/CW+
  // output, use (TurnSequence) getNorthFacingTurnSequence() or
  // (XYSegList) getNFaceDisc(double) piecewise length and spacing is
  // computed using dynamic resolution as discussed below for constant
  // x-interval discretization, use
  // getXYSLForConstantXSamplingInterval(double, double);

  XYSegList xysl_out;
  xysl_out.add_vertex(0,0);
  long double x_a = 0;                                                                    
  long double y_a = 0;                                                                    
  vector<double> poly_coeff = getPolyCoefficients();
  long double y_b;                                                                        
  double x_step;  //step size along x-axis; nominally set to
		  //m_x_precision but different variable to account
		  //for negative x-direction
  if(m_x_end > 0){                                                                          
    x_step = m_x_precision;                                                               
  }                                                                                       
  else{                                                                                   
    x_step = -m_x_precision;                                                              
  }                                                                                        
  int count = 0;           
  
  for (long double x_b = 0; abs(x_b) <= abs(m_x_end); count++){ 
    // (x_a,y_a) are previous iteration's point
    // (x_b,y_b) are next full m_x_precision and its corresponding y-point

    // (x_i,y_i) are incremental points added for dynamic resolution
    // of regions with steep ascent
    
    y_b = evalPolyAtX(x_b); //poly_coeff[4]*pow(x_b,4) +
			    //poly_coeff[3]*pow(x_b,3) +
			    //poly_coeff[2]*pow(x_b,2) +
			    //poly_coeff[1]*pow(x_b,1) +
			    //poly_coeff[0];
    
    float delta_y = (y_b - y_a);
    
    // START DYNAMIC RESOLUTION If x-precision gives canonical
    // spacing, then this dynamic resolution algorithm; increases the
    // number of points in places of steep ascent to better shape the
    // result.  Evaluate f(x_i+1) and see if it is more than
    // #*x-precision, and if so, then set sub_domain precision to
    // 1/delta-y until back within one x-precision step in y (small 45
    // degree step for #=2) Adjust resolution for steep ascent regions
    long double x_i = x_a;
    int counter = 1;
    while( (abs(delta_y / m_x_precision) > 0.4) ){ //0.2) ){
      int sign_xstep = copysign(1,x_step); // +/- 1 corresponding to
					   // direction of step
      // ORIGINAL:
      //x_i = x_i + sign_xstep*(1 / abs(delta_y));
      
      //      x_i = x_i + sign_xstep* min(max((1 /
      //      abs(delta_y)),(float) (m_x_precision / 2)),(float)
      //      (m_x_precision / 2));
      x_i = x_i + sign_xstep* min(max((1 / abs(delta_y)),(float) (m_x_precision)),(float) (m_x_precision / 2));

      if(abs(x_i - x_b) < abs(0.3 * m_x_precision)){
	// don't want an x_i right next to x_b;
	//	x_b = x_i;
	//	y_b = evalPolyAtX(x_b);
	break;
      }
      
      if(abs(x_i) > abs(x_b)){break;}
      long double y_i = evalPolyAtX(x_i); 
      if((x_i == 0) && ( y_i < 0 )){
	// case where y-intercept of TurnPoly is negative.

	// Desire the negative y-intercept to be omitted from plotting
	// but still be used in computations.  inside this if is the
	// case where no plotting occurs.
      }
      else{
	// everything other than negative y-intercept.
	xysl_out.add_vertex(x_i,y_i);
      }
      float new_delta_y = abs(y_b) - abs(y_i);
      if(abs(new_delta_y - delta_y) > 0.5 * abs(max(delta_y, new_delta_y))){
	// delta y has gone from very large to very small.  We are needing more samples.
	delta_y *= 0.5;
      }
      counter++;
    }
    // END DYNAMIC RESOLUTION
    
    
    
    // add this iteration's point
    if((x_b == 0) && (y_b < 0)){
      // case where y-intercept of TurnPoly is negative.  Desire the negative y-intercept to be omitted from plotting but still be used in computations.
      //inside this if-statement is the case where no plotting occurs.
    }
    else{
      // everything other than negative y-intercept.
      xysl_out.add_vertex(x_b,y_b);
    }
    
    // set x_a to be the current x_b for next iteration
    x_a = x_b;
    y_a = y_b;
    
    // advance x_b by step value; y_b will be computed on top of next iteration
    x_b += x_step;
        
    // desire one last point to correspond with desired cutoff position once normal 
    // iteration would exceed value of m_x_end
    if((abs(x_b) > abs(m_x_end))){
      if((x_a != m_x_end)){
	if(abs(xysl_out.get_vx(xysl_out.size()-1) - m_x_end) < 1 * abs(m_x_precision)){
	  if(xysl_out.size() > 2){
	    xysl_out.delete_vertex(xysl_out.size()-1); // remove closest point to end to avoid clumping but want at least 2 pts
	  }
	}
	xysl_out.add_vertex(m_x_end,m_y_end);
      }
    }
  }

  xysl_out = simplifyLinearRegions(xysl_out);
  updateRelativeTimes(xysl_out);
  
  
  
  return xysl_out;
}
bool TurnPoly::updateRelativeTimes(XYSegList xysl){
  unsigned int xysl_size = xysl.size();
  if(xysl_size < 2){
    cout << "returned xysl < 2 in updateRelativeTimes()" << endl;
    return false;
  }
  double total_length = 0;
  XYPoint xy1, xy2;
  vector<double> rel_distances;


  double x1,y1,x2,y2 = 0;
  
  for(unsigned int i = 0; i < xysl_size - 1; i++){
    x1 = xysl.get_vx(i);
    y1 = xysl.get_vy(i);
    x2 = xysl.get_vx(i+1);
    y2 = xysl.get_vy(i+1);
    double length = distPointToPoint(x1,y1,x2,y2);
    //    cout << "length = " << doubleToString(length,2) << endl;
    rel_distances.push_back(length);
    total_length += length;
  }
  double rel_time = 0;
  m_relative_timestamps.clear();
  double cum_time = 0;
  for(unsigned int i = 0; i < xysl_size - 1; i++){
    rel_time = m_t_cutoff * rel_distances[i] / total_length;
    cum_time += rel_time;
    m_relative_timestamps.push_back(rel_time);
    m_cumulative_timestamps.push_back(cum_time);
  }
  return true;
}
bool TurnPoly::getSimplifiedNorthFacingTurn(Seglr& seglr){
  if(!isProper()){
    return false;
  }
  XYSegList xysl = getNorthFacingTurnSequence().getPositionData();
  seglr = Seglr(m_delta_hdg);
  for(unsigned int i = 0; i < xysl.size(); i++){
    seglr.addVertex(xysl.get_vx(i),xysl.get_vy(i));
  }
  return true;
}

TurnSequence TurnPoly::getNorthFacingTurnSequence(){
  // inherits attributes of TurnSequence from which TurnPoly was
  // created output is NORTH-facing/CW+ for an original
  // NORTH-facing/CW+ TurnSequence input
  TurnSequence ts_out;

  if(!isProper()){
    return ts_out;
  }
  
  XYSegList xysl;
  xysl = projectPWLinear();
  xysl.get_spec_pts(5);
  ts_out.setPositionData(xysl);

  ts_out.setDeltaHeading(getDeltaHeading());
  ts_out.setSpeed(getSpeed());
  ts_out.setTurnDuration(getTCutoff());

  xysl = ts_out.getPositionData();

  // adjust negative X values to the y-axis
  //  for (unsigned int i = 1; i < (xysl.size() - 1); i++){
  for (unsigned int i = 1; i < (xysl.size()); i++){
    if(i == xysl.size()){
      break;
    }
    if(xysl.get_vx(i) > 1){
      break;
    }
    double x1 = xysl.get_vx(i);
    if(x1 < 0) {
      // first point was rotated into quadrant II.  Move to the y-axis along line to third point
      double y1 = xysl.get_vy(i);
#if 0
      double y2 = xysl.get_vy(i+1);
      double x2 = xysl.get_vx(i+1);
      double slope = (y2 - y1) / (x2 - x1);
#endif
      double yb = y1; //y1 - slope * x1;
      xysl.delete_vertex(i); // first point after (0,0)
      xysl.insert_vertex(0,yb);
    }
  }
  ts_out.setPositionData(xysl);
  
  setFinalRay(ts_out.getPositionData());
  return ts_out;
}
bool TurnPoly::setFinalTangent(XYSegList xysl_in){
  unsigned int sl_size = xysl_in.size();
  if(sl_size < 1){
    return false;
  }
  double xend =  xysl_in.get_vx(sl_size-1);
  double yend =  xysl_in.get_vy(sl_size-1);
  double din = (relAng(xysl_in.get_vx(sl_size-2),
		       xysl_in.get_vy(sl_size-2),
		       xend,yend));

  if(m_final_ray_is_delta_hdg){
    m_final_tangent = angle360( (m_delta_hdg)); // [degrees]
  }
  else{
    m_final_tangent = angle360( (din));// [degrees]
  }
  return true;
}


bool TurnPoly::setNumUnderlyingPoints(unsigned int num_pts){
  if(num_pts <= 0){
    return false;
  }
  m_num_underlying_pts = num_pts;
  return true;
}
bool TurnPoly::setFinalRay(double radius){
  XYSegList xysl;
  xysl.add_vertex(0,0);
  xysl.add_vertex(m_x_domain,m_y_domain);
  return setFinalRay(xysl);
}

bool TurnPoly::setFinalRayMode(string mode){
  return setBooleanOnString(m_final_ray_is_delta_hdg,mode,true);
}
bool TurnPoly::setProjectPWLMode(string mode){
  if(mode == "delta_angle"){
    m_simplify_by_angle = true;
    return true;
  }
  else if(mode == "num_segments"){
    m_simplify_by_angle = false;
    return true;
  }
  return false;
}

bool TurnPoly::setFinalRay(const XYSegList pw_linear, double radius, double& y_b){
  // Assumes pw_linear is the XYSegList representation of a
  // North-facing/CW+ trajectory. Output is a two-point XYSegList
  // representing an extension of the final trajectory point in the
  // direction tangent to the Final trajectory by NAV_X/NAV_Y (not
  // necessarily equivalent to final desired heading)

  // x_0 is final point, x_a is penultimate point, x_b is projected
  // point from x_0 of length r

  radius = abs(radius);
  XYSegList xysl_out;
  int xysl_size = pw_linear.size();
  double x_0, y_0, x_b, theta, r = radius;
  
  x_0 = pw_linear.get_vx(xysl_size-1);
  y_0 = pw_linear.get_vy(xysl_size-1);

#if 0
  setFinalTangent(pw_linear);
#endif
  
  //  theta = headingToRadians(m_final_tangent); // [degrees -> radians]
  theta = headingToRadians(m_delta_hdg); // [degrees -> radians]
 
  x_b = x_0 + r * cos(theta);
  y_b = y_0 + r * sin(theta);
      
  xysl_out.add_vertex(x_0,y_0);   // final point of XYSL is first point of ray
  xysl_out.add_vertex(x_b,y_b);   // point projected of length r

  m_final_ray = xysl_out;
  return true;
}
bool TurnPoly::setFinalRay(const XYSegList pw_linear){
  double foo = 0;
  return setFinalRay(pw_linear,2,foo);
}


XYSegList TurnPoly::getXYSLForConstantXSamplingInterval(double x_interval) const{
  double max_x = m_x_end;
  return getXYSLForConstantXSamplingInterval( x_interval,  max_x);
}


XYSegList TurnPoly::getXYSLForConstantXSamplingInterval(double x_interval, double max_x) const{
  // max_x is the maximum desired sampling x-value. necessary for interpolation algorithm due to not wanting to exceed well-modeled domain of the other TurnPoly in question.

  XYSegList xysl_out;
  xysl_out.insert_vertex(0,0);
  long double x_b = 0;
  long double y_b = 0;
  vector<double> poly_coeff = getPolyCoefficients();
  if(m_x_end < 0){ // account for quadrant two TPs
    x_interval = -x_interval;
  }

  for(x_b = x_interval; abs(x_b) <= max_x; x_b += x_interval){
    y_b = evalPolyAtX(x_b); 
    xysl_out.add_vertex(x_b,y_b);
    //    cout << "sample (x,y) = "<< doubleToString(x_b,1) <<","<<doubleToString(y_b,1) << ")" << endl;
  }

  if(x_b != max_x){ // final sample at max_x
    xysl_out.delete_vertex(x_b,y_b);
    x_b = max_x;
    y_b = evalPolyAtX(x_b); 
    xysl_out.add_vertex(x_b,y_b);
  }
  return xysl_out;
}

XYSegList TurnPoly::getNFaceDisc(double res) const{
  // Get a North facing discretization of the poly
  // This is a dense representation without simplification or smoothing
  XYSegList rotated = getXYSLForConstantXSamplingInterval(res);

// #if 0  // original follows
  
//   // INSERT NEGATIVE X FIX HERE...
//   double x1 = rotated.get_vx(1);
//   if(x1 < 0) { // first point was rotated into quadrant II.  Move to the y-axis along line to third point
//     double y2 = rotated.get_vy(2);
//     double y1 = rotated.get_vy(1);
//     double x2 = rotated.get_vx(2);
//     double slope = (y2 - y1) / (x2 - x1);
//     double yb = y1 - slope * x1;
//     rotated.insert_vertex(0,yb);
//     rotated.delete_vertex(1); // first point after (0,0)
//   }

// #endif
  
  TurnSequence out;
  out.storePtsFromSpec(rotated.get_spec_pts(5));
  
  out.customProject(0,0,-1*m_rotation_angle,0);
// #if 0 // altered follows
//   rotated = out.getPositionData();
//   // INSERT NEGATIVE X FIX HERE...
//   for (unsigned int i = 1; i < 10; i++){
//     if(i == rotated.size()){
//       break;
//     }
//     double x1 = rotated.get_vx(i);
//     //    cout << "x1 = " << doubleToString(x1,2) << endl;
//     if(x1 < 0) { // first point was rotated into quadrant II.  Move to the y-axis along line to third point
//       double y2 = rotated.get_vy(i+1);
//       double y1 = rotated.get_vy(i);
//       double x2 = rotated.get_vx(i+1);
//       double slope = (y2 - y1) / (x2 - x1);
//       double yb = y1 - slope * x1;
//       rotated.insert_vertex(0,yb);
//       rotated.delete_vertex(i); // first point after (0,0)
//     }
//   }
//   XYSegList final = rotated;
// #endif

#if 1
    XYSegList final = out.getPositionData();

#endif
  //getXYSL... includes a point at 0,0 that isn't really part of the
  //poly, but having the first point at zero simplifies rotation back
  //to north. Don't need it anymore.
    final.delete_vertex(0,0);

  return(final);
}

double TurnPoly::evalPolyAtX(double x_b) const {
  // evaluate the polynomial at a given x_b
  vector<double> poly_coeff = getPolyCoefficients();
  //  return poly_coeff[4]*pow(x_b,4) + poly_coeff[3]*pow(x_b,3) + poly_coeff[2]*pow(x_b,2) + poly_coeff[1]*pow(x_b,1) + poly_coeff[0];
  float result = 0;
  //int j;
  int poly_size = poly_coeff.size();
  for(int i = 0; i < poly_size; i++){
    //    j = poly_size - i - 1;
    //    cout << doubleToString(poly_c << endl;
    result +=  poly_coeff[i] * pow(x_b,i);  // orig
    //    result +=  poly_coeff[i] * pow(x_b,j);
  }
  return result;
}

string TurnPoly::getSpec() const {
  if(!isProper()){
    string str =  "improper TurnPoly for spd = ";
    str += doubleToString(m_speed,1) + ", hdg = " + doubleToString(m_delta_hdg,0);
    return str;
  }
  
  string spec;

  spec += "spd=";
  spec += doubleToString(getSpeed(),1);
  spec += ",";

  spec += "dh=";
  spec += doubleToString(getDeltaHeading(),1);
  spec += ",";

  spec += "type=";
  spec += m_poly_type;
  spec += ",";

  spec += "coeffs=";
  vector<double> poly_coef = getPolyCoefficients();
  int psize = poly_coef.size();
  for(int i = 0; i < psize; i++){
    spec+= doubleToString(poly_coef[i],5);
    if(i != psize-1){
      spec += ":";
    }
  }
  spec += ",";

  spec += "xend=";
  spec += doubleToString(m_x_end,1);
  spec += ",";

  spec += "rot=";
  spec += doubleToString(getRotationAngle(),3);
  spec += ",";

  spec += "time=";
  spec += doubleToString(getTCutoff(),1);

  return(spec);
}

string TurnPoly::getVerboseSpec() const {
  string spec;

  spec += "tp_type=";
  spec += m_poly_type;
  spec += ",";

  spec += "poly_coef=";
  vector<double> poly_coef = getPolyCoefficients();
  int psize = poly_coef.size();
  for(int i = 0; i < psize; i++){
    spec+= doubleToString(poly_coef[i],5);
    if(i != psize-1){
      spec += ":";
    }
  }
  spec += ",";

  spec += "x_end=";
  spec += doubleToString(m_x_end,1);
  spec += ",";
  spec += "y_end=";
  spec += doubleToString(m_y_end,1);
  spec += ",";

  spec += "x_domain=";
  spec += doubleToString(m_x_domain,1);
  spec += ",";
  spec += "y_domain=";
  spec += doubleToString(m_y_domain,1);
  spec += ",";

  spec += "rotation_angle=";
  spec += doubleToString(getRotationAngle(),3);
  spec += ",";

  spec += "pt_precision=";
  spec += doubleToString(getPtPrecision(),2);
  spec += ",";

  spec += "init_speed=";
  spec += doubleToString(getSpeed(),1);
  spec += ",";

  spec += "delta_hdg=";
  spec += doubleToString(getDeltaHeading(),1);
  spec += ",";

  spec += "t_cutoff=";
  spec += doubleToString(getTCutoff(),1);
  spec += ",";

  return(spec);
}

bool TurnPoly::getRelativeTimestamps(string& spec) const{
  // return true if PWL already projected, false otherwise; places
  // relative time stamp string in spec.
  
  spec = "";
  unsigned int vsize = m_relative_timestamps.size();
  // add t_vector iff PWLinear already projected
  if(vsize > 0){
    spec += "t_vector=";
    for(unsigned int i = 0; i < vsize; i++){
      spec += doubleToString(m_relative_timestamps[i],5);
      spec += "#";
    }
    return(true);
  }
  return false;
}
bool TurnPoly::getCumulativeTimestamps(string& spec) const{
  // return true if PWL already projected, false otherwise; places
  // cumulative time stamp string in spec.
  
  spec = "";
  unsigned int vsize = m_cumulative_timestamps.size();
  // add t_vector iff PWLinear already projected
  if(vsize > 0){
    spec += "t_vector=";
    for(unsigned int i = 0; i < vsize; i++){
      spec += doubleToString(m_cumulative_timestamps[i],5);
      spec += "#";
    }
    return(true);
  }
  return false;
}
/*
bool TurnPoly::populateFromTPSpec(string spec) {
  // deprecated
  string str_tp_type, str_poly_coef, str_x_end, str_y_end, str_rotation_angle;
  string str_pt_precision, str_init_speed, str_delta_hdg, str_t_cutoff;
  if(tokStringParse(spec,"tp_type",',','=') == "poly"){
    // parse data as strings
    str_poly_coef = tokStringParse(spec,"poly_coef",',','=');
    str_x_end = tokStringParse(spec,"x_end",',','=');
    str_y_end = tokStringParse(spec,"y_end",',','=');
    str_rotation_angle = tokStringParse(spec,"rotation_angle",',','=');
    str_pt_precision = tokStringParse(spec,"pt_precision",',','=');
    str_init_speed = tokStringParse(spec,"init_speed",',','=');
    str_delta_hdg = tokStringParse(spec,"delta_hdg",',','=');
    str_t_cutoff = tokStringParse(spec,"t_cutoff",',','=');

    // populate the TurnPoly
    storeCoefficientsFromSpec(str_poly_coef);
    setXEnd(atof(str_x_end.c_str()));
    setYEnd(atof(str_y_end.c_str()));
    setRotationAngle(atof(str_rotation_angle.c_str()));
    setPtPrecision(atof(str_pt_precision.c_str()));
    setSpeed(atof(str_init_speed.c_str()));
    setDeltaHeading(atof(str_delta_hdg.c_str()));
    setTCutoff(atof(str_t_cutoff.c_str()));
    return isProper();
  }
  else{
    return false;
  }
}
*/
bool TurnPoly::generateFromSpec(string spec) {
  string str_tp_type, str_poly_coef, str_x_end, str_y_end, str_rotation_angle;
  string str_pt_precision, str_init_speed, str_delta_hdg, str_t_cutoff;
  if(tokStringParse(spec,"type",',','=') == "poly"){
    // parse data as strings
    str_init_speed = tokStringParse(spec,"spd",',','=');
    str_delta_hdg = tokStringParse(spec,"dh",',','=');

    str_poly_coef = tokStringParse(spec,"coeffs",',','=');
    str_x_end = tokStringParse(spec,"xend",',','=');

    str_rotation_angle = tokStringParse(spec,"rot",',','=');
    str_pt_precision = tokStringParse(spec,"pt_precision",',','=');
    str_t_cutoff = tokStringParse(spec,"time",',','=');

    // populate the TurnPoly
    storeCoefficientsFromSpec(str_poly_coef);
    setXEnd(atof(str_x_end.c_str()));
    setYEnd();  
    setRotationAngle(atof(str_rotation_angle.c_str()));

    setSpeed(atof(str_init_speed.c_str()));
    setDeltaHeading(atof(str_delta_hdg.c_str()));
    setTCutoff(atof(str_t_cutoff.c_str()));
    return isProper();
  }
  else{
    return false;
  }
}



bool TurnPoly::storeCoefficientsFromSpec(string pts){
  vector<string> spts = parseString(pts,':');
  vector<double> poly_vect;
 
  for(unsigned int i = 0; i < spts.size() ; i++){
    poly_vect.push_back(atof(spts[i].c_str()));
  }
  return setPolyCoefficients(poly_vect);  
}




string TurnPoly::getString() const {

  string str = getSpec();
  string out = findReplace(str,",","\n");
  out = "\nTurnPoly getString():" + out + "\n----- END TurnPoly getString() -----";
  return out;
}


bool TurnPoly::generateTurnPolyFromTurnSequence(TurnSequence ts_in, double rotation_angle){
  // Note: assumes single quartic function for model unless otherwise
  // told; assumes ts_in is NORTH-facing; this function rotates data
  // internally as needed.
  bool verbose = false;
  vector<double> pfit4;  // polynomial coefficients
  XYSegList ts_xysl;     // SegList for TurnSeq (passed to polyfit)
  
  if(verbose){cout<<"ts_in  (size=" << doubleToString((ts_in.getPositionData()).size(),0) << ") spec: "<<ts_in.getSegListSpec() << endl;}
  
  bool trunc_needed = truncateNFacingTurnSeqAtMaxAngle(ts_in,140);
  if(verbose){cout << "trunc_needed_excessive_turn_angle(140) = " << boolToString(trunc_needed) << endl;}
  if(verbose){cout<<"post-truncation north-facing (size=" << doubleToString((ts_in.getPositionData()).size(),0) << ") spec: "<<  getMatlabVector(ts_in.getPositionData()) << endl;}

  ts_in = removeLinearTail(ts_in);
  if(verbose){cout<<"post-remLinTail north-facing (size=" << doubleToString((ts_in.getPositionData()).size(),0) << ") spec: "<<  getMatlabVector(ts_in.getPositionData()) << endl;}

  double xstart = 0, ystart = 0;

#if 0 // reserved for future work
  xstart = -1;
  ystart = -1;
  ts_in = removeLinearBeginning(ts_in, xstart, ystart);
#endif
  if(verbose){cout<<"post-remLinBeginning north-facing (size=" << doubleToString((ts_in.getPositionData()).size(),0) << ") spec: "<<  getMatlabVector(ts_in.getPositionData()) << endl;}

  setRotationAngle(rotation_angle);
  // rotation angle set before other TP variables to facilitate use in
  // setting proper tangent angle.  Tangent angle set in
  // removeLinearTails function to draw final ray on AVD Table.
 
  ts_in.customProject(0,0,rotation_angle,0);

  if(verbose){cout<<"post non-north rotation (size=" << doubleToString((ts_in.getPositionData()).size(),0) << ") spec: " <<  getMatlabVector(ts_in.getPositionData()) <<  endl;}
  
  
  ts_xysl = ts_in.getPositionData();
  unsigned int num_xy_points = ts_xysl.size();

  if(verbose){for(unsigned int i = 0; i < ts_xysl.size(); i++){cout << "x= " << doubleToString(ts_xysl.get_vx(i),3) << ";  y= " << doubleToString(ts_xysl.get_vy(i),3) << endl; }
    cout << "xend: " << doubleToString((ts_xysl.get_vx(num_xy_points-1)),3) << endl;
    cout << "TurnPoly generation from TurnSequence Complete." << endl;
  }


  double xi, max_x = 0;
  // check for bogus points left of true max_x
  for(unsigned int i = 0; i < ts_xysl.size(); i++){
    xi = ts_xysl.get_vx(i);
    if(xi > max_x){
      max_x = xi;
    }
  }
  
  pfit4 = polyfit4(ts_xysl);
  // initialize TurnPoly
  setSpeed(ts_in.getSpeed());
  setTCutoff(ts_in.getTurnDuration());
  setDeltaHeading((ts_in.getDeltaHeading()));
  setFinalTangent(ts_xysl);
  setPolyCoefficients(pfit4);
  setXEnd(max_x);
  setXStart(xstart);
  setYStart(ystart);

  if(isProper()){
    return true;
  }

  else{
    if(verbose){
      cout << "Bad data passed to TurnPoly from TurnSequence... "
           << "Unable to generate TurnPoly" << endl;
      cout << getString() << endl;
    }
    return false;
  }
}
