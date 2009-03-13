PROJECT(newmat)

SET(NEWMAT_SRCS 
newmat1.cpp newmat2.cpp newmat3.cpp newmat4.cpp newmat5.cpp newmat6.cpp newmat7.cpp newmat8.cpp newmatex.cpp bandmat.cpp submat.cpp myexcept.cpp cholesky.cpp evalue.cpp fft.cpp hholder.cpp jacobi.cpp newfft.cpp sort.cpp svd.cpp newmatrm.cpp newmat9.cpp)

ADD_LIBRARY(newmat ${NEWMAT_SRCS})
