PROJECT(IVP_NEWMAT10D)

ADD_LIBRARY(newmat
   newmat1.cpp 
   newmat2.cpp 
   newmat3.cpp 
   newmat4.cpp 
   newmat5.cpp 
   newmat6.cpp 
   newmat7.cpp 
   newmat8.cpp 
   newmatex.cpp 
   bandmat.cpp 
   submat.cpp 
   newmat9.cpp
   myexcept.cpp 
   cholesky.cpp 
   evalue.cpp 
   fft.cpp 
   hholder.cpp 
   jacobi.cpp 
   newfft.cpp 
   sort.cpp 
   svd.cpp 
   newmatrm.cpp 
   )

#===============================================================================
# Below are the classic GNU Make makefile rules for newmat's example programs.
# I'm not going to bother porting them to CMake at the moment, because they're
# not actually interesting to IvP users... -CJC 
#===============================================================================

# tmt_obj = tmt.o tmt1.o tmt2.o tmt3.o tmt4.o tmt5.o tmt6.o tmt7.o tmt8.o tmt9.o tmta.o tmtb.o tmtc.o tmtd.o tmte.o tmtf.o tmtg.o tmth.o tmti.o tmtj.o tmtk.o tmtl.o tmtm.o
# 
# tmt:           	$(tmt_obj) libnewmat.a
# 		$(CXX) -o $@ $(tmt_obj) -L. -lnewmat -lm
# 
# example_obj = example.o
# 
# example:       	$(example_obj) libnewmat.a
# 		$(CXX) -o $@ $(example_obj) -L. -lnewmat -lm
# 
# test_exc_obj = test_exc.o
# 
# test_exc:      	$(test_exc_obj) libnewmat.a
# 		$(CXX) -o $@ $(test_exc_obj) -L. -lnewmat -lm
# 
# nl_ex_obj = nl_ex.o newmatnl.o
# 
# nl_ex:         	$(nl_ex_obj) libnewmat.a
# 		$(CXX) -o $@ $(nl_ex_obj) -L. -lnewmat -lm
# 
# sl_ex_obj = sl_ex.o solution.o myexcept.o
# 
# sl_ex:         	$(sl_ex_obj)
# 		$(CXX) -o $@ $(sl_ex_obj) -L. -lm
# 
# garch_obj = garch.o newmatnl.o
# 
# garch:         	$(garch_obj) libnewmat.a
# 		$(CXX) -o $@ $(garch_obj) -L. -lnewmat -lm
