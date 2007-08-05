all: _MOOS.so
	@echo Finished building uVoice

MOOS_wrap.cxx:
	@echo Generating wrapper code
	swig -python -c++ -shadow -dirprot MOOS.i

_MOOS.so: MOOS_wrap.cxx
	@echo Building Python module
	g++ -shared -fPIC MOOS_wrap.cxx -lMOOS -lMOOSGen -L../MOOSLIB \
	-L../MOOSGenLib -I/usr/include/python2.4 -I.. -I../MOOSLIB \
	-I../MOOSGenLib -DUNIX -o _MOOS.so

clean:
	rm -f *.pyc sphinx2.* *wrap* _MOOS.so MOOS.py
	@echo Finished cleaning uVoice
		
depend:
	
