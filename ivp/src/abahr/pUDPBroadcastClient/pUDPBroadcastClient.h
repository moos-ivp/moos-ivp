
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

class CpUDPBroadcastClient : public CMOOSApp
{
 public:

  CpUDPBroadcastClient();
  virtual ~CpUDPBroadcastClient();

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:

  //From p245 of "Linux Socket Programming", 2000
  int mkaddr(void *addr, int *addrlen, char *str_addr, char *protocol);
  
  // p(339-341)
  int z;
  int x;
  struct sockaddr_in adr; // AF_NET
  int len_inet; // length
  int s; // socket
  char dgram[4096]; //Recv buffer
  int so_reuseaddr;
  char *bc_addr;
};
