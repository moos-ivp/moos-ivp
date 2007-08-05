/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: FunctionEncoder.h                                    */
/*    DATE: April 17th 2006 (Patriots Day)                       */
/*****************************************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoder.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: StringToIvPFunction
//   Purpose: 

IvPFunction *StringToIvPFunctionTest(const string& str)
{
  int i, index = 0;

  vector<string> svector = parseString(str, ',');

  if(svector[0] != "H") {
    return(0);
  }
  int dim = atoi(svector[1].c_str());
  int pcs = atoi(svector[2].c_str());
  int deg = atoi(svector[3].c_str());
  double pwt = atof(svector[4].c_str());

  // Calculate length of a box entry based on dimension, degree.
  int wtc = (deg*dim)+1;
  int box_len = (2*dim) + wtc;

  index = 5;
  if(svector[index] != "D") {
    return(0);
  }
  
  string domain_str = findReplace(svector[6], ';', ',');
  IvPDomain domain = stringToDomain(domain_str);

  index = 7;
  if(svector[index] != "G")  {
    return(0);
  }

  index += 1;
  IvPBox gelbox(dim);
  for(i=0; i<dim; i++) {
    int val = atoi(svector[index+i].c_str());
    gelbox.setPTS(i, 0, val);
  }

  index += dim;
  if(svector[index] != "F")  {
    return(0);
  }

  string low_str, hgh_str;
  int    low_val, hgh_val, d, local_index;

  index += 1;
  PDMap *pdmap = new PDMap(pcs, domain, deg);
  for(i=0; i<pcs; i++) {
    IvPBox *newbox = new IvPBox(dim,deg);
    for(d=0; d<dim; d++) {
      local_index = index  + (d*2);
      low_str = svector[local_index];
      hgh_str = svector[local_index+1];
      if(low_str[0] == 'X') {
	newbox->bd(d,0) = 0;
	low_str[0] = '+';
      }
      if(hgh_str[0] == 'X') {
	newbox->bd(d,1) = 0;
	hgh_str[0] = '+';
      }
      low_val = atoi(low_str.c_str());
      hgh_val = atoi(hgh_str.c_str());
      newbox->setPTS(d, low_val, hgh_val);
    }  
    index += 2*dim;
    for(d=0; d<wtc; d++) {
      double wt_val = atof(svector[index+d].c_str());
      newbox->wt(d) = wt_val;
    }
    index += wtc;
    pdmap->bx(i) = newbox;
  }
  pdmap->setGelBox(gelbox);
  pdmap->updateGrid(1,1);

  IvPFunction *new_of = new IvPFunction(pdmap);
  new_of->setPWT(pwt);
  
  return(new_of);
}

//--------------------------------------------------------------
// Procedure: doubleToString2
//   Purpose: 

string doubleToString2(double dval)
{
  long int rval, ival = (long int)((dval * 10000)+0.5);

  char buff[100];

  const int dp = 4;

  char rbuff[100];
  
  int c_ix = 0;
  int b_ix = -1;
  int e_ix = 0;


  // For input 12345.6789  and 12.3

  // [0]  [1]  [2]  [3] . [4]  [5]  [6]  [7]  [8]  [9]  
  // 
  //  9    8    7    6  .  5    4    3    2    1    0    e_ix = 8
  //  
  //  0    0    0    3   . 2    1    0                   e_ix = 5

  bool done = false;
  while(!done) {
    rval = ival % 10;
    ival = (long int)(ival/10);
    if((ival == 0) && (c_ix >= dp))
      done = true;
    buff[c_ix] = (char)(48+rval);
    if((b_ix==-1) && (rval!=0)) {
      b_ix = c_ix;
    }
    c_ix++;
  }
  e_ix = c_ix-1;

  if(b_ix==-1) b_ix=0;

  c_ix = 0; 
  for(int i=e_ix; i>=b_ix; i--) {
    rbuff[c_ix] = buff[i];
    if(i == dp) {
      rbuff[c_ix+1] = '.';
      c_ix++;
    }
    c_ix++;
  }
  rbuff[c_ix] = '\0';

  return(rbuff);
}


//--------------------------------------------------------------
// Procedure: intToString2
//   Purpose: 

string intToString2(int ival)
{
  if(ival==0)
    return("0");

  char buff[100];

  char rbuff[100];
  
  int c_ix = 0;
  int e_ix = 0;


  // For input 12345  and 120

  // [0]  [1]  [2]  [3]  [4]  [5]  [6] 
  // 
  //  5    4    3    2    1             e_ix = 48
  //  
  //  0    2    1                       e_ix = 2

  int  rval;
  while(ival != 0) {
    rval = ival % 10;
    ival = (long int)(ival/10);
    buff[c_ix] = (char)(48+rval);
    c_ix++;
  }
  e_ix = c_ix-1;

  c_ix = 0; 
  for(int i=e_ix; i>=0; i--) {
    rbuff[c_ix] = buff[i];
    c_ix++;
  }
  rbuff[c_ix] = '\0';

  return(rbuff);
}
