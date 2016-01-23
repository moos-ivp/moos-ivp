/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVD_Table.h                                          */
/*    DATE: Nov 8th, 2014                                        */
/*****************************************************************/

#ifndef AVD_TABLE_HEADER
#define AVD_TABLE_HEADER

#include <vector>
#include "AVD_Entry.h"
#include "TurnSequence.h"
#include "XYSegList.h"

class AVD_Table 
{
 public:
  AVD_Table();
  ~AVD_Table() {};
  
 public: // Setters
  bool init(double vmin, double vmax, double vgap,
            double hmin, double hmax, double hgap);  
  bool addTurnSequ(TurnSequence);
  bool addTurnSequ(TurnSequence, unsigned int& spdix, unsigned int& hdgix);
  void setVerbose(bool v) {m_verbose=v;};
 
 public: // Getters
  unsigned int getRows() const {return(m_rows);};
  unsigned int getCols() const {return(m_cols);};
  AVD_Entry getEntry(unsigned int spd_ix, unsigned int hdg_ix) const;
  
  std::string getSpec() const;
  

 public: // Analyzers
  bool   turnSequSet(unsigned int spd_ix, unsigned int hdg_ix) const;
  bool   turnPolySet(unsigned int spd_ix, unsigned int hdg_ix) const;
  bool   isInterpolated(unsigned int spd_ix, unsigned int hdg_ix) const;
  double getTurnDur(unsigned int spd_ix, unsigned int hdg_ix) const;
  double getDHGoal(unsigned int spd_ix, unsigned int hdg_ix) const;
  double getDHSeen(unsigned int spd_ix, unsigned int hdg_ix) const;
  double getLookX(unsigned int spd_ix, unsigned int hdg_ix) const;
  double getLookY(unsigned int spd_ix, unsigned int hdg_ix) const;
  double getTurnSpd(unsigned int spd_ix) const;

  std::string  getCoeffs(unsigned int spd_ix, unsigned int hdg_ix) const;
  XYSegList    getSegListRaw(unsigned int spd_ix, unsigned int hdg_ix) const;
  XYSegList    getSegListLSQ(unsigned int spd_ix, unsigned int hdg_ix) const;
  XYSegList    getSegListPWL(unsigned int spd_ix, unsigned int hdg_ix) const;
  XYSegList    getSegListRay(unsigned int spd_ix, unsigned int hdg_ix) const;
  unsigned int getEndIX(unsigned int spd_ix, unsigned int hdg_ix) const;
  unsigned int getLookIX(unsigned int spd_ix, unsigned int hdg_ix) const;
  
  double getTurnSpdFromIX(unsigned int) const;
  double getDeltHdgFromIX(unsigned int) const;  

  unsigned int getSpdIX(double pct) const;
  unsigned int getHdgIX(double pct) const;

  double getGlobalMaxRawX() const;
  double getGlobalMaxRawY() const;

 public: // Modifiers
  void incEndIX(unsigned int spd_ix, unsigned int hdg_ix);
  void decEndIX(unsigned int spd_ix, unsigned int hdg_ix);
  void incLookIX(unsigned int spd_ix, unsigned int hdg_ix);
  void decLookIX(unsigned int spd_ix, unsigned int hdg_ix);
  bool interpolateAll();
  bool pcwiseBuildAll();

  void print(bool terse=false) const;

  bool cellInterpolate(unsigned int spd_ix, unsigned int hdg_ix); 
  bool cellPWLBuild(unsigned int spd_ix, unsigned int hdg_ix); 

  bool findInterpEntries(unsigned int spd_ix, unsigned int hdg_ix, 
			 AVD_Entry& entry_a, AVD_Entry& entry_b, double& pct);

 protected:
  bool getSpdIX(double, unsigned int&) const;
  bool getHdgIX(double, unsigned int&) const;
  
  bool retmsg(std::string) const;
  
 protected: // Config variables
  double m_v_min;
  double m_v_max;
  double m_v_gap;
  double m_h_min;
  double m_h_max; 
  double m_h_gap;
  bool   m_verbose;
  
 protected: // State variables
  
  std::vector<std::vector<AVD_Entry> > m_avd_table;

  unsigned int m_rows;
  unsigned int m_cols;  
};

#endif 
