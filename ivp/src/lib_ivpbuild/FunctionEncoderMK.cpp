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
#include <string.h>
#include "MBUtils.h"
#include "BuildUtils.h"
#include "FunctionEncoderMK.h"

using namespace std;

//--------------------------------------------------------------
// Procedure: IvPFunctionToString
//      Note: cstr is short for context_string
//
// H,cstr_len,cstr,dim,pcs,deg,pwt,
// D,course;0;359;360:speed;0;8;9,G,9,4,
// G,10,10,10,
// F,0,40,0,50,0.1,0.2,0.3,

class MK_itoa
{
public:
  int itoa(int ival, char *buff) {
    if(ival < 10000) {
      int i;
      for(i = 0; i < smaps[ival].len; ++i) {
	buff[i] = smaps[ival].str[i];
      }
      return i;
    }
    else {
      int upper = ival / 10000;
      int lower = ival % 10000;
      int slen = itoa(upper, buff);
      slen += itoa(lower, buff + slen);
      return slen;
    }
  }

  int doubleToString(double jwt, char *buff)
  {
    int slen = 0;
    if(jwt<0) {
      buff[slen++] = '-';
      jwt *= -1;
    }
    if(jwt==0) {
      buff[slen++] = '0';
    }
    else {
      long int ival = (long int)((jwt * 10000)+0.5);
      // long int upper = ival / 10000; // before the decimal, assume lower than 10k for now
      // long int upper = (long int)(jwt + 0.00005);
      long int upper = ival / 10000;
      long int lower = ival % 10000; // after the decimal, guaranteed to be withing 10k
      // precision of 4 digits
      // -- assume at least 32 bit long int (~4 billion), easily alterable

      // We're shifting the number left 4 digits, therefore the
      // four lowest digits will be after the decimal and everything
      // else will be before
      slen += itoa(upper, buff + slen);

      if(lower != 0)
      {
         int space_len = smaps[lower].len;
         buff[slen++] = '.';
         for( ; space_len < 4; ++space_len)
         {
            buff[slen++] = '0';
         }
         // can't use itoa here if smaps[lower].len < 4...need to pad with
         // 0's to the left of the value
         slen += itoa(lower, buff + slen);

         // Knock off the zeroes at the end of the after the decimal
         for(int i = 0; i < 4; ++i)
         {
            if(buff[slen - 1] == '0')
	      --slen;
            else
	      break;
         }
      }
    }
    return slen;
  }

  MK_itoa()
  {
    for(unsigned int i = 0; i < 10000; ++i)
    {  
      smaps[i].str = new char[7];
      memset(smaps[i].str, 0, 7);
      sprintf(smaps[i].str, "%d", i);                                                 smaps[i].len = strlen(smaps[i].str);
    }
  }

private:
  struct smap 
  {
    int len;
    char *str;
  };

  smap smaps[10000];
};

MK_itoa mk_itoa;

string IvPFunctionToStringMK(IvPFunction *ivp_function)
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
      len += mk_itoa.itoa(ival, &buff[len]);
      buff[len++] = ',';
      // END  INLINE INT-TO-STRING CODE  ----------------

      if(ibox->bd(d,1)==0) {
	buff[len] = 'X';
	len++;
      }
      // BEGIN INLINE INT-TO-STRING CODE  ----------------
      ival = ibox->pt(d,1);
      len += mk_itoa.itoa(ival, &buff[len]);
      buff[len++] = ',';
    }

    for(int j=0; j<wtc; j++) {
      // BEGIN DOUBLE-TO-STRING INLINE CODE --------------------
      double jwt = ibox->wt(j);
      len += mk_itoa.doubleToString(jwt, &buff[len]);

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

vector<string> IvPFunctionToVectorMK(const string& whole_string,
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

IvPFunction *StringToIvPFunctionMK(const string& str)
{
  int d, i;

  const char *cstr = str.c_str();
  cstr += 2; // to account for H, in the header

  // Determine the length of the context string
  int cstr_len = 0;
  while(*cstr != ',') {
    cstr_len = cstr_len * 10;
    cstr_len += (int)(*cstr-48);
    ++cstr;
  }
  ++cstr;

  // Determine the context string, if any
  char *cstr_buff = new char[cstr_len+10];
  int  cbix = 0;
  while(*cstr != ',') {
    cstr_buff[cbix] = *cstr;
    cbix++;
    ++cstr;
  }
  cstr_buff[cbix] = '\0';
  ++cstr;

  // Determine the number of dimensions
  int dim = 0;
  while(*cstr != ',') {
    dim = dim * 10;
    dim += (int)(*cstr-48);
    ++cstr;
  }
  ++cstr;

  // Determine the number of pieces
  int pcs = 0;
  while(*cstr != ',') {
    pcs = pcs * 10;
    pcs += (int)(*cstr-48);
    ++cstr;
  }
  ++cstr;
   
  // Determine the degree
  int deg = 0;
  while(*cstr != ',') {
    deg = deg * 10;
    deg += (int)(*cstr-48);
    ++cstr;
  }
  ++cstr;
   
  // Determine the priority weight
  double pwt  = 0.0;
  double frac = 0.1;
  bool   left_of_decimal = true;
  while(*cstr != ',') {
    if(*cstr == '.') {
      left_of_decimal = false;
    }
    else {
      if(left_of_decimal) {
	pwt = pwt * 10;
	pwt += (double)(*cstr-48);
      }
      else {
	pwt += ((double)(*cstr-48)) * frac;
	frac = frac / 10.0;
      }
    }
    ++cstr;
  }
  ++cstr;
   
  // Determine the domain
  cstr += 2;
  char buff[5000];
  int  bix = 0;
  while(*cstr != ',') {
    buff[bix] = *cstr;
    if(buff[bix] == ';')
      buff[bix] = ',';
    bix++;
    ++cstr;
  }
  buff[bix] = '\0';
  ++cstr;

  IvPDomain domain = stringToDomain(buff);

  // Determine the gridbox
  cstr += 2;
  IvPBox gelbox(dim,0);
  for(d=0; d<dim; d++) {
    // Determine the grid length for this dimension
    int val = 0;
    while(*cstr != ',') {
      val = val * 10;
      val += (int)(*cstr-48);
      ++cstr;
    }
    ++cstr;
    gelbox.setPTS(d,0,val);
  }

  // Build the PDMap
  int wtc = (deg*dim)+1;
  cstr += 2;

  PDMap *pdmap = new PDMap(pcs, domain, deg);
  for(i=0; i<pcs; i++) {
    IvPBox *newbox = new IvPBox(dim,deg);
    for(d=0; d<dim; d++) {
      // Check the LowBound
      if(*cstr == 'X') {
	newbox->bd(d,0) = 0;
	++cstr;
      }

      // Determine the low value
      int low = 0;
      while(*cstr != ',') {
	low = low * 10;
	low += (int)(*cstr-48);
	++cstr;
      }
      ++cstr;

      // Check the HighBound
      if(*cstr == 'X') {
	newbox->bd(d,1) = 0;
	++cstr;
      }
      // Determine the high value
      int hgh = 0;
      while(*cstr != ',') {
	hgh = hgh * 10;
	hgh += (int)(*cstr-48);
	++cstr;
      }
      ++cstr;
      newbox->setPTS(d, low, hgh);
    }  

    // Determine the interior function coefficients
    for(d=0; d<wtc; d++) {
      // Begin Extract-Double-From-String Code
      double sign = 1.0;
      double coef = 0.0;
      double frac = 0.1;
      bool   left_of_decimal = true;

      if(*cstr=='-') {
	sign = -1.0;
	++cstr;
      }
      while((*cstr != ',') && (*cstr != '\0')) {
	if(*cstr == '.') {
	  left_of_decimal = false;
	}
	else {
	  if(left_of_decimal) {
	    coef = coef * 10;
	    coef += (double)(*cstr-48);
	  }
	  else {
	    coef += ((double)(*cstr-48)) * frac;
	    frac = frac / 10.0;
	  }
	}
	++cstr;
      }
      ++cstr;
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
