# Makefile definitions particular to the peval app

LTARGET  = peval
LIFLAGS  = 
LCFLAGS  = 

PLIBS    =  libnavplot.a libmbutil.a libgeometry.a
LIBS     = -lm $(patsubst lib%.a,-l%,$(PLIBS))
PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE

Evaluator.o: /usr/include/math.h /usr/include/features.h
Evaluator.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Evaluator.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Evaluator.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
Evaluator.o: /usr/include/bits/mathcalls.h Evaluator.h
Evaluator.o: ../lib_navplot/NavPlot.h ../lib_navplot/LogPlot.h VReport.h
Evaluator.o: SlotReport.h PEVAL_Utilities.h
Launcher.o: Launcher.h ../lib_navplot/NavPlot.h ../lib_navplot/LogPlot.h
Launcher.o: ../lib_mbutil/MBUtils.h /usr/include/time.h
Launcher.o: /usr/include/features.h /usr/include/sys/cdefs.h
Launcher.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Launcher.o: /usr/include/gnu/stubs-32.h /usr/include/bits/time.h
Launcher.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Launcher.o: ../lib_mbutil/FileBuffer.h ../lib_mbutil/MBTimer.h
Launcher.o: /usr/include/sys/times.h ../lib_navplot/Populator_NavPlot.h
Launcher.o: ../lib_navplot/Populator_LogPlots.h Evaluator.h VReport.h
Launcher.o: SlotReport.h
main.o: ../lib_mbutil/MBUtils.h /usr/include/time.h /usr/include/features.h
main.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
main.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
main.o: /usr/include/bits/time.h /usr/include/bits/types.h
main.o: /usr/include/bits/typesizes.h Launcher.h ../lib_navplot/NavPlot.h
main.o: ../lib_navplot/LogPlot.h
PEVAL_Utilities.o: ../lib_mbutil/MBUtils.h /usr/include/time.h
PEVAL_Utilities.o: /usr/include/features.h /usr/include/sys/cdefs.h
PEVAL_Utilities.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
PEVAL_Utilities.o: /usr/include/gnu/stubs-32.h /usr/include/bits/time.h
PEVAL_Utilities.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
PEVAL_Utilities.o: PEVAL_Utilities.h VReport.h SlotReport.h
SlotReport.o: SlotReport.h
VReport.o: VReport.h SlotReport.h
