/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorRegistry.h                                   */
/*    DATE: May 11th 2011                                        */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#ifndef BEHAVIOR_REGISTRY_HEADER
#define BEHAVIOR_REGISTRY_HEADER

#include <string>
#include <vector>
#include "IvPFunction.h"

class BehaviorRegistry {
public:
  BehaviorRegistry();
  virtual ~BehaviorRegistry() {};
  
  bool addIPF(IvPFunction*, unsigned int, std::string="");
  
  unsigned int size() const    {return(m_keys.size());};
  bool         isEmpty() const {return(m_keys.empty());};

  IvPFunction *getIPF(unsigned int index) const;
  IvPFunction *getIPF(std::string key) const;

 protected:
  std::vector<std::string>  m_keys;
  std::vector<IvPFunction*> m_ipfs;
  std::vector<unsigned int> m_iter;

  bool  m_keys_unique;
  
  unsigned int m_latest_iteration;
};

#endif 

