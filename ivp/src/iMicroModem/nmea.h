/*
 *  nmea.h -  NMEA parser for umodem messaging
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
// noted by lines containing "added by abahr"


#include <string>
#include <stdio.h>

#define  NMEA_NAN      (-999)
#define  NMEA_MAXSTR   (512)

/* NMEA parse errors */
#define NMEA_NOERROR    (9)           /* no error */        
#define NMEA_CHKSUMERR  (11)          /* bad checksum */          
#define NMEA_UNKNOWNCMD (12)          /* unknown cmd (parser not in table) */
#define NMEA_NODOLLAR   (13)          /* missing $ */  
#define NMEA_NOCOMMA    (14)          /* missing at least 1 comma */
#define NMEA_WRONGNARGS (15)          /* wrong number of arguments */
#define NMEA_STAR       (16)          /* too many or misplaced stars '*'  */
#define NMEA_RANGE      (17)          /* arg out of range */
#define NMEA_BLANK      (18)          /* required arg is blank */
#define NMEA_TERMERR    (19)	      /* bad terminator(s) */
#define NMEA_TALKER     (20)          /* unknown talker id */


/* NMEA message IDs all known messages (this list grows) */
#define  NMEA_UNKNOWN (0x00)
#define  NMEA_REVERT  (0x01)
#define  NMEA_READY   (0x02)
#define  NMEA_ADDRESS (0x03)
#define  NMEA_APING   (0x04)
#define  NMEA_CPING   (0x05)
#define  NMEA_RPING   (0x06)
#define  NMEA_QUALITY (0x07)
#define  NMEA_TXPKT   (0x08)
#define  NMEA_TXMINI  (0x09)
#define  NMEA_CI      (0x0a)
#define  NMEA_RXD     (0x0b)
#define  NMEA_RXA     (0x0c)
#define  NMEA_BADPKT  (0x0d)
#define  NMEA_BADMINI (0x0e)
#define  NMEA_NOPKT   (0x0f)
#define  NMEA_AEXTSEL (0x10)     
#define  NMEA_CEXTSEL (0x11)     
#define  NMEA_REXTSEL (0x12)     
#define  NMEA_DATREQ  (0x13)
#define  NMEA_ACK     (0x14)
#define  NMEA_ALWAYS  (0x15)
#define  NMEA_DEBUG   (0x16)
#define  NMEA_ERROR   (0x17)
#define  NMEA_TXDACK  (0x18)
#define  NMEA_CFG     (0x19)
#define  NMEA_CFR     (0x1a)
#define  NMEA_MFD     (0x1b)
#define  NMEA_DOP     (0x1c)
#define  NMEA_CSLP    (0x1d)
#define  NMEA_ASLP    (0x1e)
#define  NMEA_RSLP    (0x1f)
#define  NMEA_TTIMES  (0x20)
#define  NMEA_LBLPING (0x21)
#define  NMEA_NAVMFD  (0x22)
#define  NMEA_TXPKTDONE (0x23)
#define  NMEA_TXMINIDONE (0x24)
#define  NMEA_TOA     (0x25)

/* protos */
int   NMEA_Scan(unsigned char *buf,unsigned int *buflen,char *msg);
int   NMEA_Type(char *msg);
int   NMEA_IntArg(char *msg,int n);
long  NMEA_LongArg(char *msg,int n);
// added by abahr 21 August 2007
double  NMEA_DoubleArg(char *msg,int n);
void  NMEA_StrArg(char *msg,int n,char *arg);
float NMEA_FloatArg(char *msg,int n);
int   NMEA_HexData(char *msg,int n, char *buf);
int   NMEA_HexToData(char *hex,unsigned char *buf);
int   NMEA_DataToHex(unsigned char *buf,int nBytes,char *hex);
void  NMEA_AppendChecksum(char *str);
int   NMEA_CheckChecksum(char *str);
int   NMEA_GetNArgs(char *str);
