// $Header: /home/cvsroot/project-marine-shell/src/pACom/CpACom.h,v 1.17 2005/11/12 22:27:08 osagie Exp $
// CpACom.h: interface for the CpACom class.

#ifndef __CpACom_h__
#define __CpACom_h__

#include <cstdio>
#include <string>
#include <vector>
#include "MOOSLib.h"
#include "MOOSThread.h"

/**
 * Computes the length of an array.
 */
#ifndef LENGTH
#define LENGTH(array) (sizeof array)/(sizeof array[0])
#endif

#ifndef VERBOSE
#define VERBOSE if(beVerbose)
#endif

using namespace std;

/** 
 * Simple MOOS program that uses the WHOI Micromodem for acoustic communication
 * (see http://acomms.whoi.edu/micromodem/). The program is able to send pings,
 * transmit data and receive data. Ping time values are stored in the MOOS
 * variable \c PING_TIME and message data is stored in the variable specified in
 * the configuration block of pACom.
 *
 * Even though this is a MOOS application most of the code relies very little
 * on MOOS. This allows the code to be used in applications outside MOOS.
 *
 * Port setting, like baudrate, are still hardcode, this might change later on.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place - Suite 330, Boston, MA 02111-1307, USA. 
 * 
 * @author Peter Osagie
 *
 * @version $Revision: 1.17 $
 *
 * @date $Date: 2005/11/12 22:27:08 $
 *
 * Contact: osagie@mit.edu
 *
 * Created on: June 29 2005
 *
 * $Id: CpACom.h,v 1.17 2005/11/12 22:27:08 osagie Exp $
 */
class CpACom: public CMOOSApp {
  public:
  /**
   * Default constructor.
   *
   * Variable initialization is done here.
   */
  CpACom();

  /**
   * Default destructor.
   *
   * Kills all associated threads.
   */
  ~CpACom();

  /**
   * All MOOS variables are updated here.
   *
   * @return true if the mail was successfully read.
   */
  bool OnNewMail(MOOSMSG_LIST &NewMail);

  /**
   * Called AppTick times per second, but all program executions are preformed
   * in the different threads.
   *
   * ùX¬@return true if no error error accrued, false otherwise.
   */
  bool Iterate();

  /**
   * Called when successfully connected to the server.
   * 
   * @return true if the application successfully connected to the \c MOOSDB,
   * false otherwise.
   */
  bool OnConnectToServer();

  /**
   * Called when successfully disconnecting server.
   * 
   * @return true if the application successfully disconnected the \c MOOSDB,
   * false otherwise.
   */
  bool OnDisconnectFromServer();
  
  /**
   * Called when the application starts.
   *
   * Starts the different threads, pingThread, sendThread or readThread
   * depending on Boolean variables in configuration block for pACom.
   *
   * @return true if the application started up successfully, false otherwise.
   */
  bool OnStartUp();
  
  protected:
  /**
   * Reads the settings from the configuration block for pACom.
   * The variables serialPort, srcModem, dstModem, initCommands will be set
   * here.
   *
   * @return true if the file was read successfully, false otherwise.
   */
  bool ReadMissionFile();

  /**
   * Uses the string from initCommands to initialize the modem. The address given
   * by srcModem is also set here.
   *
   * @return true if the modem was successfully initialized, false otherwise.
   */
  bool initModem();

  /**
   * Open serial port given by SerialPort in the configuration block for pACom.
   *
   * NB! Port options like baudrate are are hardcoded.
   *
   * Code obtained from http://www.easysw.com/~mike/serial/serial.html
   *
   * @return the file descriptor on success or -1 on error.
   */
  int openSerialPort();

  /**
   * Closes the serial port given by fileDescriptor.
   */
  void closeSerialPort();

  /**
   * Writes to the serial port.
   *
   * @param msg the message to write, which has to be less then
   * MAX_MESSAGE_LENGTH.
   *
   * @return the number of bytes sent or -1 if an error occurred.
   */
  int writeToSerialPort(string msg);

  /**
   * Reads from the serial port.
   *
   * @param msg the message read from the port.
   *
   * @return the number of bytes read.
   */
  int readFromSerialPort(string &msg);

  public:
  /**
   * Sends a ping.
   * 
   * The destination modem automatically responds to the ping and the source
   * modem reads the response time and posts that value in the MOOS variable
   * PING_TIME.
   *
   * This function is called every ping_delay seconds specified in the
   * configuration block for pACom.  
   */
  void ping();

  /**
   * Sends the MOOS variable values for the variables specified in the
   * configuration block for pACom.
   * 
   * Called by the send data thread, sendThread(), where it sends data to the
   * port in intervals specified by send_delay in the configuration pACom.
   * Function blocks the port for SEND_TIMEOUT seconds.
   *
   * @return true if the data was successfully sent to the port, false
   * otherwise.
   */
  bool sendData();

  /**
   * Called by the read data thread, readThread(), and polls the modem every
   * read_delay seconds for a total length of RECIVE_TIMEOUT.
   *
   * Data is stored in the MOOS variables specified by configuration block.
   *
   * @return true if data was read successfully false otherwise.
   */
  bool readData();

  private:

  /**
   * The ping thread pointer.
   */
  CMOOSThread *pingThreadPointer;

  /**
   * The send data thread pointer.
   */  
  CMOOSThread *sendThreadPointer;

  /**
   * The read data thread pointer.
   */
  CMOOSThread *readThreadPointer;

  /**
   * Object used when reading from the mission file (\c .moos file).
   */
  CProcessConfigReader m_MissionReader;

  /**
   * Serial port to use.
   */
  string serialPort;

  /**
   * The address of the host modem (the modem we use for communicating).
   */
  string srcModem;

  /**
   * The address of the destination modem (the modem we want to communicate to).
   */
  string dstModem;

  /**
   * Modem initialization commands are stored here.
   * The command for modem source address is set automatically by initModem().
   */
  string initCommands;

  /**
   * Packet typ to use when sending data.
   * \n
   * packet type #frames bytes/frame bytes/packet bps\n
   * 0           1       32          32           80\n
   * 1           3       32          96           250\n
   * 2           2       64          192          500\n
   * 3           2       256         512          1200\n
   * 4           2       256         512          1300\n
   * 5           8       256         2048         5300
   */
  string packetType;

  /**
   * File descriptor referring to the serial port.
   */
  int fileDescriptor;

  /**
   * Set to true in the .moos file in order to output continuous information.
   */
  bool beVerbose;

  /**
   * Set true if the modem is pinging.
   * N.B! Only source modem needs this set to true. Destination modem will
   * respond to pings automatically.
   */
  bool toSendPing;

  /**
   * Set true if modem is to send data.
   */
  bool toSendData;

  /**
   * Set true if modem is to read check for incoming data.
   */
  bool toReadData;

  /**
   * Set true if data should be sent in a predefined format. No variable names
   * will be sent, but just their values so that they fit in one 32 byte
   * message.
   */
  bool toCompressData;

  /**
   * Use the names when sending receiving moos variables. That is the MOOSDB
   * mapped names are stored here. Check your \c .moos file for more info.
   */
  vector<string > moosVariables;

  /**
   * Number of seconds between each ping.
   */
  int pingDelay;

  /**
   * Number of seconds between each variable transmission.
   */
  int sendDelay;

  /**
   * Number of seconds between each check for incoming data.
   */
  int readDelay;

  /**
   * The maximum message length when sending.
   * Sending is done in ASCII where a single character is 1 byte. So this is the
   * total in bytes.
   */
  int MAX_MESSAGE_LENGTH;

  /**
   * The input buffer size in bytes.
   * It is assumed that the data on the serial port is in ASCII, so a single
   * character is 1 byte. So this is the total size of the buffer + 1 in bytes.
   */
  int BUFFER_SIZE;

  /**
   * Timeout in seconds for sending ping messages.
   */
  int SEND_TIMEOUT;

  /**
   * Timeout in seconds for waiting for a ping to return.
   */
  int RECIVE_TIMEOUT;

  /**
   * The least number of bytes to be regarded as a transmission.
   * Sometimes when sending a message a single byte can be echoed back. Any
   * message with a byte size less then this will be disregarded.
   */
  int BYTE_LIMIT;

  /**
   * Time in ms to wait after a command is sent to the modem.
   */
  int SEND_SLEEP;

  /**
   * Time in ms to wait after reading from the serial port.
   */
  int READ_SLEEP;
};
#endif
