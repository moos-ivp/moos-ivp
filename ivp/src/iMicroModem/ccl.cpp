//
// ccl.c - ccl protocol layer
//

#include <stdio.h>
#include <string>
#include <math.h>
#include "ccl.h"

char caMsg[256];
sDataFrame cclframe;

//
// init the CCL state structs
//
void CCL_Init(void) {

}

int CCL_FrameIsTest(sDataFrame *frame)
{
        int iTest=1;
        int i;
        
        for (i=0;i<frame->iSize;i++)
                iTest = iTest && ((frame->caData[i])==(unsigned char)(i%256));
        
        return(iTest);
}



void CCL_RxFrame(sDataFrame *f) {


        switch(f->caData[0]) {

        case CCLEMPTY:
                break;
        case CCLREDIRECT:
                break;
        case CCLUSBLPOS:
                break;
        case CCLBATHY:
                CCL_RxBathyFrame(f);
                break;
        case CCLCTD:
                CCL_RxCTDFrame(f);
                break;
        case CCLCOMMAND:

                break;
        case CCLUSBLDATA:
                break;
        case CCLCADCACDET:
                break;
        case CCLSTATE:
                CCL_RxStateFrame(f);
                break;
        case CCLERROR:
                break;
        case CCLRANGER:
                CCL_RxRangerFrame(f);
                break;
        case CCLMLOREQ:
                break;
        }
}

void CCL_TxFrame(sDataFrame *f){

}
//
// decode state: just position and faults
//
void CCL_RxStateFrame(sDataFrame *frame){

        // double dLatDeg;
        // double dLonDeg;
        // unsigned char f1,f2,f3;  // fault bytes, f1,f2=>faults1...f3=>faults_2
        
        //
        // decode the pos'n
        //
        //dLatDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[1])));
        //dLonDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[4])));
        //UTIL_NewNodeFix(frame->iSrc,dLatDeg,dLonDeg);
        // if (user[frame->iSrc].bShow){
        //        sprintf(caMsg,"CCL: Latitude:  %9.5f",dLatDeg);
        //        DATA_UserMsg(caMsg);
	//       sprintf(caMsg,"CCL: Longitude: %9.5f",dLonDeg);
        //        DATA_UserMsg(caMsg);
        // }
        // decode the fault lights
}

void CCL_RxBathyFrame(sDataFrame *frame){
     //double dLatDeg;
     //double dLonDeg;

        // first bathy
        //dLatDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[8])));
        //dLonDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[17])));
        //UTIL_NewNodeFix(frame->iSrc,dLatDeg,dLonDeg);
        // second bathy
        //dLatDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[11])));
        //dLonDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[20])));
        //UTIL_NewNodeFix(frame->iSrc,dLatDeg,dLonDeg);
        // third bathy
        //dLatDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[14])));
        //dLonDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[23])));
        //UTIL_NewNodeFix(frame->iSrc,dLatDeg,dLonDeg);

}


void CCL_RxCTDFrame(sDataFrame *frame){
     //double dLatDeg;
     //double dLonDeg;

        // first CTD
        //dLatDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[12])));
        //dLonDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[18])));
        //UTIL_NewNodeFix(frame->iSrc,dLatDeg,dLonDeg);

        // second CTD
        //dLatDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[15])));
        //dLonDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[21])));
        //UTIL_NewNodeFix(frame->iSrc,dLatDeg,dLonDeg);

}


void CCL_RxRangerFrame(sDataFrame *frame){
     //double dLatDeg;
     //double dLonDeg;

        //dLatDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[1])));
        //dLonDeg=CCL_DecodeLatLon((LATLON_COMPRESSED *)(&(frame->caData[4])));
        //UTIL_NewNodeFix(frame->iSrc,dLatDeg,dLonDeg);


}

//
// expand compressed coordinate
//
double CCL_DecodeLatLon(LATLON_COMPRESSED l){

        LONG_AND_COMP in;
        in.as_long=0;  // clear the MSB
        in.as_compressed = l;
        if (in.as_long & 0x00800000L) // extend the sign
                in.as_long|=0xFF000000L;
        return((double)(in.as_long)*(180.0/(double)(0x007FFFFFL)));
}

float CCL_DecodeDepth(unsigned short depth) {
	/* 0 -100 meters: 0-1000 (10 cm resolution) 
	 * 100 -200 meters: 1001-1500 (20 cm resolution) * 20 
	 * 1000-6000 meters: 3101-8100 (1 meter resolution) 
	 */ 
		//depth &= DEPTH_MODE_MASK ;
		if (depth <= 1000)  
			return((float)(depth * 0.1/1.0)); 
		else if (depth <= 1500) 
			return((float)(100 + (depth-1000) * 0.2/1.0));
		else if (depth <= 3100) 
			return((float)(200 + (depth-1500) * 0.5/1.0)); 
		else if (depth <= 8100)
			return((float)(1000 + ( depth-3100) * 1.0/1.0));
		else 
			return(6000); 
}


float CCL_DecodeAltitude(unsigned short alt) {

	return(alt/100.0F);
}


//
// Make Test frame
//
sDataFrame *CCL_MakeTestFrame(int src, int dest, int ack, int fsize)
{
	int l;

	cclframe.iSrc = src ;
	cclframe.iDest = dest ;
        cclframe.bAck = ack%2;
	cclframe.iFrameNum=1;
	cclframe.bValid = 1;
	cclframe.iType = CCLTEST;
	cclframe.iSize = MIN(fsize,MAXFRAMESIZE);

	memset(cclframe.caData,0,MAXFRAMESIZE);

	// test data is sequential bytes
	for (l=0;l<fsize;l++)
		cclframe.caData[l]=l;

	return(&cclframe);
	
}

//
// Make Command Frame
//
sDataFrame *CCL_MakeCommandFrame(int src,int dest,int ack,unsigned short cmdno)
{
     // init the frame struct
     cclframe.iSrc = src ;
     cclframe.iDest = dest ;
     cclframe.bAck = ack%2;
     cclframe.iFrameNum=1;
     cclframe.bValid = 1;
     cclframe.iType = CCLCOMMAND;
     cclframe.iSize = 32;
     memset(cclframe.caData,0,MAXFRAMESIZE);
     
     // now set the command
     cclframe.caData[0]=CCLCOMMAND;
     cclframe.caData[2]=(cmdno&(0x0ff));
     cclframe.caData[3]=(cmdno&(0x0ff00))>>8;

     return(&cclframe);
}


//
// make StateXY frame
//
sDataFrame *CCL_MakeStateXYFrame(
	int src,
	int dest,
	int ack, 
    CCL_statexy_t *sXY){
	int ix,iy,idepth,ialt;
	
    // zeros everywhere 
    memset(cclframe.caData,0,MAXFRAMESIZE);

	// frame header
	cclframe.iSrc = src ;
	cclframe.iDest = dest ;
    cclframe.bAck = ack%2;
	cclframe.iFrameNum=1;
	cclframe.bValid = 1;
	cclframe.iType = CCLSTATEXY;
	cclframe.iSize = 32;

	// byte 0: the CCL "mode" - frame type
    cclframe.caData[0]=CCLSTATEXY;

    // byte 1-3: X in meters (24 bit precision)
	ix=CCL_EncodeXY(sXY->x);                      
	cclframe.caData[3]=(((ix)&(0x00ff0000))>>16);       // MSB
	cclframe.caData[2]=(((ix)&(0x0000ff00))>>8);        // middle byte
	cclframe.caData[1]=(ix)&(0x000000ff);               // LSB

    // byte 4-6: Y in meters (24 bit precision)
	iy=CCL_EncodeXY(sXY->y);
	cclframe.caData[6]=(((iy)&(0x00ff0000))>>16);       // MSB
	cclframe.caData[5]=(((iy)&(0x0000ff00))>>8);        // middle byte
	cclframe.caData[4]=(iy)&(0x000000ff);               // LSB

	// byte 7: fix age
	cclframe.caData[7]=(sXY->fix_age)&(0x0ff);
	
    // byte 8-10: time_date

    // byte 11: heading
    cclframe.caData[11]=(int)floor((sXY->heading)/360.0 * 256.0);

    // byte 12-13: mission_mode_depth
	idepth=CCL_EncodeDepth(sXY->depth);
    cclframe.caData[13]=((idepth)&(0xff00))>>8;
	cclframe.caData[12]=(idepth)&(0x0ff);

    // byte 18: mission leg
	cclframe.caData[18]=(sXY->goal_id)&(0x0ff);                // mod 256

    //byte 19-20: goal depth
	idepth=CCL_EncodeDepth(sXY->goal_depth);
        cclframe.caData[20]=((idepth)&(0xff00))>>8;
	cclframe.caData[19]=(idepth)&(0x0ff);

    //byte 21: watts                                     

	//byte 23-25: goal x
	ix=CCL_EncodeXY(sXY->goal_x);                      
	cclframe.caData[25]=(((ix)&(0x00ff0000))>>16);       // MSB
	cclframe.caData[24]=(((ix)&(0x0000ff00))>>8);        // middle byte
	cclframe.caData[23]=(ix)&(0x000000ff);               // LSB

	//byte 26-28: goal y
	iy=CCL_EncodeXY(sXY->goal_y);                      
	cclframe.caData[28]=(((iy)&(0x00ff0000))>>16);       // MSB
	cclframe.caData[27]=(((iy)&(0x0000ff00))>>8);        // middle byte
	cclframe.caData[26]=(iy)&(0x000000ff);               // LSB

        //byte 29: battery percent

        //bytes 30-31: altitude
	if (sXY->alt==-98765.0) sXY->alt=(float)654.32;
	ialt=CCL_EncodeAltitude(sXY->alt);
        cclframe.caData[31]=((ialt)&(0xff00))>>8;        // MSB
	cclframe.caData[30]=(ialt)&(0x0ff);                  // LSB
	return(&cclframe);	
}


sDataFrame *CCL_MakeNavXYFrame(
	int src,
	int dest,
	int ack, 
	CCL_navxy_t *sNavXY){

        int idepth,icep,ix,iy;

        // zeros everywhere 
        memset(cclframe.caData,0,MAXFRAMESIZE);

	// frame header
	cclframe.iSrc = src ;
	cclframe.iDest = dest ;
        cclframe.bAck = ack%2;
	cclframe.iFrameNum=1;
	cclframe.bValid = 1;
	cclframe.iType = CCLNAVXY;
	cclframe.iSize = 32;

	// byte 0: the CCL "mode" - frame type
        cclframe.caData[0]=CCLNAVXY;

	// byte 1: fix type
        cclframe.caData[1]=sNavXY->fix_type;  

        // byte 2: heading
        cclframe.caData[2]=(int)MIN(floor((sNavXY->heading)/360.0 * 256.0),255);

	// byte 3: speed
	cclframe.caData[3]=(int)MIN(floor((sNavXY->speed)/6.4 * 256.0),255);

	// byte 4-5: depth 
	idepth=CCL_EncodeDepth(sNavXY->depth);
        cclframe.caData[5]=((idepth)&(0xff00))>>8;
	cclframe.caData[4]=(idepth)&(0x0ff);

	// byte 6-7: CEP
        icep=(int)(sNavXY->cep*10);
        cclframe.caData[7]=((icep)&(0xff00))>>8;
	cclframe.caData[6]=(icep)&(0x0ff);

	//byte 8-11: csound_gpsstd

	//byte 12-15: x meters
	ix=(int)(sNavXY->x*100); // cm accurate
	cclframe.caData[15]=(((ix)&(0xff000000))>>24);       // MSB
	cclframe.caData[14]=(((ix)&(0x00ff0000))>>16);       // middle byte
	cclframe.caData[13]=(((ix)&(0x0000ff00))>>8);        // middle byte
	cclframe.caData[12]=(ix)&(0x000000ff);               // LSB

	//byte 16-19: y meters
	iy=(int)(sNavXY->y*100); // cm accurate
	cclframe.caData[19]=(((iy)&(0xff000000))>>24);       // MSB
	cclframe.caData[18]=(((iy)&(0x00ff0000))>>16);       // middle byte
	cclframe.caData[17]=(((iy)&(0x0000ff00))>>8);        // middle byte
	cclframe.caData[16]=(iy)&(0x000000ff);               // LSB

	//byte 20-23: unix time of packet
 	cclframe.caData[23]=(((sNavXY->packet_time)&(0xff000000))>>24); // MSB
	cclframe.caData[22]=(((sNavXY->packet_time)&(0x00ff0000))>>16); // middle byte
	cclframe.caData[21]=(((sNavXY->packet_time)&(0x0000ff00))>>8);  // middle byte
	cclframe.caData[20]=(sNavXY->packet_time)&(0x000000ff);         // LSB

	//byte 24-27: unix time of fix
 	cclframe.caData[27]=(((sNavXY->fix_time)&(0xff000000))>>24); // MSB
	cclframe.caData[26]=(((sNavXY->fix_time)&(0x00ff0000))>>16); // middle byte
	cclframe.caData[25]=(((sNavXY->fix_time)&(0x0000ff00))>>8);  // middle byte
	cclframe.caData[24]=(sNavXY->fix_time)&(0x000000ff);         // LSB

	// byte 28-29: minutes since synch: 
    cclframe.caData[29]=((sNavXY->synch_min)&(0xff00))>>8;  // MSB
	cclframe.caData[28]=(sNavXY->synch_min)&(0x0ff);        // LSB
   
	// bytes 30-31: gps hdop nsat

	return(&cclframe);	
}

sDataFrame *CCL_MakeContactXYFrame(
	int src,
	int dest,
	int ack, 
	CCL_contactxy_t	*sCXY){

	int ix,iy,iheading,packedhms;
    int irange,ibearing,ispeed;

	// frame header
	cclframe.iSrc = src ;
	cclframe.iDest = dest ;
    cclframe.bAck = ack%2;
	cclframe.iFrameNum=1;
	cclframe.bValid = 1;
	cclframe.iType = CCLCONTACTXY;
	cclframe.iSize = 32;

	// byte 0: the CCL "mode" - frame type
    cclframe.caData[0]=CCLCONTACTXY;

    // byte 1-3: X in meters (24 bit precision)
	ix=CCL_EncodeXY(sCXY->x);                      
	cclframe.caData[3]=(((ix)&(0x00ff0000))>>16);       // MSB
	cclframe.caData[2]=(((ix)&(0x0000ff00))>>8);        // middle byte
	cclframe.caData[1]=(ix)&(0x000000ff);               // LSB

    // byte 4-6: Y in meters (24 bit precision)
	iy=CCL_EncodeXY(sCXY->y);
	cclframe.caData[6]=(((iy)&(0x00ff0000))>>16);       // MSB
	cclframe.caData[5]=(((iy)&(0x0000ff00))>>8);        // middle byte
	cclframe.caData[4]=(iy)&(0x000000ff);               // LSB

	// byte 7: heading
    iheading=(int)(sCXY->heading/360.0*256.0);
    cclframe.caData[7]=iheading;

	// byte 8-9: packed HMS of observer position
    packedhms = ((sCXY->hrs&(0x001f))<<11) |            // 5 bit for hrs
		        ((sCXY->min&(0x003f))<<5) |             // 6 bits for min
		        (((sCXY->sec/2)&(0x001f)));             // 5 bits for sec 
				
	cclframe.caData[9]=(((packedhms)&(0x0000ff00))>>8); // MSB
	cclframe.caData[8]=(packedhms)&(0x000000ff);        // LSB

    // byte 10: # of contacts here
    cclframe.caData[10]= sCXY->ncontacts & (0x00ff);

    // byte 11: contact#1 ID & type (packed)
    cclframe.caData[11]= ((sCXY->det[0].id & (0x001f))<<3) |
                         (sCXY->det[0].type & (0x0007));

    // byte 12: contact#1 range
    irange= (int)(sCXY->det[0].range/20) ;
    cclframe.caData[12]= MIN(irange,255);

    // byte 13: contact#1 bearing
    ibearing=(int)(sCXY->det[0].bearing/360*256);
    cclframe.caData[13]= MIN(ibearing,255);

    // byte 14: contact#1 heading
    iheading=(int)(sCXY->det[0].heading/360*256);
    cclframe.caData[14]= MIN(iheading,255);

    // byte 15: contact#1 speed
    ispeed=(int)(sCXY->det[0].speed/5) ; 
    cclframe.caData[15]= MIN(ispeed,255);

	// byte 16-17: packed HMS of contact#1
    packedhms = ((sCXY->det[0].hrs&(0x001f))<<11) |      // 5 bit for hrs
		        ((sCXY->det[0].min&(0x003f))<<5) |       // 6 bits for min
		        (((sCXY->det[0].sec/2)&(0x001f)));       // 5 bits for sec 
				
	cclframe.caData[17]=(((packedhms)&(0x0000ff00))>>8); // MSB
	cclframe.caData[16]=(packedhms)&(0x000000ff);        // LSB


	// byte 18: contact#2 ID & type (packed)
    cclframe.caData[18]= ((sCXY->det[1].id & (0x001f))<<3) |
                         (sCXY->det[1].type & (0x0007));

    // byte 19: contact#2 range
    irange= (int)(sCXY->det[1].range/20) ;
    cclframe.caData[19]= MIN(irange,255);

    // byte 20: contact#2 bearing
    ibearing=(int)(sCXY->det[1].bearing/360*256) ;
    cclframe.caData[20]= MIN(ibearing,255);

    // byte 21: contact#2 heading
    iheading=(int)(sCXY->det[1].heading/360*256) ;
    cclframe.caData[21]= MIN(iheading,255);

    // byte 22: contact#2 speed
    ispeed= (int)(sCXY->det[1].speed/5) ; 
    cclframe.caData[22]= MIN(ispeed,255);

	// byte 23-24: packed HMS of contact#2
    packedhms = ((sCXY->det[1].hrs&(0x001f))<<11) |      // 5 bit for hrs
		        ((sCXY->det[1].min&(0x003f))<<5) |       // 6 bits for min
		        (((sCXY->det[1].sec/2)&(0x001f)));       // 5 bits for sec 
				
	cclframe.caData[24]=(((packedhms)&(0x0000ff00))>>8); // MSB
	cclframe.caData[23]=(packedhms)&(0x000000ff);        // LSB


	// byte 25: contact#3 ID & type (packed)
    cclframe.caData[25]= ((sCXY->det[2].id & (0x001f))<<3) |
                         (sCXY->det[2].type & (0x0007));

    // byte 26: contact#3 range
    irange=(int)(sCXY->det[2].range/20) ;
    cclframe.caData[26]= MIN(irange,255);

    // byte 27: contact#3 bearing
    ibearing=(int)(sCXY->det[2].bearing/360*256) ;
    cclframe.caData[27]= MIN(ibearing,255);

    // byte 28: contact#3 heading
    iheading= (int)(sCXY->det[2].heading/360*256) ;
    cclframe.caData[28]= MIN(iheading,255);

    // byte 29: contact#3 speed
    ispeed= (int)(sCXY->det[2].speed/5) ; 
    cclframe.caData[29]= MIN(ispeed,255);

	// byte 30-31: packed HMS of contact#3
    packedhms = ((sCXY->det[2].hrs&(0x001f))<<11) |      // 5 bit for hrs
		        ((sCXY->det[2].min&(0x003f))<<5) |       // 6 bits for min
		        (((sCXY->det[2].sec/2)&(0x001f)));       // 5 bits for sec 
				
	cclframe.caData[31]=(((packedhms)&(0x0000ff00))>>8); // MSB
	cclframe.caData[30]=(packedhms)&(0x000000ff);        // LSB

	return(&cclframe);	
}



unsigned int CCL_EncodeXY(float xy){
  // 24 bits, sign bit and 23 magnitude bits
  //
  int i,sgn;

  i=(int)floor(xy+0.5);                      
  if (i<0) {
    sgn=0x800000;
    i=-1*i;
  }else{
    sgn=0;
  }
  i=i|(sgn);
  return(i);
}


unsigned short CCL_EncodeDepth(float depth) 
   /* 0 -100 meters: 0-1000 (10 cm resolution) 
    * 100 -200 meters: 1001-1500 (20 cm resolution) 
    * 200 -1000 meters: 1501-3100 (50 cm resolution) 
    * 2 10 * */ 
{ 
  if (depth < 0) return(0); 
  if (depth < 100) return((int)((depth+.05) * 1.0/0.1)); 
  if (depth < 200) return((int)(((depth-100) + 0.1 ) * 1.0/0.2 + 1000)); 
  if (depth < 1000)return((int)(((depth-200) + 0.25) * 1.0/0.5 + 1500)); 
  if (depth < 6000)return((int)(((depth-1000) + 0.5 ) * 1.0/1.0 + 3100)); 
  return(8100); 
}

unsigned short CCL_EncodeAltitude(float alt)

{
  alt*=100;
  if (alt>65535.0)
    return(65535U);
  else if (alt<0)
    return(0);
  return((unsigned short)alt);
}

//
// decode frame data to command struct
//
void CCL_RxCommandFrame(sDataFrame *f, CCL_command_t *c)
{
     int l;

     // copy the header fields
     c->src  = f->iSrc;
     c->dest = f->iDest;
     c->mode = f->iType;
     
     // now the data
     c->command = f->caData[2]+f->caData[3]*256;
     for (l=0;l<28;l++)
	  c->parameter[l]=f->caData[l+4];
	  
     return;
     
}

void CCL_RxStateXYFrame(sDataFrame *f, CCL_statexy_t *s){

        unsigned short ialt,idepth;

	// mode
	s->mode=CCLSTATEXY;

    // unpack X
	if ((f->caData[3])>127)	 // check sign bit: if (neg)
		s->x=-1*(((f->caData[3]&(0x7f))<<16)+(f->caData[2]<<8)+f->caData[1]);	
	else
		s->x=((f->caData[3]&(0x7f))<<16)+(f->caData[2]<<8)+f->caData[1];	
	
	// unpack Y
	if ((f->caData[6])>127)	 // check sign bit: if (neg)
		s->y=-1*(((f->caData[6]&(0x7f))<<16)+(f->caData[5]<<8)+f->caData[4]);	
	else
		s->y=((f->caData[6]&(0x7f))<<16)+(f->caData[5]<<8)+f->caData[4];	

    // unpack depth
	idepth=f->caData[12]+(f->caData[13]*256);
	s->depth=CCL_DecodeDepth(idepth);

	// goal ID
	s->goal_id = f->caData[18];
        	
	// unpack goal depth
	idepth=f->caData[19]+(f->caData[20]*256);
	s->goal_depth=CCL_DecodeDepth(idepth);
 
	// unpack goal_X
	if ((f->caData[25])>127)	 // check sign bit: if (neg)
		s->goal_x=-1*(((f->caData[25]&(0x7f))<<16)+(f->caData[24]<<8)+f->caData[23]);	
	else
		s->goal_x=((f->caData[25]&(0x7f))<<16)+(f->caData[24]<<8)+f->caData[23];	

	// unpack goal_Y
	if ((f->caData[28])>127)	 // check sign bit: if (neg)
		s->goal_y=-1*(((f->caData[28]&(0x7f))<<16)+(f->caData[27]<<8)+f->caData[26]);	
	else
		s->goal_y=((f->caData[28]&(0x7f))<<16)+(f->caData[27]<<8)+f->caData[26];	

    // unpack alt
	ialt=f->caData[30]+(f->caData[31]*256);
	s->alt=CCL_DecodeAltitude(ialt);

	return;
	
}

void CCL_RxNavXYFrame(sDataFrame *f, CCL_navxy_t *s){
  
  int iDepth,iCEP,iX,iY;
  
  // fix type      
  s->fix_type=f->caData[1];

  // heading
  s->heading=(f->caData[2]*360.0)/256.0;

  // speed
  s->speed=(f->caData[3]*6.4)/256.0;

  // depth
  iDepth=f->caData[5]*256+f->caData[4];
  s->depth=CCL_DecodeDepth(iDepth);

  // CEP
  iCEP=f->caData[7]*256+f->caData[6];
  s->cep=iCEP/10.0;

  // csound...gpsstd

  // X
  iX = (f->caData[15]<<24)|(f->caData[14]<<16)|(f->caData[13]<<8)|(f->caData[12]);
  s->x=iX/100.0;

  // Y
  iY = (f->caData[19]<<24)|(f->caData[18]<<18)|(f->caData[17]<<8)|(f->caData[16]);
  s->y=iY/100.0;

  // packet_time
  s->packet_time=(f->caData[23]<<24)|(f->caData[22]<<18)|(f->caData[21]<<8)|(f->caData[20]);

  // fix_time
  s->fix_time=(f->caData[27]<<24)|(f->caData[26]<<18)|(f->caData[25]<<8)|(f->caData[24]);
 
  // synch_min
  s->synch_min=(f->caData[29]<<8)|(f->caData[28]);

  // hdop_nsat
 
  return;
	
}

void CCL_RxContactXYFrame(sDataFrame *f, CCL_contactxy_t *s){

 
	return;
	
}

void CCL_RxContactCMDFrame(sDataFrame *f, CCL_contact_cmd_t *s){

 
	return;
	
}
