#include "BehaviorSet.h"
#include "InfoBuffer.h"
#include "Populator_BehaviorSet.h"
#include "IvPDomain.h"
#include "StringTree.h"
#include "MBUtils.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdio>

int main(int argc, char* argv[])
{
  if(argc != 3) {
    std::cerr << "Usage: bhv2graphviz file1.bhv output.dot" << std::endl;
    return 1;
  }
  
    
  InfoBuffer* m_info_buffer = new InfoBuffer;
  IvPDomain m_ivp_domain;
  m_ivp_domain.addDomain("placeholder-domain", 0, 100, 100);
  
  Populator_BehaviorSet *p_bset;
  p_bset = new Populator_BehaviorSet(m_ivp_domain, m_info_buffer);
  
  std::set<std::string> m_bhv_files;
  m_bhv_files.insert(argv[1]);
  
  BehaviorSet* m_bhv_set = p_bset->populate(m_bhv_files);
  
  if(m_bhv_set) {
    std::map<std::string, std::vector<LogicCondition> > logic_conds = m_bhv_set->getModeLogicConditions();
    
    StringTree new_tree;
    
    std::vector<std::string> svector = parseString(m_bhv_set->getModeSetDefinition(), '#');
    unsigned int i, vsize = svector.size();
    for(i=0; i<vsize; i++) {
      std::string parent = stripBlankEnds(biteString(svector[i], ','));
      std::string child  = stripBlankEnds(svector[i]);
      
      if(!logic_conds.count(parent) && parent != "---" && child != "")
	logic_conds[parent] = std::vector<LogicCondition>();
      
      if(!logic_conds.count(child) && child != "")
	logic_conds[child] = std::vector<LogicCondition>();
      
      
      if(child == "") {
	new_tree.setKey(parent);
      }
      else {
	new_tree.addParChild(parent, child);
      }
    }
    
    std::string output = argv[2];
    ofstream of(output.c_str());
    
    std::stringstream labels;
    
    std::map<std::string, std::vector<LogicCondition> >::iterator it, end;
    for(it = logic_conds.begin(), end = logic_conds.end(); it != end; ++it) {
      std::string clean_label=it->first;
      std::replace(clean_label.begin(), clean_label.end(), '-', '_');
      
      labels << clean_label << " [shape=box label=<<b>" << it->first << "</b>";
      
      if(it->second.size()) {
	labels << "<font point-size=\"10\">";
	std::vector<LogicCondition>::iterator jt, jend;
	for(jt = it->second.begin(), jend = it->second.end(); jt != jend; ++jt)
	  labels << "<br/> " << jt->getRawCondition();
	labels << "</font>;";
      }
      labels << ">];\n";    
    }
    
        
    if(of.is_open())
      new_tree.writeGraphviz(of, labels.str());
    else
      std::cerr << "Failed to open output file: " << output << std::endl;
  }
  
  return 0;
}
