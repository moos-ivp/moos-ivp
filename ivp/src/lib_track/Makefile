# Makefile for library opt
.SILENT:

LIB_DIR  = .
LTARGET  = libopt.a
TARGET   = $(LIB_DIR)/$(LTARGET)
IFLAGS   = -I. -I..
CFLAGS   = $(DEBUG_LEVEL) -DUNIX -Wno-deprecated
SOURCES  = $(wildcard *.c)
OBJECTS  = $(SOURCES:.c=.o)

all:  	$(TARGET)
	@makedepend > /dev/null 2>&1; rm -f Makefile.bak

$(TARGET): $(OBJECTS)
	@echo "building library now....." $(LTARGET)
	$(AR) rcs  $@ $(TARGET) $(OBJECTS)
	ranlib $@

depend:
	makedepend $(IFLAGS) $(SOURCES) > /dev/null 2>&1

%.o: %.cpp
	@echo "compiling" $(LTARGET) "source code" $<
	$(CXX) $(CFLAGS) $(IFLAGS) -c $<

removelib:
	@rm -f $(TARGET) $(LTARGET)

clean:	removelib
	@makedepend > /dev/null 2>&1
	@rm -f *.bak *~ $(OBJECTS)
	@echo Finished cleaning $(LTARGET)

# DO NOT DELETE THIS LINE -- make depend depends on it.
