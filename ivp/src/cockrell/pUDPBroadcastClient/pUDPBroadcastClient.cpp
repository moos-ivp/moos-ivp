//pUDPBroadcastClient.cpp    Kevin Cockrell, MIT, August 2007


#include "pUDPBroadcastClient.h"

//***************************************************************************
//*  CpUDPBroadcastClient::CpUDPBroadcastClient()                                                 *
//*                                                                         *
//*  The constructor for the CBearings class. Variables can be initialized  *
//*  here.                                                                  *
//*                                                                         *
//***************************************************************************

using namespace std;

CpUDPBroadcastClient::CpUDPBroadcastClient()
{
    
}

//***************************************************************************
//*  CpUDPBroadcastClient::~CpUDPBroadcastClient()                                                *
//*                                                                         *
//*  The destructor for the CpUDPBroadcastClient class. Dynamic memory can be freed    *
//*  here.                                                                  *
//***************************************************************************

CpUDPBroadcastClient::~CpUDPBroadcastClient()
{
  //DOES ANYTHING NEED TO HAPPEN IN THIS DESTRUCTOR?    
}

//***************************************************************************
//*  CpUDPBroadcastClient::OnConnectToServer()                                          *
//*                                                                         *
//*  Called by the MOOSApp base class upon connecting to the MOOS database. *
//*  Register for variables here.                                           *
//*                                                                         *
//***************************************************************************

bool CpUDPBroadcastClient::OnConnectToServer()
{
  //( p 339-341)
  so_reuseaddr = 1;

  string bc_addr_string("*:9097");

  if(m_MissionReader.GetConfigurationParam("BroadcastAddress", bc_addr_string))
    printf("\nUsing BroadcastAddress from .moos file: '%s'\n", bc_addr_string.c_str());
  else
    printf("\nNo BroadcastAddress specified in .moos file. Using default: '%s'\n", bc_addr_string.c_str());

  bc_addr = strdup(bc_addr_string.c_str());

  // Create UDP socket
  s = socket(AF_INET, SOCK_DGRAM, 0);
  if ( s == -1)
    MOOSTrace("Error creating UDP socket");

  // Form broadcast address
  len_inet = sizeof adr;
  z = mkaddr(&adr, &len_inet, bc_addr, "udp");
  if (z == -1)
    MOOSTrace("Error forming broadcast address");

  // Allow mutiple listenerts on same address
  z = setsockopt(s, SOL_SOCKET, SO_REUSEADDR,&so_reuseaddr, sizeof so_reuseaddr);

  // Bind socket to broadcast address:
  z = bind(s,
      (struct sockaddr *)&adr,
      len_inet);
  if(z == -1)
    MOOSTrace("Error binding broadcast address");

  return true;
}

//***************************************************************************
//*  CpUDPBroadcastClient::OnNewMail()                                                 *
//*                                                                         *
//*  Called by the MOOSApp base class when a variable this app is subscribed*
//*  to changes.                                                            *
//*                                                                         *
//***************************************************************************/

bool CpUDPBroadcastClient::OnNewMail(MOOSMSG_LIST &NewMail)
{
  CMOOSMsg Msg;

  MOOSMSG_LIST::reverse_iterator p;
  for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
    CMOOSMsg &Msg = *p;

    //Leave some example commands in here...
    /* if(Msg.m_sKey == "PROSECUTE_STATE")
       {
    //MOOSTrace("Got PROSECUTE_STATE message");
    string val;
    val = Msg.m_sVal;
    if(MOOSStrCmp(val,"PROSECUTE"))
    {
    m_Comms.Notify("TRACK_CONTROL","ON");
    }
    }
    else if(Msg.m_sKey == "DEPLOY_STATE")
    {
    }
    else if(Msg.m_sKey == "NAV_Y")
    {	
    auv_y = Msg.m_dfVal;
    }
    else
    MOOSTrace("Unrecognized command: [%s]\n",Msg.m_sKey.c_str());  
    */
  }
  return true;
}

//***************************************************************************
//*  CpUDPBroadcastClient::Iterate()                                                  *
//*                                                                         *
//*  Called by the MOOSApp base class at a fixed rate specified in the      *
//*  .moos file. Calls to this method trigger processing of data that is    *
//*  ready to be processed.                                                  *
//****************************************************************************/
bool CpUDPBroadcastClient::Iterate()
{
  // NOTE: recvfrom just sits there until it gets something, so iterate will only be called
  // once each time recvfrom actually recieves a packet

  
  z = recvfrom(s, // socket
      dgram,          // receiving buffer
      sizeof dgram,    // max rcv buffer size
      0,              // Flags: no options
      (struct sockaddr *)&adr, //addr
      (socklen_t*)&len_inet); // addr len, in & out (NOTE I HAD TO TYPE CAST THE INT)

  if (z == -1)
    MOOSTrace("Error recieving UDP packet");

    printf("Received Packet: %s", dgram);

   // Publish recieved packet as a string to the MOOSDB
      m_Comms.Notify("BF_UDP_Broadcast",dgram);


}
//***************************************************************************
//*  CpUDPBroadcastClient::OnStartUp()                                                 *
//*                                                                         *
//*  Called by the MOOSApp base class when tis application is started       *
//*                                                                         *
//***************************************************************************

bool CpUDPBroadcastClient::OnStartUp()
{  


  return true;
}

int CpUDPBroadcastClient::mkaddr(void *addr, int *addrlen, char *str_addr, char *protocol)
{
  // (p243-248)
  //
  // Create a socket address

  //Insert documentation on arguments here
  //
  // Returns:
  // 0   = Sucess
  // -1  = Bad host part.
  // -2  = Bad port part.
  //
  // Examples:
  // "www.lwn.net:80"
  // "localhost:telnet"
  // "*:21"
  // "*:*"
  // "ftp.redhat.com:ftp"
  // "sunsite.unc.edu"
  // "sunsite.unc.edu:*"

  char *inp_addr = strdup(str_addr); // Duplicate input string
  char *host_part = strtok(inp_addr,":"); // Prase out host string
  char *port_part = strtok(NULL,"\n"); // Parse out port string
  struct sockaddr_in *ap = (struct sockaddr_in *) addr;
  struct hostent *hp = NULL;
  struct servent *sp = NULL;
  char *cp;
  long lv;


  // Setup input defaults
  if (!host_part)
    host_part = "*";
  if (!port_part)
    port_part = "*";
  if (!protocol)
    protocol = "tcp";

  // Initilize the address structure
  memset(ap,0,*addrlen);
  ap->sin_family = AF_INET; // (p48) set the address family to AF_INET
  ap->sin_port = 0; // (p48) Set the port number 
  ap->sin_addr.s_addr = INADDR_ANY; // (p48, IP address??)

  // Fill in the host address
  if(strcmp(host_part,"*") == 0)
    ; 
  else if ( isdigit(*host_part) )
  {
    ap->sin_addr.s_addr = inet_addr(host_part);
    //if ( ap->sin_addr.s_addr == INADDR_NONE) (from ch10)
    if ( !inet_aton(host_part, &ap->sin_addr) ) //p331 correction. see p77 for inet_aton
      return -1;
  }
  else
  {
    // Assume a hostname
    hp = gethostbyname(host_part);
    if (!hp)
      return -1;
    if (hp->h_addrtype != AF_INET )
      return -1;
    ap->sin_addr = *(struct in_addr *) hp->h_addr_list[0];
  }

  // Process an optional port number
  if (!strcmp(port_part, "*"))
    ;
  else if ( isdigit(*port_part))
  {
    // Process numeric port number
    lv = strtol(port_part, &cp,10);
    if (cp!= NULL && *cp)
      return -2;
    if (lv < 0L || lv > 32768)
      return -2;
    ap->sin_port = htons((short)lv);
  }
  else
  {
    // Lookup the service by name
    sp = getservbyname( port_part, protocol);
    if (!sp)
      return -2;
    ap->sin_port = (short) sp->s_port;
  }

  *addrlen = sizeof *ap;

  free(inp_addr);
  return 0;
}

