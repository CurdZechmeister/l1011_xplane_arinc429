/**
 *
 *  BTI429 DRIVER EXAMPLE 2  (06/09/2011)
 *  Copyright (c) 2001-2011
 *  Ballard Technology, Inc.
 *  www.ballardtech.com
 *  support@ballardtech.com
 *  ALL RIGHTS RESERVED
 *
 *  NAME:   EXAMP2.C -- Transmitter example
 *
 **/

/**
 *
 *  This program opens Card Number 0; and configures the
 *  first compatible core to build a transmit schedule
 *  using transmit interval information, and then transmit
 *  the messages with changing data.
 *
 **/

#include "BTICard.H"
#include "BTI429.H"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <memory.h>
#include <math.h>
#include <WinSock2.h>

#define SSM_NORMAL    6
#define RAD_STEPS_15  182
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 200				  //Max length of buffer
#define PORT 25001                //The port on which to listen for incoming data

#define     METERS_PER_SEC  1.943844924406

#define	CARDNUM		  0
#define	MAX_CORES     4
#define	MAX_CHANNELS  32
#define MAX_SLOTS     32

#define RX1_NumLabel   0			// Radio 1 RX Slow Speed
#define RX2_NumLabel   0			// Radio 2 RX Slow Speed
#define RX3_NumLabel   0			// Radio 3 RX Slow Speed
#define RX4_NumLabel   0			// ATC RX Slow Speed
#define RX5_NumLabel   0
#define RX6_NumLabel   0
#define RX7_NumLabel   0
#define RX8_NumLabel   0
#define RX9_NumLabel   0
#define RX10_NumLabel  0
#define RX11_NumLabel  0
#define RX12_NumLabel  0
#define RX13_NumLabel  0
#define RX14_NumLabel  0
#define RX15_NumLabel  0
#define RX16_NumLabel  0


#define TX1_NumLabel   6			// Capt ARINC 429 High Speed 1
#define TX2_NumLabel  18			// Capt ARINC 429 Slow Speed 1
#define TX3_NumLabel   1			// Capt ARINC 575 Slow Speed 2
#define TX4_NumLabel   6			// F/O ARINC 429 High Speed 1
#define TX5_NumLabel  18			// F/O ARINC 429 Slow Speed 1
#define TX6_NumLabel   1			// F/O ARINC 575 Slow Speed 2
#define TX7_NumLabel   1			// Radio TX Channel
#define TX9_NumLabel   0			// Capt ARINC 429 Slow Speed 3
#define TX10_NumLabel  0			// Capt F/O 429 Slow Speed 3
#define TX11_NumLabel  0
#define TX12_NumLabel  0
#define TX13_NumLabel  0
#define TX14_NumLabel  0
#define TX15_NumLabel  0
#define TX16_NumLabel  0

/* UDP Payload Structure */
typedef struct _payload {
    char control;
	__int16 label;
    float data;
} sim429payload;


void main(void)
{
    
	int value;
    
	HCARD hCard;
	HCORE hCore;
	ERRVAL errval;
	INT corenum;
	INT xmtchan;
	MSGADDR tx1_msgaddr[TX1_NumLabel];         // Captains HS Bus 1
	INT	tx1_min_intrvls[TX1_NumLabel];
	INT	tx1_max_intrvls[TX1_NumLabel];
    
	MSGADDR tx2_msgaddr[TX2_NumLabel];        // Captains LS Bus 1
	INT	tx2_min_intrvls[TX2_NumLabel];
	INT	tx2_max_intrvls[TX2_NumLabel];
    
	MSGADDR tx3_msgaddr[TX3_NumLabel];        // Captains LS Bus 1
	INT	tx3_min_intrvls[TX3_NumLabel];
	INT	tx3_max_intrvls[TX3_NumLabel];
    
	MSGADDR tx4_msgaddr[TX4_NumLabel];        // F/O HS Bus 1
	INT	tx4_min_intrvls[TX4_NumLabel];
	INT	tx4_max_intrvls[TX4_NumLabel];
    
	MSGADDR tx5_msgaddr[TX5_NumLabel];        // F/O LS Bus 1
	INT	tx5_min_intrvls[TX5_NumLabel];
	INT	tx5_max_intrvls[TX5_NumLabel];
    
	MSGADDR tx6_msgaddr[TX6_NumLabel];        // F/O LS Bus 1
	INT	tx6_min_intrvls[TX6_NumLabel];
	INT	tx6_max_intrvls[TX6_NumLabel];
    
    float data_value;
    
	/* Label Defintions */
	MSGSTRUCT429 distance_togo;     // Label 001 12Kb/sec  200ms
	MSGSTRUCT429 ground_speed;		// Label 012 100Kb/sec 200ms (BCD)
	MSGSTRUCT429 radial_select;		// Label 100 12Kb/sec  62.5ms
	MSGSTRUCT429 heading_select;    // Label 101 12Kb/sec  62.5ms
	MSGSTRUCT429 qfu;				// Label 105 12Kb/sec  62.5ms
	MSGSTRUCT429 nav_course;		// Label 114 12Kb/sec  62.5ms
	MSGSTRUCT429 cross_track;       // Label 116 12Kb/sec  62.5ms
	MSGSTRUCT429 nav_vertical;      // Label 117 12Kb/sec  62.5ms
	MSGSTRUCT429 adf1;				// Label 162 12Kb/sec  62.5ms
	MSGSTRUCT429 adf2;				// Label 162 12Kb/sec  62.5ms
	MSGSTRUCT429 fd_roll;			// Label 140 12Kb/sec  62.5ms
	MSGSTRUCT429 fd_pitch;			// Label 141 12Kb/sec  62.5ms
	MSGSTRUCT429 fast_slow;			// Label 142 12Kb/sec  62.5ms
	MSGSTRUCT429 fd_yaw;			// Label 143 12KB/sec  62.5ms
	MSGSTRUCT429 localizer;			// Label 173 12Kb/sec  62.5ms
	MSGSTRUCT429 glideslope;		// Label 174 12Kb/sec  62.5ms
	MSGSTRUCT429 omnibearing;		// Label 222 12Kb/sec  62.5ms
	MSGSTRUCT429 discrete1;         // Label 270 12Kb/sec  100ms
	MSGSTRUCT429 true_heading;      // Label 314 12Kb/sec  50ms
	MSGSTRUCT429 magnetic_heading;  // Label 320 100Kb/sec 50ms
	MSGSTRUCT429 drift;				// Label 321 100Kb/sec 50ms
	MSGSTRUCT429 roll_angle;		// Label 325 100Kb/sec 50ms
	MSGSTRUCT429 pitch_angle;		// Label 324 100Kb/sec 50ms
	MSGSTRUCT429 vvi;               // Label 365 100Kb/sec 50ms
	MSGSTRUCT429 baro_altitude;     // Label 204 12Kb/sec 62.5ms
    
    /* UDP Datas Setup */
	sim429payload *payload;
	SOCKET s;
	struct sockaddr_in server;
	struct sockaddr_in si_other;
    int slen;
	int recv_len;
    char buf[BUFLEN];
    WSADATA wsa;
	slen = sizeof(si_other);
    
	printf("\n    **********************************************************************");
	printf("\n    *                                                                    *");
	printf("\n    *  L1011 Simulator ARINC 429 Driver                                  *");
	printf("\n    *                                                                    *");
	printf("\n    **********************************************************************");
	printf("\n");
    
	/* Allocate UDP payload memory */
    payload = (sim429payload*)(malloc(sizeof(sim429payload)));
    memset(payload, '\0', sizeof(payload));
    
    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
    
    //Create a socket
    if((s = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");
    
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );
    
    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");
    
    /**
     *
     *  Open the card with the specified card number.
     *  An error value is returned which is tested
     *  to determine if an error occurred.
     *
     **/
    
	errval = BTICard_CardOpen(&hCard,CARDNUM);
    
	if (errval)
	{
		printf("\nError:  Either card number %u is not present, or",CARDNUM);
		printf("\n        an error occurred (%i) opening the card.",errval);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		printf("\n");
		exit(1);
	}
    
	printf("\nUsing ARINC 429 card #%d",CARDNUM);
    
    /**
     *
     *  Find the first 429 core with a transmit channel.
     *
     **/
    
	for (corenum=0;corenum<MAX_CORES;corenum++)
	{
		errval = BTICard_CoreOpen(&hCore,corenum,hCard);
		if (errval) break;
        
		for (xmtchan=0;xmtchan<MAX_CHANNELS;xmtchan++)	if (BTI429_ChIsXmt(xmtchan,hCore))	break;
        
		if (xmtchan != MAX_CHANNELS) break;
	}
    
	if (errval || corenum == MAX_CORES || xmtchan == MAX_CHANNELS)
	{
		printf("\nError:  No ARINC 429 transmit channels present in card #%d.",CARDNUM);
		printf("\n");
		BTICard_CardClose(hCard);
		exit(1);
	}
    
	printf("\nUsing ARINC 429 core #%d",corenum);
	printf("\nUsing transmit channel #%d",xmtchan);
	printf("\n");
    
    /**
     *
     *  Configure xmtchan to transmit at high speed.
     *
     **/
    
	BTICard_CardReset(hCore);
    
	/* Confgiure First Fast Channel */
    
	errval = BTI429_ChConfig(CHCFG429_HIGHSPEED,CH8,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while configuring",errval);
		printf("\n        channel #%d on core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
    
	/* Confgure Second Slow Channel */
	errval = BTI429_ChConfig(CHCFG429_LOWSPEED,CH9,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while configuring",errval);
		printf("\n        channel #%d on core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
	/* Confgure Altimeter Channel */
    errval = BTI429_ChConfig(CHCFG429_LOWSPEED,CH10,hCore);
    errval = BTI429_ChConfig(CHCFG429_PARDATA,CH10,hCore);
    
    if (errval < 0)
    {
        printf("\nError:  An error was encountered (%i) while configuring",errval);
        printf("\n        channel #%d on core #%d.",xmtchan,corenum);
        printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
        BTICard_CardClose(hCard);
        exit(1);
    }
    
	errval = BTI429_ChConfig(CHCFG429_HIGHSPEED,CH11,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while configuring",errval);
		printf("\n        channel #%d on core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
    
	/* Confgure Second Slow Channel */
	errval = BTI429_ChConfig(CHCFG429_LOWSPEED,CH12,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while configuring",errval);
		printf("\n        channel #%d on core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
	/* Confgure Altimeter Channel */
    errval = BTI429_ChConfig(CHCFG429_LOWSPEED,CH13,hCore);
    errval = BTI429_ChConfig(CHCFG429_PARDATA,CH13,hCore);
    
    if (errval < 0)
    {
        printf("\nError:  An error was encountered (%i) while configuring",errval);
        printf("\n        channel #%d on core #%d.",xmtchan,corenum);
        printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
        BTICard_CardClose(hCard);
        exit(1);
    }
    
    /**
     *
     *  Create a message for each of the labels we are transmitting.
     *
     **/
    
	distance_togo.addr    = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	ground_speed.addr     = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	radial_select.addr    = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	heading_select.addr   = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	qfu.addr              = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	nav_course.addr       = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	cross_track.addr      = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	nav_vertical.addr     = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	adf1.addr             = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	adf2.addr             = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	fd_roll.addr          = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	fd_pitch.addr         = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	fast_slow.addr        = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	fd_yaw.addr           = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	localizer.addr        = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	glideslope.addr       = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	omnibearing.addr      = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	discrete1.addr        = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	true_heading.addr     = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	magnetic_heading.addr = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	drift.addr            = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	roll_angle.addr       = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	pitch_angle.addr      = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
	vvi.addr              = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
    baro_altitude.addr    = BTI429_MsgCreate(MSGCRT429_DEFAULT,hCore);
    
    
    /**
     *
     *  Set up the arrays of message addresses; and intervals
     *  needed to build the schedule.  Note that the nth element
     *  in each array correspond to each other.
     *
     **/
    
    /* **************************************************
     * * TX Channel 1 - Captain High Speed 1            *
     * **************************************************/
    
	tx1_msgaddr[0] = ground_speed.addr;
	tx1_min_intrvls[0] = 100;
	tx1_max_intrvls[0] = 200;
    
	tx1_msgaddr[1] = magnetic_heading.addr;
	tx1_min_intrvls[1] = 25;
	tx1_max_intrvls[1] = 50;
    
	tx1_msgaddr[2] = drift.addr;
	tx1_min_intrvls[2] = 25;
	tx1_max_intrvls[2] = 50;
    
	tx1_msgaddr[3] = roll_angle.addr;
	tx1_min_intrvls[3] = 25;
	tx1_max_intrvls[3] = 50;
    
	tx1_msgaddr[4] = pitch_angle.addr;
	tx1_min_intrvls[4] = 25;
	tx1_max_intrvls[4] = 50;
    
	tx1_msgaddr[5] = vvi.addr;
	tx1_min_intrvls[5] = 50;
	tx1_max_intrvls[5] = 50;
    
    /* **************************************************
     * * TX Channel 2 - Captain Low Speed 1             *
     * **************************************************/
    
	tx2_msgaddr[0] = distance_togo.addr;
	tx2_min_intrvls[0] = 100;
	tx2_max_intrvls[0] = 200;
    
	tx2_msgaddr[1] = radial_select.addr;
	tx2_min_intrvls[1] = 32;
	tx2_max_intrvls[1] = 62.5;
    
	tx2_msgaddr[2] = heading_select.addr;
	tx2_min_intrvls[2] = 32;
	tx2_max_intrvls[2] = 62.5;
    
	tx2_msgaddr[3] = qfu.addr;
	tx2_min_intrvls[3] = 32;
	tx2_max_intrvls[3] = 62.5;
    
	tx2_msgaddr[4] = nav_course.addr;
	tx2_min_intrvls[4] = 32;
	tx2_max_intrvls[4] = 62.5;
    
	tx2_msgaddr[5] = cross_track.addr;
	tx2_min_intrvls[5] = 32;
	tx2_max_intrvls[5] = 62.5;
    
	tx2_msgaddr[6] = nav_vertical.addr;
	tx2_min_intrvls[6] = 32;
    tx2_max_intrvls[6] = 62.5;
    
	tx2_msgaddr[7] = adf1.addr;
	tx2_min_intrvls[7] = 32;
	tx2_max_intrvls[7] = 62.5;
    
	tx2_msgaddr[8] = adf1.addr;
	tx2_min_intrvls[8] = 32;
	tx2_max_intrvls[8] = 62.5;
	
	tx2_msgaddr[9] = fd_roll.addr;
	tx2_min_intrvls[9] = 32;
	tx2_max_intrvls[9] = 62.5;
    
	tx2_msgaddr[10] = fd_pitch.addr;
	tx2_min_intrvls[10] = 32;
	tx2_max_intrvls[10] = 62.5;
    
	tx2_msgaddr[11] = fast_slow.addr;
	tx2_min_intrvls[11] = 32;
	tx2_max_intrvls[11] = 62.5;
    
	tx2_msgaddr[12] = fd_yaw.addr;
	tx2_min_intrvls[12] = 32;
	tx2_max_intrvls[12] = 62.5;
    
	tx2_msgaddr[13] = localizer.addr;
	tx2_min_intrvls[13] = 32;
	tx2_max_intrvls[13] = 62.5;
    
	tx2_msgaddr[14] = glideslope.addr;
	tx2_min_intrvls[14] = 32;
	tx2_max_intrvls[14] = 62.5;
    
	tx2_msgaddr[15] = omnibearing.addr;
	tx2_min_intrvls[15] = 32;
	tx2_max_intrvls[15] = 62.5;
    
	tx2_msgaddr[16] = discrete1.addr;
	tx2_min_intrvls[16] = 50;
	tx2_max_intrvls[16] = 100;
    
	tx2_msgaddr[17] = true_heading.addr;
	tx2_min_intrvls[17] = 50;
	tx2_max_intrvls[17] = 50;
    
	/* **************************************************
     * * TX Channel 3 - Captain Low Speed 2             *
     * **************************************************/
	
	tx3_msgaddr[0] = baro_altitude.addr;
    tx3_min_intrvls[0] = 30.5;
    tx3_max_intrvls[0] = 62.5;
    
	/* **************************************************
     * * TX Channel 4 - F/O High Speed 1                *
     * **************************************************/
    
	tx4_msgaddr[0] = ground_speed.addr;
	tx4_min_intrvls[0] = 100;
	tx4_max_intrvls[0] = 200;
    
	tx4_msgaddr[1] = magnetic_heading.addr;
	tx4_min_intrvls[1] = 25;
	tx4_max_intrvls[1] = 50;
    
	tx4_msgaddr[2] = drift.addr;
	tx4_min_intrvls[2] = 25;
	tx4_max_intrvls[2] = 50;
    
	tx4_msgaddr[3] = roll_angle.addr;
	tx4_min_intrvls[3] = 25;
	tx4_max_intrvls[3] = 50;
    
	tx4_msgaddr[4] = pitch_angle.addr;
	tx4_min_intrvls[4] = 25;
	tx4_max_intrvls[4] = 50;
    
	tx4_msgaddr[5] = vvi.addr;
	tx4_min_intrvls[5] = 50;
	tx4_max_intrvls[5] = 50;
    
    /* **************************************************
     * * TX Channel 5 - F/O Low Speed 1                 *
     * **************************************************/
    
	tx5_msgaddr[0] = distance_togo.addr;
	tx5_min_intrvls[0] = 100;
	tx5_max_intrvls[0] = 200;
    
	tx5_msgaddr[1] = radial_select.addr;
	tx5_min_intrvls[1] = 32;
	tx5_max_intrvls[1] = 62.5;
    
	tx5_msgaddr[2] = heading_select.addr;
	tx5_min_intrvls[2] = 32;
	tx5_max_intrvls[2] = 62.5;
    
	tx5_msgaddr[3] = qfu.addr;
	tx5_min_intrvls[3] = 32;
	tx5_max_intrvls[3] = 62.5;
    
	tx5_msgaddr[4] = nav_course.addr;
	tx5_min_intrvls[4] = 32;
	tx5_max_intrvls[4] = 62.5;
    
	tx5_msgaddr[5] = cross_track.addr;
	tx5_min_intrvls[5] = 32;
	tx5_max_intrvls[5] = 62.5;
    
	tx5_msgaddr[6] = nav_vertical.addr;
	tx5_min_intrvls[6] = 32;
	tx5_max_intrvls[6] = 62.5;
    
	tx5_msgaddr[7] = adf1.addr;
	tx5_min_intrvls[7] = 32;
	tx5_max_intrvls[7] = 62.5;
    
	tx5_msgaddr[8] = adf2.addr;
	tx5_min_intrvls[8] = 32;
	tx5_max_intrvls[8] = 62.5;
    
	tx5_msgaddr[9] = fd_roll.addr;
	tx5_min_intrvls[9] = 32;
	tx5_max_intrvls[9] = 62.5;
    
	tx5_msgaddr[10] = fd_pitch.addr;
	tx5_min_intrvls[10] = 32;
	tx5_max_intrvls[10] = 62.5;
    
	tx5_msgaddr[11] = fast_slow.addr;
	tx5_min_intrvls[11] = 32;
	tx5_max_intrvls[11] = 62.5;
    
	tx5_msgaddr[12] = fd_yaw.addr;
	tx5_min_intrvls[12] = 32;
	tx5_max_intrvls[12] = 62.5;
    
	tx5_msgaddr[13] = localizer.addr;
	tx5_min_intrvls[13] = 32;
	tx5_max_intrvls[13] = 62.5;
    
	tx5_msgaddr[14] = glideslope.addr;
	tx5_min_intrvls[14] = 32;
	tx5_max_intrvls[14] = 62.5;
    
	tx5_msgaddr[15] = omnibearing.addr;
	tx5_min_intrvls[15] = 32;
	tx5_max_intrvls[15] = 62.5;
    
	tx5_msgaddr[16] = discrete1.addr;
	tx5_min_intrvls[16] = 50;
	tx5_max_intrvls[16] = 100;
    
	tx5_msgaddr[17] = true_heading.addr;
	tx5_min_intrvls[17] = 50;
	tx5_max_intrvls[17] = 50;
    
	/* **************************************************
     * * TX Channel 6 - F/O Low Speed 2                 *
     * **************************************************/
	
	tx6_msgaddr[0] = baro_altitude.addr;
    tx6_min_intrvls[0] = 30.5;
    tx6_max_intrvls[0] = 62.5;
    
    /**
     *
     *  Next, build a schedule for each channel using the minimum and maximum
     *  transmit intervals.
     *
     **/
    
	errval = BTI429_SchedBuild(6,tx1_msgaddr,tx1_min_intrvls,tx1_max_intrvls,CH8,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while building",errval);
		printf("\n        a schedule on channel #%d of core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
	errval = BTI429_SchedBuild(16,tx2_msgaddr,tx2_min_intrvls,tx2_max_intrvls,CH9,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while building",errval);
		printf("\n        a schedule on channel #%d of core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
	errval = BTI429_SchedBuild(1,tx3_msgaddr,tx3_min_intrvls,tx3_max_intrvls,CH10,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while building",errval);
		printf("\n        a schedule on channel #%d of core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
	errval = BTI429_SchedBuild(6,tx4_msgaddr,tx1_min_intrvls,tx4_max_intrvls,CH11,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while building",errval);
		printf("\n        a schedule on channel #%d of core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
	errval = BTI429_SchedBuild(16,tx5_msgaddr,tx5_min_intrvls,tx5_max_intrvls,CH12,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while building",errval);
		printf("\n        a schedule on channel #%d of core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
	errval = BTI429_SchedBuild(1,tx6_msgaddr,tx6_min_intrvls,tx6_max_intrvls,CH13,hCore);
    
	if (errval < 0)
	{
		printf("\nError:  An error was encountered (%i) while building",errval);
		printf("\n        a schedule on channel #%d of core #%d.",xmtchan,corenum);
		printf("\n        (%s)",BTICard_ErrDescStr(errval,hCard));
		BTICard_CardClose(hCard);
		exit(1);
	}
    
    /**
     *
     *  Initialize the Message Records.
     *
     **/
    
    /* Label Assignements */
	distance_togo.data      = BTI429_FldPutLabel(0L,01);
	ground_speed.data       = BTI429_FldPutLabel(0L,012);
	radial_select.data      = BTI429_FldPutLabel(0L,0100);
	heading_select.data     = BTI429_FldPutLabel(0L,0101);
	qfu.data                = BTI429_FldPutLabel(0L,0105);
	nav_course.data         = BTI429_FldPutLabel(0L,0114);
	cross_track.data        = BTI429_FldPutLabel(0L,0116);
	nav_vertical.data       = BTI429_FldPutLabel(0L,0117);
	adf1.data               = BTI429_FldPutLabel(0L,0162);
	adf2.data               = BTI429_FldPutLabel(0L,0162);
	fd_roll.data            = BTI429_FldPutLabel(0L,0140);
	fd_pitch.data           = BTI429_FldPutLabel(0L,0141);
	fast_slow.data          = BTI429_FldPutLabel(0L,0142);
	fd_yaw.data             = BTI429_FldPutLabel(0L,0143);
	localizer.data          = BTI429_FldPutLabel(0L,0173);
	glideslope.data         = BTI429_FldPutLabel(0L,0174);
    baro_altitude.data      = BTI429_FldPutLabel(0L,0204);
	omnibearing.data        = BTI429_FldPutLabel(0L,0222);
	discrete1.data          = BTI429_FldPutLabel(0L,0270);
	true_heading.data       = BTI429_FldPutLabel(0L,0314);
	magnetic_heading.data   = BTI429_FldPutLabel(0L,0320);
	drift.data              = BTI429_FldPutLabel(0L,0321);
	pitch_angle.data        = BTI429_FldPutLabel(0L,0324);
	roll_angle.data         = BTI429_FldPutLabel(0L,0325);
	vvi.data                = BTI429_FldPutLabel(0L,0365);
    
    
    /* Initial Value Assignments */
	distance_togo.data      = BTI429_BCDPutMant(distance_togo.data,0L,4,3);
	ground_speed.data       = BTI429_BCDPutMant(ground_speed.data,0L,4,3);
	radial_select.data      = BTI429_BNRPutMant(radial_select.data,0L,15,3);
	heading_select.data     = BTI429_BNRPutMant(heading_select.data,0L,15,3);
	qfu.data                = BTI429_BNRPutMant(qfu.data,0L,15,3);
	nav_course.data         = BTI429_BNRPutMant(nav_course.data,0L,15,3);
	cross_track.data        = BTI429_BNRPutMant(cross_track.data,0L,15,3);
	nav_vertical.data       = BTI429_BNRPutMant(nav_vertical.data,0L,15,3);
	adf1.data               = BTI429_BNRPutMant(adf1.data,0L,15,3);
	adf2.data               = BTI429_BNRPutMant(adf2.data,0L,15,3);
	fd_roll.data			= BTI429_BNRPutSSM(fd_roll.data,SSM_NORMAL);
	fd_roll.data            = BTI429_BNRPutMant(fd_roll.data,0L,15,3);
	fd_pitch.data			= BTI429_BNRPutSSM(fd_pitch.data,SSM_NORMAL);
	fd_pitch.data           = BTI429_BNRPutMant(fd_pitch.data,0L,15,3);
	fd_roll.data			= BTI429_BNRPutSSM(fd_roll.data,SSM_NORMAL);
	fast_slow.data			= BTI429_BNRPutSSM(fast_slow.data,SSM_NORMAL);
	fast_slow.data          = BTI429_BNRPutMant(fast_slow.data,0L,15,3);
	fd_yaw.data			    = BTI429_BNRPutSSM(fd_yaw.data,SSM_NORMAL);
	fd_yaw.data             = BTI429_BNRPutMant(fd_yaw.data,0L,15,3);
	localizer.data			= BTI429_BNRPutSSM(localizer.data,SSM_NORMAL);
	localizer.data          = BTI429_BNRPutMant(localizer.data,0L,15,3);
	glideslope.data			= BTI429_BNRPutSSM(glideslope.data,SSM_NORMAL);
	glideslope.data         = BTI429_BNRPutMant(glideslope.data,0L,15,3);
	omnibearing.data        = BTI429_BNRPutMant(omnibearing.data,0L,15,3);
	discrete1.data          = BTI429_BNRPutMant(discrete1.data,0L,15,3);
	true_heading.data       = BTI429_BNRPutMant(true_heading.data,0L,15,3);
	magnetic_heading.data   = BTI429_BNRPutMant(magnetic_heading.data,0L,15,3);
	drift.data              = BTI429_BNRPutMant(drift.data,0L,15,3);
	roll_angle.data         = BTI429_BNRPutMant(roll_angle.data,0L,15,3);
	pitch_angle.data        = BTI429_BNRPutMant(pitch_angle.data,0L,15,3);
	vvi.data                = BTI429_BNRPutMant(vvi.data,0L,15,3);
	baro_altitude.data      = BTI429_BNRPutData(baro_altitude.data,0L,32,9);
    
    
	BTI429_MsgDataWr(distance_togo.data,distance_togo.addr,hCore);
	BTI429_MsgDataWr(ground_speed.data,ground_speed.addr,hCore);
	BTI429_MsgDataWr(radial_select.data,radial_select.addr,hCore);
	BTI429_MsgDataWr(heading_select.data,heading_select.addr,hCore);
	BTI429_MsgDataWr(qfu.data,qfu.addr,hCore);
	BTI429_MsgDataWr(nav_course.data,nav_course.addr,hCore);
	BTI429_MsgDataWr(cross_track.data,cross_track.addr,hCore);
	BTI429_MsgDataWr(nav_vertical.data,nav_vertical.addr,hCore);
	BTI429_MsgDataWr(adf1.data,adf1.addr,hCore);
	BTI429_MsgDataWr(fd_roll.data,fd_roll.addr,hCore);
	BTI429_MsgDataWr(fd_pitch.data,fd_pitch.addr,hCore);
	BTI429_MsgDataWr(fast_slow.data,fast_slow.addr,hCore);
	BTI429_MsgDataWr(fd_yaw.data,fd_yaw.addr,hCore);
	BTI429_MsgDataWr(localizer.data,localizer.addr,hCore);
	BTI429_MsgDataWr(glideslope.data,glideslope.addr,hCore);
	BTI429_MsgDataWr(omnibearing.data,omnibearing.addr,hCore);
	BTI429_MsgDataWr(discrete1.data,discrete1.addr,hCore);
	BTI429_MsgDataWr(true_heading.data,true_heading.addr,hCore);
	BTI429_MsgDataWr(magnetic_heading.data,magnetic_heading.addr,hCore);
	BTI429_MsgDataWr(drift.data,drift.addr,hCore);
	BTI429_MsgDataWr(roll_angle.data,roll_angle.addr,hCore);
	BTI429_MsgDataWr(pitch_angle.data,pitch_angle.addr,hCore);
	BTI429_MsgDataWr(vvi.data,vvi.addr,hCore);
	BTI429_MsgDataWr(baro_altitude.data,baro_altitude.addr,hCore);
    
    
    /**
     *
     *  Start operation of the card.
     *
     **/
    
	BTICard_CardStart(hCore);
    
    /**
     *
     *  Change data until a key is pressed.
     *
     **/
    
	printf("\nPress any key to exit....\n\n");
    
	while(!kbhit())
	{
        
		//try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code : %d" , WSAGetLastError());
            exit(EXIT_FAILURE);
        }
        
		payload = (sim429payload*)&buf;
		data_value = payload->data;
        
        //print details of the client/peer and the data received
        //printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        //printf("Data: %03o %f\n" , payload->label, payload->data);
        //printf("I am new     \n");
        
        /* TX Channel Switcher */
        switch ( payload->label ) {
                
            case(01): /* Distance To Go */
                distance_togo.data = BTI429_BCDPutSSM(distance_togo.data,SSM_NORMAL);
                distance_togo.data = BTI429_BCDPutMant(distance_togo.data,data_value,4,0);
                BTI429_MsgDataWr(distance_togo.data,distance_togo.addr,hCore);
                break;
            case(012): /* Ground Speed */
				data_value = data_value * METERS_PER_SEC;
                ground_speed.data = BTI429_BCDPutSSM(ground_speed.data,SSM_NORMAL);
                ground_speed.data = BTI429_BCDPutMant(ground_speed.data,data_value,4,0);
                BTI429_MsgDataWr(ground_speed.data,ground_speed.addr,hCore);
                break;
            case(0100): /* Radial Select */
				radial_select.data = BTI429_BNRPutSSM(radial_select.data,SSM_NORMAL);
  			    if ( payload->data < 180.0 ) {
                    data_value = data_value*RAD_STEPS_15;
                    radial_select.data = BTI429_BNRPutSign(radial_select.data,0);
                    radial_select.data = BTI429_BNRPutMant(radial_select.data,data_value,15,0);
                } else {
                    data_value = (360-data_value)*RAD_STEPS_15;
                    radial_select.data = BTI429_BNRPutSign(radial_select.data,1);
                    radial_select.data = BTI429_BNRPutMant(radial_select.data,fabs(data_value),15,1);
                }
			    BTI429_MsgDataWr(radial_select.data,radial_select.addr,hCore);
                break;
            case(0101): /* Heading Select */
				heading_select.data = BTI429_BNRPutSSM(heading_select.data,SSM_NORMAL);
                if ( payload->data <= 180.0 ) {
                    data_value = data_value*RAD_STEPS_15;
                    heading_select.data = BTI429_BNRPutSign(heading_select.data,0);
                    heading_select.data = BTI429_BNRPutMant(heading_select.data,data_value,15,0);
                } else {
                    data_value = (360-data_value)*RAD_STEPS_15;
                    heading_select.data = BTI429_BNRPutSign(heading_select.data,1);
                    heading_select.data = BTI429_BNRPutMant(heading_select.data,fabs(data_value),15,1);
                }
			    BTI429_MsgDataWr(heading_select.data,heading_select.addr,hCore);
                break;
            case(0105): /* QFU */
                qfu.data = BTI429_BNRPutSSM(qfu.data,SSM_NORMAL);
                qfu.data = BTI429_BNRPutMant(qfu.data,data_value,15,0);
                BTI429_MsgDataWr(qfu.data,qfu.addr,hCore);
                break;
            case(0114): /* NAV Course */
                nav_course.data = BTI429_BNRPutSSM(nav_course.data,SSM_NORMAL);
                nav_course.data = BTI429_BNRPutMant(nav_course.data,data_value,15,0);
                BTI429_MsgDataWr(nav_course.data,nav_course.addr,hCore);
                break;
            case(0116): /* Cross Track */
                cross_track.data = BTI429_BNRPutSSM(cross_track.data,SSM_NORMAL);
                cross_track.data = BTI429_BNRPutMant(cross_track.data,data_value,15,0);
                BTI429_MsgDataWr(cross_track.data,cross_track.addr,hCore);
                break;
            case(0117): /* NAV Vertical */
                nav_vertical.data = BTI429_BNRPutSSM(nav_vertical.data,SSM_NORMAL);
                nav_vertical.data = BTI429_BNRPutMant(nav_vertical.data,data_value,15,0);
                BTI429_MsgDataWr(nav_vertical.data,nav_vertical.addr,hCore);
                break;
            case(0162): /* ADF 1 */
                adf1.data = BTI429_BNRPutSSM(adf1.data,SSM_NORMAL);
                printf("ADF1 Bearing %f\n", data_value);
                if ( payload->data >= 0 ) {
                    data_value = data_value*RAD_STEPS_15;
                    adf1.data = BTI429_BNRPutSign(adf1.data,0);
                    adf1.data = BTI429_BNRPutMant(adf1.data,data_value,15,0);
                } else {
					data_value = 360.0 - data_value;
                    data_value = (360-data_value)*RAD_STEPS_15;
                    adf1.data = BTI429_BNRPutSign(adf1.data,1);
                    adf1.data = BTI429_BNRPutMant(adf1.data,fabs(data_value),15,1);
                }
			    BTI429_MsgDataWr(adf1.data,adf1.addr,hCore);
                break;
            case(01162): /* ADF 2 */
                adf2.data = BTI429_BNRPutSSM(adf2.data,SSM_NORMAL);
                printf("ADF2 Bearing %f\n", data_value);
                if ( payload->data >= 0 ) {
                    data_value = data_value*RAD_STEPS_15;
                    adf2.data = BTI429_BNRPutSign(adf2.data,0);
                    adf2.data = BTI429_BNRPutMant(adf2.data,data_value,15,0);
                } else {
					data_value = 360.0 - data_value;
                    data_value = (360-data_value)*RAD_STEPS_15;
                    adf2.data = BTI429_BNRPutSign(adf2.data,1);
                    adf2.data = BTI429_BNRPutMant(adf2.data,fabs(data_value),15,1);
                }
			    BTI429_MsgDataWr(adf2.data,adf2.addr,hCore);
                break;
			case(0140): /* FD Roll Angle */
                fd_roll.data = BTI429_BNRPutSSM(fd_roll.data,SSM_NORMAL);
                printf("Roll %f\n", data_value);
                if ( data_value >= 0 ) {
                    /* Up */
					data_value = data_value*RAD_STEPS_15;
                    fd_roll.data = BTI429_BNRPutSign(fd_roll.data,0);
                    fd_roll.data = BTI429_BNRPutMant(fd_roll.data,data_value,15,0);
                } else {
                    /* Down */
					data_value -= 0.01;
					data_value = (data_value)*RAD_STEPS_15;
                    fd_roll.data = BTI429_BNRPutSign(fd_roll.data,1);
                    fd_roll.data = BTI429_BNRPutMant(fd_roll.data,fabs(data_value),15,1);
                }
                BTI429_MsgDataWr(fd_roll.data,fd_roll.addr,hCore);
                break;
			case(0141): /* FD Pitch Angle */
                fd_pitch.data = BTI429_BNRPutSSM(fd_pitch.data,SSM_NORMAL);
                printf("Pitch %f\n", data_value);
                if ( data_value >= 0 ) {
                    /* Up */
					data_value = data_value*RAD_STEPS_15;
                    fd_pitch.data = BTI429_BNRPutSign(fd_pitch.data,0);
                    fd_pitch.data = BTI429_BNRPutMant(fd_pitch.data,data_value,15,0);
                } else {
                    /* Down */
					data_value -= 0.01;
					data_value = (data_value)*RAD_STEPS_15;
                    fd_pitch.data = BTI429_BNRPutSign(fd_pitch.data,1);
                    fd_pitch.data = BTI429_BNRPutMant(fd_pitch.data,fabs(data_value),15,1);
                }
                BTI429_MsgDataWr(fd_pitch.data,fd_pitch.addr,hCore);
                break;
            case(0142): /* Fast Slow */
                fast_slow.data = BTI429_BNRPutSSM(fast_slow.data,SSM_NORMAL);
				if ( data_value >= 0 ) {
					fast_slow.data = BTI429_BNRPutMant(fast_slow.data,data_value*220,11,0);
				} else {
					data_value = data_value-0.01;
					fast_slow.data = BTI429_BNRPutMant(fast_slow.data,fabs(data_value*220),11,1);
				}
                BTI429_MsgDataWr(fast_slow.data,fast_slow.addr,hCore);
                break;
            case(0143): /* Flight Director Yaw */
                fd_yaw.data = BTI429_BNRPutSSM(fd_yaw.data,SSM_NORMAL);
                if ( data_value >= 0 ) {
                    /* Up */
					data_value = data_value*RAD_STEPS_15;
                    fd_yaw.data = BTI429_BNRPutSign(fd_yaw.data,0);
                    fd_yaw.data = BTI429_BNRPutMant(fd_yaw.data,data_value,15,0);
                } else {
                    /* Down */
					data_value -= 0.01;
					data_value = (data_value)*RAD_STEPS_15;
                    fd_yaw.data = BTI429_BNRPutSign(fd_yaw.data,1);
                    fd_yaw.data = BTI429_BNRPutMant(fd_yaw.data,fabs(data_value),15,1);
                }
                BTI429_MsgDataWr(fd_yaw.data,fd_yaw.addr,hCore);
                break;
            case(0173): /* Localizer */
                localizer.data = BTI429_BNRPutSSM(localizer.data,SSM_NORMAL);
				if ( data_value >= 0 ) {
					localizer.data = BTI429_BNRPutMant(localizer.data,data_value*220,11,0);
				} else {
					data_value = data_value-0.01;
					localizer.data = BTI429_BNRPutMant(localizer.data,fabs(data_value*220),11,1);
				}
				printf("Localizer %f\n", data_value);
                BTI429_MsgDataWr(localizer.data,localizer.addr,hCore);
                break;
            case(0174): /* Glide Slope */
                glideslope.data = BTI429_BNRPutSSM(glideslope.data,SSM_NORMAL);
				if ( data_value >= 0 ) {
					glideslope.data = BTI429_BNRPutMant(glideslope.data,data_value*220,11,0);
				} else {
					data_value = data_value-0.01;
					glideslope.data = BTI429_BNRPutMant(glideslope.data,fabs(data_value*220),11,1);
				}
				
				BTI429_MsgDataWr(glideslope.data,glideslope.addr,hCore);
                break;
			case(0204): /* Altitude */
                baro_altitude.data = BTI429_BNRPutData(baro_altitude.data,data_value,32,13);
                BTI429_MsgDataWr(baro_altitude.data,baro_altitude.addr,hCore);
                break;
            case(0222): /* Omnibearing */
                omnibearing.data = BTI429_BNRPutSSM(omnibearing.data,SSM_NORMAL);
                if ( payload->data < 180.0 ) {
                    data_value = data_value*RAD_STEPS_15;
                    omnibearing.data = BTI429_BNRPutSign(omnibearing.data,0);
                    omnibearing.data = BTI429_BNRPutMant(omnibearing.data,data_value,15,0);
                } else {
                    data_value = (360-data_value)*RAD_STEPS_15;
                    omnibearing.data = BTI429_BNRPutSign(omnibearing.data,1);
                    omnibearing.data = BTI429_BNRPutMant(omnibearing.data,fabs(data_value),15,1);
                }
			    BTI429_MsgDataWr(omnibearing.data,omnibearing.addr,hCore);
                break;
            case(0270): /* Discrete Data */
                discrete1.data = BTI429_BNRPutSSM(discrete1.data,SSM_NORMAL);
                discrete1.data = BTI429_BNRPutMant(discrete1.data,data_value,15,0);
                BTI429_MsgDataWr(discrete1.data,discrete1.addr,hCore);
                break;
            case(0314): /* True Heading */
                true_heading.data = BTI429_BNRPutSSM(true_heading.data,SSM_NORMAL);
                true_heading.data = BTI429_BNRPutMant(true_heading.data,data_value,12,0);
                BTI429_MsgDataWr(true_heading.data,true_heading.addr,hCore);
                break;
            case(0320): /* Magnetic Heading */
                magnetic_heading.data = BTI429_BNRPutSSM(magnetic_heading.data,SSM_NORMAL);
				if ( payload->data <= 180.0 ) {
                    data_value = data_value*RAD_STEPS_15;
                    magnetic_heading.data = BTI429_BNRPutSign(magnetic_heading.data,0);
                    magnetic_heading.data = BTI429_BNRPutMant(magnetic_heading.data,data_value,15,0);
                } else {
                    data_value = (360-data_value)*RAD_STEPS_15;
                    magnetic_heading.data = BTI429_BNRPutSign(magnetic_heading.data,1);
                    magnetic_heading.data = BTI429_BNRPutMant(magnetic_heading.data,fabs(data_value),15,1);
                }
                BTI429_MsgDataWr(magnetic_heading.data,magnetic_heading.addr,hCore);
                break;
            case(0321): /* Drift Data */
                drift.data = BTI429_BNRPutSSM(drift.data,SSM_NORMAL);
                drift.data = BTI429_BNRPutMant(drift.data,data_value,15,0);
                BTI429_MsgDataWr(drift.data,drift.addr,hCore);
                break;
            case(0324): /* ADI Pitch Angle */
                pitch_angle.data = BTI429_BNRPutSSM(pitch_angle.data,SSM_NORMAL);
                if ( data_value >= 0 ) {
                    /* Up */
					data_value = data_value*RAD_STEPS_15;
                    pitch_angle.data = BTI429_BNRPutSign(pitch_angle.data,0);
                    pitch_angle.data = BTI429_BNRPutMant(pitch_angle.data,data_value,15,0);
                } else {
                    /* Down */
					data_value -= 0.01;
					data_value = (data_value)*RAD_STEPS_15;
                    pitch_angle.data = BTI429_BNRPutSign(pitch_angle.data,1);
                    pitch_angle.data = BTI429_BNRPutMant(pitch_angle.data,fabs(data_value),15,1);
                }
                BTI429_MsgDataWr(pitch_angle.data,pitch_angle.addr,hCore);
                break;
            case(0325): /* ADI Roll Angle */
				data_value -= 0.35;
                roll_angle.data = BTI429_BNRPutSSM(roll_angle.data,SSM_NORMAL);
                if ( data_value >=0 ) {
                    /* Right */
					data_value = data_value*RAD_STEPS_15;
                    roll_angle.data = BTI429_BNRPutSign(roll_angle.data,0);
                    roll_angle.data = BTI429_BNRPutMant(roll_angle.data,data_value,15,0);
                } else {
                    /* Left */
					data_value -= 0.01;
					data_value = (data_value)*RAD_STEPS_15;
                    roll_angle.data = BTI429_BNRPutSign(roll_angle.data,1);
                    roll_angle.data = BTI429_BNRPutMant(roll_angle.data,fabs(data_value),15,1);
                }
                BTI429_MsgDataWr(roll_angle.data,roll_angle.addr,hCore);
                break;
            case(0365): /* Vertical Velocity Indicated */
                //vvi.data = BTI429_BNRPutSSM(vvi.data,SSM_NORMAL);
				vvi.data = BTI429_BNRPutSSM(vvi.data,6);
				//printf("---- %f\n", data_value);
                if ( data_value >= 0 ) {
                    /* Up */
                    //vvi.data = BTI429_BNRPutSign(vvi.data,0);
                    vvi.data = BTI429_BNRPutMant(vvi.data,data_value,15,0);
                } else {
                    /* Down */
					data_value -= 1;
                    vvi.data = BTI429_BNRPutSign(vvi.data,1);
                    vvi.data = BTI429_BNRPutMant(vvi.data,fabs(data_value),15,1);
                }
                BTI429_MsgDataWr(vvi.data,vvi.addr,hCore);
                break;
            default:
                break;
        }
        
		//_sleep(150);
        
	}
    
    /**
     *
     *  Stop the card.
     *
     **/
    
	BTICard_CardStop(hCore);
    
    /**
     *  The card MUST be closed before exiting the program.
     *
     **/
    
	BTICard_CardClose(hCard);
    
    
    /* UDP Data Stream Closeout */
    closesocket(s);
    WSACleanup();
}