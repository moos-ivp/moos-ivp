/*****************************************************************************
 *
 ****************************************************************************/

#include "ProcessWin.h"

#ifdef _WIN32



ProcessWin::ProcessWin(){
   memset(&m_startup_info, 0, sizeof(m_startup_info));
   memset(&m_process_info, 0, sizeof(m_process_info));
}

ProcessWin::~ProcessWin(){
	::CloseHandle(m_process_info.hThread);
	::CloseHandle(m_process_info.hProcess);
}

bool ProcessWin::start(){

	// Check if the launch command has been set
    if( m_args.size() < 2 ){
        return false;
    }

	std::list<std::string>::iterator it;
	std::string command = "";
	for(it = m_args.begin(); it != m_args.end(); it++){
		command.append( (*it) );	
		command.append(" ");
	}

	m_startup_info.cb = sizeof(m_startup_info);

	LPWSTR cmd = (LPWSTR) _T( command.c_str() );

	if(CreateProcess( (LPCSTR) NULL, // Application name
	(LPSTR) cmd, // Additional application arguments
	NULL, /* LPSECURITY_ATTRIBUTES lpProcessAttributes */
	NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes */
	FALSE, /* bInheritHandles */
	CREATE_NEW_CONSOLE, /* dwCreationFlags */
	NULL, /* lpEnvironment */
	NULL, /* lpCurrentDirectory */
	&m_startup_info, /* lpStartupInfo */
	&m_process_info /*lpProcessInformation*/      ) == FALSE){
		printf("Failed to spawn child.\n");
		return false;
	} else {
		printf("Spawned child.\n");
		// Wait until application has terminated
		
   m_job = CreateJobObject(NULL, NULL);
		if( AssignProcessToJobObject(m_job, m_process_info.hProcess)){
			printf("Associated pAntler job.\n");
		} else {
			printf("Failed to associate pAntler job\n");
			return false;
		} 
		
	}
	
	return true;
} // END start

bool ProcessWin::stop(){
   if( TerminateJobObject(m_job, 0 ) ){
      printf("Killed child\n");
	  return true;
   } else {
      printf("Failed to kill child\n");
	  return false;
   }
}

bool ProcessWin::isAlive(){

   return false;
}


#endif // _WIN32

/*****************************************************************************
 *
 ****************************************************************************/

