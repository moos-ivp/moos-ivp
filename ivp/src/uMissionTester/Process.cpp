/*****************************************************************************
 *
 ****************************************************************************/

#include "Process.h"

Process::Process(){

}
Process::~Process(){


}


bool Process::setCommand(char* app_name, ...){
    
    m_args.push_back(app_name);
    va_list ap;
    va_start(ap, app_name);
	char* arg;
	while( (arg = va_arg(ap, char*) ) ){
        m_args.push_back( arg );
    }

	return true;
}




/*****************************************************************************
 *
 ****************************************************************************/
