// Kyle Woerner
// k_w@mit.edu
// functions useful for regression of data

#include <string>
#include <cmath>
#include <vector>
#include <iostream>
#include "AngleUtils.h"
#include "XYSegList.h"
#include "MOOS/libMOOS/Utils/MOOSUtilityFunctions.h" //MOOSFormat
#include "CurveFitting.h"

using namespace std;

bool verbose = true;

vector<double> leastSquares(XYSegList xy_in,const int degree_fit){
  // ########################################################################################//
  // Polynomial Least Squares Approximation Using Linear Methods
  // (Gauss-Seidel)
  // ########################################################################################//
  // Input: an XYSegList of (x,y) points Output: coefficients of a
  // polynomial of degree [degree_fit] approximating the data
  
  //*** // NOTE:  all data must start from origin with proper rotation for convergence
  vector<double> v_out;

  // initialize algorithm variables
  long int max_iterations = pow(10,6);
  float iter_tolerance = pow(10,-5);

  double x_trans = 0, y_trans = 0;
  // translate first point to origin for translation and proper
  // computation for convergence
    
    
  vector <double> xdata,ydata,weight;
  // initialized poly coefficients; values representative of typical
  // marine vehicle turning but any non-zero values are acceptable.

  long double a0=100,a1=-100,a2=5 ,a3=0.5,a4=0.5;
  // initialize summation variables to zero.

  long double SX1=0,SX2=0,SX3=0,SX4=0,SX5=0,SX6=0,SX7=0,SX8=0,SY=0,SX1Y=0,SX2Y=0,SX3Y=0,SX4Y=0;

  double weightedN = 0;
  // place holder for reweighting data at later time

  long double a0old=0.1, a1old=0.1,a2old=0.1, a3old=0.1,a4old=0.1;
  // initialized to non-zero values due to division within algorithm;
  // one-iteration memory of poly coefficient values for use in linear
  // methods rather than matrix operations.

  bool a0tol=false, a1tol=false, a2tol=false, a3tol=false, a4tol=false;
  // boolean variables for status of reaching tolerance of each
  // coefficient.

  // passed variables
  const int amax = degree_fit;
  // desired degree of curve fitting (1-linear, 2-quadratic, 3-cubic,
  // 4-quartic)

  int N = xy_in.size();
  
  x_trans = xy_in.get_vx(0);
  y_trans = xy_in.get_vy(0);
    
  // extract xdata and ydata from XYSegList
  for(int count = 0; count < N; count++){
    xdata.push_back(xy_in.get_vx(count) - x_trans);
    // subtract out translation

    ydata.push_back(xy_in.get_vy(count) - y_trans);
    // subtract out translation
    
    weight.push_back(01);
    // full weight of 1 until weighting implemented
  }

    // compute sums for iterations
    for(int i=0; i < N ; i++){
        weightedN += weight[i];
        SX1 += xdata[i];
        SX2 += pow((weight[i] * xdata[i]),2);
        SX3 += pow((weight[i] * xdata[i]),3);
        SX4 += pow((weight[i] * xdata[i]),4);
        if(amax>=3){
            SX5 += pow((weight[i] * xdata[i]),5);
            SX6 += pow((weight[i] * xdata[i]),6);
        }
        if(amax>=4){
            SX7 += pow((weight[i] * xdata[i]),7);
            SX8 += pow((weight[i] * xdata[i]),8);
        }
        
        SY   += (ydata[i]);
        SX1Y += pow((weight[i] * xdata[i]),1) * weight[i] * ydata[i];
        SX2Y += pow((weight[i] * xdata[i]),2) * weight[i] * ydata[i];
        if(amax>=3){
            SX3Y += pow((weight[i] * xdata[i]),3) * weight[i] * ydata[i];
        }
        if(amax>=4){
            SX4Y += pow((weight[i] * xdata[i]),4) * weight[i] * ydata[i];
        }

    }

    //iterate on coefficients rather than call a matrix library
    long double a0inv = pow(weightedN,-1);
    long double a1inv = pow(SX2,-1);
    long double a2inv = pow(SX4,-1);
    long double a3inv = pow(SX6,-1);
    long double a4inv = pow(SX8,-1);
    
    for(long int j = 0; j < max_iterations; j++){

        a0 = a0inv   * (SY              - a1 * SX1 - a2 * SX2 - a3 * SX3 - a4 * SX4);
        a1 = a1inv * (SX1Y - a0 * SX1            - a2 * SX3 - a3 * SX4 - a4 * SX5);
        
        switch(amax){
            case 1:
                a2 = 0;
                a2tol = true;
                a3 = 0;
                a3tol = true;
                a4 = 0;
                a4tol = true;
                break;
            case 2:
                a2 = a2inv * (SX2Y - a0 * SX2 - a1 * SX3            - a3 * SX5 - a4 * SX6);
                a3 = 0;
                a3tol = true;
                a4 = 0;
                a4tol = true;
                break;
            case 3:
                
                a2 = a2inv * (SX2Y - a0 * SX2 - a1 * SX3            - a3 * SX5 - a4 * SX6);
                a3 = a3inv * (SX3Y - a0 * SX3 - a1 * SX4 - a2 * SX5            - a4 * SX7);
                a4 = 0;
                a4tol  =true;
                break;
            case 4:
                a2 = a2inv * (SX2Y - a0 * SX2 - a1 * SX3            - a3 * SX5 - a4 * SX6);
                a3 = a3inv * (SX3Y - a0 * SX3 - a1 * SX4 - a2 * SX5            - a4 * SX7);
                a4 = a4inv * (SX4Y - a0 * SX4 - a1 * SX5 - a2 * SX6 - a3 * SX7           );
                break;
            default:
                if(amax>4){cout << "ERROR::: polynomial of degree > 4 attempted in CurveFitting algorithm." << endl; break;}
        }

        if((a0tol) || (abs((a0 - a0old)/a0old )< iter_tolerance)){
            if(!a0tol){
                a0tol = true;
            }
            if((a1tol) || (abs((a1 - a1old)/a1old )< iter_tolerance)){
                if(!a1tol){
                    a1tol = true;
                }
                if((a2tol) || (abs((a2 - a2old)/a2old )< iter_tolerance) || (amax<2)){
                    if(!a2tol){
                        a2tol = true;
                    }
                    if((a3tol) || (abs((a3 - a3old)/a3old )< iter_tolerance) || (amax<3)){
                        if(!a3tol){
                            a3tol = true;
                        }
                        if ((abs((a4 - a4old)/a4old )< iter_tolerance) || (amax<4)) {
                            break;  // all a_i are within tolerance
                            // a4tol reserved for future consideration
			    if(!a4tol){
			      a4tol = true;
			    }
			}
                    }
                }
            }
        }
        
        // adjust new to old and repeat
        a0old = a0;
        a1old = a1;
        a2old = a2;
        a3old = a3;
        a4old = a4;
        
        if(j == max_iterations -1){
          cout << "**** exiting due to exceeding max_iterations ****  ( " << j << " iterations)" << endl;
          return v_out;
        }
    }
    v_out.push_back(a0);
    v_out.push_back(a1);
    v_out.push_back(a2);
    v_out.push_back(a3);
    v_out.push_back(a4);
    return v_out;
}


vector<double> polyfit1(XYSegList xy_in){
    return leastSquares(xy_in,1);
}
vector<double> polyfit2(XYSegList xy_in){
    return leastSquares(xy_in,2);
}
vector<double> polyfit3(XYSegList xy_in){
    return leastSquares(xy_in,3);
}
vector<double> polyfit4(XYSegList xy_in){
    return leastSquares(xy_in,4);
}

vector<double> polyfit1(TurnSequence ts_in){
  return leastSquares(ts_in.getPositionData(),1);
}
vector<double> polyfit2(TurnSequence ts_in){
    return leastSquares(ts_in.getPositionData(),2);
}
vector<double> polyfit3(TurnSequence ts_in){
    return leastSquares(ts_in.getPositionData(),3);
}
vector<double> polyfit4(TurnSequence ts_in){
    return leastSquares(ts_in.getPositionData(),4);
}

bool truncateNFacingTurnSeqAtMaxAngle(TurnSequence& ts_in,double max_allowed_ang){
  // returns TurnSeq as ts_in truncated at point of first angle
  // exceeding max_allowed_ang return true if truncation necessary; false
  // otherwise
  XYSegList xysl = ts_in.getPositionData();
  double x1,x2,y1,y2, relang;
  unsigned int sl_size = 0;
  
  max_allowed_ang = angle360(max_allowed_ang);
  sl_size = xysl.size();

  if(sl_size<2){
    if(verbose){cout << "truncateTS: size < 2.  size = " << doubleToString(sl_size,0) << endl;}
    return false;
  }
    
  for(unsigned int i = 0; i < sl_size -1; i++){
    // -1 to avoid out of bounds on (x2,y2)
    x1 = xysl.get_vx(i);
    y1 = xysl.get_vy(i);
    x2 = xysl.get_vx(i+1);
    y2 = xysl.get_vy(i+1);
    relang = angle180(relAng(x1,y1,x2,y2));
    // ang180 prevents 359.9 @ start

    if(verbose){cout << "x,y,x,y,ang=" <<
        doubleToString(x1,4) << "," <<
        doubleToString(y1,4) << "," <<
        doubleToString(x2,4) << "," <<
        doubleToString(y2,4) << "," <<
        doubleToString(relang,2) << endl;}
    
    if(relang > max_allowed_ang){
      unsigned int num_del = 0;
      for(unsigned int j = i+1; j < sl_size; j++){
        // deletion of [i+1] because j++ while indices of xysl shift
        // left
        if(verbose){cout << "truncate: removed point (x,y) = (" <<
            doubleToString(xysl.get_vx(i+1),2) << "," <<
            doubleToString(xysl.get_vy(i+1),2) << ")" << endl;}
        xysl.delete_vertex(i+1);
        num_del++;
      }
      double new_time = (sl_size - num_del) / (sl_size) * ts_in.getTurnDuration();
      ts_in.setTurnDuration(new_time);
      ts_in.setPositionData(xysl);
      return true;
    }
  }
  return false;
}

TurnSequence  removeLinearBeginning(TurnSequence ts_in, double& xstart, double& ystart, double heading_tolerance){
  // not functional. reserved for future work.

  // remove the beginning of the turn within heading_tolerance
  // [degrees] to encourage a better polynomial fit.  Replace the
  // removed points with a SegList.
  double turn_dur = ts_in.getTurnDuration();
  TurnSequence ts_out;
  // this should not change when removing the front end of the turn,
  // but it is changed witin removeLinearTail.  Will reset back to
  // original value.

  // reverse order of TurnSeq points
  XYSegList xysl_orig = ts_in.getPositionData();
  XYSegList xysl_reverse;
  unsigned int sl_size = xysl_orig.size();
  XYSegList test = ts_in.getPositionData(), test2;
  
  cout << "begin for loop.  size = " << doubleToString(sl_size,0) << endl;

  for(int i = sl_size; i >= 0; --i){
    xysl_reverse.add_vertex(xysl_orig.get_vx(i),xysl_orig.get_vy(i));
  }
  ts_in.setPositionData(xysl_reverse);
  
  cout<<"pre-Begin-remove  (size=" << doubleToString((test).size(),0) << ") spec: "<<  getMatlabVector(test) << endl;
  ts_out =  removeLinearTail(ts_in,5);
  
  test2 = ts_out.getPositionData();
  test2.delete_vertex(0,0); // added during removeLinTails
  ts_out.setPositionData(test2);
  cout<<"post-Begin-remove  (size=" << doubleToString((test2).size(),0) << ") spec: "<<  getMatlabVector(test2) << endl;

  ts_out.setTurnDuration(turn_dur);
  sl_size -= (ts_out.getPositionData()).size();
  // sl_size now equals number of points removed, so find the first point from original data

  xstart = xysl_reverse.get_vx(sl_size);
  ystart = xysl_reverse.get_vy(sl_size);
  cout << "started at (x,y) = " << doubleToString(xstart,2) << "," << doubleToString(ystart,2) << "with " << doubleToString(sl_size,0) << " points removed. " << endl;
  return ts_out;
}
TurnSequence  removeLinearTail(TurnSequence ts_in, double heading_tolerance){
  // removeLinearTail looks at the heading created from a point to the
  // final point and compares this to the final expected heading.
  // This starts from the tail and moves back removing all points up
  // to and not including the point first outside of the angle
  // tolerance.  This does not yet have a catch for outliers.
  //  double heading_tolerance = 5; // degrees

  int min_num = 3; // minimum number of data points to retain in
                   // TurnSequence; must be >= 2

  XYSegList xysl_in = ts_in.getPositionData();
  int sl_size = xysl_in.size();
  
  if(sl_size < min_num){
    //require at least min_num points for model
    return ts_in;
  }
  double alpha1, alpha2;
  int num_pts_deleted = 0;
  
  // pick end point (assumes end point is not an outlier)
  double xend =  xysl_in.get_vx(sl_size-1);
  double yend =  xysl_in.get_vy(sl_size-1);
  double angle,xa,ya,xb,yb, final_tangent;

  alpha1 = relAng(xysl_in.get_vx(sl_size-2),xysl_in.get_vy(sl_size-2),xend,yend);
  alpha2 = relAng(xysl_in.get_vx(sl_size-3),xysl_in.get_vy(sl_size-3),xend,yend);
  final_tangent = (alpha1 + alpha2)/2; // average of two angles [degrees]

  bool point_removed = false;
  // track when points are removed to stop deleting the tail once a
  // non-removable point is found. This is necessary because the track
  // is not necessarily monotonic with respect to angle.
  
  for(int i = sl_size-2; i > min_num; i--){
    // -1 for not out of bounds index and -1 for not checking against self
    xa = xysl_in.get_vx(i-1);
    ya = xysl_in.get_vy(i-1);
    xb = xysl_in.get_vx(i);
    yb = xysl_in.get_vy(i);

    cout << "inside rLT loop with i = " << doubleToString(i,0) << endl;
    if(xa < 0){
      cout<< "xa = " << doubleToString(xa,2) << endl;
      xa = 0;
    }

    angle = relAng(xa,ya,xb,yb);//xend,yend);

    double compare_angle = abs(final_tangent - angle);

    if((compare_angle > heading_tolerance) && (point_removed)){
      break;
    }
    
    if(compare_angle < heading_tolerance){ 
      point_removed = true;
      if(verbose){
        cout << "removing point (" << doubleToString(xa,2) << "," << doubleToString(ya,2) << "), "
             << "comparing point (" << doubleToString(xb,2) << "," << doubleToString(yb,2)
             << ") at compare_angle=" << doubleToString(compare_angle,2)
             << ", and angle =" << doubleToString(angle,2)
             << ", and final_tangent=" << doubleToString(final_tangent,2)
             << endl;
      }
      xysl_in.delete_vertex(i);
      num_pts_deleted++;
    }
  }
  if(num_pts_deleted > 0){
    xysl_in.delete_vertex(xend,yend);
    num_pts_deleted++;
  // adjust the time required to traverse the trajectory by assuming
  // that each data point of the initial data (ts_in) is plotted with
  // a pseudo-constant time interval.  Therefore, the % of points
  // removed should be equal to the % reduction in time to traverse
  // the trajectory.
    double new_time = (sl_size - num_pts_deleted) / (sl_size) * ts_in.getTurnDuration();
    ts_in.setTurnDuration(new_time);
  }

  ts_in.setPositionData(xysl_in);
  return ts_in;
}
XYSegList simplifyLinearRegions(XYSegList xysl_in, double ang_tol){
  unsigned int min_num = 3; // at least min_num segments
  //  unsigned int max_num = 6;

  unsigned int sl_size = xysl_in.size();

  if(sl_size <= min_num){
    return xysl_in;
  }
#if 0
  double alpha1, alpha2, xi, yi, xi1, yi1, xi2, yi2;
  unsigned int num_del, cum_del = 0;

  if(verbose){
    string verbose_string = "\n";
    for(unsigned int i = 0; i < sl_size; i++){
      verbose_string += doubleToString(xysl_in.get_vx(i),4) + "," + doubleToString(xysl_in.get_vy(i),4) + "\n";
    }
    cout << "input to simpLR() = " << verbose_string << endl;
  }
  
  for (unsigned int i = 0; i < sl_size - 2; i++) {
    num_del = 0;
    for(unsigned int j = i+1; j < sl_size -1; j++){
      xi  =  xysl_in.get_vx(i);
      yi  =  xysl_in.get_vy(i);
      xi1 =  xysl_in.get_vx(i+1);
      yi1 =  xysl_in.get_vy(i+1);
      xi2 =  xysl_in.get_vx(j+1);
      yi2 =  xysl_in.get_vy(j+1);

      alpha1 = angleFromThreePoints(xi,yi,xi1,yi1,xi2,yi2);
      alpha2 = angleFromThreePoints(xi2,yi2,xi1,yi1,xi,yi);

      if(verbose){
        cout << "considered angles in simpLR() at ("
             << doubleToString(xi,3) << ","
             << doubleToString(yi,3) << "),("
             << doubleToString(xi1,3) << ","
             << doubleToString(yi1,3) << "),("
             << doubleToString(xi2,3) << ","
             << doubleToString(yi2,3) << "),angs="
             << doubleToString(alpha1,2) << ","
             << doubleToString(alpha2,2) << ","
             << "i=" << doubleToString(i,0)
             << ",j=" << doubleToString(j,0)
             << ",imax=" << doubleToString(sl_size-2,0)
             << ",jmax=" << doubleToString(sl_size-1,0)
             << endl;
      }
      
      if((alpha1 < ang_tol) && (alpha2 < ang_tol)){
        num_del++;
        cum_del++;
      }
      else{
        break;
      }
    }
    for(unsigned int k = 0; k < num_del; k++){
      if(verbose){
        double xdel = xysl_in.get_vx(i+1);
        double ydel = xysl_in.get_vy(i+1);
        cout << "deleted point in simpLR() at ("
             << doubleToString(xdel,3) << ","
             << doubleToString(ydel,3) << ")" << endl;
      }
      xysl_in.delete_vertex(i+1);
    }
    sl_size -= num_del; // must account for one fewer points in seglist
  }
#endif
#if 1  // original follows
  double alpha1, alpha2, final_tangent;
  double angle,xa,ya,xend,yend;

  if(verbose){cout<<"pre-simplify  (size=" << doubleToString((xysl_in).size(),0) << ") spec: "<<  getMatlabVector(xysl_in) << endl;}
  double ymax = xysl_in.get_max_y();

  for (int i = sl_size-2; i > 0; i--) {
    if(verbose){
      cout << "inside simplify for loop for i = " << doubleToString(i,0) << endl;
    }
    xend =  xysl_in.get_vx(i+1);
    yend =  xysl_in.get_vy(i+1);
    if(verbose){cout <<"simplify checking x=" << doubleToString(xend,3) << ", y=" <<  doubleToString(yend,3) << endl;}
    // want the end point of seglist to be last point of previous
    // iteration... first time through is overall endpoint
    alpha1 = relAng(xysl_in.get_vx(i),xysl_in.get_vy(i),xend,yend);
    //alpha2 = relAng(xysl_in.get_vx(i-1),xysl_in.get_vy(i-1),xend,yend);
    alpha2 = relAng(xysl_in.get_vx(i-1),xysl_in.get_vy(i-1),xysl_in.get_vx(i),xysl_in.get_vy(i));

    if(verbose){ cout << "(x,y) = "
                      << doubleToString(xysl_in.get_vx(i),2) << ","
                      << doubleToString(xysl_in.get_vy(i),2) << ";"
                      << doubleToString(xysl_in.get_vx(i-1),2) << ","
                      << doubleToString(xysl_in.get_vy(i-1),2)
                      << ", with alpha1 = " <<doubleToString(alpha1,2)
                      << ", alpha2 = " <<doubleToString(alpha2,2)
                      << endl;
        }
    
    // special case for x <= 0 (found y-intercept)
    if(xend <= 0.0){
      for(int k = 1; k < i+1; k++){
        if(verbose){cout << "k=" << doubleToString(k,0) << ", i+1 = " << doubleToString(i+1,0) << endl;}
        if(verbose){cout << "deleted from y-axis: (x,y) = "
                         << doubleToString(xysl_in.get_vx(1),2) << ","
                         << doubleToString(xysl_in.get_vy(1),2) << endl;}
        xysl_in.delete_vertex(1); // assume origin is first point and xend is last point on y-axis
      }
      break;
    }
    
    if(abs(alpha1-alpha2) >  ang_tol){
      //uninterested in removing segments that show a change of angle
      continue;
    }

    
    final_tangent = (alpha1 + alpha2)/2; // average of two angles                
    for(int j = i-1; i > 0; j--){
      // Note: -1 for not out of bounds index and -1 for not checking
      // against self
      
      // Note: additional -1 for not checking adjacent point (want to
      // remove middle points)
      
      xa = xysl_in.get_vx(j);
      ya = xysl_in.get_vy(j);

      double xj1 = xysl_in.get_vx(j+1);
      double yj1 = xysl_in.get_vy(j+1);
      
      //      angle = relAng(xa,ya,xend,yend);  //  orig; removed 2015.090
      angle = relAng(xa,ya,xj1,yj1);
      double compare_ang = abs(final_tangent - angle);
      if( (compare_ang < ang_tol) &&
          (xj1 > 0) &&        // desire north-facing initial segment
          (yj1 < ymax)     // desire to keep most conservative point for contacts
          ){ 
        if(verbose){cout << "deleted: (x,y) = "
                         << doubleToString(xysl_in.get_vx(j+1),2) << ","
                         << doubleToString(xysl_in.get_vy(j+1),2)
                         << ", with compare_ang = " <<doubleToString(compare_ang,2)
                         << ", final_tangent = " <<doubleToString(final_tangent,2)
                         << ", ang_tol = " <<doubleToString(ang_tol,2)
                         << endl;}
	xysl_in.delete_vertex(j+1);
        //remove the point between you and the end
        i--;
        sl_size--;
      }
      else{
	break;
      }
    }
  }
  #endif
  return xysl_in;
 }

bool turnPolyInterpolator(TurnPoly tp1, TurnPoly tp2, double new_heading, double new_speed, TurnPoly& tp_interp, string interp_type){
  // returns tp_interp using the desired interpolation technique
  // functions for tp1 and tp2

  // Note: defaults to averaging the polynomial coefficients.  To use
  // averaging of the PWL approximations by sampling over a
  // discretized x-value, set tp_interp="pwl-averaging" on call.

  // tuning  ... KW TBD: make configurable from .moos file
  double x_sampling_interval = 0.05;
  // sampling frequency along x-direction of each TurnPoly

  double max_heading_difference = 45;
  // only allow interpolation if known trajectories are within 45
  // degrees

  double max_speed_ratio = 0.5;
  // delta_t / t_max must be less than this value

  // interpolation type
  bool interp_coeffs;
  interp_type = tolower(interp_type);
  if(interp_type == "coefficients"){
    interp_coeffs = true;
  }
  else if (interp_type == "pwl-averaging"){
    interp_coeffs = false;
  }
  else{
    return false;
  }
  

  if(verbose){  cout << "interpolator input: spd=" << doubleToString(new_speed,3) << "; hdg=" << doubleToString(new_heading,0) << endl;}

  // type check
  if(!(tp1.isProper() && tp2.isProper())){
    cout << "unable to interpolate because one or more TurnPolys is improper." << endl;
    return false;
  }

  //TODO: update max_heading_difference dynamically OR refine the
  //number based on models.
  
  // require all angles to be [0,180] for interpolation

  double h1 = abs(angle180(tp1.getDeltaHeading()));
  double h2 = abs(angle180(tp2.getDeltaHeading()));
  
  double spd1 = tp1.getSpeed();
  double spd2 = tp2.getSpeed();
  
  double t_cutoff1 = tp1.getTCutoff();
  double t_cutoff2 = tp2.getTCutoff();
  
  // sanity check: only interpolate if new_heading is between h1 and
  // h2 (< 180 degrees)
  if((new_heading > h1) && (new_heading > h2)){
    cout << "ERROR: Requested heading must be between heading 1 and heading 2. " << endl;
    return false;
  }

  // sanity check: only interpolate if heading spread is < tolerance
  if(abs(h1-h2) > max_heading_difference){
    cout << "ERROR: Heading difference too excessive to interpolate. " << endl;
    return false;
  }

  // sanity check within tolerance
  if(abs(spd2-spd1) / max(spd1,spd2) > max_speed_ratio){
    cout << "ERROR: Speed difference too excessive to interpolate. " << endl;
    return false;
  }

  // alpha is the weight for the weighted average between tp1 and tp2
  double alpha = calcInterpolationWeight(tp1,tp2,new_heading,new_speed, max_heading_difference);

  if(!interp_coeffs){
    // sample y values of the two known TPs at a regular x-sampling
    // interval up to and including the min_xend determined above
    XYSegList tp1_samples = tp1.getXYSLForConstantXSamplingInterval(x_sampling_interval, tp1.getXEnd()); 
    XYSegList tp2_samples = tp2.getXYSLForConstantXSamplingInterval(x_sampling_interval, tp2.getXEnd()); 
    
    // ROTATION: must rotate these samples to be North-facing before accurate comparison is possible
    TurnSequence ts_1, ts_2;
    ts_1.setPositionData(tp1_samples);
    ts_2.setPositionData(tp2_samples);
    
    ts_1.customProject(0,0,-tp1.getRotationAngle(),0);
    ts_2.customProject(0,0,-tp2.getRotationAngle(),0);
    
    tp1_samples = ts_1.getPositionData();
    tp2_samples = ts_2.getPositionData();
    // XYSegList is now rotated back to North.
    
    vector<double> v_xi, v_y1, v_y2;
    double precision = 0.1;
    
    for(double xi = precision; xi < tp1_samples.get_vx(tp1_samples.size()-1); xi = xi + precision){
    
      v_xi.push_back(xi);
      // function rounds up in giveY, and don't want a point on xi=0

      v_y1.push_back(giveYforFirstXInstance(tp1_samples, xi, precision));
      v_y2.push_back(giveYforFirstXInstance(tp2_samples, xi, precision));
    
      if(verbose){
        cout << "check: xi=" << doubleToString(xi,3) << ", y1=" << doubleToString(giveYforFirstXInstance(tp1_samples, xi, precision),3) << ", y2=" << doubleToString(giveYforFirstXInstance(tp2_samples, xi, precision),3) << endl;
      }
    }     
    
    double y_hat;
    vector<double> v_y_hat;
    int not_found_num = -789;

    XYSegList xy_interp;
    if(verbose){cout << "using alpha = " << doubleToString(alpha,3) << endl;}
    xy_interp.insert_vertex(0,0);
    
    for(unsigned int i = 0; i < v_xi.size(); i++){
      double y1 = v_y1[i];
      double y2 = v_y2[i];
      
      if( (y1 != not_found_num) && (y2 != not_found_num)){
        // a point exists for both functions in this bin[i]
        y_hat = y1 + alpha * (y2 - y1);
        v_y_hat.push_back(y_hat);
        xy_interp.add_vertex(v_xi[i],y_hat);
      }
      if(verbose){
        cout  <<  doubleToString(v_xi[i],3) <<", " << doubleToString(y1,3)<<  ", " << doubleToString(y2,3) << "; y_hat = " << doubleToString(y_hat,3) << endl;
      }
    }
    
    TurnSequence ts_interp;
    ts_interp.setPositionData(xy_interp);
    
    if(verbose){for(unsigned int i = 0; i < v_y1.size(); i++){ cout << "v_xi,v_yi = " << doubleToString(xy_interp.get_vx(i),3) << ", " << doubleToString(xy_interp.get_vy(i),3) << endl;  }
    }
    
    XYSegList out =  ts_interp.getPositionData();
    ts_interp.setSpeed(new_speed);
    ts_interp.setTurnDuration(t_cutoff1 + alpha * (t_cutoff2 - t_cutoff1));
    ts_interp.setDeltaHeading(new_heading);
    if(verbose){cout << "new delta heading = " << doubleToString(new_heading,1) << endl;}
    
    if(tp_interp.generateTurnPolyFromTurnSequence(ts_interp)){
      return true;
    }
    else
      return false;

  }
  if(interp_coeffs){
    cout << "interpolating with coefficients" << endl;
    double tp1_ang, tp2_ang;
    tp1_ang = tp1.getRotationAngle();
    tp2_ang = tp2.getRotationAngle();
    if(tp1_ang != tp2_ang){
      cout << "failed interpolation due to different rotation angles" << endl;
      return false;
    }
    
    vector<double> tp1_coeff, tp2_coeff, tpI_coeff;
    tp1_coeff = tp1.getPolyCoefficients();
    tp2_coeff = tp2.getPolyCoefficients();
    double c1, c2, ci, tc, xin, yin;
    for(unsigned int i = 0; i < tp1_coeff.size(); i++){
      c1 = tp1_coeff[i];
      c2 = tp2_coeff[i];
      ci = c1 + alpha * (c2 - c1);
      tpI_coeff.push_back(ci);
    }
    tc = tp1.getTCutoff() + alpha * (tp2.getTCutoff() - tp1.getTCutoff());
    
    tp_interp.setPolyCoefficients(tpI_coeff);

    xin = tp1.getXEnd() + alpha * (tp2.getXEnd() - tp1.getXEnd());
    yin = tp_interp.evalPolyAtX(xin);
    // yin does not necessarily vary linearly with xin, but we can set
    // xin somewhat arbitrarily as long as the (xin,yin) point falls
    // on the poly

    
    tp_interp.setTCutoff(tc);
    tp_interp.setRotationAngle(tp1_ang);
    tp_interp.setXYEnd(xin, yin);
    tp_interp.setDeltaHeading(new_heading);
    tp_interp.setSpeed(new_speed);
    tp_interp.setFinalRay();


    cout << "tp1 xend = " << doubleToString(tp1.getXEnd(),1) << endl;
    
    cout << tp_interp.getString() << endl;
    
    if(tp_interp.isProper()){
      if (verbose){ cout << "interpolated Poly String:  \n\n\n" << tp_interp.getString() << endl;}
      return true;
    }
    else{
      if(verbose){
        cout << "Improper TurnPoly created in interpolation algorithm..."
             << "Unable to generate TurnPoly" << endl;
        cout << "Failed to build interpolated poly for desired_speed = " 
             << doubleToString(new_speed,3) << ", desired_heading = " 
             << doubleToString(new_heading,0) << endl;
        tp_interp.getString();
      }
      return false;
    }
  }
  return false;
}

double giveYforFirstXInstance(XYSegList xysl,double xi, double precision){
  int not_found_num = -789;
  for(unsigned int i = 0; i < xysl.size(); i++){
    double x_sample= xysl.get_vx(i);
    if(x_sample >= xi){
      if(x_sample < xi + precision){
	return xysl.get_vy(i);
      }
      else{
	return not_found_num;
      }
    }
  }
  return not_found_num;
}


double calcInterpolationWeight(TurnPoly tp1, TurnPoly tp2, double new_heading, double new_speed, double max_heading_difference){
  // determine a linear weighting value from the difference in speeds
  // and headings of the 2 TPs and the new values alpha is the linear
  // interpolation weight.  alpha=1 is y2 dominated, alpha=0 is y1
  // dominated.
  double alpha, alpha_h, alpha_t, weight_t, weight_h, gamma;
  double speed_bare_steerage = 0;
  double h1 = abs(angle180(tp1.getDeltaHeading()));
  double h2 = abs(angle180(tp2.getDeltaHeading()));
  
  double spd1 = tp1.getSpeed();
  double spd2 = tp2.getSpeed();
 

  // note: if same headings and speeds provided, then consider this as
  // a request to average two TurnPolys of the same heading/speed.
  if((h1 == h2) && (spd1==spd2)){
    alpha = 0.5;
  }
  else{
    // Note: assume linearly scaled weight for speed and heading to
    // relate speed and heading, a unique weighting scheme is proposed
    // (2014.341) consider a heading weighting and speed weighting
    // indepedent of each other initially.

    // Note: for two known TurnPolys (shown by X below), must given a
    // consideration to both speed and heading influence on the degree
    // of change of any interpolated TurnPoly within the domain
    // (speed1,speed2) and domain(heading1,heading2) denoted by I.

    // Note: there must be some weight alpha (0 to 1) such that a
    // combination of the difference in interpolated speed and heading
    // relative to the two known TurnPolys can be achieved while
    // considering the relative change of speed and heading.  For
    // example, TurnPolys with speeds 1.0 and 2.5 with headings 030
    // and 035 should give greater consideration to speed than
    // heading, not just a nominal 50/50 split.

    // Note: by linearly combining the effects of speed and heading
    // while accounting for the relative influence of each with
    // respect to their allowed variations (max allowed heading change
    // for an interpolation and maximum speed compared to bare
    // steerageway), a realistic weight can be inserted on a (0,1)
    // scheme.

    /*
    spd    
    |                         X
    |
    |
    |
    |      I
    |
    |
    |X
    ---------------------------  (heading)  
    */

    // note:  t denotes speed, not time
    if(spd1==spd2){
      alpha_t = 0;
    }
    else{
      alpha_t = abs(new_speed - spd1)/abs(spd2-spd1);
      // alpha_t on [0,1]
    }
    if(angleDiff(h2,h1) == 0){
      alpha_h = 0;
    }
    else{
      alpha_h =abs(angleDiff(new_heading,h1) / angleDiff(h2, h1));
      // alpha_h on [0,1]
    }
    weight_t = abs( (spd2-spd1) / (max(spd1,spd2) - speed_bare_steerage));
    weight_h = abs((h2-h1) / (max_heading_difference));
    gamma = 1 / (weight_h + weight_t);
    
    alpha = gamma * weight_h * alpha_h + gamma * weight_t * alpha_t;
    // alpha on [0,1] .. proof in notebook.2014.341
    
    if(verbose){
      cout << "Interpolation for SPD: " << doubleToString(new_speed,3) <<"   ;HDG: " << doubleToString(new_heading,0) << "   ;alpha:  " << doubleToString(alpha,3) << endl;
    }
  }
  return alpha;
}

string getMatlabVector(XYSegList xysl){
  string out = "[";
  for(unsigned int i = 0; i < xysl.size(); i++){
    out += doubleToString(xysl.get_vx(i),4);
    out += ",";
    out += doubleToString(xysl.get_vy(i),4);
    if(i +1 < xysl.size()){
      out += ";\n";
    }
  }
  out += "];";
  return out;
}
