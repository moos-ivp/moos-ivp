# Generic makefile structure for all application modules
.SILENT:

BIN_DIR  = ../bin
TARGET   = $(BIN_DIR)/$(LTARGET)
IFLAGS   = -I. -I.. $(patsubst %,-I%,$(PLDIRS))
LFLAGS   = -L/usr/X11R6/lib -L/usr/X11R6/lib64     \
	   -L/sw/lib $(patsubst %,-L%,$(PLDIRS))  
SOURCES  = $(wildcard *.cpp)
OBJECTS  = $(SOURCES:.cpp=.o)
CFLAGS   = $(DEBUG_LEVEL) -Wno-deprecated  $(FP) $(LCFLAGS)

vpath %.a  $(PLDIRS) 

all:  	$(TARGET)
	makedepend > /dev/null 2>&1 ; rm -f *akefile.bak

depend:
	makedepend $(IFLAGS) $(SOURCES) > /dev/null 2>&1

clean:
	@rm -f $(TARGET) $(LTARGET)
	makedepend > /dev/null 2>&1
	@rm -f *.bak *~ $(OBJECTS)
	@echo Finished cleaning $(LTARGET)

$(TARGET): $(OBJECTS) $(PLIBS)
	@echo "Linking now....." $(LTARGET) 
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(IFLAGS) $(LFLAGS) $(LIBS) 
	@chmod 777 $(TARGET)

%.o: %.cpp
	@echo $(CXX) "compiling" $(LTARGET) "source code" $<
	$(CXX) $(CFLAGS) $(IFLAGS) -c $<

# DO NOT DELETE THIS LINE -- make depend depends on it.
