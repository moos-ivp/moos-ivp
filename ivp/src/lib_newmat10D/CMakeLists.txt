#--------------------------------------------------------
# The CMakeLists.txt for:                       newmat10D
# Author(s):                                
#--------------------------------------------------------

# Specify only those source files that aren't used for test / example programs.
# That's why we're not just adding all *.cpp files. -CJC
SET(SRC 
   bandmat.cpp                                                       
   cholesky.cpp                                                      
   evalue.cpp                                                        
   fft.cpp                                                           
   hholder.cpp                                                       
   jacobi.cpp
   myexcept.cpp
   newfft.cpp
   newmat1.cpp
   newmat2.cpp
   newmat3.cpp
   newmat4.cpp
   newmat5.cpp
   newmat6.cpp
   newmat7.cpp
   newmat8.cpp
   newmat9.cpp
   newmatex.cpp
   newmatnl.cpp
   newmatrm.cpp
   solution.cpp
   sort.cpp
   submat.cpp
   svd.cpp
)

ADD_LIBRARY(newmat10D ${SRC})
