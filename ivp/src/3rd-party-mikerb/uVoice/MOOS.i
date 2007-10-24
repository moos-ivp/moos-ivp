%module(directors="1",dirprot="1") MOOS
%feature("director") CMOOSApp;
%feature("director:except") {
    if ($error != NULL) {
		PyObject *ptype, *pvalue, *ptraceback;
		PyErr_Fetch( &ptype, &pvalue, &ptraceback);
		void *result;
		try {
			PyErr_Restore( ptype, pvalue, ptraceback);
			PyErr_Print();
			Py_Exit(1);
		} catch (...) {
			Py_XDECREF(ptype);
			Py_XDECREF(pvalue);
			Py_XDECREF(ptraceback);
			throw;
		}
		//throw Swig::DirectorMethodException();
	}
}

%include "std_string.i"
%include "std_list.i"

%{
#define protected public
#include <iostream>
#include "MOOSMsg.h"
#include "MOOSApp.h"
%}

class CMOOSMsg
{
public:
  bool GetTime();
  bool GetDouble();
  std::string GetString();
  std::string GetKey();
  std::string GetSource();
  std::string GetCommunity();
};

class CMOOSApp
{
public:
  virtual bool OnConnectToServer();
  bool Run( char * sName,char * sMissionFile);
  CMOOSApp();
  virtual ~CMOOSApp();

protected:
  virtual bool OnStartUp();
  virtual bool Iterate();
  virtual bool OnNewMail(MOOSMSG_LIST & NewMail);
  CMOOSCommClient m_Comms;
};

%extend CMOOSApp {
  bool Notify(const std::string &Var, const std::string &Val, double Time=-1) 
    { return self->m_Comms.Notify(Var, Val, Time); }
  bool Notify(const std::string &Var, double Val, double Time=-1) 
    { return self->m_Comms.Notify(Var, Val, Time); }

  bool Register(const std::string &Var, double Interval) 
    { return self->m_Comms.Register(Var, Interval); }
};

typedef std::list<CMOOSMsg> MOOSMSG_LIST;
%template(MOOSMSG_LIST) std::list<CMOOSMsg>;

%exception {
    try { $action }
    catch (Swig::DirectorException &e) { SWIG_fail; }
}
