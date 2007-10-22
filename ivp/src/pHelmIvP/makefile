# Makefile definitions particular to the pHelmIvP app

LTARGET  = pHelmIvP
LIFLAGS  = 
LCFLAGS  = 

REQUIRED_MOOS_LIBS = 			\
	libMOOS.a 			\
	libMOOSGen.a 		

# Removed linkage to libbehaviors-sandbox.a because it's a private library. -CJC


PLIBS    = libhelmivp.a    		\
	   libbehaviors-marine.a 	\
	   libbhvutil.a			\
	   libbehaviors.a  		\
	   libmbutil.a 			\
	   libivpbuild.a 		\
	   libivpcore.a 		\
	   libgeometry.a 		\
	   liblogic.a

LIBS     = -lm -lpthread 				\
	$(patsubst lib%.a,-l%,$(PLIBS)) 		\
	$(patsubst lib%.a,-l%,$(REQUIRED_MOOS_LIBS)) 

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE

HelmIvP.o: /usr/include/math.h /usr/include/features.h
HelmIvP.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
HelmIvP.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
HelmIvP.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
HelmIvP.o: /usr/include/bits/mathcalls.h HelmIvP.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSLib.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSCommServer.h
HelmIvP.o: /usr/include/sys/time.h /usr/include/bits/types.h
HelmIvP.o: /usr/include/bits/typesizes.h /usr/include/time.h
HelmIvP.o: /usr/include/bits/time.h /usr/include/sys/select.h
HelmIvP.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
HelmIvP.o: /usr/include/pthread.h /usr/include/sched.h
HelmIvP.o: /usr/include/bits/sched.h /usr/include/signal.h
HelmIvP.o: /usr/include/bits/pthreadtypes.h /usr/include/bits/setjmp.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSCommObject.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSCommPkt.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSMsg.h
HelmIvP.o: ../../../MOOS/Core/MOOSGenLib/MOOSLock.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSGlobalHelper.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSCommClient.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSApp.h
HelmIvP.o: ../../../MOOS/Core/MOOSGenLib/MOOSGenLib.h
HelmIvP.o: ../../../MOOS/Core/MOOSGenLib/MOOSGenLibGlobalHelper.h
HelmIvP.o: ../../../MOOS/Core/MOOSGenLib/MOOSFileReader.h
HelmIvP.o: ../../../MOOS/Core/MOOSGenLib/ProcessConfigReader.h
HelmIvP.o: ../../../MOOS/Core/MOOSGenLib/MOOSLinuxSerialPort.h
HelmIvP.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
HelmIvP.o: /usr/include/sys/types.h /usr/include/endian.h
HelmIvP.o: /usr/include/bits/endian.h /usr/include/sys/sysmacros.h
HelmIvP.o: /usr/include/sys/signal.h /usr/include/termios.h
HelmIvP.o: /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h
HelmIvP.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
HelmIvP.o: /usr/include/bits/confname.h /usr/include/getopt.h
HelmIvP.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
HelmIvP.o: /usr/include/wchar.h /usr/include/bits/wchar.h
HelmIvP.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
HelmIvP.o: /usr/include/bits/sys_errlist.h
HelmIvP.o: ../../../MOOS/Core/MOOSGenLib/MOOSSerialPort.h
HelmIvP.o: ../../../MOOS/Core/MOOSGenLib/MOOSLock.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSVariable.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSInstrument.h
HelmIvP.o: ../../../MOOS/Core/MOOSLIB/MOOSException.h
HelmIvP.o: ../lib_behaviors/InfoBuffer.h ../lib_ivpcore/IvPDomain.h
HelmIvP.o: ../lib_ivpcore/IvPBox.h ../lib_behaviors/BehaviorSet.h
HelmIvP.o: ../lib_behaviors/IvPBehavior.h ../lib_ivpcore/IvPFunction.h
HelmIvP.o: ../lib_ivpcore/PDMap.h ../lib_ivpcore/BoxSet.h
HelmIvP.o: ../lib_ivpcore/BoxSetNode.h ../lib_ivpcore/IvPGrid.h
HelmIvP.o: ../lib_behaviors/VarDataPair.h ../lib_logic/LogicCondition.h
HelmIvP.o: ../lib_logic/ParseNode.h ../lib_helmivp/HelmEngine.h
HelmIvP.o: ../lib_helmivp/HelmReport.h ../lib_mbutil/MBUtils.h
HelmIvP.o: ../lib_mbutil/MBTimer.h /usr/include/sys/times.h
HelmIvP.o: ../lib_ivpbuild/FunctionEncoder.h ../lib_geometry/AngleUtils.h
HelmIvP.o: ../lib_ivpcore/IvPProblem.h ../lib_ivpcore/Problem.h
HelmIvP.o: ../lib_ivpcore/Compactor.h ../lib_helmivp/Populator_BehaviorSet.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSLib.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSCommServer.h
pHelmIvPMain.o: /usr/include/sys/time.h /usr/include/features.h
pHelmIvPMain.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
pHelmIvPMain.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
pHelmIvPMain.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
pHelmIvPMain.o: /usr/include/time.h /usr/include/bits/time.h
pHelmIvPMain.o: /usr/include/sys/select.h /usr/include/bits/select.h
pHelmIvPMain.o: /usr/include/bits/sigset.h /usr/include/pthread.h
pHelmIvPMain.o: /usr/include/sched.h /usr/include/bits/sched.h
pHelmIvPMain.o: /usr/include/signal.h /usr/include/bits/pthreadtypes.h
pHelmIvPMain.o: /usr/include/bits/setjmp.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSCommObject.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSCommPkt.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSMsg.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/MOOSLock.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSGlobalHelper.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSCommClient.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSApp.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/MOOSGenLib.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/MOOSGenLibGlobalHelper.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/MOOSFileReader.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/ProcessConfigReader.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/MOOSLinuxSerialPort.h
pHelmIvPMain.o: /usr/include/fcntl.h /usr/include/bits/fcntl.h
pHelmIvPMain.o: /usr/include/sys/types.h /usr/include/endian.h
pHelmIvPMain.o: /usr/include/bits/endian.h /usr/include/sys/sysmacros.h
pHelmIvPMain.o: /usr/include/sys/signal.h /usr/include/termios.h
pHelmIvPMain.o: /usr/include/bits/termios.h /usr/include/sys/ttydefaults.h
pHelmIvPMain.o: /usr/include/unistd.h /usr/include/bits/posix_opt.h
pHelmIvPMain.o: /usr/include/bits/confname.h /usr/include/getopt.h
pHelmIvPMain.o: /usr/include/stdio.h /usr/include/libio.h
pHelmIvPMain.o: /usr/include/_G_config.h /usr/include/wchar.h
pHelmIvPMain.o: /usr/include/bits/wchar.h /usr/include/gconv.h
pHelmIvPMain.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/MOOSSerialPort.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/MOOSLock.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSVariable.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSInstrument.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSLIB/MOOSException.h
pHelmIvPMain.o: ../../../MOOS/Core/MOOSGenLib/MOOSGenLib.h HelmIvP.h
pHelmIvPMain.o: ../lib_behaviors/InfoBuffer.h ../lib_ivpcore/IvPDomain.h
pHelmIvPMain.o: ../lib_ivpcore/IvPBox.h ../lib_behaviors/BehaviorSet.h
pHelmIvPMain.o: ../lib_behaviors/IvPBehavior.h ../lib_ivpcore/IvPFunction.h
pHelmIvPMain.o: ../lib_ivpcore/PDMap.h ../lib_ivpcore/BoxSet.h
pHelmIvPMain.o: ../lib_ivpcore/BoxSetNode.h ../lib_ivpcore/IvPGrid.h
pHelmIvPMain.o: ../lib_behaviors/VarDataPair.h ../lib_logic/LogicCondition.h
pHelmIvPMain.o: ../lib_logic/ParseNode.h ../lib_helmivp/HelmEngine.h
pHelmIvPMain.o: ../lib_helmivp/HelmReport.h
