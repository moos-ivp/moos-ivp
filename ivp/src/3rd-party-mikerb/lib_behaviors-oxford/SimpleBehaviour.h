#ifndef CSimpleBehaviorH
#define CSimpleBehaviorH

#include "BuildUtils.h"
#include "OF_Reflector.h"
#include <math.h>


class CSimpleBehavior : public IvPBehavior
{

 public:
  class MyAOF : public AOF
    {
    public:
      void * m_pUserParam;
      double (*m_pfnUserCallBack)( double*,int,void*);		
      
      bool SetCallBack(double (*pfn)(double*,int,void*),void * pUserParam)
	{
	  m_pUserParam = pUserParam;
	  m_pfnUserCallBack  = pfn;
	  return true;
	}
      
      
      double* m_dfUserArray;
      int * m_nIndexArray;
      int m_nArraySize;
      
      MyAOF(){};
      MyAOF(IvPDomain D):AOF(D)
	{
	  m_nArraySize = D.size();
	  m_dfUserArray= new double[m_nArraySize];
	  m_nIndexArray = new int[m_nArraySize];
	  m_pUserParam = NULL;
	  m_pfnUserCallBack = NULL;
	  
	}
      
      bool SetDimension(int n,const std::string & sName)
	{
	  if(n>=m_nArraySize)
	    return false;
	  
	  int k =  m_domain.getIndex(sName);
	  if(k<0)
	    return false;
	  
	  m_nIndexArray[k] =n;
	  
	  return true;
	  
	}
      
      double evalBox(const IvPBox* pBox) const
	{
	  ///stuff
	  if(m_pfnUserCallBack!=NULL) {
	    //do index->action space look up
	    for(int i = 0;i<m_domain.size();i++) {
	      double dfT;
	      if(m_domain.getVal(i,pBox->pt(i,0),dfT)) {		
		int kUser = m_nIndexArray[i];
		m_dfUserArray[kUser] = dfT;
	      }
	      else {
		return 0;
	      }
	    }
	    
	    return m_pfnUserCallBack(m_dfUserArray,m_nArraySize,m_pUserParam);
	  }
	}
    };
  
  CSimpleBehavior(IvPDomain D):IvPBehavior(D)
    {
      m_pAOF  = new MyAOF(D);
      m_pAOF->SetCallBack(_EvalCB,this);
    }
  
  static double _EvalCB(double * pActions,int nActions, void * pMeVoid)
    {
      CSimpleBehavior* pMe = (CSimpleBehavior*)(pMeVoid);
      return pMe->EvaluateThis(pActions,nActions);
    }
  
  bool setParam(std::string s1,std::string s2)
    {
      return setParamCommon(s1,s2);
    }
  
  virtual double EvaluateThis(double * pActions,int nActions)=0;
  
  bool SetDimension(int n,const std::string & sName)
    {
      return m_pAOF->SetDimension(n,sName);
    }
  
  virtual bool OnPreEvaluate()
    {
      return true;
    }
  
  virtual IvPFunction *produceOF()	
    {
      if(!OnPreEvaluate())
	return NULL;
      
      OF_Reflector Reflector((const AOF*)m_pAOF);
      
      Reflector.create_uniform(unif_box,grid_box);
      IvPFunction* ipf = Reflector.extractOF();
      
    }
  
  bool SetSubDomain(std::string sDomain)
    {
      domain = subDomain(domain,sDomain);
      return true;
    }
  
  MyAOF* m_pAOF;
};

#endif
