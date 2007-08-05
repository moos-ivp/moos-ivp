/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI                                    */
/*    FILE: TestEncode.cpp                                       */
/*    DATE: April 2006                                           */
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

using namespace std;

int testEncode(int argc, char*argv[])
{

  string str1 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

  string foo = str1.c_str() + 5;
  
  cout << "foo: " << foo << endl;
  exit(0);


  int amt = 2;
  if(argc >= 2)
    amt = atoi(argv[1]);

  double tm = 10.0;
  if(argc==3)
    tm = atof(argv[2]);

  int dim = 3;
  if(argc >= 4)
    dim = atoi(argv[3]);

  cout << "amt:  " << amt << endl;
  cout << "time: " << tm  << endl;
  cout << "dim:  " << dim  << endl;

  IvPDomain domain;
  IvPBox grid_box(dim,1);
  for(int d=0; d<dim; d++) {
    string varname = intToString(d);
    domain.addDomain(varname, 0, 200, 201);
    grid_box.setPTS(d, 10, 10);
  }

#if 1
  AOF_Rings aof(domain);
  aof.setParam("location", "113,52");
  aof.setParam("radius",7);
  aof.setParam("exp",13);
  aof.setParam("base",-100);
  aof.setParam("range",200);
  aof.setParam("peak","true");
#endif

#if 0
  AOF_SCALAR aof(domain);
  aof.setParam("scalar", 12.2);
#endif


#if 1
  OF_Reflector reflector(&aof,1);
  IvPBox unifbox(2);
  unifbox.setPTS(0,0,180);
  unifbox.setPTS(1,0,180);

  reflector.createUniform(&unifbox, &unifbox);
#endif
#if 0
  OF_Reflector reflector(&aof, 1);
  vector<string> params;
  vector<double> vals;
  reflector.create_random(amt, &grid_box, params, vals);
#endif


  IvPFunction *of = reflector.extractOF();

  of->setPWT(123.456);

  //of->getPDMap()->print();

  string str = IvPFunctionToString(of);

  if(str.length() < 500) {
    cout << "str: " << endl;
    cout << str << endl;
  }

  int counter;
  bool done;

  // Experiment 1   -------------------------------------
#if 1
  MBTimer timer1((int)tm);
  timer1.start();
  counter = 0;
  done = false;
  while(!done) {
    str = IvPFunctionToString(of);
    counter ++;
    if((counter % 5) == 0)
      done = timer1.timeOutCheck();
  }
  timer1.stop();

  cout << "Total Encodings1: " << counter << endl;
  cout << "String Length: " << str.length() << endl;
  if(str.length() < 500) {
    cout << "str exp1: " << endl;
    cout << "[" << str << "]" << endl;
  }
#endif


#if 0
  // Experiment 2   -------------------------------------
  MBTimer timer2((int)tm);
  timer2.start();
  counter = 0;
  done = false;
  while(!done) {
    string str2 = str;
    counter ++;
    if((counter % 5) == 0)
      done = timer2.timeOutCheck();
  }
  timer2.stop();

  cout << "Total String Duplications: " << counter << endl;
  cout << "String Length: " << str.length() << endl;
#endif

#if 1
  // Experiment Z   -------------------------------------
  MBTimer timerZ((int)tm);
  timerZ.start();
  counter = 0;
  done = false;
  while(!done) {
    IvPFunction *new_of = StringToIvPFunction(str);
    if(new_of)
      delete(new_of);
    counter++;
    done = timerZ.timeOutCheck();
  }
  timerZ.stop();
  cout << "Total De-codings: " << counter << endl;
#endif

  IvPFunction *new_of  = StringToIvPFunction(str);
  string new_str = IvPFunctionToString(new_of);
  if(new_str.length() < 500) {
    cout << "new_str expZ: " << endl;
    cout << "[" << new_str << "]" << endl;
  }

#if 1
  //string new_str = IvPFunctionToString(new_of);
  
  if(new_str == str) 
    cout << "The two strings are equivalent!" << endl;
  else {
    cout << "The two strings are NOT equivalent!" << endl;
    int len1 = str.length();
    int len2 = new_str.length();
    cout << "Len1 : " << len1 << endl;
    cout << "Len2 : " << len2 << endl;
  }
#endif  

}
