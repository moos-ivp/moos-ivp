/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: LegRun.h                                        */
/*    DATE: May 26th, 2023                                  */
/************************************************************/

#ifndef LEG_RUN_OBJ_HEADER
#define LEG_RUN_OBJ_HEADER

#include <string>
#include "XYPoint.h"
#include "XYSegList.h"

class LegRun
{
public:
   LegRun();
  ~LegRun() {};

public: // Leg Setters
  bool setParams(std::string);
  bool setParam(std::string, std::string);
  bool setPoint1(std::string);
  bool setPoint2(std::string);

public: // Indirect Leg Setters/modifiers
  bool setFullLeg(std::string);
  bool setLegByCtr(std::string);

  bool modCenterPt(std::string);
  bool modCenterPt(XYPoint);
  bool setLegLen(double);
  bool modLegLen(double);
  bool setLegAng(double);
  bool modLegAng(double);

protected:
  bool setLegByCtr1(std::string);
  bool setLegByCtr2(std::string);
  bool setLegAux(double x, double y, double ang, double len);
  
public: // Turn Setters/Modifiers
  bool setTurn1Rad(double);
  bool setTurn2Rad(double);
  bool setTurnRad(double);
  bool setTurnRadMin(double);
  bool modTurn1Rad(double);
  bool modTurn2Rad(double);
  bool modTurnRad(double);

  bool setTurn1Ext(double);
  bool setTurn2Ext(double);
  bool setTurnExt(double);
  bool modTurn1Ext(double);
  bool modTurn2Ext(double);
  bool modTurnExt(double);

  bool setTurn1Bias(double);
  bool setTurn2Bias(double);
  bool setTurnBias(double);
  bool modTurn1Bias(double);
  bool modTurn2Bias(double);
  bool modTurnBias(double);

  bool setTurn1Dir(std::string);
  bool setTurn2Dir(std::string);
  bool setTurnDir(std::string);

  bool setTurnPtGap(double);
  bool modTurnPtGap(double);

  bool setLaneIX(int);
  void setLegModified(bool v=true) {m_leg_modified=v;}
  
public: // Getters
  double  getTurn1Ext() const {return(m_turn1_ext);}
  double  getTurn2Ext() const {return(m_turn2_ext);}
  double  getTurn1Rad() const {return(m_turn1_rad);}
  double  getTurn2Rad() const {return(m_turn2_rad);}
  double  getTurn1Bias() const {return(m_turn1_bias);}
  double  getTurn2Bias() const {return(m_turn2_bias);}
  int     getLaneIX() const   {return(m_lane_ix);}
  bool    isSetLaneIX() const {return(m_lane_ix_set);}
  
  XYPoint getPoint1() const {return(m_p1);}
  XYPoint getPoint2() const {return(m_p2);}

  std::string getTurn1Dir() const {return(m_turn1_dir);}
  std::string getTurn2Dir() const {return(m_turn2_dir);}
  std::string getLegID() const    {return(m_leg_id);}
  
  std::string getSpec(char c=',') const;
  
public: // Analyzers
  bool valid() const;

  double  getLegLen() const;
  double  getLegAng() const;
  XYPoint getCenterPt() const;
  XYPoint getDistPt(double dist); 
  
  double  getTurn1Len();
  double  getTurn2Len();
  double  getTotalLen();
  
  bool    legModified()  {return(m_leg_modified);}
  bool    turnModified() {return(m_turn_modified);}

  
public: // Turn Points
  XYSegList initTurnPoints1(bool adjust=false, double sx=0, double sy=0);
  XYSegList initTurnPoints2(bool adjust=false, double sx=0, double sy=0);

protected:
  void clearTurnSegls();
  
private: // Config vars
  double  m_turn_pt_gap;
  XYPoint m_p1;
  XYPoint m_p2;
  double  m_turn1_rad;
  double  m_turn2_rad;
  double  m_turn_rad_min;
  double  m_turn1_ext;
  double  m_turn2_ext;
  double  m_turn1_bias;
  double  m_turn2_bias;
  int     m_lane_ix;
  
  std::string m_turn1_dir;
  std::string m_turn2_dir;

  std::string m_leg_id;
  
private: // State var

  bool m_leg_modified;
  bool m_turn_modified;
  
  bool m_lane_ix_set;

private: // State var (turn info cache)

  XYSegList m_turn1_segl;
  XYSegList m_turn2_segl;
  double    m_turn1_len;
  double    m_turn2_len;
};

LegRun stringToLegRun(std::string);

#endif 
