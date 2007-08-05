PROJECT(newmat)

SET(NEWMAT_SRCS 
newmat1.cpp newmat2.cpp newmat3.cpp newmat4.cpp newmat5.cpp newmat6.cpp newmat7.cpp newmat8.cpp newmatex.cpp bandmat.cpp submat.cpp myexcept.cpp cholesky.cpp evalue.cpp fft.cpp hholder.cpp jacobi.cpp newfft.cpp sort.cpp svd.cpp newmatrm.cpp newmat9.cpp)

ADD_LIBRARY(newmat ${NEWMAT_SRCS})

#===============================================================================
# For installation and Debian packaging...
#===============================================================================

INSTALL(
   TARGETS newmat
   DESTINATION lib/moos-${MOOS_VERSION}
   COMPONENT Moos_Thirdparty_Dev)

# Note: If there are any header files that are truly unneeded by users of this
# library, it should be OK to remove them from the PUBLIC_HEADERS variable,
# which in turn will prevent them from being installed. -CJC
SET(PUBLIC_HEADERS 
   boolean.h
   controlw.h
   include.h
   myexcept.h
   newmatap.h
   newmat.h
   newmatio.h
   newmatnl.h
   newmatrc.h
   newmatrm.h
   precisio.h
   solution.h
)

INSTALL(
   FILES ${PUBLIC_HEADERS}
   DESTINATION include/moos-${MOOS_VERSION}/Thirdparty/newmat
   COMPONENT Moos_Thirdparty_Dev)

#===============================================================================
