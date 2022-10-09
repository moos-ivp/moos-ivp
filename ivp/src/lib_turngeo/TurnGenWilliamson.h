/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TurnGenWilliamson.h                                  */
/*    DATE: July 29th, 2020                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef TURN_GEN_WILLIAMSON_HEADER
#define TURN_GEN_WILLIAMSON_HEADER

#include <vector>
#include <string>
#include "TurnGenerator.h"

class TurnGenWilliamson : public TurnGenerator
{
 public:
  TurnGenWilliamson();
  ~TurnGenWilliamson() {};

 public: // virtual functiona overloaded 
  void generate();
  bool setTurnRadius(double radius);
  void setEndPos(double endx, double endy);
  void setEndHeading(double endh);

 public: 
  void   setLaneGap(double lane_gap);
  void   setDesiredExtent(double extent);
  void   setBias(double bias);
  void   setBiasPct(double bias_pct);
  double getAngW() const {return(m_ang_w);}
  double getDistB() const {return(m_dist_b);}
  double getDesiredExtent() const {return(m_desired_extent);}
  double getNaturalExtent() const {return(m_natural_extent);}
  double getDistX() const {return(m_dist_b + m_natural_extent + m_desired_extent);}

  XYPoint getPointC0() const {return(m_c0);}
  XYPoint getPointC1() const {return(m_c1);}
  XYPoint getPointC2() const {return(m_c2);}
  XYPoint getPointC3() const {return(m_c3);}

  double getTheta1() const {return(m_theta1);}
  double getTheta2() const {return(m_theta2);}
  double getBias() const {return(m_bias);}
  double getMaxBias() const {return(m_max_bias);}
  
 private: 
  void generateLaneSwitch();
  void generateLaneSwitch1();
  void generateLaneSwitch2();
  void generateNormal();

 private: // config vars
  double m_lane_gap;
  double m_desired_extent;
  double m_natural_extent;

  double m_bias;

 private: // state vars
  double m_ang_w;
  double m_dist_b;

  double m_max_bias;
  
  XYPoint m_c0;
  XYPoint m_c1;
  XYPoint m_c2;
  XYPoint m_c3;

  double m_theta1;
  double m_theta2;
};

#endif 
