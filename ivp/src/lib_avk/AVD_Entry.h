/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVD_Entry.h                                          */
/*    DATE: Nov 9th, 2014                                        */
/*****************************************************************/

#ifndef AVD_ENTRY_HEADER
#define AVD_ENTRY_HEADER

#include <string>
#include "XYSegList.h"
#include "TurnSequence.h"
#include "TurnPoly.h"

class AVD_Entry
{
 public:
  AVD_Entry();
  ~AVD_Entry() {};
  
 public: // Setters
  bool   setSpd(double spd);
  bool   setDeltaHdg(double dh);
  bool   setTurnSequ(TurnSequence tseq); 
  bool   setTurnPoly(TurnPoly poly, std::string source=""); 
  bool   setTurnPWL(XYSegList pwl);

  bool   setInterpStatus(std::string);
  void   setInterpDirect(bool v=true) {m_interp_direct=v;};
  void   setInterpSecond(bool v=true) {m_interp_second=v;};

 public: // Getters
  double getSpd() const     {return(m_spd);};
  double getDeltaHdgGoal() const {return(m_dh);};
  double getDeltaHdgSeen() const {return(m_turn_sequ.getActualDelta());};

  TurnSequence getTurnSequ() const     {return(m_turn_sequ);};
  TurnPoly     getTurnPoly() const     {return(m_turn_poly);};
  XYSegList    getTurnSeglRaw() const  {return(m_turn_segl_raw);};
  XYSegList    getTurnSeglLSQ() const  {return(m_turn_segl_lsq);};
  XYSegList    getTurnSeglPWL() const  {return(m_turn_segl_pwl);};
  XYSegList    getTurnSeglRay() const  {return(m_turn_segl_ray);}
  std::string  getInterpStatus() const {return(m_interp_status);};

 public: // Modifiers
  void  incEndIX();
  void  decEndIX();
  void  resetEndIX();

  void  incLookIX();
  void  decLookIX();
  void  resetLookIX();
  
  bool  fitPolynomial();
  bool  buildPieceWise();

 public: // Queries
  bool   turnSequSet() const   {return(m_turn_sequ_set);};
  bool   turnPolySet() const   {return(m_turn_poly_set);};
  bool   turnSeglSet() const   {return(m_turn_segl_raw_set);};
  bool   turnPWLSet() const    {return(m_turn_segl_pwl_set);};

  bool   isInterpolated() const {return(m_interp_direct || m_interp_second);};
  bool   isInterpDirect() const {return(m_interp_direct);};
  bool   isInterpSecond() const {return(m_interp_second);};

  bool   pWiseExceedsGoal() const;

  double getTurnDur() const;
  double getLookX() const;
  double getLookY() const;

  unsigned int getEndIX() const  {return(m_end_ix);};
  unsigned int getLookIX() const {return(m_look_ix);};

  std::string  getSummary(unsigned int pad=0) const;
  
 protected: // Config variables
  double       m_spd;
  double       m_dh;
  TurnSequence m_turn_sequ;

 protected: // State variables
  bool         m_turn_sequ_set;
  bool         m_turn_poly_set;
  bool         m_turn_segl_raw_set;
  bool         m_turn_segl_lsq_set;
  bool         m_turn_segl_pwl_set;
  bool         m_interp_direct;
  bool         m_interp_second;

  std::string  m_interp_status;  // none, pair, diag, fail
  XYSegList    m_turn_segl_raw;  // represents raw data
  XYSegList    m_turn_segl_lsq;  // represents lsq fit
  XYSegList    m_turn_segl_pwl;  // represents pcwise approx of lsq
  XYSegList    m_turn_segl_ray;  // represents final ray for pwl
  TurnPoly     m_turn_poly;

  unsigned int m_end_ix;
  unsigned int m_look_ix;
};

#endif 
