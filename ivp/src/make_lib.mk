# Generic makefile structure for all library modules
.SILENT:

LIB_DIR  = .
TARGET   = $(LIB_DIR)/$(LTARGET)

# CJC: Modified LFLAGS and IFLAGS to also work for building code that's two 
# directories below ivp/src/, rather than only for code that's in a subdirectory
# immediately below ivp/src/.  This was necessary to accomodate private code
# directories.

MOOS_IFLAGS = \
	-I ../../../MOOS/Essentials \
	-I ../../../MOOS/Core \
	-I ../../../MOOS/NavigationAndControl \
	-I ../../../MOOS/Thirdparty/FLTKVW \
	-I ../../../MOOS/Essentials/MOOSUtilityLib \
	-I ../../../MOOS/Core/MOOSLIB \
	-I ../../../MOOS/Core/MOOSGenLib \
	-I ../../../MOOS/NavigationAndControl\MOOSNavLib \
	-I ../../../MOOS/NavigationAndControl\MOOSTaskLib \
	-I ../../../MOOS/Thirdparty/FLTKVW \
	-I ../../../../MOOS/Essentials \
	-I ../../../../MOOS/Core \
	-I ../../../../MOOS/NavigationAndControl \
	-I ../../../../MOOS/Thirdparty/FLTKVW \
	-I ../../../../MOOS/Essentials/MOOSUtilityLib \
	-I ../../../../MOOS/Core/MOOSLIB \
	-I ../../../../MOOS/Core/MOOSGenLib \
	-I ../../../../MOOS/NavigationAndControl\MOOSNavLib \
	-I ../../../../MOOS/NavigationAndControl\MOOSTaskLib \
	-I ../../../../MOOS/Thirdparty/FLTKVW

IFLAGS   = -I. -I.. -I../.. -I/usr/X11R6/include -I/sw/include \
	    $(LIFLAGS) $(patsubst %, -I% , $(wildcard ../lib*) $(wildcard ../../lib*)) \
	    $(MOOS_IFLAGS)

CFLAGS   = $(DEBUG_LEVEL) -Wno-deprecated  $(FP) $(LCFLAGS)
SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(SOURCES:.cpp=.o)

all:  	$(TARGET)
	makedepend > /dev/null 2>&1; rm -f *akefile.bak

depend:
	makedepend  $(IFLAGS) $(SOURCES) > /dev/null 2>&1

clean:
	@rm -f $(TARGET) $(LTARGET)
	makedepend > /dev/null 2>&1
	@rm -f *.bak *~ $(OBJECTS)
	@echo Finished cleaning $(LTARGET)

$(TARGET): $(OBJECTS)
	@rm -f $(TARGET) $(LTARGET)
	@echo "building library now....." $(LTARGET)
	$(AR) rcs  $@ $(TARGET) $(OBJECTS)
	ranlib $@

%.o: %.cpp
	@echo "compiling" $(LTARGET) "source code" $<
	$(CXX) $(CFLAGS) $(IFLAGS) -c $<
