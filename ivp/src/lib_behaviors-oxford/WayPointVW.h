#include "IvPBehavior.h"
#include "AOF.h"
#include "SimpleBehaviour.h"

class CWayPointVW2: public CSimpleBehavior
{
 public:
  CWayPointVW2(IvPDomain D) : CSimpleBehavior(D)
    {
      SetSubDomain("v,w");
      SetDimension(0,"v");
      SetDimension(1,"w");
      
      setParam("unifbox","v=3,w=3");
      setParam("gridbox","v=6,w=6");
      
    }
  
  virtual bool OnPreEvaluate()
    {
      //read in what you'll need to answer sampling questions here
      return true;
    }
  
  virtual double EvaluateThis(double * pActions,int nActions)
    {
      return 0;
    }
};


class CWayPointVW : public IvPBehavior
{
  typedef IvPBehavior BASE;
 public:
  
  class MyAOF: AOF
    {
    public:
      MyAOF(IvPDomain);
      double evalBox(const IvPBox* pBox) const;
      bool SetParams(double dfX,double dfY,double dfTheta,double dfGoalX,double dfGoalY);
      
      int m_nVidx;
      int m_nWidx;
      double m_dfX;
      double m_dfY;
      double m_dfTheta;
      double m_dfGoalX;
      double m_dfGoalY;
    };
  
  //hatch and dispatch
  CWayPointVW(IvPDomain);
  
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);
  
 protected:
  std::string m_sPoseInput;
  bool GetDynamic(const std::string & sName,std::string & sVal);

};
