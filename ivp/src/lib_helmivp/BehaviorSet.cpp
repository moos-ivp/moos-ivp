/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BehaviorSet.cpp                                      */
/*    DATE: Oct 27th 2004 Sox up 3-0 in the Series               */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <set>
#include "BehaviorSet.h"
#include "MBUtils.h"
#include "IvPFunction.h"
#include "FunctionEncoder.h"
#include "ColorParse.h"

using namespace std;

//------------------------------------------------------------
// Constructor

BehaviorSet::BehaviorSet()
{
  m_report_ipf = true;
  m_curr_time  = -1;
  m_bfactory_dynamic.loadEnvVarDirectories("IVP_BEHAVIOR_DIRS");

  m_total_behaviors_ever = 0;
  m_bhv_entry.reserve(1000);
  m_completed_pending = false;
}

//------------------------------------------------------------
// Destructor

BehaviorSet::~BehaviorSet()
{
  clearBehaviors();
}

//------------------------------------------------------------
// Procedure: addBehaviorDir

bool BehaviorSet::addBehaviorDir(string dirname)
{
  cout << termColor("blue");
  cout << "Loading behavior dynamic library specified in .moos file: ";
  cout << dirname << endl;
  cout << termColor();

  bool ok = m_bfactory_dynamic.loadDirectory(dirname);

  cout << termColor("blue");
  cout << "Done Loading behavior dynamic library: " << dirname << endl;
  cout << termColor();
  return(ok);
}

//------------------------------------------------------------
// Procedure: addBehavior

void BehaviorSet::addBehavior(IvPBehavior *bhv)
{
  if(!bhv)
    return;

  BehaviorSetEntry bse(bhv);
  string bhv_name = bhv->getDescriptor();
  //bse.setName(bhv_name);

  m_bhv_entry.push_back(bse);
  m_bhv_names.insert(bhv_name);

  m_total_behaviors_ever++;

  cout << "Total Behaviors now: " << m_total_behaviors_ever << endl;
}

//------------------------------------------------------------
// Procedure: setDomain

void BehaviorSet::setDomain(IvPDomain domain)
{
  m_bfactory_static.setDomain(domain);
  m_bfactory_dynamic.setDomain(domain);
}

//------------------------------------------------------------
// Procedure: clearBehaviors()

void BehaviorSet::clearBehaviors()
{
  m_bhv_entry.clear();
  m_bhv_names.clear();
}


//------------------------------------------------------------
// Procedure: addBehaviorSpec

void BehaviorSet::addBehaviorSpec(BehaviorSpec spec)
{
  m_behavior_specs.push_back(spec);
}


//------------------------------------------------------------
// Procedure: connectInfoBuffer()
//      Note: Connects info_buffer to all behaviors, behavior_specs
//      Note: The info_buffer is not "owned" by behaviors or specs

void BehaviorSet::connectInfoBuffer(InfoBuffer *info_buffer)
{
  unsigned int i, vsize = m_bhv_entry.size();
  for(i=0; i<vsize; i++)
    if(m_bhv_entry[i].getBehavior())
      m_bhv_entry[i].getBehavior()->setInfoBuffer(info_buffer);

  vsize = m_behavior_specs.size();
  for(i=0; i<vsize; i++)
    m_behavior_specs[i].setInfoBuffer(info_buffer);    
}

//------------------------------------------------------------
// Procedure: buildBehaviorsFromSpecs()
//      Note: Enforcement of unique behaviors names is made here.

bool BehaviorSet::buildBehaviorsFromSpecs()
{
  vector<SpecBuild> spec_builds;
  
  // Set of new behavior names instantiated in this invocation
  set<string> bhv_names;

  // First try to build behaviors from all non-template specs
  bool all_builds_ok = true;
  unsigned int i, vsize = m_behavior_specs.size();
  cout << "BehaviorSet: Total # of specs: " << vsize << endl;
  for(i=0; i<vsize; i++) {
    BehaviorSpec spec = m_behavior_specs[i];
    SpecBuild sbuild  = buildBehaviorFromSpec(spec, "", true);
    // All spec_builds should be put in the vector of spec_builds, even
    // if they were not valid. The only ones not added were validly 
    // created builds from a behavior with templating type = spawn. We
    // need to create such behaviors to see if they are syntactically 
    // correct, but delete them immediately by definition of type=spawn.
    cout << "spec_build: " << i << "  result:" << sbuild.valid() << endl;

    if(!sbuild.valid()) {
      all_builds_ok = false;
      spec_builds.push_back(sbuild);
    }
    else {
      vector<VarDataPair> msgs = sbuild.getHelmStartMessages();
      m_helm_start_msgs.insert(m_helm_start_msgs.end(), msgs.begin(), msgs.end());

      if(spec.getTemplatingType() == "spawn")
	sbuild.deleteBehavior();
      else {
	spec_builds.push_back(sbuild);
	string bhv_name   = sbuild.getBehaviorName();
      	// If otherwise valid, check if the new bhv_name is unique
	if(!uniqueNameX(bhv_name, bhv_names) ||
	   !uniqueNameX(bhv_name, m_bhv_names)) {
	  cerr << "Duplicate behavior name found: " << bhv_name << endl;
	  all_builds_ok = false;
	  addWarning("Duplicate behavior name found: " + bhv_name);
	}
	else
	  bhv_names.insert(bhv_name);
      }
    }
  }
  
  if(all_builds_ok)
    cout << "BehaviorSet: all_builds_ok: true" << endl;
  else
    cerr << "BehaviorSet: all_builds_ok: false" << endl;

  // If any of the builds failed, (1) output error messages for
  // each failed build, and (2) delete the IvPBehavior instances
  // for all successful builds, and (3) abort this attempt by
  // returning false immediately.
  if(!all_builds_ok) {
    unsigned int k, ksize = spec_builds.size();
    for(k=0; k<ksize; k++) {
      if(spec_builds[k].valid()) 
	spec_builds[k].deleteBehavior();
      else {
	unsigned int j, jsize = spec_builds[k].numBadConfigs();
	for(j=0; j<jsize; j++) {
	  string bad_config = spec_builds[k].getBadConfigLine(j);
	  unsigned int lnum = spec_builds[k].getBadConfigLineNum(j);
	  cerr << "Fatal Behavior Configuration Line " << lnum << endl;
	  cerr << "  [" << lnum << "]: " << bad_config << endl;	  
	}
      }
    }
    return(false);
  }

  // If all the builds were successful, populate the behavior_set
  // with all the new IvPBehaviors, and add LifeEvents.
  unsigned int k, ksize = spec_builds.size();
  cout << "total specs::" << ksize << endl;
  for(k=0; k<ksize; k++) {
    IvPBehavior *bhv = spec_builds[k].getIvPBehavior();
    addBehavior(bhv);

    LifeEvent life_event;
    life_event.setBehaviorName(spec_builds[k].getBehaviorName());
    life_event.setBehaviorType(spec_builds[k].getBehaviorKind());
    life_event.setSpawnString("helm_startup");
    life_event.setEventType("spawn");
    m_life_events.push_back(life_event);
  }

  return(true);
}


//------------------------------------------------------------
// Procedure: buildBehaviorFromSpec()

SpecBuild BehaviorSet::buildBehaviorFromSpec(BehaviorSpec spec, 
					     string update_str,
					     bool on_startup)
{
  SpecBuild    sbuild;
  string       bhv_kind = spec.getKind();
  unsigned int lnum     = spec.getKindLine();
  
  sbuild.setBehaviorKind(bhv_kind, lnum);
  
  IvPBehavior *bhv = m_bfactory_static.newBehavior(bhv_kind);
  if(bhv) 
    sbuild.setKindResult("static");
  else {
    bhv = m_bfactory_dynamic.newBehavior(bhv_kind);
    if(bhv)
      sbuild.setKindResult("dynamic");
    else {
      string err = "BehaviorSpec: failed to load dynamic behavior " + bhv_kind;
      cerr << err << endl;
      addWarning(err);
      addWarning("Check IVP_BEHAVIOR_DIRS Env Variable or ivp_behavior_dir param");
      sbuild.setKindResult("failed");
      return(sbuild);
    }
  }
  
  bhv->setBehaviorType(bhv_kind);
  bhv->IvPBehavior::setParam("us", m_ownship);


  // First apply all the behavior specs from the original specification
  // given in the .bhv file. All bad specs are noted, not just the first.
  bool specs_valid = true;
  unsigned int i, count = spec.size();
  for(i=0; i<count; i++) {
    string orig  = spec.getConfigLine(i);
    string cline = spec.getConfigLine(i);
    string left  = tolower(biteStringX(cline, '='));
    string right = cline;
    bool valid = false;
    if((left == "name") || (left == "descriptor"))
      valid = bhv->IvPBehavior::setBehaviorName(right);
    else
      valid = bhv->IvPBehavior::setParam(left, right);

    if(!valid)
      valid = bhv->setParam(left, right);
    if(!valid) {
      unsigned int bad_line = spec.getConfigLineNum(i);
      sbuild.addBadConfig(cline, bad_line);

      string filename = spec.getFileName();
      string msg = filename + ": ";
      msg += "Line " + uintToString(bad_line) + ": " + orig;
      addWarning(msg);
    }

    specs_valid = specs_valid && valid;
  }

  // Then apply all the behavior specs from an UPDATES string which may
  // possibly be empty.
  // NOTE: If the update_str is non-empty we can assume this is a spawning
  vector<string> jvector = parseStringQ(update_str, '#');
  unsigned int j, jsize = jvector.size();
  for(j=0; j<jsize; j++) {
    string orig  = jvector[j];
    string cline = jvector[j];
    string left  = tolower(biteStringX(cline, '='));
    string right = cline;
    bool   valid = false;
    if((left == "name") || (left == "descriptor"))
      valid = bhv->IvPBehavior::augBehaviorName(right);
    else
      valid = bhv->IvPBehavior::setParam(left.c_str(), right.c_str());

    if(!valid)
      valid = bhv->setParam(left.c_str(), right.c_str());
    if(!valid) {
      sbuild.addBadConfig(jvector[j], 0);
      
      string msg = "Failed to spawn " + bhv->getDescriptor();
      msg += + ". Bad config: " + orig;
      addWarning(msg);
    }

    specs_valid = specs_valid && valid;
  }

  if(specs_valid) {
    sbuild.setIvPBehavior(bhv);
    // Added Oct 1313 mikerb - allow template behaviors to make an
    // initial posting on helm startup, even if no instance made on
    // startup (or ever).
    if(on_startup) {
      cout << bhv->getDescriptor() << endl;
      bhv->onHelmStart();
    }
    // The behavior may now have some messages (var-data pairs) ready
    // for retrieval
  }
  else {
    delete(bhv);
  }

  return(sbuild);
}


//------------------------------------------------------------
// Procedure: handlePossibleSpawnings
//   Purpose: Called typically once on each iteration of the helm
//            to check if any new spawnings were requested. 

bool BehaviorSet::handlePossibleSpawnings()
{
  unsigned int i, isize = m_behavior_specs.size();
  for(i=0; i<isize; i++) {

    if(!m_behavior_specs[i].templating())
      continue;

    vector<string> update_strs = m_behavior_specs[i].checkForSpawningStrings();     
    unsigned int jsize = update_strs.size();
    for(unsigned int j=0; j<jsize; j++) {
      string update_str = update_strs[j];
      
      // Check for unique behavior name
      // e.g. if name is "henry", make sure "henry" and "prefix_henry"
      // don't already exist.
      
      string bname = tokStringParse(update_str, "name", '#', '=');
      string fullname = m_behavior_specs[i].getNamePrefix() + bname;

      // For example: If the behavior name prefix is avd_obstacle_,
      // and a behavior has already been spawned with the name
      // avd_obstacle_blue, then an update with name=avd_obstacle_blue or
      // name=blue would be applied to the previously spawned behavior.

      if((m_bhv_names.count(fullname)==0) && (m_bhv_names.count(bname) == 0)) {
	SpecBuild sbuild = buildBehaviorFromSpec(m_behavior_specs[i], update_str);
	m_behavior_specs[i].spawnTried();
	//sbuild.print();
	
	LifeEvent life_event;
	life_event.setBehaviorName(sbuild.getBehaviorName());
	life_event.setBehaviorType(sbuild.getBehaviorKind());
	life_event.setSpawnString(update_str);
	
	if(sbuild.valid()) {
	  IvPBehavior *bhv = sbuild.getIvPBehavior();
	  // Called here now since it was just spawned and could not have
	  // been called previously. 07/18/12 mikerb
	  bhv->onSetParamComplete(); 
	  bhv->onSpawn();
	  bhv->postFlags("spawnflags", true);
	  addBehavior(bhv); 
	  life_event.setEventType("spawn");
	  m_behavior_specs[i].spawnMade();
	}
	else
	  life_event.setEventType("abort");
	m_life_events.push_back(life_event);
      }
      // If base+name already exists AND base does not exist, post warning
      else {
	if(m_bhv_names.count(bname)!=0) 
	  addWarning("Unhandled update: Existing bhv named [" + bname + "] not found. ");

	// The below check is disabled for now because the contact manager is
	// erroneously posting two idential alerts. Once the contact manager is
	// fixed, the below check should be re-enabled.

	//if(m_bhv_names.count(fullname) != 0)
	//  addWarning("Unhandled update: Spawned bhv named [" + fullname + "] is taken.");
      }
    }
  }
  return(true);
}

//------------------------------------------------------------
// Procedure: refreshMapUpdateVars()
//      Note: Returns true if changed since the previous call.

bool BehaviorSet::refreshMapUpdateVars()
{
  // Part 1: Create the new map from bhv name to update var
  map<string, string> new_map_update_vars;
  for(unsigned int i=0; i<m_bhv_entry.size(); i++) {
    if(m_bhv_entry[i].getBehavior()) {
      string update_var = m_bhv_entry[i].getBehavior()->getUpdateVar();
      if(update_var != "") {
	string bhv_name = m_bhv_entry[i].getBehavior()->getDescriptor();
	new_map_update_vars[bhv_name] = update_var;
      }
    }
  }

  // Part 2: Check if the new map is different from the old map
  bool changed = false;
  if(m_map_update_vars.size() != new_map_update_vars.size())
    changed = true;
  else {
    map<string, string>::iterator p;
    for(p=m_map_update_vars.begin(); p!=m_map_update_vars.end(); p++) {
      string bhv_name = p->first;
      string update_var = p->second;
      if(new_map_update_vars.count(bhv_name) == 0)
	changed = true;
      else if(new_map_update_vars[bhv_name] != m_map_update_vars[bhv_name])
	changed = true;
    }
  }

  m_map_update_vars = new_map_update_vars;
  return(changed);
}

//------------------------------------------------------------
// Procedure: getUpdateVarSummary()
//   Example: loiter=LOITER_UPDATE,survey=SURVEY_UPDATE,dive=DIVE_UPDATE

string BehaviorSet::getUpdateVarSummary()
{
  string summary;
  map<string, string>::iterator p;
  for(p=m_map_update_vars.begin(); p!=m_map_update_vars.end(); p++) {
    string bhv_name = p->first;
    string update_var = p->second;
    if(summary != "")
      summary += ",";
    summary += bhv_name + "=" + update_var;
  }
  
  return(summary);
}

//------------------------------------------------------------
// Procedure: produceOF

IvPFunction* BehaviorSet::produceOF(unsigned int ix, 
				    unsigned int iteration, 
				    string& new_activity_state,
				    bool& ipf_reuse)
{
  // Quick index sanity check
  if(ix >= m_bhv_entry.size())
    return(0);
  
  // ===================================================================
  // Part 1: Prepare and update behavior, determine its new activity state
  // ===================================================================
  IvPFunction *ipf = 0;
  IvPBehavior *bhv = m_bhv_entry[ix].getBehavior();

  // possible vals: "", "idle", "running", "active"
  string old_activity_state = m_bhv_entry[ix].getState();

  // Look for possible dynamic updates to the behavior parameters
  bool update_made = bhv->checkUpdates();
  if(update_made) 
    bhv->onSetParamComplete();

  vector<string> update_results = bhv->getUpdateResults();
  for(unsigned int i=0; i<update_results.size(); i++)
    m_update_results.push_back(update_results[i]);


  bhv->setHelmIteration(iteration);
  // Check if the behavior duration is to be reset
  bhv->checkForDurationReset();
  
  // Possible vals: "completed", "idle", "running"
  new_activity_state = bhv->isRunnable();
  
  // ===================================================================
  // Part 2: With new_activity_state set, act appropriately for
  //         each behavior.
  // ===================================================================
  // Part 2A: Handle completed behaviors (marked as comleted at the start
  // of this iteration. Behaviors that become complete in this iteration 
  // are handled below, after executing onIdleState, onRunState() etc.
  if(new_activity_state == "completed")
    bhv->onCompleteState();
  
  // Part 2B: Handle idle behaviors
  if(new_activity_state == "idle") {
    if(old_activity_state != "idle") {
      bhv->postFlags("idleflags", true);
      bhv->postFlags("inactiveflags", true);
    }
    if((old_activity_state == "running") || (old_activity_state == "active"))
      bhv->onRunToIdleState();
    bhv->onIdleState();
    bhv->updateStateDurations("idle");
  }
  
  // Part 2C: Handle running behaviors
  if(new_activity_state == "running") {
    double pwt = 0;
    int    pcs = 0;
    if((old_activity_state == "idle") || (old_activity_state == ""))
      bhv->postFlags("runflags", true); // true means repeatable
    bhv->postDurationStatus();
    if(old_activity_state == "idle")
      bhv->onIdleToRunState();

    // Step 1: Ask the behavior to build a IvP function
    bool need_to_run = bhv->onRunStatePrior();
    ipf_reuse = !need_to_run;
    bhv->noteLastRunCheck(need_to_run, getCurrTime());

    if(need_to_run)
      ipf = bhv->onRunState();

    // Step 2: If IvP function contains NaN components, report and abort
    if(ipf && !ipf->freeOfNan()) {
      bhv->postEMessage("NaN detected in IvP Function");
      delete(ipf);
      ipf = 0;
    }
    // Step 3: If IvP function has non-positive priority, abort
    if(ipf) {
      pwt = ipf->getPWT();
      pcs = ipf->getPDMap()->size();
      if(pwt <= 0) {
	delete(ipf);
	ipf = 0;
	pcs = 0;
      }
    }
    // Step 4: If we're serializing and posting IvP functions, do here
    if(ipf && m_report_ipf) {
      string desc_str = bhv->getDescriptor();
      string iter_str = uintToString(iteration);
      string ctxt_str = iter_str + ":" + desc_str;
      ipf->setContextStr(ctxt_str);
      string ipf_str = IvPFunctionToString(ipf);
      bhv->postMessage("BHV_IPF", ipf_str);
    }
    // Step 5: Handle normal case of healthy IvP function returned
    if(ipf) {
      if(old_activity_state != "active")
	bhv->postFlags("activeflags", true); // true means repeatable
      new_activity_state = "active";
      bhv->statusInfoAdd("pwt", doubleToString(pwt));
      bhv->statusInfoAdd("pcs", intToString(pcs));
    }
    // Step 6: Handle where behavior decided not to product an IPF
    else if(!ipf && !ipf_reuse) {
      if(old_activity_state == "active")
	bhv->postFlags("inactiveflags", true); // true means repeatable
      bhv->onInactiveState();
    }
    bhv->updateStateDurations("running");
  }

  // Bug fix Jan 26th, 2016 mikerb
  // Possibly during the execution of onRunState(), onIdleState() etc,
  // the behavior may have completed. Note it here so all the complete
  // stuff can be executed during this iteration. Behavior should be
  // deleted with this iteration by the helm by noting its complete state.
  string post_process_activity_state = bhv->isRunnable();
  if(post_process_activity_state == "completed") {
    new_activity_state = "completed";
    bhv->onCompleteState();
  }    
   
  
  // =========================================================================
  // Part 3: Update all the bookkeeping structures 
  // =========================================================================
  bhv->statusInfoAdd("state", new_activity_state);
  bhv->statusInfoPost();
  
  // If this represents a change in states from the previous
  // iteration, note the time at which the state changed.
  if(old_activity_state != new_activity_state)
    m_bhv_entry[ix].setStateTimeEntered(m_curr_time);
  
  m_bhv_entry[ix].setState(new_activity_state);
  double state_time_entered = m_bhv_entry[ix].getStateTimeEntered();
  double elapsed = (m_curr_time - state_time_entered);
  m_bhv_entry[ix].setStateTimeElapsed(elapsed);

  // Return either the IvP function or NULL
  return(ipf);
}

//------------------------------------------------------------
// Procedure: produceOFX

#if 0
BehaviorReport BehaviorSet::produceOFX(unsigned int ix, 
				       unsigned int iteration, 
				       string& new_activity_state)
{
  //IvPFunction *ipf = 0;
  BehaviorReport bhv_report_empty;
  BehaviorReport bhv_report;

  if(ix < m_bhv_entry.size()) {
    IvPBehavior *bhv = m_bhv_entry[ix].getBehavior();
    string old_activity_state = m_bhv_entry[ix].getState();
    if(old_activity_state == "")
      old_activity_state = "idle";

    // Look for possible dynamic updates to the behavior parameters
    bool update_made = bhv->checkUpdates();
    if(update_made)
      bhv->onSetParamComplete();

    // Check if the behavior duration is to be reset
    bhv->checkForDurationReset();

    new_activity_state = bhv->isRunnable();

    // Now that the new_activity_state is set, act appropriately for
    // each behavior.
    if(new_activity_state == "completed")
      bhv->onCompleteState();

    if(new_activity_state == "idle") {
      bhv->postFlags("idleflags");
      bhv->postFlags("inactiveflags");
      if((old_activity_state == "running") ||
	 (old_activity_state == "active"))
	bhv->onRunToIdleState();
      bhv->onIdleState();
      bhv->updateStateDurations("idle");
    }
    
    if(new_activity_state == "running") {
      bhv->postDurationStatus();
      bhv->postFlags("runflags");
      if(old_activity_state == "idle")
	bhv->onIdleToRunState();

      bhv_report = bhv->onRunState("");
      bool nans_detected = bhv_report.checkForNans();
      if(nans_detected) {
	bhv->postEMessage("NaN detected in IvP Function");
	bhv_report = BehaviorReport();
      }

      unsigned int i, vsize = bhv_report.size();
      for(i=0; i<vsize; i++) {
	if(bhv_report.hasIPFString(i))
	  bhv->postMessage("BHV_IPF", bhv_report.getIPFString(i));
      }

      if(bhv_report.size() > 0) {
	new_activity_state = "active";
	bhv->postFlags("activeflags");
      }
      else
	bhv->postFlags("inactiveflags");
      bhv->updateStateDurations("running");
    }
    bhv->statusInfoAdd("state", new_activity_state);
    bhv->statusInfoPost();

    // If this represents a change in states from the previous
    // iteration, note the time at which the state changed.
    if(old_activity_state != new_activity_state)
      m_bhv_entry[ix].setStateTimeEntered(m_curr_time);
    
    m_bhv_entry[ix].setState(new_activity_state);
    double state_time_entered = m_bhv_entry[ix].getStateTimeEntered();
    double elapsed = (m_curr_time - state_time_entered);
    m_bhv_entry[ix].setStateTimeElapsed(elapsed);
  }
  return(bhv_report);
}
#endif


//------------------------------------------------------------
// Procedure: stateOK

bool BehaviorSet::stateOK(unsigned int ix)
{
  if(ix < m_bhv_entry.size())
    return(m_bhv_entry[ix].getBehavior()->stateOK());
  else
    return(false);
}

//------------------------------------------------------------
// Procedure: resetStateOK

void BehaviorSet::resetStateOK()
{
  unsigned int i, vsize = m_bhv_entry.size();
  for(i=0; i<vsize; i++) 
    m_bhv_entry[i].getBehavior()->resetStateOK();
}


//------------------------------------------------------------
// Procedure: removeCompletedBehaviors

unsigned int BehaviorSet::removeCompletedBehaviors()
{
  if(!m_completed_pending)
    return(0);

  unsigned int i, vsize = m_bhv_entry.size();
  unsigned int completed_count = 0;

  vector<BehaviorSetEntry> new_bhv_entry;
  new_bhv_entry.reserve(vsize);

  for(i=0; i<vsize; i++) {
    if(m_bhv_entry[i].getState() != "completed")
      new_bhv_entry.push_back(m_bhv_entry[i]);
    else {
      LifeEvent life_event;
      life_event.setBehaviorName(m_bhv_entry[i].getBehaviorName());
      life_event.setBehaviorType(m_bhv_entry[i].getBehaviorType());
      life_event.setSpawnString("");
      life_event.setEventType("death");
      m_life_events.push_back(life_event);

      m_bhv_names.erase(m_bhv_entry[i].getBehaviorName());
      m_bhv_entry[i].deleteBehavior();   // Actual deletion of IvP behavior
      completed_count++;
    }
  }
  
  m_bhv_entry.clear();
  m_bhv_entry = new_bhv_entry;

  m_completed_pending = false;
  return(completed_count);
}

//------------------------------------------------------------
// Procedure: getBehavior

IvPBehavior* BehaviorSet::getBehavior(unsigned int ix)
{
  if(ix < m_bhv_entry.size())
    return(m_bhv_entry[ix].getBehavior());
  return(0);
}

//------------------------------------------------------------
// Procedure: getDescriptor

string BehaviorSet::getDescriptor(unsigned int ix)
{
  if(ix < m_bhv_entry.size())
    return(m_bhv_entry[ix].getBehaviorName());
  return("");
}

//------------------------------------------------------------
// Procedure: getStateElapsed

double BehaviorSet::getStateElapsed(unsigned int ix)
{
  if(ix < m_bhv_entry.size())
    return(m_bhv_entry[ix].getStateTimeElapsed());
  else
    return(-1);
}

//------------------------------------------------------------
// Procedure: getStateTimeEntered

double BehaviorSet::getStateTimeEntered(unsigned int ix)
{
  if(ix < m_bhv_entry.size())
    return(m_bhv_entry[ix].getStateTimeEntered());
  else
    return(-1);
}

//------------------------------------------------------------
// Procedure: getUpdateSummary()

string BehaviorSet::getUpdateSummary(unsigned int ix)
{
  if(ix < m_bhv_entry.size())
    return(m_bhv_entry[ix].getBehavior()->getUpdateSummary());
  else
    return("err");
}

//------------------------------------------------------------
// Procedure: getFilterLevel

int BehaviorSet::getFilterLevel(unsigned int ix)
{
  if(ix < m_bhv_entry.size())
    return(m_bhv_entry[ix].getBehavior()->getFilterLevel());
  else
    return(0);
}

//------------------------------------------------------------
// Procedure: filterBehaviorsPresent

bool BehaviorSet::filterBehaviorsPresent()
{
  unsigned int ix, vsize = m_bhv_entry.size();
  for(ix=0; ix<vsize; ix++) 
    if(m_bhv_entry[ix].getBehavior()->getFilterLevel() != 0)
      return(true);
  return(false);
}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> BehaviorSet::getMessages(unsigned int ix, 
					     bool clear)
{
  vector<VarDataPair> mvector;
  if(ix < m_bhv_entry.size()) {
    mvector = m_bhv_entry[ix].getBehavior()->getMessages();
    if(clear)
      m_bhv_entry[ix].getBehavior()->clearMessages();
  }
  return(mvector);
}

//------------------------------------------------------------
// Procedure: getInfoVars

vector<string> BehaviorSet::getInfoVars()
{
  vector<string> rvector;
  unsigned int i, vsize = m_bhv_entry.size();
  for(i=0; i<vsize; i++) {
    vector<string> jvector = m_bhv_entry[i].getBehavior()->getInfoVars();
    unsigned j, jsize = jvector.size();
    for(j=0; j<jsize; j++)
      rvector.push_back(jvector[j]);
  }

  vector<string> mvector = m_mode_set.getConditionVars();

  rvector = mergeVectors(rvector, mvector);

  rvector = removeDuplicates(rvector);
  
  return(rvector);
}

//------------------------------------------------------------
// Procedure: getNewInfoVars

vector<string> BehaviorSet::getNewInfoVars()
{
  vector<string> rvector;

  vector<string> cvector = getInfoVars();
  unsigned int i, csize = cvector.size();
  for(i=0; i<csize; i++) {
    if(!m_prev_info_vars.count(cvector[i])) {
      rvector.push_back(cvector[i]);
      m_prev_info_vars.insert(cvector[i]);
    }
  }

  return(rvector);
}

//------------------------------------------------------------
// Procedure: getSpecUpdateVars()

vector<string> BehaviorSet::getSpecUpdateVars()
{
  vector<string> rvector;
  unsigned int i, vsize = m_behavior_specs.size();
  for(i=0; i<vsize; i++) {
    string templating = m_behavior_specs[i].getTemplatingType();
    string updates_var = m_behavior_specs[i].getUpdatesVar();
    if((templating != "disallowed") && (updates_var != ""))
      rvector.push_back(updates_var);
  }
  return(rvector);
}

//------------------------------------------------------------
// Procedure: addWarning()

void BehaviorSet::addWarning(const string& str)
{
  m_warnings.push_back(str);
}

//------------------------------------------------------------
// Procedure: uniqueNameX()
//   Purpose: Determine if the given name matches any of the names
//            in the given set of names.
//      Note: A "match" is declared if either (a) the two names 
//            are identical, or (b) one name of N characters 
//            matches the first N characters of the other name.

bool BehaviorSet::uniqueNameX(const string& given_name, 
			      const set<string>& given_set)
{
  set<string>::const_iterator p;
  for(p=given_set.begin(); p!=given_set.end(); p++) {
    string this_name = *p;
    if(strBegins(given_name, this_name))
      return(false);
    if(strBegins(this_name, given_name))
      return(false);
  }

  return(true);
}
    
//------------------------------------------------------------
// Procedure: updateStateSpaceVars()
//      Note: Since duplicates are checked for here, and this is
//            the only place where the state_space_vars vector
//            is updated, we know there are no duplicates here.
//      Note: Returns true only if a new variable has been detected
//            in any of the instantiated behaviors.

bool BehaviorSet::updateStateSpaceVars()
{
  unsigned int old_size = m_state_space_vars.size();

  unsigned int i, vsize = m_bhv_entry.size();
  for(i=0; i<vsize; i++) {
    if(m_bhv_entry[i].getBehavior() != 0) {
      vector<string> ivars = m_bhv_entry[i].getBehavior()->getStateSpaceVars();
      unsigned int k, isize = ivars.size();
      for(k=0; k<isize; k++)
	m_state_space_vars.insert(ivars[k]);
    }
  }

  unsigned int new_size = m_state_space_vars.size();
  return(new_size > old_size);
}

//------------------------------------------------------------
// Procedure: getStateSpaceVars()

string BehaviorSet::getStateSpaceVars()
{
  string return_str;

  set<string>::iterator p;
  for(p=m_state_space_vars.begin(); p!=m_state_space_vars.end(); p++) {
    string str = *p;
    if(return_str != "")
      return_str += ",";
    return_str += str;
  }
  return(return_str);
}

//------------------------------------------------------------
// Procedure: getTemplatingSummary()

vector<string> BehaviorSet::getTemplatingSummary() const
{
  vector<string> summary;
  for(unsigned int i=0; i<m_behavior_specs.size(); i++) {
    if(m_behavior_specs[i].templating()) {
      string bhv_kind = m_behavior_specs[i].getKind();
      string bhv_name_prefix = m_behavior_specs[i].getNamePrefix();
      string entry = bhv_kind + "::" + bhv_name_prefix;
      
      unsigned int spawns_made  = m_behavior_specs[i].getSpawnsMade();
      unsigned int spawns_tried = m_behavior_specs[i].getSpawnsTried();
      
      entry += " (Spawned:" + uintToString(spawns_made) + "/" +
	uintToString(spawns_tried) + ")";
      summary.push_back(entry);
    }
  }

  return(summary);
}


//------------------------------------------------------------
// Procedure: print

void BehaviorSet::print()
{
  unsigned int i, vsize = m_bhv_entry.size();

  cout << "BehaviorSet::print() " << endl;
  for(i=0; i<vsize; i++) {
    cout << "Behavior[" << i << "]: " << endl;
    
    IvPBehavior *bhv = m_bhv_entry[i].getBehavior();

    cout << "Behavior descriptor: " << bhv->getDescriptor() << endl;
    cout << " priority weight: " << bhv->m_priority_wt << endl;
    cout << " BuildInfo: " << endl;
    cout << bhv->m_build_info << endl;
    cout << "-------" << endl;
  }
}

//------------------------------------------------------------
// Procedure: size()

unsigned long int BehaviorSet::size() const
{
  unsigned long int amt = m_bhv_entry.size();

  amt += m_bhv_names.size();
  amt += m_initial_vars.size();

  amt += m_initial_vars.size();
  amt += m_default_vars.size();
  amt += m_helm_start_msgs.size();

  amt += m_prev_info_vars.size();
  amt += m_state_space_vars.size();

  amt += m_warnings.size();

  amt += m_behavior_specs.size();

  amt += m_life_events.size();

  amt += m_update_results.size();

  amt += m_map_update_vars.size();

  return(amt);
}
