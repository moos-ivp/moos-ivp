/*
 * CommsTools.cpp
 *
 *  Created on: Mar 1, 2014
 *      Author: pnewman
 */
#if !defined(_WIN32)
  #include <sys/time.h>
  #include <sys/types.h>
  #include <unistd.h>
  #include <sys/socket.h>
  #include <sys/select.h>
  #include <arpa/inet.h>
#else
  #include <winsock2.h>
  #include <stdio.h>
  #include <stdlib.h>
#endif

#include <iostream>

namespace MOOS
{
bool WaitForSocket(int fd, int nTimeoutSeconds)
{

    // The socket file descriptor set is cleared and the socket file
    // descriptor contained within tcpSocket is added to the file
    // descriptor set.

    fd_set fdset;// Set of "watched" file descriptors
    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);

    // The timeout value for the select system call
    struct timeval timeout;
    timeout.tv_sec    = nTimeoutSeconds;
    timeout.tv_usec = 0;


    // A select is setup to return when data is available on the socket
    // for reading.  If data is not available after timeout seconds, select
    // returns with a value of 0.  If data is available on the socket,
    // the select returns and data can be retrieved off the socket.
    int iSelectRet = select(fd + 1,
       &fdset,
       NULL,
       NULL,
       &timeout);

    // If select returns a -1, then it failed and the thread exits.
    switch(iSelectRet)
    {
    case -1:
       std::cerr<<"system select call failed\n";
       return false;

    case 0:
       //timeout...nothing to read
       return false;

    default:

       if (FD_ISSET(fd, &fdset) != 0)
       {
           //something to do read:
           return true;
       }
       break;
    }
    return false;
}


}
