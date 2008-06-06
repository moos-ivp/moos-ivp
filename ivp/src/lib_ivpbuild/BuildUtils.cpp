/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BuildUtils.cpp                                       */
/*    DATE: May 15th, 2003                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <iostream>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "BuildUtils.h"
#include "MBUtils.h"

using namespace std;

#define min(x, y) ((x)<(y)?(x):(y))
#define max(x, y) ((x)>(y)?(x):(y))

//-------------------------------------------------------------
// Procedure: genUnifBox
//   Purpose: Generate a box, "ubox" s.t. if a uniform PDMap were
//            made with ubox, it would have no more than "maxAmount"
//            of pieces. 
//      Note: The algorithm tries to generate a ubox with low aspect
//            ratio. But once it becomes impossible to grow an
//            edge of ubox due to the maxAmount restriction, it will
//            grow the other edges regardless of the aspect ratio
//            until it is as large as possible.
//      Note: The significant info of the returned box, is the value
//            of the high edge. A domain of [0,283] would indicate
//            an edge length of 284, for example.

IvPBox genUnifBox(const IvPDomain &domain, int maxAmount)
{
  if(maxAmount <= 0) {
    IvPBox null_box;
    return(null_box);
  }
  
  int d, dim = domain.size();

  // Allocate some arrays determined from the value of "dim". 
  // Would like to allocate these from the stack using g++
  // extensions, but this is not compatable on some platforms.
  int*    uhgh     = new int[dim];                    
  int*    maxSplit = new int[dim];
  bool*   dmaxed   = new bool[dim];
  double* dsplit   = new double[dim];
  double* dsize    = new double[dim];
  
  for(d=0; d<dim; d++)                     // Store vals locally to
    uhgh[d] = domain.getVarPoints(d);      // minimize function calls.

  for(d=0; d<dim; d++) {
    double dval;
    dval = (double)(uhgh[d] + 1);
    dval = ceil(dval / 2.0);
    maxSplit[d] = (int)(dval);
  }

  for(d=0; d<dim; d++) {
    dsplit[d] = 1.0;                   // Num splits for dim d
    dmaxed[d] = false;                 // False if more splits ok
    //dsize[d]  = (double)(uhgh[d]+1); // benign bugfix
    dsize[d]  = (double)(uhgh[d]);     // Size of unif box for dim d    
  }

  bool done = false;
  while(!done) {
    done = true;                      // Check to see if all dims 
    for(d=0; d<dim; d++)              // have been maxed-out. If so
      done = done && dmaxed[d];       // we're done, and loop ends.

    if(!done) {                       // Find the dimension most  
      int    biggestDim;              // worthy of one more split.
      double biggestVal = 0.0;        // It is the dimension that
      for(d=0; d<dim; d++) {          // has the largest dsize[]
	if(!dmaxed[d])                // value that has not been
	  if(dsize[d] > biggestVal) { // deemed to be maxed-out. A
	    biggestVal = dsize[d];    // maxed-out dim, d, has the
	    biggestDim = d;           // value dmaxed[d] = TRUE.
	  }
      }
      int bd = biggestDim;             
      
      // Now calculate the extents of the uniform box if it were
      // to be made with the current dsplit[] values. Then calc
      // the amount of uniform boxes that would be made. 
      double amt = 1.0;                    
      for(d=0; d<dim; d++) {            
	// double ddom =(double)(uhgh[d] + 1);  // benign bugfix
	double ddom =(double)(uhgh[d]);
	double dval = ceil(ddom / dsplit[d]);
	if(d==bd) 
	  dval = ceil(ddom / (dsplit[d]+1.0));	
	double boxesPerEdge = ceil(ddom / dval);
	amt = amt * boxesPerEdge;
      }

      if(amt > maxAmount)             
	dmaxed[bd] = true;
      else {
	dsplit[bd] = dsplit[bd] + 1.0;  
	//dsize[bd]  = (double)(uhgh[bd]+1); // benign bugfix
	dsize[bd]  = (double)(uhgh[bd]);
	dsize[bd]  = dsize[bd] / dsplit[bd];
      }
      if(dsplit[bd] >= maxSplit[bd])  
	dmaxed[bd] = true;            
    }
  }

  // Now calculate the size of the uniform box based on
  // the splits, and calculate the total anticipated boxes.
  
  IvPBox ubox(dim,0);
  double total = 1.0;
  for(d=0; d<dim; d++) {
    //double ddom =(double)(uhgh[d] + 1); // benign bugfix
    double ddom =(double)(uhgh[d]);
    double dval = ceil(ddom / dsplit[d]);
    double boxesPerEdge = ceil(ddom / dval);
    total = total * boxesPerEdge;
    ubox.setPTS(d, 0, (int)(dval) -1);
  }

  delete [] uhgh;
  delete [] maxSplit;
  delete [] dmaxed;
  delete [] dsplit;
  delete [] dsize;

  return(ubox);
}


//-------------------------------------------------------------
// Procedure: makeRand
//   Purpose: Create a point-box with values somewhere randomly
//            inside the container box.


IvPBox makeRand(const IvPBox& container_box)
{
  IvPBox return_box = container_box;
  
  int dim = container_box.getDim();
  int max_size, low_edge, high_edge;
  for(int d=0; d<dim; d++) {
    low_edge  = container_box.pt(d, 0);
    high_edge = container_box.pt(d, 1);
    max_size  = (high_edge - low_edge) + 1;
    
    return_box.pt(d,0) = (rand() % max_size) + low_edge;
    return_box.pt(d,1) = return_box.pt(d,0);
  }

  assert(containedWithinBox(return_box, container_box));

  return(return_box);
}


//-------------------------------------------------------------
// Procedure: makeRand
//   Purpose: Create a point-box with values somewhere randomly
//            inside the given domain.

IvPBox makeRand(const IvPDomain& domain)
{
  int dim = domain.size();

  IvPBox return_box(dim);
  for(int d=0; d<dim; d++) {
    return_box.pt(d,0) = (rand() % domain.getVarPoints(d));
    return_box.pt(d,1) = return_box.pt(d,0);
  }
  return(return_box);
}

//-------------------------------------------------------------
// Procedure: subtractBox
//      Note: Return only the space outside of orig_box. The orig_box
//            is not affected at all. 
//            If (case#2 below) the sub_box does not intersect orig_box
//            then simply return a copy of orig_box as the sole component.


BoxSet* subtractBox(const IvPBox& orig_box, const IvPBox& sub_box)
{
  BoxSet *boxset = new BoxSet;

  // Error case: The boxes are not of same dimension - return empty set.
  int dim = orig_box.getDim();
  if(dim != sub_box.getDim())
    return(boxset);
  
  // Special case #1: the subtracted box fully contains the original box
  // so there is nothing remaining after subtraction - return empty set.
  if(containedWithinBox(orig_box, sub_box))
    return(boxset);                          

  // Special case #2: the subtracted box does not intersect the original
  // so the orig_box is returned in the vector.
  if(!orig_box.intersect(&sub_box)) {  
    boxset->addBox(orig_box.copy());
    return(boxset);               
  }
  
  int d;
  for(d=0; d<dim; d++) {
    if(sub_box.pt(d,1) < orig_box.pt(d,1)) {
      IvPBox *newbox = orig_box.copy();
      newbox->pt(d,0) = sub_box.pt(d,1)+1;
      boxset->addBox(newbox);
    }
    if(sub_box.pt(d,0) > orig_box.pt(d,0)) {
      IvPBox *newbox = orig_box.copy();
      newbox->pt(d,1) = sub_box.pt(d,0)-1;
      boxset->addBox(newbox);
    }
  }
  return(boxset);
}

//-------------------------------------------------------------
// Procedure: containedWithin
//   Purpose: o Determine if all points in tbox are also in gbox.
//            o Returns either true or false;

bool containedWithinBox(const IvPBox& tbox, const IvPBox &gbox)
{
  int dim = tbox.getDim();
  assert(dim == gbox.getDim());
  for(int d=0; d<dim; d++) {
    if(tbox.pt(d,0) < gbox.pt(d,0))
      return(false);
    if(tbox.pt(d, 1) > gbox.pt(d, 1))
      return(false);

    if(tbox.pt(d, 0) == gbox.pt(d, 0))    // Handle situations
      if(gbox.bd(d, 0) == 0)               // where the given
	if(tbox.bd(d, 0) == 1)             // box may have
	  return(false);                    // non-inclusive edges.
    if(tbox.pt(d, 1) == gbox.pt(d, 1))
      if(gbox.bd(d, 1) == 0)
	if(tbox.bd(d, 1) == 1)
	  return(false);
  }
  return(true);
}

//---------------------------------------------------------------
// Procdudure: cutBox
//    Purpose: Split off a new box from oldbox. Return new box.
//      Notes: Nothing is done with the WEIGHT of either box.
//    Returns: A new box, or nullbox if cannot be split. Box cannot
//             be split if demanded side has an edge length of one.

IvPBox* cutBox(IvPBox* oldbox, int dim)
{
  int split_length = (oldbox->pt(dim,1) - oldbox->pt(dim,0)) + 1;
  if(split_length <= 1)
    return(0);

  IvPBox* newbox = oldbox->copy();

  int  newHigh = oldbox->pt(dim, 0);

  newHigh += (int)((split_length+1)/2);

  oldbox->pt(dim, 1) = newHigh-1;
  newbox->pt(dim, 0) = newHigh;

  oldbox->bd(dim, 1) = 1;
  newbox->bd(dim, 0) = 1;

  return(newbox);
}


//---------------------------------------------------------------
// Procdudure: intersectDomain
//    Purpose: Return true if the two domains share one or more
//             common domain variables. ONLY check for variable
//             name. No regard to match on variable range or pts.

bool intersectDomain(const IvPDomain& dom1, const IvPDomain& dom2)
{
  int size1 = dom1.size();
  for(int i=0; i<size1; i++)
    if(dom1.hasDomain(dom2.getVarName(i)))
      return(true);
  return(false);
}

  
  
//---------------------------------------------------------------
// Procdudure: unionDomain
//    Purpose: Return a new IvPDomain reflecting the union of the
//             two given domains. If there are variables in common
//             they are ignored. No check is made for the case 
//             where domain variables are in both domains but 
//             disagree on range and number of points.

IvPDomain unionDomain(const IvPDomain& dom1, const IvPDomain& dom2)
{
  IvPDomain new_domain = dom1;

  int dsize = dom2.size();
  for(int i=0; i<dsize; i++) {
    string varname = dom2.getVarName(i);
    double varlow  = dom2.getVarLow(i);
    double varhigh = dom2.getVarHigh(i);
    int    varpts  = dom2.getVarPoints(i);
    new_domain.addDomain(varname, varlow, varhigh, varpts);
  }
  return(new_domain);
}

//---------------------------------------------------------------
// Procdudure: subDomain
//    Purpose: Return an IvPDomain with only the individual vars
//             specified.
//
//    example: new_domain = subDomain(old_domain, "speed,depth");

IvPDomain subDomain(const IvPDomain& domain, string vars)
{
  vector<string> svector = parseString(vars, ',');
  int vsize = svector.size();

  IvPDomain subdomain;

  int i;

  // Check that given domain has all given domain variables.
  for(i=0; i<vsize; i++) {
    if(!domain.hasDomain(svector[i]))
      return(subdomain);
  }
  
  for(i=0; i<vsize; i++)
    subdomain.addDomain(domain, svector[i]);
  
  return(subdomain);
}

//--------------------------------------------------------------
// Procedure: stringToDomain
//    Format: "X,0,100,101 : Y,0,1,11"

IvPDomain stringToDomain(const string& domain_descriptor)
{
  IvPDomain null_domain; // return this is something goes wrong
  IvPDomain good_domain; // build up this and return if all OK

  vector<string> svector = parseString(domain_descriptor, ':');
  int vsize = svector.size();
  if(vsize == 0)
    return(null_domain);
  for(int i=0; i<vsize; i++) {
    string dstr = stripBlankEnds(svector[i]);
    vector<string> svector2 = parseString(dstr, ',');
    int vsize2 = svector2.size();
    if(vsize2 != 4)
      return(null_domain);
    string dname   = svector2[0];
    string dlow    = svector2[1];
    string dhigh   = svector2[2];
    string dpoints = svector2[3];
    double f_dlow    = atof(dlow.c_str());
    double f_dhigh   = atof(dhigh.c_str());
    int    i_dpoints = atoi(dpoints.c_str());

    bool ok = good_domain.addDomain(dname.c_str(), f_dlow, f_dhigh, i_dpoints);
    if(!ok)
      return(null_domain);
  }
  return(good_domain);
}

//--------------------------------------------------------------
// Procedure: boxToDomain
//    Format: "X:0:100:101,Y:0:1:11"

IvPDomain boxToDomain(const IvPBox& box)
{
  IvPDomain null_domain; // return this is something goes wrong
  IvPDomain good_domain; // build up this and return if all OK

  int vsize = box.getDim();
  if(vsize == 0)
    return(null_domain);
  for(int i=0; i<vsize; i++) {
    string dname   = intToString(i);
    double f_dlow    = (double)(box.pt(i,0));
    double f_dhigh   = (double)(box.pt(i,1));
    int    i_dpoints = (box.pt(i,1)-box.pt(i,0))+1;

    bool ok = good_domain.addDomain(dname.c_str(), f_dlow, f_dhigh, i_dpoints);
    if(!ok)
      return(null_domain);
  }
  return(good_domain);
}

//--------------------------------------------------------------
// Procedure: domainToString
//   example: "x,0,200,201:y,0,10,11:z,20,25,6"

string domainToString(const IvPDomain& domain)
{
  string return_string;

  int dcount = domain.size();
  for(int i=0; i<dcount; i++) {
    return_string += domain.getVarName(i);
    return_string += ",";
    return_string += dstringCompact(doubleToString(domain.getVarLow(i)));
    return_string += ",";
    return_string += dstringCompact(doubleToString(domain.getVarHigh(i)));
    return_string += ",";
    return_string += intToString(domain.getVarPoints(i));
    if(i < dcount-1)
      return_string += ":";
  }
  return(return_string);
}


//---------------------------------------------------------------
// Procdudure: domainToBox
//    Purpose: Return an IvPBox that has boundaries/intervals 
//             equivalent to the universe representation of the 
//             IvP Domain.

IvPBox domainToBox(const IvPDomain& domain)
{
  int dim = domain.size();

  IvPBox  newbox(dim);
  for(int i=0; i<dim; i++)
    newbox.setPTS(i, 0, domain.getVarPoints(i)-1);

  return(newbox);
}

//--------------------------------------------------------------
// Procedure: stringToBox

IvPBox stringToBox(const string& str)
{
  return(domainToBox(stringToDomain(str)));
}

//------------------------------------------------------------------
// Procedure: makeUniformDistro

BoxSet* makeUniformDistro(const IvPBox& outer_box, 
			  const IvPBox& unif_box, 
			  int degree)
{
  BoxSet *boxset = new BoxSet;

  // Error case: The boxes are not of the same dimension - return empty.
  int dim = outer_box.getDim();
  if(dim != unif_box.getDim())
    return(boxset);
  
  int d;
  // Store the size of the outer_box for easy access later.
  int  *uhgh = new int[dim];     
  int  *ulow = new int[dim];     
  int  *uval = new int[dim];
  for(d=0; d<dim; d++) {
    ulow[d] = outer_box.pt(d,0);
    uhgh[d] = outer_box.pt(d,1);
    uval[d] = unif_box.pt(d, 1) + 1;
  }

  // Next, determine the total number of boxes to be made
  // based upon the size of the given uniform box.
  int dimVal, remVal;
  int unifPieces = 1;                
  for(d=0; d<dim; d++) {             
    dimVal = (uhgh[d]-ulow[d]+1) / uval[d];  
    remVal = (uhgh[d]-ulow[d]+1) % uval[d];  
    if(remVal > 0) dimVal++;
    unifPieces = unifPieces * dimVal;
  }
  
  int  currix = 0;
  bool unif_done = false;
  while(!unif_done) {
    IvPBox* newbox = new IvPBox(dim, degree);
    for(d=0; d<dim; d++)
      newbox->setPTS(d, ulow[d], min((ulow[d]+uval[d]-1), uhgh[d]));
    boxset->addBox(newbox);
    currix++;
    
    unif_done = true;
    for(d=0; d<dim; d++)
      unif_done = unif_done && ((ulow[d]+uval[d]) > uhgh[d]);
    
    if(!unif_done) {
      int next = false;
      for(d=0; (d<dim)&&(!next); d++) {
	ulow[d] += (uval[d]);
	if(ulow[d] <= uhgh[d])
	  next = true;
	else
	  ulow[d] = outer_box.pt(d,0);
      }
    }
  }
  
  delete [] uhgh;   // Now free up the memory in all those
  delete [] ulow;   // temporary 'convenience' arrays.
  delete [] uval;

  return(boxset);
}


//-------------------------------------------------------------
// Procedure: stringToPointBox
//
//   Purpose: There are two other functions for creating a PointBox
//            from a string discription. One assumes the extents
//            are given in discrete units, the other in float units.
//            They are:
//              (1) stringDiscreteToPointBox(), and
//              (2) stringFloatToPointBox().
//            This function handles a call where the type of extent
//            is declared in the string itself with a pre-cursor tag. 
//            Examples:
//               "discrete @ x:4, y:5"
//               "float @ x:4.2, y:5.4"

IvPBox stringToPointBox(const string& given_str, 
			const IvPDomain& domain, 
			const char gsep, const char lsep)
{
  IvPBox null_box;

  vector<string> svector = parseString(given_str, '@');
  int vsize = svector.size();

  // If string was empty or more then two separators found
  if((vsize == 0) || (vsize > 2))
    return(null_box);
  
  // If no separator, then no preface, and assume float extents
  if(vsize == 1)
    return(stringFloatToPointBox(given_str, domain, gsep, lsep));

  // Otherwise, a single separator gives a preface and remainder
  string preface = tolower(stripBlankEnds(svector[0]));
  string remainder = stripBlankEnds(svector[1]);

  if(preface == "discrete") 
    return(stringDiscreteToPointBox(remainder, domain, gsep, lsep));
  else if(preface == "float") 
    return(stringFloatToPointBox(remainder, domain, gsep, lsep));
  else
    return(null_box);
}


//-------------------------------------------------------------
// Procedure: stringDiscreteToPointBox
//
//    - Process a string of the form "x=10,y=20" and return an IvPBox
//      with the corresponding extents. 
//    - Also examine the given IvPDomain and ensure those variables 
//      exist, and that domain.
//    - Re-order the dimensions of the requested box to match the 
//      domain.
//    - gsep is the "global separator", the character that separates 
//      the outer chunks.
//    - lsep is the "local separator", the character that separates
//      the innner fields.  
//  Example:
//      IvPDomain: x:0:20:21, y:5:10:6
//      String: "y:2, x:9" 
//      Resulting Box: dim0:8:8, dim1:1:1


IvPBox stringDiscreteToPointBox(const string& given_str, 
				const IvPDomain& domain, 
				const char gsep, const char lsep)
{
  IvPBox null_box;

  if(given_str == "")
    return(null_box);

  int i, dim;
  dim = domain.size();

  vector<int> extents;

  for(i=0; i<dim; i++) {
    string varname = domain.getVarName(i);
    string strval;
    bool ok = tokParse(given_str, varname, gsep, lsep, strval);
    strval = stripBlankEnds(strval);
    if(!ok || !isNumber(strval))
      return(null_box);

    int extent = atoi(strval.c_str());
    if(extent <= 0)
      return(null_box);

    int varpts = domain.getVarPoints(i);
    if(extent > varpts)
      extent = varpts;
    extents.push_back(extent);
  }
  
  // All is good, so go ahead and create the IvP Box.
  IvPBox ret_box(dim);
  for(i=0; i<dim; i++) {
    ret_box.pt(i,0) = extents[i] - 1;
    ret_box.pt(i,1) = extents[i] - 1;
  }
  
  return(ret_box);
}

//-------------------------------------------------------------
// Procedure: stringFloatToPointBox
//
// Purpose: This procedure takes a given IvPDomain and string which
//          specifies a single float values for a set of domain
//          names. The objective is to create an IvPBox point-box, 
//          with the discrete point being the closest to the float
//          value.
// Example:
//      IvPDomain: x:0:20:21, y:5:10:6
//      String: "y:7.4, x:12.85" 
//      Resulting Box: dim0:14:14, dim1:4:4
// 
// Notes: The float value can be replace with the string "all" which
//        will be interpreted as the extreme high float value
//        associated with that variable as specified by the IvPDomain.
// Example:
//      IvPDomain: x:0:20:21, y:5:10:6
//      String: "y:7.4, x:all" 
//      Resulting Box: dim0:20:20, dim1:4:4
//
// Notes: All floats not exactly on a discrete IvPDomain point are 
//        rounded to the nearest discrete point.
//
// Notes: The resulting IvPBox will have the implied domain ordering
//          given by the IvPDomain, not the ordering of string tuples.
//        An error will return a "null_box" or an IvPBox of dimension
//          zero. This can be tested by "bool ok = result_box.null();"
//
// Notes: Some pseudo-errors are not checked for and thus allowed.
//        1. If the domain name tuple is specified more than once
//           in the given string, only the first tuple is used, the
//           others are ignored.
//        2. If extra domain name tuples are specified in the string,
//           unknown to the IvPDomain, they are simply ignored.
//        3. If a float value is specified that is lower or higher 
//           than the IvPDomain extreme values, it is clipped to 
//           the corresponding extreme value.
//        4. Domain name matching is case insensitive.
//
// Notes: Things that *will* create an error:
//        1. If a IvPDomain variable is unspecified in the string


IvPBox stringFloatToPointBox(const string& given_str, 
			     const IvPDomain& domain, 
			     const char gsep, const char lsep)
{
  IvPBox null_box;
  if(given_str == "")
    return(null_box);

  int i, j, k, dim;
  dim = domain.size();

  vector<string> dvar_name;
  vector<bool>   dvar_legal;
  vector<int>    dvar_discrete_val;
  vector<double> dvar_float_val;

  // For all the variables in the IvP domain, check that the 
  //   variable is specified in the given string. 
  // For each variable in the IvP domain, create a record:
  //           dvar_name[i] is the ith IvPDomain variable
  //          dvar_legal[i] is true if var also present in the string
  //  dvar_discrete_high[i] is eventual box bound, zero for this pass
  //      dvar_float_val[i] is the float value given by the string

  for(i=0; i<dim; i++) {
    // Initialize the above record for this variable name. 
    // The val_low and val_high initial values are initialized to the
    // max boundaries of the domain. 
    dvar_name.push_back(tolower(domain.getVarName(i)));
    dvar_legal.push_back(false);
    dvar_discrete_val.push_back(0);
    dvar_float_val.push_back(0);

    vector<string> svector = parseString(given_str, gsep);
    int vsize = svector.size();
    for(j=0; j<vsize; j++) {
      svector[j] = stripBlankEnds(svector[j]);
      vector<string> svector2 = parseString(svector[j], lsep);
      int vsize2 = svector2.size();
      for(k=0; k<vsize2; k++)
	svector2[k] = tolower(stripBlankEnds(svector2[k]));

      // svector2 example [0]"x" [1]"2.3"
      //              or  [0]"x" [1]"all"
      if((vsize2 == 2) && (svector2[0] == dvar_name[i])) {
	if(svector2[1] == "all") {
	  dvar_legal[i] = true;
	  dvar_float_val[i] = domain.getVarHigh(i);
	}
	else if(isNumber(svector2[1])) {
	  dvar_legal[i] = true;
	  double fval = atof(svector2[1].c_str());
	  // Check the fval wrt domain bound and modify if necessary
	  double dom_low_i  = domain.getVarLow(i);
	  double dom_high_i = domain.getVarHigh(i);
	  if(fval > dom_high_i)
	    fval = dom_high_i;
	  else if(fval < dom_low_i)
	    fval = dom_low_i;
	  dvar_float_val[i] = fval;
	}
      }  
    }
  }

#if 0 // for Debugging/Validation
  cout << endl << endl;
  for(i=0; i<dim; i++) {
    cout << "dvar_name["      << i << "]: [" << dvar_name[i] << "]" << endl;
    cout << "dvar_legal["     << i << "]: "  << dvar_legal[i] << endl;
    cout << "dvar_float_val[" << i << "]: "  << dvar_float_val[i] << endl;
    cout << "dvar_discrete_val[" << i << "]: " 
	 << dvar_discrete_val[i] << endl << endl;
  }
#endif

  // If any one of the variables in the IvPDomain were not legally
  // specified in one of the elements of the string, return null_box
  for(i=0; i<dim; i++)
    if(!dvar_legal[i])
      return(null_box);
  
  // Convert the raw float values into Domain discrete indices.
  for(i=0; i<dim; i++)
    dvar_discrete_val[i] = domain.getDiscreteVal(i, dvar_float_val[i], 2); 
  
  // All is good, so go ahead and create the IvP Box.
  IvPBox ret_box(dim);
  for(i=0; i<dim; i++) {
    ret_box.pt(i,0) = dvar_discrete_val[i];
    ret_box.pt(i,1) = dvar_discrete_val[i];
  }
  return(ret_box);
}

//-------------------------------------------------------------
// Procedure: stringToRegionBox
//
// Purpose: This procedure takes a given IvPDomain and string which
//          specifies low and high float values for a set of domain
//          names. The objective is to create an IvPBox which also
//          has low and high values, but in the standard form of 
//          discrete indices of the IvPDomain.
// Example:
//      IvPDomain: x:0:20:21, y:5:10:6
//      String: "y:7.5:9, x:10:12.5" 
//      Resulting Box: dim0:10:13, dim1:1:4
// 
// Notes: A low-high pair can be replace with the string "all" which
//        will be interpreted as the extreme low and high float value
//        associated with that variable as specified by the IvPDomain.
// Example:
//      IvPDomain: x:0:20:21, y:5:10:6
//      String: "y:7.5:9, x:all" 
//      Resulting Box: dim0:0:20, dim1:1:4
//
// Notes: A lower bound float not exactly on a discrete IvPDomain 
//          point is rounded down to the next lowest discrete point.
//        An upper bound float not exactly on a discrete IvPDomain 
//          point is rounded up to the next highest discrete point.
//
// Notes: The resulting IvPBox will have the implied domain ordering
//          given by the IvPDomain, not the ordering of string tuples.
//        An error will return a "null_box" or an IvPBox of dimension
//          zero. This can be tested by "bool ok = result_box.null();"
//
// Notes: Some pseudo-errors are not checked for and thus allowed.
//        1. If the domain name tuple is specified more than once
//           in the given string, only the first tuple is used, the
//           others are ignored.
//        2. If extra domain name tuples are specified in the string,
//           unknown to the IvPDomain, they are simply ignored.
//        3. If low or high double values are specified that are 
//           lower or higher than the IvPDomain extreme values, they
//           are simply clipped to those extreme values.
//        4. Domain name matching is case insensitive.
//
// Notes: Things that *will* create an error:
//        1. For any tuple, if low value is greater than high value.
//        2. If a IvPDomain variable is unspecified in the string


IvPBox stringFloatToRegionBox(const string& given_str, 
			      const IvPDomain& domain, 
			      const char gsep, const char lsep)
{
  IvPBox null_box;
  if(given_str == "")
    return(null_box);

  int i, j, k, dim;
  dim = domain.size();

  vector<string> dvar_name;
  vector<bool>   dvar_legal;
  vector<int>    dvar_box_low;
  vector<int>    dvar_box_high;
  vector<double> dvar_val_low;
  vector<double> dvar_val_high;

  // For all the variables in the IvP domain, check that the 
  //   variable is specified in the given string. 
  // For each variable in the IvP domain, create a record:
  //       dvar_name[i] is the ith IvPDomain variable
  //      dvar_legal[i] is true if var also present in the string
  //    dvar_box_low[i] is eventual box lower bound, zero for this pass
  //   dvar_box_high[i] is eventual box upper bound, zero for this pas
  //    dvar_val_low[i] is the float lower bound given by the string
  //   dvar_val_high[i] is the float upper bound given by the string

  for(i=0; i<dim; i++) {
    // Initialize the above record for this variable name. 
    // The val_low and val_high initial values are initialized to the
    // max boundaries of the domain. 
    dvar_name.push_back(tolower(domain.getVarName(i)));
    dvar_legal.push_back(false);
    dvar_box_low.push_back(0);
    dvar_box_high.push_back(0);
    dvar_val_low.push_back(domain.getVarLow(i));
    dvar_val_high.push_back(domain.getVarHigh(i));

    vector<string> svector = parseString(given_str, gsep);
    int vsize = svector.size();
    for(j=0; j<vsize; j++) {
      svector[j] = stripBlankEnds(svector[j]);
      vector<string> svector2 = parseString(svector[j], lsep);
      int vsize2 = svector2.size();
      for(k=0; k<vsize2; k++)
	svector2[k] = tolower(stripBlankEnds(svector2[k]));

      // svector2 example [0]"x" [1]"2.3" [2]"5.0"
      //              or  [0]"x" [1]"all"
      if((vsize2 > 0) && (svector2[0] == dvar_name[i])) {
	if((vsize2 == 2) && (svector2[1] == "all"))
	  dvar_legal[i] = true;
	else if(vsize2 == 3) {
	  if(isNumber(svector2[1]) && isNumber(svector2[2])) {
	    double lval = atof(svector2[1].c_str());
	    double hval = atof(svector2[2].c_str());

	    if(lval <= hval) {
	      dvar_legal[i] = true;
	      // Check the lval and modify if necessary
	      // Assumes dvar_val_low[i] has been init to extreme low
	      if(lval > dvar_val_high[i])
		dvar_val_low[i] = dvar_val_high[i];
	      else
		dvar_val_low[i] = lval;

	      // Check the hval and modify if necessary
	      // Assumes dvar_val_high[i] has been init to extreme high
	      if(hval < dvar_val_low[i])
		dvar_val_high[i] = dvar_val_low[i];
	      else
		dvar_val_high[i] = hval;
	    }
	    else 
	      cout << "Bad 77" << endl;
	  }
	  else 
	    cout << "Bad 88" << endl;
	}
	else
	  cout << "badd 66" << endl;
      }  
    }
  }

#if 0 // for Debugging/Validation
  cout << endl << endl;
  for(i=0; i<dim; i++) {
    cout << "dvar_name["     << i << "]: [" << dvar_name[i] << "]" << endl;
    cout << "dvar_legal["    << i << "]: " << dvar_legal[i] << endl;
    cout << "dvar_box_low["  << i << "]: " << dvar_box_low[i] << endl;
    cout << "dvar_box_high[" << i << "]: " << dvar_box_high[i] << endl;
    cout << "dvar_val_low["  << i << "]: " << dvar_val_low[i] << endl;
    cout << "dvar_val_high[" << i << "]: " << dvar_val_high[i] << endl << endl;
  }
#endif

  // If any one of the variables in the IvPDomain were not legally
  // specified in one of the elements of the string, return null_box
  for(i=0; i<dim; i++)
    if(!dvar_legal[i])
      return(null_box);
  
  // Convert high and low raw double values into Domain discrete indices.
  for(i=0; i<dim; i++) {
    dvar_box_low[i]  = domain.getDiscreteVal(i, dvar_val_low[i], true); 
    dvar_box_high[i] = domain.getDiscreteVal(i, dvar_val_high[i], false); 
  }
  
    // All is good, so go ahead and create the IvP Box.
  IvPBox ret_box(dim);
  for(i=0; i<dim; i++) {
    ret_box.pt(i,0) = dvar_box_low[i];
    ret_box.pt(i,1) = dvar_box_high[i];
  }
  return(ret_box);
}
