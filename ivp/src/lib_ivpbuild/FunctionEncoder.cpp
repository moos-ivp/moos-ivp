/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FunctionEncoder.h                                    */
/*    DATE: April 17th 2006 (Patriots Day)                       */
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
#include <stdio.h>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoder.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: IvPFunctionToString
//      Note: cstr is short for context_string
//
// H,cstr_len,cstr,dim,pcs,deg,pwt,
// D,course;0;359;360:speed;0;8;9,G,9,4,
// G,10,10,10,
// F,0,40,0,50,0.1,0.2,0.3,

string IvPFunctionToString(IvPFunction *ivp_function)
{
  PDMap *pdmap = ivp_function->getPDMap();

  if(!pdmap) 
    return("");

  int amt = 0;
  int len = 0;

  int dim = ivp_function->getDim();
  int pcs = pdmap->size();
  int deg = pdmap->getDegree();
  int wtc = pdmap->bx(0)->getWtc();
  double pwt  = ivp_function->getPWT();
  string cstr = ivp_function->getContextStr();
  int cstr_len = cstr.length();

  int bsize = pcs * (((dim*2)+wtc)*50);
  bsize += cstr_len;

  char *buff = new char[bsize];

  IvPBox gelbox = pdmap->getGelBox();

  string pwt_str = dstringCompact(doubleToString(pwt));
  amt = sprintf(buff+len, "H,%d,%s,%d,%d,%d,%s,D,", 
		cstr_len, cstr.c_str(),
		dim,pcs,deg,pwt_str.c_str());
  len += amt;

  string domain_str = domainToString(pdmap->getDomain());
  domain_str = findReplace(domain_str, ',', ';');
  amt = sprintf(buff+len, "%s,", domain_str.c_str());
  len += amt;

  buff[len] = 'G';
  buff[len+1] = ',';
  len += 2;
  for(int d=0; d<dim; d++) {
    amt = sprintf(buff+len, "%d,", gelbox.pt(d, 1));
    len += amt;
  }

  // A 2D Box with linear function (three weights) has seven fields
  //  
  //    Number of elements is ((2*DIM+WTC)*PIECES) where WTC is the
  //    weight count which is ((DEG*DIM)+1)
  //
  //    F,1,2,3,4,5,6,7,
  //
  //    F,0,100,0,100,0.5,0.6,0.7,

  
  buff[len] = 'F';
  buff[len+1] = ',';
  len += 2;
  for(int i=0; i<pcs; i++) {
    IvPBox *ibox = pdmap->bx(i);
    //cout << "Box [" << i  << "]: " << endl;
    //ibox->print();


    for(int d=0; d<dim; d++) {
      if(ibox->bd(d,0)==0) {
	buff[len] = 'X';
	len++;
      }

      // BEGIN INLINE INT-TO-STRING CODE  ----------------
      int ival = ibox->pt(d,0);
      if(ival<10) {
	buff[len] = (char)(48+ival); len++;
	buff[len] = ','; len++;
      }
      else {
	char tbuff[100];
	int c_ix = 0;
	int e_ix = 0;	
	// For input 12345  and 120
	// [0]  [1]  [2]  [3]  [4]  [5]  [6] 
	//  5    4    3    2    1             e_ix = 48
	int  rval;
	while(ival != 0) {
	  rval = ival % 10;
	  ival = (long int)(ival/10);
	  tbuff[c_ix] = (char)(48+rval);
	  c_ix++;
	}
	e_ix = c_ix-1;
	c_ix = 0; 
	for(int i=e_ix; i>=0; i--) {
	  buff[len+c_ix] = tbuff[i];
	  c_ix++;
	}
	len += c_ix;
	buff[len] = ','; len++;
      }
      // END  INLINE INT-TO-STRING CODE  ----------------
      if(ibox->bd(d,1)==0) {
	buff[len] = 'X';
	len++;
      }
      // BEGIN INLINE INT-TO-STRING CODE  ----------------
      ival = ibox->pt(d,1);
      if(ival==0) {
	buff[len] = (char)(48+ival); len++;
	buff[len] = ','; len++;
      }
      else {
	char tbuff[100];
	int c_ix = 0;
	int e_ix = 0;	
	// For input 12345  and 120
	// [0]  [1]  [2]  [3]  [4]  [5]  [6] 
	//  5    4    3    2    1             e_ix = 48
	int  rval;
	while(ival != 0) {
	  rval = ival % 10;
	  ival = (long int)(ival/10);
	  tbuff[c_ix] = (char)(48+rval);
	  c_ix++;
	}
	e_ix = c_ix-1;
	c_ix = 0; 
	for(int i=e_ix; i>=0; i--) {
	  buff[len+c_ix] = tbuff[i];
	  c_ix++;
	}
	len += c_ix;
	buff[len] = ','; len++;
      }
    }

    for(int j=0; j<wtc; j++) {
      // BEGIN DOUBLE-TO-STRING INLINE CODE --------------------
      double jwt = ibox->wt(j);

      if(jwt<0) {
	buff[len] = '-';
	len++;
	jwt *= -1;
      }
      if(jwt==0) {
	buff[len] = '0';
	len++;
      }
      else {
	long int rval, ival = (long int)((jwt * 10000)+0.5);
	char tbuff[100];
	const int dp = 4;
	int c_ix = 0;
	int b_ix = -1;
	int e_ix = 0;
	
	bool done = false;
	while(!done) {
	  rval = ival % 10;
	  ival = (long int)(ival/10);
	  if((ival == 0) && (c_ix >= dp))
	    done = true;
	  tbuff[c_ix] = (char)(48+rval);
	  if((b_ix==-1) && (rval!=0)) {
	    b_ix = c_ix;
	  }
	  c_ix++;
	}
	e_ix = c_ix-1;
	
	if(b_ix==-1) b_ix=0;
	if(b_ix>dp) b_ix=dp;

	c_ix = 0; 
	for(int k=e_ix; k>=b_ix; k--) {
	  buff[len+c_ix] = tbuff[k];
	  if(k == dp) {
	    buff[len+c_ix+1] = '.';
	    c_ix++;
	  }
	  c_ix++;
	}
	len += c_ix;
      }
      // END DOUBLE-TO-STRING INLINE CODE --------------------
      if(i<(pcs-1) || (j<(wtc-1))) {
	buff[len] = ',';
	len++;
      }
    }
  }
  buff[len] = '\0';

  string return_string = buff;
  delete [] buff;
  return(return_string);
}

//--------------------------------------------------------------
// Procedure: IvPFunctionToVector
//   Purpose: 
//
//
//  "P,Function-ID,Total-Packets,Packet-Index,0,200,1,800,3.002"

vector<string> IvPFunctionToVector(const string& whole_string,
				   const string& function_id,
				   int packet_size)
{
  vector<string> svector;

  int  whole_size  = whole_string.length();
  int  header_size = function_id.length() + 50;
  int  body_size   = packet_size - header_size;
  int  num_packets = ((int)(whole_size / body_size)) + 1;

#if 0
  cout << "whole_string: " << whole_string  << endl;
  cout << "whole_size:   " << whole_size    << endl;
  cout << "header_size:  " << header_size   << endl;
  cout << "body_size:    " << body_size     << endl;
  cout << "num_packets: " << num_packets << endl;
#endif

  char *buff = new char[packet_size];
  int a_ix = sprintf(buff, "P,%s,%d,", function_id.c_str(), num_packets);
  
  for(int i=0; i<num_packets; i++) {
    int aa_ix = sprintf(buff+a_ix, "%d,", i+1);
    const char *whole_buff = whole_string.c_str();
    int src_ix  = (i * body_size) + 0;
    int dest_ix = aa_ix + a_ix + 0;
    for(int j=0; ((j<body_size)&&(src_ix <whole_size)); j++) {
      buff[dest_ix] = whole_buff[src_ix];
      dest_ix++;
      src_ix++;
    }
    buff[dest_ix] = '\0';
    //buff[a_ix + aa_ix + body_size] = '\0';
    svector.push_back(buff);
  }
  delete [] buff;
  return(svector);
}



//--------------------------------------------------------------
// Procedure: StringToIvPFunction
//   Purpose: 

IvPFunction *StringToIvPFunction(const string& str)
{
  int d, i;

  int cix = 2; // To account for the H, in the header

  // Determine the length of the context string
  int cstr_len = 0;
  while(str[cix] != ',') {
    cstr_len = cstr_len * 10;
    cstr_len += (int)(str[cix]-48);
    cix++;
  }
  cix++;

  // Determine the context string, if any
  char *cstr_buff = new char[cstr_len+10];
  int  cbix = 0;
  while(str[cix] != ',') {
    cstr_buff[cbix] = str[cix];
    cbix++;
    cix++;
  }
  cstr_buff[cbix] = '\0';
  cix++;

  // Determine the number of dimensions
  int dim = 0;
  while(str[cix] != ',') {
    dim = dim * 10;
    dim += (int)(str[cix]-48);
    cix++;
  }
  cix++;

  // Determine the number of pieces
  int pcs = 0;
  while(str[cix] != ',') {
    pcs = pcs * 10;
    pcs += (int)(str[cix]-48);
    cix++;
  }
  cix++;
   
  // Determine the degree
  int deg = 0;
  while(str[cix] != ',') {
    deg = deg * 10;
    deg += (int)(str[cix]-48);
    cix++;
  }
  cix++;
   
  // Determine the priority weight
  double pwt  = 0.0;
  double frac = 0.1;
  bool   left_of_decimal = true;
  while(str[cix] != ',') {
    if(str[cix] == '.') {
      left_of_decimal = false;
    }
    else {
      if(left_of_decimal) {
	pwt = pwt * 10;
	pwt += (double)(str[cix]-48);
      }
      else {
	pwt += ((double)(str[cix]-48)) * frac;
	frac = frac / 10.0;
      }
    }
    cix++;
  }
  cix++;
   
  // Determine the domain
  cix += 2;
  char buff[5000];
  int  bix = 0;
  while(str[cix] != ',') {
    buff[bix] = str[cix];
    if(buff[bix] == ';')
      buff[bix] = ',';
    bix++;
    cix++;
  }
  buff[bix] = '\0';
  cix++;

  IvPDomain domain = stringToDomain(buff);

  // Determine the gridbox
  cix += 2;
  IvPBox gelbox(dim,0);
  for(d=0; d<dim; d++) {
    // Determine the grid length for this dimension
    int val = 0;
    while(str[cix] != ',') {
      val = val * 10;
      val += (int)(str[cix]-48);
      cix++;
    }
    cix++;
    gelbox.setPTS(d,0,val);
  }

  // Build the PDMap
  int wtc = (deg*dim)+1;
  cix += 2;

  PDMap *pdmap = new PDMap(pcs, domain, deg);
  for(i=0; i<pcs; i++) {
    IvPBox *newbox = new IvPBox(dim,deg);
    for(d=0; d<dim; d++) {
      // Check the LowBound
      if(str[cix] == 'X') {
	newbox->bd(d,0) = 0;
	cix++;
      }

      // Determine the low value
      int low = 0;
      while(str[cix] != ',') {
	low = low * 10;
	low += (int)(str[cix]-48);
	cix++;
      }
      cix++;

      // Check the HighBound
      if(str[cix] == 'X') {
	newbox->bd(d,1) = 0;
	cix++;
      }
      // Determine the high value
      int hgh = 0;
      while(str[cix] != ',') {
	hgh = hgh * 10;
	hgh += (int)(str[cix]-48);
	cix++;
      }
      cix++;
      newbox->setPTS(d, low, hgh);
    }  

    // Determine the interior function coefficients
    for(d=0; d<wtc; d++) {
      // Begin Extract-Double-From-String Code
      double sign = 1.0;
      double coef = 0.0;
      double frac = 0.1;
      bool   left_of_decimal = true;

      if(str[cix]=='-') {
	sign = -1.0;
	cix++;
      }
      while((str[cix] != ',') && (str[cix] != '\0')) {
	if(str[cix] == '.') {
	  left_of_decimal = false;
	}
	else {
	  if(left_of_decimal) {
	    coef = coef * 10;
	    coef += (double)(str[cix]-48);
	  }
	  else {
	    coef += ((double)(str[cix]-48)) * frac;
	    frac = frac / 10.0;
	  }
	}
	cix++;
      }
      cix++;
      // End Extract-Double-From-String Code
      newbox->wt(d) = coef * sign;
    }
    pdmap->bx(i) = newbox;
  }

  pdmap->setGelBox(gelbox);
  pdmap->updateGrid(1,1);
  IvPFunction *new_of = new IvPFunction(pdmap);
  new_of->setPWT(pwt);
  new_of->setContextStr(cstr_buff);
  delete [] cstr_buff;
  
  return(new_of);
}









