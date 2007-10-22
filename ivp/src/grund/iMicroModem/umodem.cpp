/*
 *  umodem.c  - wrapper around umodem specific calls 
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

/* removed serial port interface -mg 2004-10-08 */

//
// some debugging by Alex Bahr, MIT abahr@mit.edu
// modified/added lines are marked by
// "modified/added by abahr"



#include "umodem.h"

UmodemObj u;

int UmodemInit(){
	int l;

	// init the link status
	for(l=0;l<UMODEM_MAXNODES;l++){
		u.link[l].dopshift=0.0;	
		u.link[l].lastdqf=0;
		u.link[l].lastping=0;
		u.link[l].lastrx=0;
		u.link[l].lasttx=0;
		u.link[l].mfdpeak=0;
		u.link[l].mfdpwr=0;
		u.link[l].mfdratio=0;
		u.link[l].ncycles=0;
		u.link[l].nrxpkts=0;
		u.link[l].ntxpkts=0;
		u.link[l].nbadpkts=0;
	}

	// init the modem state
	// u.state.address=src;
	u.state.nrevert=0;
        UmodemSetStatus(UMODEM_UNKNOWN);
	return true;
}

char *UmodemMakeTxString(sDataFrame *f){
//
// form a micromodem binary data transmit message
//

        char caHexData[UMODEM_MAXMSG],caHexByte[3];
        int i;
        
        sprintf(caHexData,"\0");

        for (i=0;i<f->iSize;i++){
                sprintf(caHexByte,"%02x",f->caData[i]);
                strcat(caHexData,caHexByte);
        }
        sprintf(u.dev.txmsg,"$CCTXD,%d,%d,%d,%s\r\n",
	          f->iSrc,f->iDest,f->bAck,caHexData);


		return u.dev.txmsg;
}

char *UmodemMakePingCmd(int iSrc, int iDest){
//
// form a micromodem ping message
//
         sprintf(u.dev.txmsg,"$CCMPC,%d,%d\r\n",
	          iSrc,iDest);

		return u.dev.txmsg;
}


char *UmodemMakeCICmd(int iSrc, int iDest, int iRate, int iAck){
//
// form a micromodem binary dat transmit message
//
         sprintf(u.dev.txmsg,"$CCCYC,1,%d,%d,%d,%d,1\r\n",
	          iSrc,iDest,iRate,iAck);

		return u.dev.txmsg;
}


char *UmodemMakeCfgCmd(char *caName, int iValue){
//
// form a micromodem config message
//
        sprintf(u.dev.txmsg,"$CCCFG,%c%c%c,%d\r\n",
	          caName[0],caName[1],caName[2],iValue);

	return u.dev.txmsg;
}

char *UmodemMakeCfgQueryCmd(char *caName){
//
// form a micromodem config query message
//
        sprintf(u.dev.txmsg,"$CCCFQ,%c%c%c\r\n",
	          caName[0],caName[1],caName[2]);

	return u.dev.txmsg;
}

char *UmodemMakeCFRCmd(int dur){
//
// form a micromodem config query message
//
        sprintf(u.dev.txmsg,"$CCCFR,%d\r\n",dur);

	    return u.dev.txmsg;
}




int UmodemParse(char *nmeamsg)
{
	int nmeatype,l;
        char arg[80];
	
	time(&u.dev.lastread);
	strcpy(u.dev.lastmsg,nmeamsg);
	nmeatype=NMEA_Type(nmeamsg);
	
	switch (nmeatype) {

	case NMEA_REVERT:
		u.state.nrevert++;
		UmodemSetStatus(UMODEM_REVERTED);
		return(UMODEM_REVERT);
		break;

	case NMEA_READY:
		if (u.state.status==UMODEM_UNKNOWN){
			UmodemSetStatus(UMODEM_IDLE);
			return(UMODEM_READY);
		}
		UmodemSetStatus(UMODEM_IDLE);
		break;

		//case NMEA_ADDRESS:
		//u.state.address=NMEA_IntArg(nmeamsg,2);
		//return(UMODEM_ADDRESS);
		//break;

	case NMEA_CFG:
		u.cfg.value=NMEA_IntArg(nmeamsg,2);
		NMEA_StrArg(nmeamsg,1,u.cfg.name);
		if (0==strncmp(u.cfg.name,"SRC",3))
			u.state.address=u.cfg.value;
		if (UmodemGetStatusInt()==UMODEM_CONFIGURING){ // we sent a command
			if ( (0==strncmp(u.cfg.name,u.cfg.requested_name,3)) &&
			     (u.cfg.value==u.cfg.requested_value) )
				UmodemSetStatus(UMODEM_IDLE);  // reply was good
			else
				UmodemSetStatus(UMODEM_CONFIGERROR);  // reply NOT good
		}
		return(UMODEM_CFG);
		break;

	case NMEA_ALWAYS:
		return(UMODEM_ALWAYS);
		break;

	case NMEA_QUALITY:
		u.state.lastdqf=NMEA_IntArg(nmeamsg,1); // intermediate var
		return(UMODEM_QUALITY);
		break;

	case NMEA_APING:  /* just update DQF for that link */
		u.ping.src=NMEA_IntArg(nmeamsg,1);
		u.ping.dest=NMEA_IntArg(nmeamsg,2);
		u.link[u.ping.src].lastdqf=u.state.lastdqf;
		UmodemSetStatus(UMODEM_GOTPINGED);
		return(UMODEM_PING);
		break;

	case NMEA_CPING: 
		u.ping.src=NMEA_IntArg(nmeamsg,1);
		u.ping.dest=NMEA_IntArg(nmeamsg,2);
		UmodemSetStatus(UMODEM_PINGING);
		return(UMODEM_PINGCMD);
		break;

	case NMEA_RPING:  /* just update the DQF, OWTT */
		u.ping.src=NMEA_IntArg(nmeamsg,1);
		u.ping.dest=NMEA_IntArg(nmeamsg,2);
		u.link[u.ping.src].lastdqf=u.state.lastdqf; 
		u.link[u.ping.src].owtt=NMEA_FloatArg(nmeamsg,3);
		UmodemSetStatus(UMODEM_IDLE);
		return(UMODEM_PINGREP);
		break;

	case NMEA_RSLP:
		u.sleep.src=NMEA_IntArg(nmeamsg,1);
		u.sleep.dest=NMEA_IntArg(nmeamsg,2);
		u.sleep.arg=NMEA_IntArg(nmeamsg,3);
		UmodemSetStatus(UMODEM_IDLE);
		return(UMODEM_SLEEPREP);
		break;

	case NMEA_ASLP:
		u.sleep.src=NMEA_IntArg(nmeamsg,1);
		u.sleep.dest=NMEA_IntArg(nmeamsg,2);
		u.sleep.arg=NMEA_IntArg(nmeamsg,3);
		return(UMODEM_SLEEP);
		break;

	case NMEA_REXTSEL:
		u.extsel.src=NMEA_IntArg(nmeamsg,1);
		u.extsel.dest=NMEA_IntArg(nmeamsg,2);
		u.extsel.line=NMEA_IntArg(nmeamsg,3);
		u.extsel.mode=NMEA_IntArg(nmeamsg,4);
		u.extsel.val=NMEA_IntArg(nmeamsg,5);
		UmodemSetStatus(UMODEM_IDLE);
		return(UMODEM_EXTSELREP);
		break;

	case NMEA_AEXTSEL:
		u.extsel.src=NMEA_IntArg(nmeamsg,1);
		u.extsel.dest=NMEA_IntArg(nmeamsg,2);
		u.extsel.line=NMEA_IntArg(nmeamsg,3);
		u.extsel.mode=NMEA_IntArg(nmeamsg,4);
		u.extsel.val=NMEA_IntArg(nmeamsg,5);
		UmodemSetStatus(UMODEM_COMMANDED);
		return(UMODEM_EXTSEL);
		break;

	case NMEA_CI:
		u.cycle.mode    = NMEA_IntArg(nmeamsg,1);
		u.cycle.src     = NMEA_IntArg(nmeamsg,2);
		u.cycle.dest    = NMEA_IntArg(nmeamsg,3);
		u.cycle.pkttype = NMEA_IntArg(nmeamsg,4);
		u.cycle.ack     = NMEA_IntArg(nmeamsg,5);
		u.cycle.npkts   = NMEA_IntArg(nmeamsg,6);

		// set status word appropriately
		if (u.cycle.src==u.state.address)
			UmodemSetStatus(UMODEM_TRANSMITTING);
		else if (u.cycle.dest==u.state.address)
			UmodemSetStatus(UMODEM_RECEIVING);
		else
			UmodemSetStatus(UMODEM_MONITORING);
		return(UMODEM_CI);
		break;

	case NMEA_RXD:
		UmodemData(nmeamsg);
		u.link[u.cycle.src].lastdqf=u.state.lastdqf;
		UmodemSetStatus(UMODEM_GOTDATA);
		return(UMODEM_BINDATA);
		break;

	case NMEA_CFR:
		UmodemNoise(nmeamsg);
		UmodemSetStatus(UMODEM_IDLE);
		return(UMODEM_NOISE);
		break;

	case NMEA_TXPKT:
		//UmodemSetStatus(UMODEM_TXON);
		break;

	case NMEA_TXPKTDONE:
		//UmodemSetStatus(UMODEM_TXOFF);
		break;
	case NMEA_TXMINI:
		//UmodemSetStatus(UMODEM_TXON);
		break;
	case NMEA_TXMINIDONE:
		//UmodemSetStatus(UMODEM_TXOFF);
		break;

	case NMEA_TOA:
		u.toa.src=u.cycle.src;
		// modified by abahr 21 August 2007
		u.toa.time=NMEA_DoubleArg(nmeamsg,1);
		u.toa.mode=NMEA_IntArg(nmeamsg,2);
		return(UMODEM_TOA);
		break;

	case NMEA_DOP:
		u.link[u.cycle.src].dopshift=NMEA_FloatArg(nmeamsg,1);
		return(UMODEM_DOP);
		break;

	case NMEA_MFD:
		u.link[u.cycle.src].mfdpeak=NMEA_LongArg(nmeamsg,1);
		u.link[u.cycle.src].mfdpwr=NMEA_IntArg(nmeamsg,2);
		u.link[u.cycle.src].mfdratio=NMEA_IntArg(nmeamsg,3);
		return(UMODEM_MFD);
		break;

	case NMEA_BADPKT:
		u.link[u.cycle.src].nbadpkts++;
		UmodemSetStatus(UMODEM_GOTBADCRC);
		return(UMODEM_BADPKT);
		break;

	case NMEA_NOPKT:
		UmodemSetStatus(UMODEM_GOTTIMEOUT);
		return(UMODEM_PKTTIMEOUT);

		break;

	case NMEA_ACK:
		u.link[u.ack.src].lastdqf = u.state.lastdqf;
		if (NMEA_IntArg(nmeamsg,4)){
			u.ack.src      = NMEA_IntArg(nmeamsg,1); 
			u.ack.dest     = NMEA_IntArg(nmeamsg,2); 
			u.ack.frameno  = NMEA_IntArg(nmeamsg,3)-1;
			// UmodemAckProc();
			return(UMODEM_ACK);
		}
		break;

	case NMEA_DATREQ:
		u.tx.src                      = NMEA_IntArg(nmeamsg,2); 
		u.tx.dest                     = NMEA_IntArg(nmeamsg,3);
		u.tx.ackrequested             = NMEA_IntArg(nmeamsg,4);
		u.tx.frameno                  = NMEA_IntArg(nmeamsg,6)-1;
		u.tx.requested                = NMEA_IntArg(nmeamsg,5);
		return(UMODEM_DATREQ);
		break;

	case NMEA_DEBUG:
		NMEA_StrArg(nmeamsg,2,u.dev.lasterrmsg);
		u.state.status=UMODEM_GOTERROR;
		sprintf(u.state.statusmsg,"Debug: msg=\"%s\" !",u.dev.lasterrmsg);
		return(UMODEM_DEBUG);
		break;

	case NMEA_ERROR:
		NMEA_StrArg(nmeamsg,2,u.dev.lasterrmsg);
		u.state.status=UMODEM_GOTERROR;
		sprintf(u.state.statusmsg,"Error: msg=\"%s\" !",u.dev.lasterrmsg);
		return(UMODEM_ERROR);
		break;

	case NMEA_TXDACK:
		u.tx.dataacksize = NMEA_IntArg(nmeamsg,4);
		return(UMODEM_TXDACK);
		break;

	case NMEA_TTIMES:
		for (l=0;l<4;l++){
			NMEA_StrArg(nmeamsg,l,arg);
			if (strlen(arg))
				u.ttimes.owtt[l]=(float)atof(arg);
			else
				u.ttimes.owtt[l]=-1.0;
		}
		NMEA_StrArg(nmeamsg,5,arg);
		u.ttimes.timestamp=(float)atof(arg);
		return(UMODEM_TTIMES);
		break;

	case NMEA_LBLPING:
		return(UMODEM_LBLPING);
		break;

	case NMEA_NAVMFD:
		return(UMODEM_NAVMFD);
		break;

	case NMEA_BADMINI:
	case NMEA_CSLP:
		break;

	default:
		return(UMODEM_UNKNOWN);
	}
	return (UMODEM_NOMSG);
}



static char caModemMsg[560];

char *UmodemConfig(char *set,int val){

	caModemMsg[0]=0;

	// log what we request
	strcpy(u.cfg.requested_name,set);
	u.cfg.requested_value=val;
	
	// now send command
	sprintf(caModemMsg,"$CCCFG,%s,%d\r\n",set,val);    
	UmodemSetStatus(UMODEM_CONFIGURING);
	return(caModemMsg);
}


char *UmodemDoCycle(int src,int dest,int pkttype, int ack){
	caModemMsg[0]=0;
	sprintf(caModemMsg,"$CCCYC,1,%d,%d,%d,%d,1\r\n",
		src,dest,pkttype,ack);    
	UmodemSetStatus(UMODEM_INITIATING);
	return(caModemMsg);
}

char *UmodemSendAck(int src, int dest, int ackword)
{
	caModemMsg[0]=0;
	sprintf(caModemMsg,"$CCACK,%d,%d,%d\r\n",src,dest,ackword);
	return(caModemMsg);
}

char *UmodemNoiseCheck(int msec)
{
	caModemMsg[0]=0;
	u.state.noiseint=msec;
	sprintf(caModemMsg,"$CCCFR,%d\r\n",msec);
	UmodemSetStatus(UMODEM_CALIBRATING);
	return(caModemMsg);
}

void UmodemNoise(char *nmeamsg)
{
	int l;
	
	for (l=0;l<14;l++)
		u.state.noise[l]=NMEA_IntArg(nmeamsg,l+1);
	
}


char *UmodemPing(int src,int dest){
	caModemMsg[0]=0;
	sprintf(caModemMsg,"$CCMPC,%d,%d\r\n",src,dest);    
	UmodemSetStatus(UMODEM_PINGING);
	return(caModemMsg);
}

char *UmodemLBLPing(int timeout, int txfreq, int txtau, int rxtau, 
		    int rxfreq1,int rxfreq2,int rxfreq3, int rxfreq4){
	caModemMsg[0]=0;
	sprintf(caModemMsg,"$CCPNT,%d,%d,%d,%d,%d,%d,%d,%d,1\r\n",
		txfreq,txtau,rxtau,timeout,
		rxfreq1,rxfreq2,rxfreq3,rxfreq4);
    UmodemSetStatus(UMODEM_NAVIGATING);
	return(caModemMsg);
}

char *UmodemSetLine(int src,int dest,int line,int val){
	caModemMsg[0]=0;
	sprintf(caModemMsg,"$CCMEC,%d,%d,%d,1,%d\r\n",src,dest,line,val);    
	return(caModemMsg);
}

char *UmodemSleep(int src,int dest,int arg){
	caModemMsg[0]=0;
	sprintf(caModemMsg,"$CCMSC,%d,%d,%d\r\n",src,dest,arg);    
	UmodemSetStatus(UMODEM_COMMANDING);
	return(caModemMsg);
}

void UmodemSetRetries(int retries){

	u.tx.frame[u.tx.frameno].retries=retries;

}

void UmodemData(char *msg){
	
	u.rx.frame.iSrc  = NMEA_IntArg(msg,1);
	u.rx.frame.iDest = NMEA_IntArg(msg,2);
	u.rx.frame.bAck  = NMEA_IntArg(msg,3);
	u.rx.frame.iFrameNum = NMEA_IntArg(msg,4);
	time(&u.rx.rxtime);
	
	/* now suck the data */
	u.rx.frame.iSize=NMEA_HexData(msg,5,(char *)u.rx.frame.caData);

        u.rx.frame.iType = u.rx.frame.caData[0];

	return;
}

int UmodemGetAddress(void){
  return(u.state.address);
}
int UmodemGetLastCfgValue(void){
  return(u.cfg.value);
}
char *UmodemGetLastCfgName(void){
  return(u.cfg.name);
}

int UmodemGetPktTimeout(void){
  return(u.state.pkttimeout);
}

float UmodemGetLastVoltage(void){
	return((float)(7.3));
	return(u.state.vbatt);
}

int UmodemGetLastDQF(void){
  return(u.state.lastdqf);
}

int UmodemGetMFDPwr(void){
  return(u.link[UmodemGetCISrc()].mfdpwr);
}

long UmodemGetMFDPeak(void){
  return(u.link[UmodemGetCISrc()].mfdpeak);
}

int UmodemGetMFDRatio(void){
  return(u.link[UmodemGetCISrc()].mfdratio);
}

float UmodemGetDoppler(void){
  return(u.link[UmodemGetCISrc()].dopshift);
}

int UmodemGetCISrc(void){
  return(u.cycle.src);
}

int UmodemGetCIDest(void){
  return(u.cycle.dest);
}

int UmodemGetCIMode(void){
  return(u.cycle.mode);
}

int UmodemGetCIAck(void){
  return(u.cycle.ack);
}

int UmodemGetCIPktType(void){
  return(u.cycle.pkttype);
}

int UmodemGetCINPkts(void){
  return(u.cycle.npkts);
}

int UmodemFrameSize(int pkttype)
{
	switch(pkttype){
	case 0:
		return(32);
		break;
	case 1:
		return(32);
		break;
	case 2:
		return(64);
		break;
	case 3:
		return(256);
		break;
	case 4:
		return(256);
		break;
	case 5:
		return(256);
		break;
	default:
		return(32);   // the minimum frame size
		
	}
}


int UmodemGetRXSrc(void){
  return(u.rx.frame.iSrc);
}

int UmodemGetRXDest(void){
  return(u.rx.frame.iDest);
}

int UmodemGetRXDataSize(void){
  return(u.rx.frame.iSize);
}

int UmodemGetNReverts(void){
  return(u.state.nrevert);
}

char *UmodemGetLastMsg(void){
  return(u.dev.lastmsg);
}

int UmodemGetLastPingSrc(void){
  return(u.ping.src);
}

int UmodemGetLastPingDest(void){
  return(u.ping.dest);
}

float UmodemGetLastPingOWTT(void){
  return(u.link[u.ping.src].owtt);
}

float *UmodemGetLastLBLTT(void){
  return(u.ttimes.owtt);
}

int UmodemGetExtSelSrc(void){
  return(u.extsel.src);
}

int UmodemGetExtSelDest(void){
  return(u.extsel.dest);
}

int UmodemGetExtSelMode(void){
  return(u.extsel.mode);
}
int UmodemGetExtSelLine(void){
  return(u.extsel.line);
}
int UmodemGetExtSelVal(void){
  return(u.extsel.val);
}
int UmodemGetSleepSrc(void){
  return(u.sleep.src);
}
int UmodemGetSleepDest(void){
  return(u.sleep.dest);
}
int UmodemGetSleepArg(void){
  return(u.sleep.arg);
}
char *UmodemBuf(void){
  return((char *)u.tx.frame[u.tx.frameno].buf);
}
int UmodemGetTXDataSize(void){
  return(u.tx.requested);
}

int UmodemGetTXFrameNo(void){
  return(u.tx.frameno);
}
int UmodemGetTXQSize(void){
	int l;
	int nfull=0;
	for (l=0;l<UMODEM_MAXFRAMES;l++){
		nfull+=(u.tx.frame[l].retries>=0)?(1):(0);
	}
	return(nfull);
}
int UmodemGetTXDataAckSize(void){
  return(u.tx.dataacksize);
}
int UmodemGetTXDataDest(void){
  return(u.tx.dest);
}
int UmodemGetTXDataAck(void){
  return(u.tx.frame[u.tx.frameno].ack);
}
int UmodemGetTXDataAckRequested(void){
  return(u.tx.ackrequested);
}
int UmodemGetLastAckSrc(void){
  return(u.ack.src);
}
int UmodemGetLastAckFrameNo(void){
  return(u.ack.frameno);
}
char *UmodemGetLastErrorMsg(void){
  return(u.dev.lasterrmsg);
}
sDataFrame *UmodemGetRXData(void){
  return(&(u.rx.frame));
}
void UmodemShowRXData(void){
  unsigned int l;
  
  for (l=0;l<u.rx.frame.iSize;l++){
    if (0==(l%16)) {
      if (l)
	printf("\n                   |");
      else 
	printf("                   |");
    }
    printf("%02x|",u.rx.frame.caData[l]);
  }
  printf("\n");
}

/* UmodemTx() - Generate a TXD in reply to a DRQ */

char *UmodemTx(void){
	char hexbyte[3];
	int l;
	
	caModemMsg[0]=0;

	/* generate the message */
	sprintf(caModemMsg,"$CCTXD,%d,%d,%d,",
		u.tx.src,u.tx.dest,u.tx.frame[u.tx.frameno].ack);
	for (l=0;l<u.tx.frame[u.tx.frameno].size;l++){
		sprintf(hexbyte,"%02x",
			u.tx.frame[u.tx.frameno].buf[l]);
		strcat(caModemMsg,hexbyte);
	}
	strcat(caModemMsg,"\r\n");
	
	/* if not an ACK frame, clear it */
	if (u.tx.frame[u.tx.frameno].ack==0) {
		u.tx.frame[u.tx.frameno].retries=-1;
	} else {
		/* decrement retries */
		u.tx.frame[u.tx.frameno].retries--;
	}

	return(caModemMsg);
	
}

char *UmodemTxDataOnly(void){
	char hexbyte[3];
	int l;
	
	caModemMsg[0]=0;

	/* generate the message */
	//sprintf(caModemMsg,"$CCTXD,%d,%d,%d,",
	//	u.tx.src,u.tx.dest,u.tx.frame[u.tx.frameno].ack);
	for (l=0;l<u.tx.frame[u.tx.frameno].size;l++){
		sprintf(hexbyte,"%02x",
			u.tx.frame[u.tx.frameno].buf[l]);
		strcat(caModemMsg,hexbyte);
	}
	//strcat(caModemMsg,"\r\n");	
	return(caModemMsg);	
}

/* UmodemAckCleanup() - moves frames not acked to
 *                      lowest numbered frame positions. */
void UmodemAckCleanup(void) 
{
	int l,n,firstempty;

	/* loop through, moving up */
	for (l=1;l<UMODEM_MAXFRAMES;l++){      
		if (u.tx.frame[l].retries>0) {      
			firstempty=-1;
			for (n=l-1;n>=0;n--) 
				if (u.tx.frame[n].retries<=0) 
					firstempty=n;
			if (firstempty>-1) {
				UmodemFrameMove(l,firstempty);
			}
		}
		
	}
}


void UmodemFrameMove(int from, int to)
{
	int l;

	/* copy */
	u.tx.frame[to].retries=u.tx.frame[from].retries;
	u.tx.frame[to].ack=u.tx.frame[from].ack;
	u.tx.frame[to].size=u.tx.frame[from].size;
	for (l=0;l<u.tx.frame[from].size;l++)
		u.tx.frame[to].buf[l]=u.tx.frame[from].buf[l];
	
	/* clear old one */
	u.tx.frame[from].retries=-1;
}

/* UmodemAckProc() - clears acked frame, when message arrives */
void UmodemAckProc(void) 
{
	u.tx.frame[u.ack.frameno].retries=-1; /* -1 => ACKED */
}

void UmodemFlushTxData(void)
{
	int l;
	
	for (l=0;l<UMODEM_MAXFRAMES;l++){
		u.tx.frame[l].retries = -1;
		u.tx.frame[l].size = 0;
	}
}

int UmodemSetStatus(int s){
	u.state.status=s;
	switch(s) { /* states for u.state.status */
	case  UMODEM_UNKNOWN:       
		sprintf(u.state.statusmsg,"Connecting");
		break;
	case  UMODEM_CONFIGERROR:     
		sprintf(u.state.statusmsg,"Config Error: %s,%d",u.cfg.requested_name,u.cfg.requested_value);
		break;
	case  UMODEM_CONFIGURING:   
		sprintf(u.state.statusmsg,"Configuring");
		break;
	case  UMODEM_IDLE:       
		sprintf(u.state.statusmsg,"Ready");
		break;
	case  UMODEM_TRANSMITTING:  
		sprintf(u.state.statusmsg,"Transmitting: dest=%d",u.cycle.dest);
		break;
	case  UMODEM_RECEIVING:     
		sprintf(u.state.statusmsg,"Receiving: src=%d",u.cycle.src);
		break;
	case  UMODEM_MONITORING:    
		sprintf(u.state.statusmsg,"Monitoring: src=%d, dest=%d",u.cycle.src,u.cycle.dest);
		break;
	case  UMODEM_INITIATING:
		sprintf(u.state.statusmsg,"New Comms Cycle: src=%d, dest=%d, rate=%d",
			u.cycle.src,u.cycle.dest,u.cycle.pkttype);
		break;
	case  UMODEM_PINGING:       
		sprintf(u.state.statusmsg,"Pinging, node=%d",u.ping.dest);
			  break;
	case  UMODEM_COMMANDING:    
		sprintf(u.state.statusmsg,"Commanding");
		break;
	case  UMODEM_NAVIGATING:    
		sprintf(u.state.statusmsg,"Navigating");
		break;
	case  UMODEM_CALIBRATING:  
		sprintf(u.state.statusmsg,"Calibrating Receiver");
		break;
	case  UMODEM_REVERTED:      
		sprintf(u.state.statusmsg,"Reverted");
		break;
	case  UMODEM_GOTERROR:      
		sprintf(u.state.statusmsg,"Error");
		break;
	case  UMODEM_GOTBADCRC:    
		sprintf(u.state.statusmsg,"Got Bad Packet");
		break;
	case  UMODEM_GOTTIMEOUT:    
		sprintf(u.state.statusmsg,"Packet Timeout");
		break;
	case  UMODEM_COMMANDED:    
		sprintf(u.state.statusmsg,"Got Command");
		break;
	case  UMODEM_ACKNOWLEDGING:    
		sprintf(u.state.statusmsg,"Sending Ack");
		break;

	case  UMODEM_TXON:    
		sprintf(u.state.statusmsg,"Transmitter On");
		break;
		
	case  UMODEM_TXOFF:    
		sprintf(u.state.statusmsg,"Transmitter Off");
		break;
		
	case  UMODEM_GOTPINGED:    
		sprintf(u.state.statusmsg,"Got Pinged: src=%d",u.ping.src);
		break;
		
	case  UMODEM_GOTDATA:    
		sprintf(u.state.statusmsg,"Got Data: size=%d, src=%d",u.rx.frame.iSize,u.rx.frame.iSrc);
		break;

	case  UMODEM_REQUESTINGDATA:    
		sprintf(u.state.statusmsg,"Got Data Request: size=%d, dest=%d",u.tx.requested,u.tx.dest);
		break;
		
	default:
		sprintf(u.state.statusmsg,"Invalid State");
	}
	return(s);
}

void UmodemCheckVoltage(void){

  printf("UmodemCheckVoltage(): FIX ME\n");

}

char *UmodemGetStatusString(void){
	return u.state.statusmsg;
}

int UmodemGetStatusInt(void){
	return u.state.status;
}

// modified by abahr 21 August 2007
double UmodemGetArrivalTime(void)
{
	return u.toa.time;
}

int UmodemGetArrivalTimeMode(void)
{
	return u.toa.mode;
}

int UmodemGetArrivalTimeSrc(void)
{
	return u.toa.src;
}
