# Makefile definitions particular to the fview app

LTARGET  = sfview
LIFLAGS  = 
LCFLAGS  = 

PLIBS    = libbehaviors-don.a		\
	   libbehaviors-sandbox.a 	\
	   libbehaviors-marine.a	\
	   libbhvutil.a			\
	   libipfview.a			\
	   libbehaviors.a		\
	   libgeometry.a 		\
	   libivpbuild.a    		\
           libivpcore.a libmbutil.a 

LIBS     = -lm -lfltk -lfltk_gl -lX11 -lGL 		\
           $(patsubst lib%.a,-l%,$(PLIBS))

PLDIRS   = $(wildcard ../lib_*)

# Make configuration parameters
include ../make_config.mk
# Generic makefile structure for all application modules
include ../make_app.mk
# DO NOT DELETE

AOF_Cache.o: AOF_Cache.h ../lib_ivpbuild/AOF.h ../lib_ivpcore/IvPBox.h
AOF_Cache.o: ../lib_ivpcore/IvPDomain.h ../lib_mbutil/FColorMap.h
GUI.o: GUI.h /usr/include/FL/Fl.H /usr/include/FL/Enumerations.H
GUI.o: /usr/include/FL/Fl_Export.H /usr/include/FL/Fl_Window.H
GUI.o: /usr/include/FL/Fl_Group.H /usr/include/FL/Fl_Widget.H
GUI.o: /usr/include/FL/Fl_Menu_Bar.H /usr/include/FL/Fl_Menu_.H
GUI.o: /usr/include/FL/Fl_Menu_Item.H /usr/include/FL/Fl_Image.H Viewer.h
GUI.o: ../lib_ivpbuild/AOF.h ../lib_ivpcore/IvPBox.h
GUI.o: ../lib_ivpcore/IvPDomain.h AOF_Cache.h ../lib_mbutil/FColorMap.h
GUI.o: ../lib_ivpcore/IvPFunction.h ../lib_ivpcore/PDMap.h
GUI.o: ../lib_ivpcore/BoxSet.h ../lib_ivpcore/BoxSetNode.h
GUI.o: ../lib_ivpcore/IvPGrid.h Quad3D.h /usr/include/FL/Fl_Gl_Window.H
GUI.o: /usr/include/FL/Fl_Window.H /usr/include/FL/gl.h /usr/include/GL/gl.h
GUI.o: /usr/include/GL/glext.h /usr/include/inttypes.h
GUI.o: /usr/include/features.h /usr/include/sys/cdefs.h
GUI.o: /usr/include/gnu/stubs.h /usr/include/stdint.h
GUI.o: /usr/include/bits/wchar.h /usr/include/bits/wordsize.h
GUI.o: /usr/include/FL/fl_draw.H
PopulatorAOF.o: /usr/include/math.h /usr/include/features.h
PopulatorAOF.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
PopulatorAOF.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
PopulatorAOF.o: /usr/include/bits/mathcalls.h ../lib_ivpcore/IvPDomain.h
PopulatorAOF.o: ../lib_ivpcore/IvPBox.h ../lib_ivpbuild/AOF_Rings.h
PopulatorAOF.o: /usr/include/stdio.h /usr/include/bits/types.h
PopulatorAOF.o: /usr/include/bits/wordsize.h /usr/include/bits/typesizes.h
PopulatorAOF.o: /usr/include/libio.h /usr/include/_G_config.h
PopulatorAOF.o: /usr/include/wchar.h /usr/include/bits/wchar.h
PopulatorAOF.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
PopulatorAOF.o: /usr/include/bits/sys_errlist.h ../lib_ivpbuild/AOF_Ring.h
PopulatorAOF.o: ../lib_ivpbuild/AOF.h
PopulatorAOF.o: ../lib_behaviors-sandbox/AOF_Waypoint2D.h
PopulatorAOF.o: ../lib_geometry/AngleUtils.h
PopulatorAOF.o: ../lib_behaviors-don/AOF_ArrayAngle.h
PopulatorAOF.o: ../lib_behaviors-attic/AOF_AvoidObstacles.h
PopulatorAOF.o: ../lib_geometry/XYPolygon.h ../lib_geometry/XYSegList.h
PopulatorAOF.o: PopulatorAOF.h ../lib_mbutil/MBUtils.h /usr/include/time.h
PopulatorAOF.o: /usr/include/bits/time.h ../lib_mbutil/FileBuffer.h
Viewer.o: /usr/include/math.h /usr/include/features.h
Viewer.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
Viewer.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
Viewer.o: /usr/include/bits/mathcalls.h Viewer.h ../lib_ivpbuild/AOF.h
Viewer.o: ../lib_ivpcore/IvPBox.h ../lib_ivpcore/IvPDomain.h AOF_Cache.h
Viewer.o: ../lib_mbutil/FColorMap.h ../lib_ivpcore/IvPFunction.h
Viewer.o: ../lib_ivpcore/PDMap.h ../lib_ivpcore/BoxSet.h
Viewer.o: ../lib_ivpcore/BoxSetNode.h ../lib_ivpcore/IvPGrid.h Quad3D.h
Viewer.o: /usr/include/FL/Fl.H /usr/include/FL/Enumerations.H
Viewer.o: /usr/include/FL/Fl_Export.H /usr/include/FL/Fl_Gl_Window.H
Viewer.o: /usr/include/FL/Fl_Window.H /usr/include/FL/gl.h
Viewer.o: /usr/include/GL/gl.h /usr/include/GL/glext.h
Viewer.o: /usr/include/inttypes.h /usr/include/stdint.h
Viewer.o: /usr/include/bits/wchar.h /usr/include/bits/wordsize.h
Viewer.o: /usr/include/FL/fl_draw.H ../lib_mbutil/MBUtils.h
Viewer.o: /usr/include/time.h /usr/include/bits/time.h
Viewer.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
Viewer.o: ../lib_ivpbuild/OF_Reflector.h ../lib_mbutil/MBTimer.h
Viewer.o: /usr/include/sys/times.h
main.o: /usr/include/time.h /usr/include/features.h /usr/include/sys/cdefs.h
main.o: /usr/include/gnu/stubs.h /usr/include/bits/time.h
main.o: /usr/include/bits/types.h /usr/include/bits/wordsize.h
main.o: /usr/include/bits/typesizes.h GUI.h /usr/include/FL/Fl.H
main.o: /usr/include/FL/Enumerations.H /usr/include/FL/Fl_Export.H
main.o: /usr/include/FL/Fl_Window.H /usr/include/FL/Fl_Group.H
main.o: /usr/include/FL/Fl_Widget.H /usr/include/FL/Fl_Menu_Bar.H
main.o: /usr/include/FL/Fl_Menu_.H /usr/include/FL/Fl_Menu_Item.H
main.o: /usr/include/FL/Fl_Image.H Viewer.h ../lib_ivpbuild/AOF.h
main.o: ../lib_ivpcore/IvPBox.h ../lib_ivpcore/IvPDomain.h AOF_Cache.h
main.o: ../lib_mbutil/FColorMap.h ../lib_ivpcore/IvPFunction.h
main.o: ../lib_ivpcore/PDMap.h ../lib_ivpcore/BoxSet.h
main.o: ../lib_ivpcore/BoxSetNode.h ../lib_ivpcore/IvPGrid.h Quad3D.h
main.o: /usr/include/FL/Fl_Gl_Window.H /usr/include/FL/Fl_Window.H
main.o: /usr/include/FL/gl.h /usr/include/GL/gl.h /usr/include/GL/glext.h
main.o: /usr/include/inttypes.h /usr/include/stdint.h
main.o: /usr/include/bits/wchar.h /usr/include/FL/fl_draw.H
main.o: ../lib_ivpbuild/AOF_Rings.h /usr/include/stdio.h /usr/include/libio.h
main.o: /usr/include/_G_config.h /usr/include/wchar.h /usr/include/gconv.h
main.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
main.o: ../lib_ivpbuild/AOF_Ring.h ../lib_mbutil/MBUtils.h
main.o: ../lib_ivpbuild/IO_Utilities.h ../lib_ivpcore/IvPProblem.h
main.o: ../lib_ivpcore/Problem.h ../lib_ivpcore/Compactor.h PopulatorAOF.h
