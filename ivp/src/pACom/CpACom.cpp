#include <iostream>
#include <cstdio>
#include <iterator>
#include <cstring>
#include <sstream>
#include <ctime>
#include "MOOSLib.h"
#include "MOOSGenLib.h"
#include "CpACom.h"

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

using namespace std;

// These return bool just to satisfy MOOSThread's signature requirements.
// The value isn't meaningful.
bool pingThread(void*);
bool sendThread(void*);
bool readThread(void*);

CpACom::CpACom() {
  serialPort = "/dev/ttyS2";
  srcModem = "1";
  dstModem = "2";
  initCommands = "";
  packetType = "0";
  fileDescriptor = -1;
  beVerbose = false;
  toSendPing = false;
  toSendData = false;
  toReadData = false;
  pingDelay = 5;
  sendDelay = 10;
  toCompressData = false;

  MAX_MESSAGE_LENGTH = 32;
  BUFFER_SIZE = 32;
  SEND_TIMEOUT = 2;
  RECIVE_TIMEOUT = 5;
  BYTE_LIMIT = 2;
  SEND_SLEEP = 300;
  READ_SLEEP = 300;
}

CpACom::~CpACom() {
  this->pingThreadPointer->Stop();
  this->sendThreadPointer->Stop();
  this->readThreadPointer->Stop();

  closeSerialPort();
}

bool CpACom::OnNewMail(MOOSMSG_LIST &NewMail) {
	MOOSMSG_LIST::reverse_iterator p;
	
	for(p = NewMail.rbegin(); p != NewMail.rend(); p++) {
		CMOOSMsg &msg = *p;
	}
  
  UpdateMOOSVariables(NewMail);
  
  return true;
}

bool CpACom::OnConnectToServer() {
//  m_Comms.Register("pACom_buffer", 0);
  return true;
}

bool CpACom::OnDisconnectFromServer() {
  pingThreadPointer->Stop();
  sendThreadPointer->Stop();
  readThreadPointer->Stop();

  closeSerialPort();
  
  return true;
}

bool CpACom::OnStartUp() {
  cout<<"\nStarting "<<m_sAppName<<endl;
  
  cout<<"Reading mission file "<<m_sMissionFile<<endl;
  if(!ReadMissionFile())
    return false;
  
  fileDescriptor = openSerialPort();
  if(fileDescriptor == -1 )
    return false;
 
  if(!initModem())
    return false;

  if(toSendPing) {
    this->pingThreadPointer = new CMOOSThread(pingThread, this);
    this->pingThreadPointer->Start();
  }
  else if(toSendData) {
    this->sendThreadPointer = new CMOOSThread(sendThread, this);
    this->sendThreadPointer->Start();
  }
  else if(toReadData) {
    this->readThreadPointer = new CMOOSThread(readThread, this);
    this->readThreadPointer->Start();
  }

  return true;
}

bool CpACom::Iterate() {
  // the different threads do everything...
  return true;
}

bool CpACom::ReadMissionFile() {
  if(!m_MissionReader.SetFile(m_sMissionFile)) {
    cout<<"Failed to read file "<<m_sMissionFile<<endl;
    return false;
  }

  m_MissionReader.SetAppName(m_sAppName);

  STRING_LIST sParams;
  if(!m_MissionReader.GetConfiguration(m_sAppName, sParams)) {
    cout<<"Faild to find configuration block for "<<m_sAppName<<
      " in file "<<m_sMissionFile<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("serial_port", serialPort))
    cout<<"Using serial port "<<serialPort<<endl;
  else {
    cout<<"No serial port specified "<<serialPort<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("src_address", srcModem))
    cout<<"Source address is "<<srcModem<<endl;
  else {
    cout<<"No modem source address specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("dst_address", dstModem))
    cout<<"Destination address is "<<dstModem<<endl;
  else {
    cout<<"No destination address specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("packet_type", packetType))
    cout<<"Using packet type "<<packetType<<endl;
  else {
    cout<<"No packet type specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("init_commands", initCommands))
    cout<<"Using modem initialization commands not specified"<<initCommands<<endl;
  else {
    cout<<"No modem initialization commands "<<endl;
    return false;
  }

  string str_val;
  if(m_MissionReader.GetValue("moos_variables", str_val)) {
    char vars[str_val.length()];
    strcpy(vars, str_val.c_str()); //stupid convertion crap

    char* moosVariable = strtok(vars, ",");
    char* myMOOSVariable= strtok(NULL, ";");

    while(moosVariable != NULL) {
      cout<<"Mapping MOOS variable "<<moosVariable<<" to "<<myMOOSVariable<<endl;

      AddMOOSVariable(myMOOSVariable, moosVariable, "", 0);

      moosVariables.push_back(string(myMOOSVariable));

      moosVariable = strtok(NULL, ",");
      myMOOSVariable= strtok(NULL, ";");
    }

    RegisterMOOSVariables();
  }
  else {
    cout<<"No MOOS variables given, check your .moos file"<<endl;
    return false;
  }
 
  if(m_MissionReader.GetValue("verbose", str_val)){
    if(str_val=="true") {
      beVerbose = true;
      cout<<"Will output verbosly"<<endl;
    }
    else
      cout<<"Will not output verbosly"<<endl;
  }
  else {
    cout<<"verbose variable not set"<<endl;
    return false;
  }
  
  if(m_MissionReader.GetValue("send_ping", str_val)){
    if(str_val=="true") {
      toSendPing = true;
      cout<<"Will send pings"<<endl;
    }
    else
      cout<<"Will not send pings"<<endl;
  }
  else {
    cout<<"send_ping variable not set"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("send_data", str_val)){
    if(str_val=="true") {
      toSendData = true;
      cout<<"Will send data"<<endl;
    }
    else
      cout<<"Will not send data"<<endl;
  }
  else {
    cout<<"send_data variable not set"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("read_data", str_val)){
    if(str_val=="true") {
      toReadData = true;
      cout<<"Will read data"<<endl;
    }
    else
      cout<<"Will not read data"<<endl;
  }
  else {
    cout<<"read_data variable not set"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("compress_data", str_val)){
    if(str_val=="true") {
      toCompressData = true;
      cout<<"Will compress data"<<endl;
    }
    else
      cout<<"Will not output verbosly"<<endl;
  }
  else {
    cout<<"verbose variable not set"<<endl;
    return false;
  }

  double val;
  if(m_MissionReader.GetValue("ping_delay", val)) {
    pingDelay = (int) val;
    cout<<"Ping intervall is "<<pingDelay<<endl;
  }
  else {
    cout<<"No ping_delay specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("send_delay", val)) {
    sendDelay = (int) val;
    cout<<"Data transmission intervall is "<<sendDelay<<endl;
  }
  else {
    cout<<"No send_delay specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("read_delay", val)) {
    readDelay = (int) val;
    cout<<"Data read intervall is "<<readDelay<<endl;
  }
  else {
    cout<<"No read_delay specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("max_message_length", val)) {
    MAX_MESSAGE_LENGTH = (int) val;
    cout<<"Maximum message length  "<<MAX_MESSAGE_LENGTH<<endl;
  }
  else {
    cout<<"Maximum message length not specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("buffer_size", val)) {
    BUFFER_SIZE = (int) val;
    cout<<"Buffer size "<<BUFFER_SIZE<<endl;
  }
  else {
    cout<<"Buffer size not specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("send_timeout", val)) {
    SEND_TIMEOUT = (int) val;
    cout<<"Send timeout "<<SEND_TIMEOUT<<endl;
  }
  else {
    cout<<"Send timeout not specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("receive_timeout", val)) {
    RECIVE_TIMEOUT = (int) val;
    cout<<"Recive timeout "<<RECIVE_TIMEOUT<<endl;
  }
  else {
    cout<<"Recive timeout not specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("byte_limit", val)) {
    BYTE_LIMIT = (int) val;
    cout<<"Byte limit "<<BYTE_LIMIT<<endl;
  }
  else {
    cout<<"Byte not specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("send_sleep", val)) {
    SEND_SLEEP = (int) val;
    cout<<"Send sleep "<<SEND_SLEEP<<endl;
  }
  else {
    cout<<"Send sleep specified"<<endl;
    return false;
  }

  if(m_MissionReader.GetValue("read_sleep", val)) {
    READ_SLEEP = (int) val;
    cout<<"Read sleep "<<READ_SLEEP<<endl;
  }
  else {
    cout<<"Read sleep not specified"<<endl;
    return false;
  }
  return true;
}

int CpACom::openSerialPort() {
  VERBOSE
  cout<<"Opening serial port for, please wait..."<<endl;

  int fd;

  fd = open(serialPort.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
  sleep(1);
 
  if (fd == -1) {
    string out = "Unable to open serial port " + serialPort;
    perror(out.c_str());
    return -1;
  }

	// blocking
  fcntl(fd, F_SETFL, 0);
  // non-blocking
//  fcntl(fd, F_SETFL, FNDELAY);


  struct termios options;
  tcgetattr(fd, &options);

  // set the baud rate to 19200
  cfsetispeed(&options, B19200);

  // enable the receiver and set 8 data bits plus local mode
  options.c_cflag |= (CS8 | CLOCAL | CREAD);

  // no parraty
//  options.c_cflag &= ~PARENB;
//  options.c_cflag &= ~CSTOPB;
//  options.c_cflag &= ~CSIZE;

  // disable hardware flow control:
//  options.c_cflag &= ~CNEW_RTSCTS;

  // set raw input
  options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // no parraty checking
  options.c_iflag |= (IGNPAR | ISTRIP);

  // disable software flow control
//  options.c_iflag &= ~(IXON | IXOFF | IXANY);
 
  // no post processes of output
//  options.c_oflag &= ~OPOST;

  // min number of char to read
  options.c_cc[VMIN] = 0;

  // time to wait for data 10ths of seconds
  options.c_cc[VTIME] = 0;

  // set the options
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd, TCSANOW, &options);

  sleep(2);
  return fd;
}

void CpACom::closeSerialPort() {
  VERBOSE
  cout<<"Closed port on " + serialPort<<endl;
  close(fileDescriptor);  
}

int CpACom::writeToSerialPort(string msg) {
  if(msg.length() > MAX_MESSAGE_LENGTH) {
    cout<<"Message "<<msg<<" is longer then "<<MAX_MESSAGE_LENGTH<<endl;
    return -1;
  }

  msg += "\r\n";

  int nbytes = write(fileDescriptor, msg.c_str(), msg.length());

  VERBOSE
  cout<<"Wrote "<<nbytes<<" bytes "<<msg;

  usleep(SEND_SLEEP*1000);

  return nbytes;
}

int CpACom::readFromSerialPort(string &msg) {
  char buffer[BUFFER_SIZE - 1];
  char *bufptr;
  int nbytes;

  // read 1 byte into buffer until we get a CR or NL
  bufptr = buffer;
  while ((read(fileDescriptor, bufptr, 1)) > 0) {
    usleep(READ_SLEEP*1000);

    bufptr++;

    if (bufptr[-1] == '\n') {
      // null terminate the string
      *bufptr = '\0';
      msg = string(buffer);
      nbytes = msg.size();

      VERBOSE
      cout<<"Read "<<nbytes<<" bytes "<<msg;
                        
      return nbytes;
      }
  }

  return 0;
}

bool CpACom::initModem() {
  int nbytes = 0; // bytes sent

  VERBOSE
  cout<<"Initializing modem..."<<endl;

  initCommands = "$CCCFG,SRC," + srcModem + ";" + initCommands;

  char commands[initCommands.length()];
  strcpy(commands, initCommands.c_str()); //stupid convertion crap

  char *command;
  command = strtok(commands, ";");

  while(command != NULL) {
    string snd_str = string(command); //stupid convertion crap
    nbytes = writeToSerialPort(snd_str);

    if(nbytes < 1)
      return false;

    command = strtok(NULL, ";");
  }

  sleep(3);
  return true;
}

void CpACom::ping() {
  string startPing = "$CCMPC," + srcModem + "," + dstModem;
  string confirmPing = "$CAMPC," + srcModem + "," + dstModem;
  string receivedPing = "$CAMPR," + dstModem + "," + srcModem;
  string msg;

  VERBOSE
  cout<<"Sending ping..."<<endl;
  writeToSerialPort(startPing);

  int nbytes = 0;
  time_t endTime = time(NULL) + SEND_TIMEOUT;

  while(time(NULL) < endTime) {
    nbytes = readFromSerialPort(msg);

    if(msg.find(confirmPing) == 0) {
      VERBOSE
      cout<<"Recived ping confirmation, sending ping..."<<endl;
      nbytes = 0;
      endTime = time(NULL) + RECIVE_TIMEOUT;

      while(time(NULL) < endTime){
        nbytes = readFromSerialPort(msg);

        if(nbytes > 0)
          if(msg.find(receivedPing) == 0) {
            string travelTime = msg.substr(receivedPing.length() + 1);
            int star = travelTime.find("*");
            if(star != string::npos)
              travelTime = travelTime.substr(0, star);

            double pingTime = atof(travelTime.c_str());
            if(pingTime > 0) {
              cout<<"Ping from " + srcModem + " to " + dstModem +
                ": "<<pingTime<<"s"<<endl;

              m_Comms.Notify("PING_TIME", pingTime);
              sleep(pingDelay);
            }
          }
      }

      VERBOSE
      cout<<"No ping received in "<<RECIVE_TIMEOUT<<"s"<<endl;
     } 
  }

  VERBOSE
  cout<<"No ping confirmation in "<<SEND_TIMEOUT<<"s"<<endl;
}

bool CpACom::sendData() {
  string cycleInitCommnad = "$CCCYC,1," + srcModem + "," + dstModem + "," +
    packetType + ",0,1";
  string cycleInitAcknowledge = "$CACYC,1," + srcModem + "," + dstModem + "," +
    packetType + ",0,1";

  writeToSerialPort(cycleInitCommnad);
  ostringstream oss;

  for(int i = 0; i < moosVariables.size(); i++) {
    
    CMOOSVariable* moosVariable = GetMOOSVar(moosVariables.at(i).c_str());

    if(moosVariable == NULL) {
      cout<<"Could not find MOOS variable "<<moosVariables.at(i)<<endl;
      return false;
    }

    if(toCompressData) {
      if(i > 0)
        oss<<";";

      double val = moosVariable->GetDoubleVal();
      char tmp[10];
      oss<<gcvt(val, 5, tmp);
    }

    else {
      if(moosVariable->IsDouble()) {
        oss<<moosVariables.at(i)<<"="<<moosVariable->GetDoubleVal();
      }

      else
        oss<<moosVariables.at(i)<<"="<<moosVariable->GetStringVal();
    }
  }  

  string inMessage;
  string outMessage = "$CCTXA," + srcModem + "," + dstModem + ",0," + oss.str();
  
  int nbytes = 10;
  time_t endTime = time(NULL) + SEND_TIMEOUT;
  while(time(NULL) < endTime) {

    nbytes = readFromSerialPort(inMessage);
     
    if(inMessage.find(cycleInitAcknowledge) == 0) {
      writeToSerialPort(outMessage);
      break;
    }
  }

  if(nbytes < BYTE_LIMIT) {
    VERBOSE
    cout<<"No cycle-init command acknowledgement in "<<SEND_TIMEOUT<<
      "s"<<endl;
    
    return false;
  }

  sleep(sendDelay);
  return true;
}



bool CpACom::readData() {
  string dataRXCommand = "$CARXA," + dstModem + "," + srcModem + ",0";
  string msg;

  int nbytes = 0;
  time_t endTime = time(NULL) + RECIVE_TIMEOUT;

  while(time(NULL) < endTime) {
    nbytes = readFromSerialPort(msg);

    if(msg.find(dataRXCommand) == 0) {
 
      string data = msg.substr(dataRXCommand.length() + 3);

      char data_array[data.length()];
      strcpy(data_array, data.c_str());
 
      if(toCompressData) {
        char* chr_val = strtok(data_array, ";");

        if(chr_val != NULL)
          for(int i = 0; i < moosVariables.size(); i++) {
            string var = moosVariables.at(i);
            double val = atof(chr_val);

            VERBOSE
            cout<<"\n got "<<var<<" = "<<val<<endl<<endl;

            m_Comms.Notify(var, val);
            chr_val = strtok(NULL, ";*");
            if(chr_val == NULL)
              chr_val = "0";
          }

        return true;
      }

      else {
        char* var = strtok(data_array, "=");
        char* chr_val = strtok(NULL, "*");
      
        if(chr_val != NULL) {
          VERBOSE
            cout<<"\n got "<<var<<"="<<atof(chr_val)<<endl<<endl;

          m_Comms.Notify(var, atof(chr_val));
          return true;
        }
      }
    }
  }

//  VERBOSE
//  cout<<"No data received in "<<RECIVE_TIMEOUT<<"s"<<endl;

  sleep(readDelay);
  return false;
}

bool pingThread(void* arg) {
  while(true)
    ((CpACom*) arg)->ping();
  return true;
}

bool sendThread(void* arg) {
  while(true)
    ((CpACom*) arg)->sendData();
  return true;
}

bool readThread(void* arg) {
  while(true)
    ((CpACom*) arg)->readData();
  return true;
}
