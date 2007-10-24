/*
 *  umodem.h  - wrapper around umodem specific calls 
 */             

// iMicroModem MOOS Modem driver, was developed 
// by Matt Grund, Woods Hole Oceanographic Institution
//
// This code is licensed under a Creative Commons
// Attribution Non-Commercial Share-A-Like License,
// version 2.5.
//
// For more information, see the file License.html
// or the web site:
//
//  http://creativecommons.org/licenses/by-nc-sa/2.5/
//
// Copyright(c)2004, Matt Grund, WHOI. <mgrund@whoi.edu>
//
// some debugging by Alex Bahr, MIT abahr@mit.edu
// modified/added lines are marked by
// "modified/added by abahr"


#ifndef _UMODEM_H
#define _UMODEM_H


#include "nmea.h"
#include <time.h>
/* states for u.state.status */
#define UMODEM_UNKNOWN        (0)   // on fire? ;)
#define UMODEM_CONFIGERROR    (1)   // couldn't open
#define UMODEM_CONFIGURING    (2)   // sent config command, waiting for reply
#define UMODEM_IDLE           (3)   // nothing happening
#define UMODEM_TRANSMITTING   (4)   // data request or transmitting
#define UMODEM_RECEIVING      (5)   
#define UMODEM_MONITORING     (6)
#define UMODEM_INITIATING     (7)
#define UMODEM_PINGING        (8)
#define UMODEM_COMMANDING     (9)
#define UMODEM_NAVIGATING     (10)
#define UMODEM_CALIBRATING    (11)
#define UMODEM_REVERTED       (12)  // if modem reverted
#define UMODEM_GOTERROR       (13)  // if the modem reported an error
#define UMODEM_GOTBADCRC      (14)  // if the modem reported a bad CRC
#define UMODEM_GOTTIMEOUT     (15)  // if the modem reported packet timeout
#define UMODEM_COMMANDED      (16)
#define UMODEM_ACKNOWLEDGING  (17)
#define UMODEM_TXON           (18)
#define UMODEM_TXOFF          (19)
#define UMODEM_GOTPINGED      (20)
#define UMODEM_REQUESTINGDATA (21)
#define UMODEM_GOTDATA        (22)

/* settings */
//#define UMODEM_MAXNODES     (16)
#define UMODEM_MAXNODES     (127)
#define UMODEM_MAXFRAMES    (8)
#define UMODEM_MAXFRAMESIZE (256)
#define UMODEM_DEV_ERROR    (-1)
#define UMODEM_OK           (42)
#define UMODEM_MSG_TIMEOUT  (45)   /* seconds */
#define UMODEM_MAXDATA      (256)
#define UMODEM_BUFLEN       (320)
#define UMODEM_MAXMSG       (UMODEM_MAXDATA*2+32)

// MicroModem device
//  
//
typedef struct UmodemDev {
	time_t        lastread;
	int           fd;
	char          name[80];
	int           baud;
	unsigned char buf[UMODEM_BUFLEN];
	unsigned int  buflen;
	char          lastmsg[UMODEM_MAXMSG];
	char          txmsg[UMODEM_MAXMSG];
	char          lasterrmsg[80];
} UmodemDevObj;

// Micromodem configuration

//  Generic data frame - Tx or Rx
//
//
typedef struct {
        int iSrc;
        int iDest;
        int bAck;
        int bValid;
        int iType;
        int iFrameNum;
        unsigned char caData[UMODEM_MAXFRAMESIZE];
        int iSize;
} sDataFrame ;


typedef struct {
	 // counters
     int    nrxpkts;
     int    ntxpkts;
     int    ncycles;
     int    nbadpkts;

	 // QoS
     int    lastdqf;
     float  dopshift;
     long   mfdpeak;
     int    mfdpwr;
     int    mfdratio;

	 // range
     time_t lastping;
     float  owtt;

	 // contact
     time_t lastrx;
     time_t lasttx;
} UmodemLinkObj;


typedef struct {
	int    status;
	char   statusmsg[80];
	int    address;
	int    pkttimeout;
	int    nrevert;
	time_t mtime;
	float  vbatt;
	int    noiseint;
	int    noise[14];
	int    lastdqf;
} UmodemStateObj;



typedef struct 
{
	unsigned char buf[UMODEM_MAXDATA];
	int           retries;
	char          ack;
	int           size;
} UmodemTxDataFrameObj;

typedef struct {
	UmodemTxDataFrameObj frame[UMODEM_MAXFRAMES];
	int                  src;
	int                  dest;
	int                  ackrequested; 
	int                  requested;
	int                  dataacksize;
	int                  nsent;
	int                  frameno;
} UmodemTxDataObj;

typedef struct {
  int           src;
  int           dest;
  int           frameno;
  time_t        time;
} UmodemAckObj;

typedef struct {
  sDataFrame    frame;
  time_t        rxtime;
} UmodemRxDataObj;

typedef struct {
  unsigned int   mode;
  unsigned int   src;
  unsigned int   dest;
  unsigned int   ack;
  unsigned int   pkttype;
  unsigned int   npkts;
} UmodemCycleObj;

typedef struct {
     unsigned int   src;
     unsigned int   dest;
     float          owtt;
} UmodemPingObj;

typedef struct {
	unsigned int   src;
	unsigned int   dest;
	unsigned int   arg;
} UmodemSleepObj;

typedef struct {
	unsigned int src;
//	float time;
	double time;
	int mode;
} UmodemArrivalTimeObj;

typedef struct {
  unsigned int   src;
  unsigned int   dest;
  unsigned int   mode;
  unsigned int   line;
  unsigned int   val;
} UmodemExtSelObj;

typedef struct 
{
     unsigned int timeout;
     unsigned int txfreq;
     unsigned int txtau;
     unsigned int rxtau;
     unsigned int rxfreqs[4];
} UmodemLBLPingObj;

typedef struct 
{
     float timestamp;
     float owtt[4];
} UmodemTravelTimeObj;

typedef struct
{
	char name[4];
	int  value;
	char requested_name[4];
	int  requested_value;
} UmodemCfgObj;

	


// Micromodem Object - tracks state, stores data
//
typedef struct {
	UmodemDevObj     dev;
	UmodemCfgObj     cfg;
	UmodemStateObj   state;
	UmodemLinkObj    link[UMODEM_MAXNODES];
	UmodemRxDataObj  rx;
	UmodemTxDataObj  tx;
	UmodemAckObj     ack;
	UmodemCycleObj   cycle;
	UmodemPingObj    ping;
	UmodemExtSelObj  extsel;
	UmodemSleepObj   sleep;
	UmodemLBLPingObj lbl;
	UmodemTravelTimeObj ttimes;
	UmodemArrivalTimeObj toa;
} UmodemObj ;


/* protptypes */
int  UmodemInit(void);
int  UmodemServe(void);
int  UmodemParse(char *msg);
int  UmodemOpen(char *dev,int baud);
int  UmodemClose(void);
char *UmodemConfig(char *set, int val);
char *UmodemPing(int src,int dest);
char *UmodemLBLPing(int timeout, int txfreq, int txtau, int rxtau, 
		    int rxfreq1,int rxfreq2,int rxfreq3, int rxfreq4);
char *UmodemSetLine(int src,int dest,int line,int val);
char *UmodemSleep(int src,int dest,int arg);
void UmodemData(char *msg);
char *UmodemDoCycle(int src, int dest, int pkttype, int ack);
char *UmodemTx(void);
char *UmodemSendAck(int src, int dest, int ackword);
void UmodemAckCleanup(void);
void UmodemAckProc(void);
void UmodemFrameMove(int from, int to);
void UmodemSetRetries(int retries);
void UmodemFlushTxData(void);

/* read from structs */
char *UmodemGetLastErrorMsg(void);
char *UmodemGetLastCfgName(void);
int UmodemGetLastCfgValue(void);
int UmodemGetAddress(void);
int UmodemGetPktTimeout(void);
int UmodemGetLastDQF(void);
int UmodemGetCISrc(void);
int UmodemGetCIDest(void);
int UmodemGetCIMode(void);
int UmodemGetCIAck(void);
int UmodemGetCIPktType(void);
int UmodemGetRXSrc(void);
int UmodemGetRXDest(void);
int UmodemGetRXDataSize(void);
int UmodemGetNReverts(void);
int UmodemGetLastPingSrc(void);
int UmodemGetLastPingDest(void);
float UmodemGetLastPingOWTT(void);
float *UmodemGetLastLBLTT(void);
int UmodemGetExtSelSrc(void);
int UmodemGetExtSelDest(void);
int UmodemGetExtSelMode(void);
int UmodemGetExtSelLine(void);
int UmodemGetExtSelVal(void);
int UmodemGetSleepSrc(void);
int UmodemGetSleepDest(void);
int UmodemGetSleepArg(void);
char *UmodemGetLastMsg(void);
sDataFrame *UmodemGetRXData(void);
char *UmodemGetTXDataBuf(void);
int UmodemGetTXDataSize(void);
int UmodemGetTXFrameNo(void);
int UmodemGetTXQSize(void);
int UmodemGetTXDataAckSize(void);
int UmodemGetTXDataAck(void);
int UmodemGetTXDataAckRequested(void);
int UmodemGetTXDataDest(void);
void UmodemShowRXData(void);
float UmodemGetLastVoltage(void);
int UmodemGetLastAckSrc(void);
int UmodemGetLastAckFrameNo(void);
char *UmodemNoiseCheck(int msec);
void UmodemNoise(char *msg);
int UmodemGetMFDPwr(void);
long UmodemGetMFDPeak(void);
int UmodemGetMFDRatio(void);
float UmodemGetDoppler(void);
int UmodemFrameSize(int pkttype);
int UmodemGetStatusInt(void);
char *UmodemGetStatusString(void);
int UmodemSetStatus(int status);
char *UmodemMakeTxString(sDataFrame *f);
char *UmodemMakePingCmd(int iSrc, int iDest);
char *UmodemMakeCICmd(int iSrc, int iDest, int iRate, int iAck);
char *UmodemMakeCfgCmd(char *name, int value);
char *UmodemMakeCfgQueryCmd(char *name);
char *UmodemMakeCFRCmd(int iMilliseconds);
// modified/added by abahr 21 August 2007
double UmodemGetArrivalTime(void);
int UmodemGetArrivalTimeMode(void);
int UmodemGetArrivalTimeSrc(void);

/* int UmodemGetRXData(unsigned char *buf); */

/* user messages - just significant events */
#define UMODEM_NOMSG      (0)
#define UMODEM_REVERT     (11)
#define UMODEM_READY      (12)
#define UMODEM_PING       (13) 
#define UMODEM_PINGREP    (14) 
#define UMODEM_DATREQ     (15)
#define UMODEM_MECREP     (16)
#define UMODEM_ADDRESS    (17)
#define UMODEM_CI         (18)
#define UMODEM_BINDATA    (19)
#define UMODEM_ASCDATA    (20)
#define UMODEM_BADPKT     (21)
#define UMODEM_PKTTIMEOUT (22)
#define UMODEM_EXTSEL     (23)
#define UMODEM_EXTSELREP  (24)
#define UMODEM_MSGTIMEOUT (25)
#define UMODEM_ACK        (26)
#define UMODEM_ALWAYS     (27)
#define UMODEM_TXDACK     (28)
#define UMODEM_DEBUG      (29)
#define UMODEM_ERROR      (30)
#define UMODEM_CFG        (31)
#define UMODEM_NOISE      (32)
#define UMODEM_RETRY      (33)
#define UMODEM_MFD        (34)
#define UMODEM_DOP        (35)
#define UMODEM_MAXRETRIES (36)
#define UMODEM_SLEEP      (37)
#define UMODEM_SLEEPREP   (38)
#define UMODEM_QUALITY    (39)
#define UMODEM_PINGCMD    (40)
#define UMODEM_LBLPING    (41)
#define UMODEM_TTIMES     (42)
#define UMODEM_NAVMFD     (43)
#define UMODEM_TOA        (44)

#endif  // #ifndef _UMODEM_H
