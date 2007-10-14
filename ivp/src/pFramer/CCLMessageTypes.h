/*
 *  CCL message types and commands types for PLUSNet 
 *
 *  Copyright(c)2006, Woods Hole Oceanographic Institution
 *
 *  Based on document "A Compact Control Language for Underwater Vehicles"
 *
 *  Author: Matt Grund, mgrund@whoi.edu
 *
 *  Last modified: July 31, 2006
 *
 *  $Id: CCLMessageTypes.h,v 1.1.1.1 2007-09-28 16:38:01 ajshafer Exp $
 *
 *  ChangeLog:   Version        Description
 *
 *                 1.0          Defines from Roger's spec
 *                 1.1          Added CNA and FSU USBL
 *                 1.2          Added Kelp and XRay engineering messages
 *                                 and PLUSNet messages
 */

#ifndef _CCL_MESSAGE_TYPES_H
#define _CCL_MESSAGE_TYPES_H


/* CCL frame types
 *
 */
#define MDAT_TEST           (0)   /* TEST - the bytes 0...<framesize-1> */
#define MDAT_EMPTY          (6)
#define MDAT_REDIRECT       (7)   /* VIP */
#define MDAT_USBLPOS        (8)
#define MDAT_BATHY          (9)   /* REMUS */
#define MDAT_CTD            (10)  /* REMUS */
#define MDAT_COMMAND        (11)  /* VIP */
#define MDAT_USBLDATA       (12) 
#define MDAT_CADCACDET      (13)
#define MDAT_STATE          (14)  /* REMUS */
#define MDAT_ERROR          (15)
#define MDAT_RANGER         (16)  /* REMUS */
#define MDAT_MLOREQ         (17)
#define MDAT_STATEXY        (18)  /* SEABED, MIT */
#define MDAT_NAV            (19)  /* BLUEFIN CNA */
#define MDAT_NAVXY          (20)  /* MIT KAYAKS */
#define MDAT_REACQ          (21)
#define MDAT_FILE           (22)

#define MDAT_ARC            (24)
#define MDAT_CADCACPLUS     (25)
#define MDAT_SENSOR_CMD     (26) /* PLUSNET NAFCON COMMANDS */
#define MDAT_SENSOR_RPT     (27) /* PLUSNET NAFCON MESSAGES */
#define MDAT_TARGETSHIP     (28) /* FSU USBL TELEMETRY */
#define MDAT_XRAY_BEHAVIOR  (29) /* XRAY GLIDER */
#define MDAT_XRAY_STATUS    (30) /* XRAY GLIDER */
#define MDAT_KELP_COMMAND   (31) /* KELP ASW NODE */
#define MDAT_KELP_STATUS    (32) /* KELP ASW NODE */
#define MDAT_CHAT           (33) /* ModemTool */
#define MDAT_EDGESERVE     (254) /* BLUEFIN TOPSIDE/AUV LINK */


/* CCL Command types( bytes 3&4 of MDAT_COMMAND )
 *
 */
#define VC_NONE                            (0)
#define VC_ABORT_END                       (1)
#define VC_ABORT                           (2)
#define VC_RUN                             (3)
#define VC_RELEASE_DESCENT_WEIGHT          (4)
#define VC_RELEASE_ASCENT_WEIGHT           (5)
#define VC_RELEASE_PICKUP_FLOAT            (6)
#define VC_COMM_IS_A_RANGER_PING           (7)
#define VC_COMM_IS_NOT_A_RANGER_PING       (8)
#define VC_SET_USBL_PITCH_OFFSET           (9)
#define VC_SET_USBL_ROLL_OFFSET            (10)
#define VC_ENABLE_MODEM_USBL               (11)
#define VC_DISABLE_MODEM_USBL              (12)
#define VC_SET_USBL_SOUND_SPEED            (13)
#define VC_ACCEPT_NEXT_FIX                 (14)
#define VC_DUMP_REDIRECT_COMMANDS          (15)
#define VC_GET_GPS_FIX                     (16)
#define VC_ABORT_START                     (17)
#define VC_ABORT_DESTINATION               (18)
#define VC_DUMP_REDIRECT_EXCEPT_CURRENT    (19)
#define VC_ABORT_DROP_ASCENT_POWER_UP      (20)
#define VC_DISABLE_PING_AND_RANGER         (21)
#define VC_ENABLE_PING_AND_RANGER          (22)
#define VC_RESTART                         (23)
#define VC_ABORT_END_LOITER                (24)
#define VC_RECORDER_ON                     (25)  /* KELP RAW RECORDER */
#define VC_RECORDER_OFF                    (26)  /* KELP RAW RECORDER */
#define VC_RECOVER_ABORT                   (50)  /* Bluefin Abort */

#endif /* #ifndef _CCL_MESSAGE_TYPES_H - end of header file */
