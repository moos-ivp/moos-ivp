/************************************************************/
/*    NAME: Jacob Gerlach                                   */
/*    ORGN: MIT                                             */
/*    FILE: TurnSequence.h                                  */
/*    DATE: 6/13/14                                         */
/************************************************************/

#ifndef TURN_SEQUENCE_HEADER
#define TURN_SEQUENCE_HEADER

#include "XYSegList.h"
#include <string>

class TurnSequence
{
 public:
  TurnSequence();
  ~TurnSequence();
  
 public: // Setters
  void setSpeed(double);
  void setDeltaHeading(double);
  void setInitialHeading(double);
  void setFinalHeading(double);
  void setPtPrecision(int);
  void setTurnDuration(double);
  void setTurnDirection(int);
  void setPositionData(XYSegList);
  void setActualDelta(double);

 public: // Getters
  double    getSpeed()          const {return m_InitialSpeed;};
  double    getDeltaHeading()   const {return m_DeltaHeading;};
  int       getPtPrecision()    const {return m_PtPrecision;};
  double    getTurnDuration()   const {return m_TurnDuration;};
  int       getTurnDirection()  const {return m_TurnDirection;};
  XYSegList getPositionData()   const {return m_PositionData;};
  double    getActualDelta()    const {return m_ActualDelta;};

  double    getInitialHeading() const {return m_InitialHeading;};
  double    getFinalHeading()   const {return m_FinalHeading;};
  
  unsigned int size()           const {return(m_PositionData.size());};

  // Input
  bool storeTurnFromSpec(std::string);
  void storePtsFromSpec(std::string);
  void addPoint(double, double);
  void truncToHeadDiff(double);

  // Output
  std::string getSpec() const;
  std::string getSegListSpec() const;

  // Manipulation
  void normalizeTurn(bool keep_dir=false);
  void reflectTurn();
  void setDirFromSignedHead();
  void customProject(double, double, double, int);
  void removeDups();

 protected:
  XYSegList projectTurn(double, double, double, int);

  //Deprecated
  std::vector<double> calcPointwiseHeading();
  //-------------

 protected: 
  XYSegList m_PositionData;
  double    m_DeltaHeading;
  double    m_InitialSpeed;
  double    m_InitialHeading;
  double    m_FinalHeading;
  double    m_TurnDuration;
  int       m_TurnDirection;
  double    m_ActualDelta;

 private: // Configuration variables
  int m_PtPrecision;

};

#endif 
