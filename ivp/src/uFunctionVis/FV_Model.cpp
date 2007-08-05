/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: FV_Model.cpp                                        */
/*    DATE: Sep 24th 2006                                       */
/****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "FV_Model.h"

#define GL_PI 3.1415926f

using namespace std;

//-------------------------------------------------------------
// Constructor

FV_Model::FV_Model()
{
  pthread_mutex_init(&m_ipf_mutex, NULL);

  m_collective = false;
  m_lock_ipf   = false;
  m_curr_descriptor = "null_descriptor";
}

//-------------------------------------------------------------
// Procedure: addIPF()

void FV_Model::addIPF(string str)
{
  pthread_mutex_lock(&m_ipf_mutex);

  if(!m_lock_ipf) {
    string new_descriptor = m_fqueue.addFunction(str);

    m_descriptors.push_back(new_descriptor);
    m_descriptors = removeDuplicates(m_descriptors);
    
    if(m_curr_descriptor == "")
      m_curr_descriptor = new_descriptor;
  }
  pthread_mutex_unlock(&m_ipf_mutex);
}

//-------------------------------------------------------------
// Procedure: modColorMap

void FV_Model::modColorMap(const string &str)
{
  pthread_mutex_lock(&m_ipf_mutex);
  m_fqueue.modColorMap(str);
  pthread_mutex_unlock(&m_ipf_mutex);
}

//-------------------------------------------------------------
// Procedure: incDescriptor()

void FV_Model::incDescriptor()
{
  pthread_mutex_lock(&m_ipf_mutex);

  int vsize = m_descriptors.size();
  bool done = false;
  for(int i=0; (i<vsize)&&!done; i++) {
    if(m_descriptors[i] == m_curr_descriptor) {
      if(i+1 < vsize)
	m_curr_descriptor = m_descriptors[i+1];
      else
	m_curr_descriptor = m_descriptors[0];
      done = true;
    }
  }
  pthread_mutex_unlock(&m_ipf_mutex);
}

//-------------------------------------------------------------
// Procedure: decDescriptor()

void FV_Model::decDescriptor()
{
  pthread_mutex_lock(&m_ipf_mutex);
  int vsize = m_descriptors.size();
  bool done = false;
  for(int i=vsize-1; (i>=0)&&!done; i--) {
    if(m_descriptors[i] == m_curr_descriptor) {
      if(i > 0)
	m_curr_descriptor = m_descriptors[i-1];
      else
	m_curr_descriptor = m_descriptors[vsize-1];
      done = true;
    }
  }
  pthread_mutex_unlock(&m_ipf_mutex);
}

//-------------------------------------------------------------
// Procedure: getCurrDescriptor()

string FV_Model::getCurrDescriptor()
{
  if(m_collective)
    return("Collective Function");
  else
    return(m_curr_descriptor);
}

//-------------------------------------------------------------
// Procedure: getQuadSet()

const QuadSet* FV_Model::getQuadSet()
{
  pthread_mutex_lock(&m_ipf_mutex);

  const QuadSet* quadset = 0;
  if(m_collective)
    quadset = m_fqueue.getCollectiveQuadSet();
  else
    quadset = m_fqueue.getQuadSet(m_curr_descriptor);

  pthread_mutex_unlock(&m_ipf_mutex);
  return(quadset);
}

