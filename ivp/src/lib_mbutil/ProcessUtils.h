/*****************************************************************/
/*    NAME: Conlan Cesar                                         */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ProcessUtils.h                                       */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*****************************************************************/

#ifndef PROCESS_UTILS_HEADER
#define PROCESS_UTILS_HEADER

#include <string>
#include <vector>

// Run argv[0] with the remaining vector entries as literal arguments. No
// command shell is used. A detached process does not leave a zombie child.
bool runProcess(const std::vector<std::string>& argv, bool detached=false);

// As above, with stdout redirected to outfile. Pass an empty outfile to leave
// stdout unchanged. A detached process does not leave a zombie child.
bool runProcessToFile(const std::vector<std::string>& argv,
                      const std::string& outfile, bool detached=false);

#endif
