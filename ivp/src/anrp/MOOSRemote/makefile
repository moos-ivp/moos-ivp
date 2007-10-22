# Makefile for MOOSRemote
.SILENT:

# Make configuration parameters
include ../../make_config.mk

BIN_DIR  = ../../bin
LTARGET  = MOOSRemote
TARGET   = $(BIN_DIR)/$(LTARGET)

PLIBS    = libMOOS.a libMOOSGen.a
LIBS     = -lm -lpthread -lcurses $(patsubst lib%.a,-l%,$(PLIBS))

PLDIRS   = ../MOOSLIB ../MOOSGenLib
IFLAGS   = -I. -I.. -I../.. $(patsubst %,-I%,$(PLDIRS))
LFLAGS   = -L.. -L../.. -L/usr/X11R6/lib $(patsubst %,-L%,$(PLDIRS))	
vpath %.a  $(PLDIRS) 

SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(SOURCES:.cpp=.o)
CFLAGS   = $(DEBUG_LEVEL) -DUNIX -Wno-deprecated $(FP)

all:  	$(TARGET)
	@makedepend >& /dev/null; rm -f *akefile.bak

$(TARGET): $(OBJECTS) $(PLIBS)
	@echo "Linking now....." $(LTARGET)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(IFLAGS) $(LFLAGS) $(LIBS)
	@chmod 777 $(TARGET)

depend:
	makedepend $(CFLAGS) $(IFLAGS) -c $(SOURCES) >& /dev/null

%.o: %.cpp
	@echo "compiling" $(LTARGET) "source code" $<
	$(CXX) $(CFLAGS) $(IFLAGS) -c $<

removex:
	@rm -f $(TARGET) $(LTARGET)

clean:	removex
	@makedepend > /dev/null
	@rm -f *.bak *~ $(OBJECTS)
	@echo Finished cleaning $(LTARGET)

# DO NOT DELETE THIS LINE -- make depend depends on it.
