
================================
Testers:
================================
  Each library function has a dedicated binary for testing the 
  function. 

  So the function segRayCPA() will have the executable:
  testSegRayCPA(). 

================================
Using the Testers:
================================
Each tester is a light wrapper, mostly grabbing the expected args
for the function. For example:

$ testSegRayCPA rx=5 ry=4 ra=90 x1=30 y1=34 x2=90 y2=34
cpa=30

================================
Test cases:
================================
Each app has a Unit Test File (.utf) alongside its main.cpp file. 
This file contains test cases with known results for given examples.
For example, in the segRayCPA() case:

$ cd testSegRayCPA()
$ ls 
CMakeLists.txt  cases.utf       main.cpp
$ more cases.utf
cmd=testSegRayCPA
x1=84 y1=55 x2=84 y2=-55   rx=100 ry=-30 ra=270 # cpa=16

================================
The utest utility
================================
The utest utility is an app also in the src_unit_tests folder. It
takes as an argument a single .utf file:

$ utest cases.utf

It will look for the appropriate command to run by looking for a line 
of the form:

   cmd=testSegRayCPA

Then it will use this command, e.g., testSegRayCPA on each of the test
lines in the file. A test line is of the form:

   args # result

For example:

   x1=84 y1=55 x2=84 y2=-55   rx=100 ry=-30 ra=270 # cpa=16

