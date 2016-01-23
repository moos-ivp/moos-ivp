/************************************************************/
/*    NAME: Kyle Woerner                                    */
/*    ORGN: MIT                                             */
/*    FILE: TurnPoly.h                                      */
/*    DATE: 01 Sep 2014                                     */
/************************************************************/

#ifndef TurnPoly_HEADER
#define TurnPoly_HEADER


#include <string>
#include <vector>
#include "XYSegList.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "Seglr.h"
#include "TurnSequence.h"

class TurnPoly
{
 public:
  TurnPoly();
  ~TurnPoly();

 public:  //askers
  bool isPoly() const;
  bool isCircular() const;
  bool isProper() const;

 public:  // creaters
  //  bool populateFromTPSpec(std::string);
  bool generateFromSpec(std::string);
  bool generateTurnPolyFromTurnSequence(TurnSequence, double rotation_angle=22.5);

 public:  //getters about the TurnPoly independent of coordinate system
  double getPolyDegree() const {return m_poly_coefficients.size();};
  std::vector<double> getPolyCoefficients()  const ;

  double getSpeed()  const {return m_speed;};
  double getDeltaHeading()  const {return m_delta_hdg;};
  double getRotationAngle()  const {return m_rotation_angle;};
  
  double getXPrecision()  const {return m_x_precision;};
  double getPtPrecision()  const {return m_point_precision;};
  double getTCutoff()  const {return m_t_cutoff;}

 public:  // getters in non-north facing coordinates
  //  double getXCutoff()  const {return m_x_end;}
  double getXEnd()  const {return m_x_end;}  // x-coord of final non-north point
  double getYEnd()  const {return m_y_end;}  // y-coord of final non-north point

  // derived output in non-north facing coordinates
  XYSegList getXYSLForConstantXSamplingInterval(double, double) const;
  XYSegList getXYSLForConstantXSamplingInterval(double) const;
  double evalPolyAtX(double)  const ;
  
 public:  // getters in north-facing coordinates
  double getTangentAngle()  const {return m_final_tangent;};
  bool getNFacingXYDomain(double&, double&);

  // derived output in north-facing coordinates
  TurnSequence getNorthFacingTurnSequence();
  bool getSimplifiedNorthFacingTurn(Seglr&);
  XYSegList getNFaceDisc(double) const;
  XYSegList getFinalRay()  const {return m_final_ray;};
  

 private: // time not currently used (moved to TurnSeq)
  unsigned int getNumUnderlyingPoints() const{return m_num_underlying_pts;};
  std::vector<double> getRelativeTimeStamps() const{return m_relative_timestamps;};
  std::vector<double> getCumulativeTimeStamps() const{return m_cumulative_timestamps;};
  
 public:  // serialization and getString
  std::string getSpec() const;
  std::string getVerboseSpec() const;
  std::string getString() const;
  bool getRelativeTimestamps(std::string&) const;
  bool getCumulativeTimestamps(std::string&) const;


 public: // setters
  bool setPolyCoefficients(double,double,double,double,double);
  bool setPolyCoefficients(std::vector<double>);
  bool storeCoefficientsFromSpec(std::string);

  bool setSpeed(double);
  void setDeltaHeading(double);
  bool setTCutoff(double);
  bool setRotationAngle(double);
  
  bool setPtPrecision(double);
  bool setXPrecision(double);
  bool setXEnd(double); // non-N facing x-max for poly fitting
  bool setYEnd(double); // non-N facing y-max for poly fitting
  bool setXStart(double); // North facing x-start for poly fitting
  bool setYStart(double); // North facing y-start for poly fitting
  
  bool setXYEnd(double, double);
  // non-N facing xy-max for poly fitting

  
  bool setFinalRayMode(std::string);
  bool setFinalRay(const XYSegList);
  bool setFinalRay(const XYSegList, double, double& y_b);
  bool setFinalRay(double radius=2);
  bool setFinalTangent(XYSegList);

  bool setProjectPWLMode(std::string);  // "delta_angle" (default = 10 deg) or "num_segments"
  bool setProjectPWLAngleTolerance(double);
  bool setMaxPWLSegments(unsigned int);
  bool setLinearTailAngleTolerance(double);
  
 protected:
  //setters
  bool setNFacingXYDomain();
  bool setYEnd();
  bool setNumUnderlyingPoints(unsigned int);
  
 protected:
  XYSegList projectPWLinear();
  XYSegList projectPWLinearWithDynamicResolution();
  bool updateRelativeTimes(XYSegList);
  
 private: // State variables
  //variables common to all poly types:
  std::string m_poly_type; // "poly" or "arc"
  double m_t_cutoff;  // time in seconds from
  std::vector<double> m_relative_timestamps;
  // times of the beginning of each PWL segment with respect to the
  // start of the turn being 0.0 sec
  unsigned int m_num_underlying_pts;    // used to compute rel times
  
  std::vector<double> m_cumulative_timestamps;
  
  double m_speed;     // speed associated with this poly
  double m_delta_hdg; // course change associated with this poly
  
  double m_x_end;  // x and y positions of the final point from raw data
  double m_y_end;  // better to match final point for ray than "close"
		   // poly at end

  // x and y positions of north-facing first point after the initial
  // linear portion is removed; improves polynomial fit
  double m_x_start;
  double m_y_start;
  
  // domain for x,y that poly is valid once rotated to N-facing
  double m_x_domain;
  double m_y_domain;

  double m_rotation_angle;  // rotation angle from NORTH-facing/CW+
			    // before fitting to polynomial [degrees]
  double m_final_tangent;   // final angle of raw data [degrees]
  XYSegList m_final_ray;    // two points that define the final
			    // trajectory direction of a turn.  This
			    // value corresponds to the final tangent
			    // line of NAV_X,NAV_Y data.  The angle
			    // therefore does not necessarily match
			    // the final desired heading due to
			    // non-linear effects of the turn.

  //poly specific
  std::vector<double> m_poly_coefficients;
  
  //arc specific
  double m_radius;
  double m_x_center;
  double m_y_center;
  double m_theta_start;
  double m_theta_end;
    
 private: // Configuration variables
  bool m_final_ray_is_delta_hdg;
  double m_x_precision;
  double m_point_precision;  // inherited from TurnSequence
  double m_pwl_ang_tol;

  bool m_simplify_by_angle; // true - remove by angle, false - remove by num segments
  unsigned int m_max_pwl_segments;
  double m_tail_tolerance; // angle [degrees] that should be chopped from end of turn (improves fitting)
};

#endif 
