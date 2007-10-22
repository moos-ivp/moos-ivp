//
// ccl.h - CCL protocol layer
//

// Copyright(c)2004, Matt Grund, WHOI. <mgrund@whoi.edu>

#ifndef _ccl_h
#define _ccl_h
#include "umodem.h"

// macros
#define MAXFRAMESIZE (256)
#define MIN(A,B) ((A<B)?(A):(B))

// CCL frame types
#define CCLTEST       (0)   // the bytes 0...<framesize-1>
#define CCLEMPTY      (6)
#define CCLREDIRECT   (7)   // VIP
#define CCLUSBLPOS    (8)
#define CCLBATHY      (9)   // REMUS
#define CCLCTD        (10)  // REMUS
#define CCLCOMMAND    (11)  // VIP
#define CCLUSBLDATA   (12) 
#define CCLCADCACDET  (13)
#define CCLSTATE      (14)  // REMUS
#define CCLERROR      (15)
#define CCLRANGER     (16)  // REMUS
#define CCLMLOREQ     (17)
#define CCLSTATEXY    (18)  // a UTM version of STATE
#define CCLNAV        (19)  
#define CCLNAVXY      (20)
#define CCLREACQ      (21)
#define CCLFILE       (22)
#define CCLEDGESERVE  (23)
#define CCLARC        (24)
#define CCLCONTACT    (25)
#define CCLCONTACTXY  (26)
#define CCLCONTACTCMD (27)


// some ASW contact types
#define CTNOCONTACT   (0)  // contact disappears
#define CTAMBBEARING  (1)  // ambiguous relative bearing only
#define CTBEARING     (2)  // bearing only
#define CTRANGE       (3)  // range only
#define CTRNGAMBBEAR  (4)  // range + amb. rel. earing
#define CTRANGEBEAR   (5)  // range + bearing
#define CTFULL        (6)  // range + bearing + speed + heading
#define CTUNUSED      (7)  // suggestions?

// some ASW contact command types
#define CTCMDNOOP     (0)  // stop/do nothing
#define CTCMDFOLLOW   (1)  // keep in FOV
#define CTCMDXC       (2)  // intercept!
#define CTCMDXCACTIVE (3)  // go active and intercept
#define CTCMDXCSENSE  (4)  // intercept and observe with sensor x

// some NAVXY fix types
#define FIXISGPS        (0)
#define FIXISINS        (1)
#define FIXISACOUSTIC   (2)
#define FIXISDEADRECKON (3)
 
/* Latitude/Longitude 
 * Each are encoded into 3 bytes, giving a resolution of about 2 
 * meters. The compressed form is stored in a LATLON_COMPRESSED 
 * struct, which is 3 bytes. */ 
typedef struct { 
        char compressed[3]; 
} LATLON_COMPRESSED;

typedef union { 
        long as_long; 
        LATLON_COMPRESSED as_compressed; 
} LONG_AND_COMP;

typedef struct 
{
	char compressed[3];
} XY_COMPRESSED;

typedef struct 
{
	char compressed[3];
} TIME_DATE ;




typedef struct {
	unsigned char  mode;                // mode = CCLSTATEXY 
	int	       src;
	int            dest;
	float          x;                      
	float          y;                      
	float          heading;              
	float          depth;
        float          alt;
	float          est_velocity;           
	char           goal_id;      
	float          goal_x;                 
	float          goal_y;
        float          goal_depth;
	float          pitch;
	float          roll;
	unsigned short mission_mode;
	unsigned char  fix_age;        
	float          watts_encoded; 
	float          battery_percent;
} CCL_statexy_t;

typedef struct 
{
     unsigned char src;
     unsigned char dest;
     unsigned char mode;
     unsigned char command;
     char          parameter[28];
} CCL_command_t;

struct CCL_contact_det_t
{
	 int id;                 // id assigned by detecting node
	 int type;               // contact type see CTxxx above
     int src;                // who spotted it
     double range;	         // range in meters
	 double bearing;          // bearing in degrees
     double heading;          // heading in degrees
	 double speed;            // speed in knots
	 int hrs;                // time_hrs
	 int min;                // time_min
	 int sec;                // time_sec;
	 bool operator <(const CCL_contact_det_t & t){return src<t.src;};
};

struct CCL_contactxy_t
{
	unsigned char src;        // sender address
	unsigned char dest;       // receiver address
	unsigned char mode;       // ==CCLCONTACTXY
	double x;                  // observer pos'n
	double y;
	unsigned char hrs;        // timestamp of obs pos'n
	unsigned char min;        
	unsigned char sec;
	unsigned char ncontacts;  // in this packet
	CCL_contact_det_t det[3]; // the dets
public:
	float heading;
} ;

struct CCL_contact_cmd_t
{
 	unsigned char src;        // sender address
	unsigned char dest;       // receiver address
	unsigned char mode;       // ==CCLCONTACTCMD
	unsigned char contactid;  // # 
    unsigned char obsaddress; // which list to look in
    unsigned char hrs;        // cmd timestamp
    unsigned char min;        // 
	unsigned char sec;
    unsigned char unit1;      // unit to obey cmd
    unsigned char unit2;      // unit to obey cmd  
    unsigned char unit3;
    unsigned char unit4;
	unsigned char cmdtype;    // see CCMDxxx types
	unsigned char args[19];   // cmd specific args
} ;

struct CCL_navxy_t
{
	unsigned char src;         // sender address;
	unsigned char dest;        // receiver address;
	unsigned char mode;        // ==CCLNAVXY
	unsigned char fix_type;    // GPS, etc. see doc.
	float heading;             // beacon heading (deg)
	float speed;               // beacon speed (knots)
	float depth;               // beacon depth (meters)
	float x;                   // beacon pos'n (x) 
	float y;                   // beacon pos'n (y)
	float cep;                 // CEP - error (cm)
	float c_sound;             // speed of sound (m/s)
    float gpsstd;              // ??
	unsigned long fix_time;    // time of beacon nav fix
    unsigned long packet_time; // time of packet tx
	int synch_min;             // min since clock synch
    int gps_nsat;              // number of satellites
	int gps_hdop;              // ??
} ;

// new seabed style protos
void CCL_RxCommandFrame(sDataFrame *frame,CCL_command_t *cmd);
void CCL_RxStateXYFrame(sDataFrame *frame,CCL_statexy_t *s);
void CCL_RxNavXYFrame(sDataFrame *frame,CCL_navxy_t *s);
void CCL_RxContactXYFrame(sDataFrame *frame,CCL_contactxy_t *s);
void CCL_RxContactCMDFrame(sDataFrame *frame,CCL_contact_cmd_t *s);

// protos
void CCL_Init(void);
void CCL_RxFrame(sDataFrame *frame);
void CCL_RxStateFrame(sDataFrame *frame);
void CCL_RxBathyFrame(sDataFrame *frame);
void CCL_RxCTDFrame(sDataFrame *frame);
void CCL_RxRangerFrame(sDataFrame *frame);
void CCL_TxFrame(sDataFrame *frame);
unsigned short CCL_EncodeDepth(float depth);
unsigned short CCL_EncodeAltitude(float alt);
unsigned int CCL_EncodeXY(float xy);
double CCL_DecodeLatLon(LATLON_COMPRESSED cCompressed);
int CCL_FrameIsTest(sDataFrame *frame);
sDataFrame *CCL_MakeTestFrame(int src,int dest,int ack,int fsize);
sDataFrame *CCL_MakeCommandFrame(int src,int dest,int ack,unsigned short cmdno);
sDataFrame *CCL_MakeStateXYFrame(
	int src,
	int dest,
	int ack, 
    CCL_statexy_t *sStateXY);

sDataFrame *CCL_MakeNavXYFrame(
	int src,
	int dest,
	int ack, 
	CCL_navxy_t *sNavXY);

sDataFrame *CCL_MakeContactXYFrame(
	int src,
	int dest,
	int ack, 
	CCL_contactxy_t	*sContact);


#endif // #ifdef _ccl_h
