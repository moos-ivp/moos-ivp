/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: TestEncodeMK.cpp                                     */
/*****************************************************************/

#include <string>
#include <iostream>
#include "MBUtils.h"
#include "IvPFunction.h"
#include "IO_Utilities.h"
#include "OF_Reflector.h"
#include "AOF_SCALAR.h"
#include "AOF_Rings.h"
#include "MBTimer.h"
#include "FunctionEncoder.h"
#include "FunctionEncoderMK.h"

using namespace std;

int testEncodeMK(int argc, char*argv[])
{
  string alg = "mb";
  int    i, res;

  double tm    = 10.0;
  int    dim   = 3;
  int    pcs   = 500;
  bool   quiet = false;

  res = getArg(argc, argv, 1, "-time");
  if(res)
    tm = atof(argv[res]);
  res = getArg(argc, argv, 1, "-dim");
  if(res)
    dim = atoi(argv[res]);
  res = getArg(argc, argv, 1, "-pcs");
  if(res)
    pcs = atoi(argv[res]);
  res = getArg(argc, argv, 0, "-quiet");
  if(res)
    quiet = true;
  res = getArg(argc, argv, 0, "-mk");
  if(res)
    alg = "mk";

  cout << "time: " << tm   << endl;
  cout << "dim:  " << dim  << endl;
  cout << "pcs:  " << pcs  << endl;

  IvPDomain domain;
  IvPBox grid_box(dim,1);
  for(int d=0; d<dim; d++) {
    string varname = intToString(d);
    domain.addDomain(varname, 0, 500, 501);
    grid_box.setPTS(d, 20, 20);
  }

  string loc_str = "150";
  for(i=1; i<dim; i++)
    loc_str += ",150";

  AOF_Rings aof(domain);
  aof.setParam("location", loc_str);
  aof.setParam("radius",7);
  aof.setParam("exp",13);
  aof.setParam("base",-100);
  aof.setParam("range",200);
  aof.setParam("peak","true");


  OF_Reflector reflector(&aof,1);
#if 0
  IvPBox unifbox(dim);
  for(i=0; i<dim; i++)
    unifbox.setPTS(i,0,200);
  reflector.createUniform(&unifbox, &unifbox, 7);
#endif
  reflector.createUniform(pcs, 7);
  IvPFunction *ipf = reflector.extractOF();

  if(!ipf) {
    cout << "Problem in creating a test IvP Function" << endl << flush;
    exit(0);
  }

  ipf->setPWT(123.456);

  string ipf_str;
  if(alg == "mb")
    ipf_str = IvPFunctionToString(ipf);
  else if(alg == "mk")
    ipf_str = IvPFunctionToStringMK(ipf);
  else 
    ipf_str = IvPFunctionToString(ipf);


  cout << "Done creating the initial IvP Function: " << endl;
  cout << "  Pieces: " << ipf->getPDMap()->size() << endl;
  cout << "  Length: " << ipf_str.length() << endl;
  cout << endl << "Performing tests......" << endl;

  int counter;
  bool done;

  // Experiment 1   -------------------------------------
  MBTimer timer1((int)tm);
  timer1.start();
  counter = 0;
  done = false;
  while(!done) {
    if(alg == "mb")
      ipf_str = IvPFunctionToString(ipf);
    else if(alg == "mk")
      ipf_str = IvPFunctionToStringMK(ipf);
    else 
      ipf_str = IvPFunctionToString(ipf);
    counter ++;
    if((counter % 5) == 0)
      done = timer1.timeOutCheck();
  }
  timer1.stop();

  double rate = ((double)(counter)) / tm;
  string rate_str = doubleToString(rate,2);

  cout << "Total Encodings1: " << counter << endl;
  cout << "Encodings/sec: " << rate_str << endl;
  cout << "String Length: " << ipf_str.length() << endl;
  if((ipf_str.length() < 500) && !quiet) {
    cout << "str exp1: " << endl;
    cout << "[" << ipf_str << "]" << endl;
  }


  // Experiment Z   -------------------------------------
  MBTimer timerZ((int)tm);
  timerZ.start();
  counter = 0;
  done = false;
  while(!done) {
    IvPFunction *new_of = 0;
    if(alg == "mb")
      new_of = StringToIvPFunction(ipf_str);
    else if(alg == "mk")
      new_of = StringToIvPFunctionMK(ipf_str);
    else 
      new_of = StringToIvPFunction(ipf_str);
    if(new_of)
      delete(new_of);
    counter++;
    done = timerZ.timeOutCheck();
  }
  timerZ.stop();

  rate = ((double)(counter)) / tm;
  rate_str = doubleToString(rate,2);

  cout << "Total De-codings: " << counter << endl;
  cout << "De-codings/sec: " << rate_str << endl;


  // Experiment Compare ---------------------------------

  IvPFunction *new_of;
  string new_str;

  if(alg == "mb") {
    new_of  = StringToIvPFunction(ipf_str);
    new_str = IvPFunctionToString(new_of);
  } else if(alg == "mk") {
    new_of  = StringToIvPFunctionMK(ipf_str);
    new_str = IvPFunctionToStringMK(new_of);
  } else  {
    new_of  = StringToIvPFunction(ipf_str);
    new_str = IvPFunctionToString(new_of);
  }

  if(new_str == ipf_str) 
    cout << "The two strings are equivalent!" << endl;
  else {
    cout << "The two strings are NOT equivalent!" << endl;
    int len1 = ipf_str.length();
    int len2 = new_str.length();
    cout << "Len1 : " << len1 << endl;
    cout << "Len2 : " << len2 << endl;
    cout << "ipf_str: " << ipf_str << endl;
    cout << "new_str: " << new_str << endl;
  }

  if((ipf_str.length() < 500) & !quiet) {
    cout << "ipf_str: " << endl;
    cout << "[" << ipf_str << "]" << endl;
  }

  if((new_str.length() < 500) & !quiet) {
    cout << "new_str: " << endl;
    cout << "[" << new_str << "]" << endl;
  }

}
