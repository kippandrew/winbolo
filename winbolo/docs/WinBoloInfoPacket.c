/* Sample code to access WinBolo's game infomation packet interface */
/* (C) John Morrison 1999-2000                                      */
/* Largely based on Stuart Cheshires boloInfoPacket sample code     */
/* which is:                                                        */
/* (C) 1993-1995 Stuart Cheshire                                    */
/* Usage:                                                           */
/* Filename.exe <port> <address>                                    */
/* to ping a game in process for its info                           */
/* Notes:                                                           */
/* * Runs under Win95/98 with Winsock 2 - To make it work under     */
/*   Linux/Unix just take out all the WSA lines and change the      */
/*   #include <winsock2.h> to #include <inet.h>                     */
/*   or                                                             */
/*   #include <arpa/inet.h>                                         */

#include <stdio.h>
#include <winsock2.h>
#include <time.h>
#include <memory.h>

/* Convert Bolo's pascal string to C string so we can work with it easily */
static void PtoCStr(unsigned char *string) {
  int count;   /* Looping varible */
  int len;     /* String len */

  len = string[0];
  for (count=0; count<len; count++) {
    string[count] = string[count+1];
  }
  string[len] = 0;
}

typedef struct { 
  u_char c[36];
} u_char36;
typedef u_char  BYTE;
typedef u_short WORD;

/* Version */
#define BoloVersion_Major    0x01
#define BoloVersion_Minor    0x01
#define BoloVersion_Revision 0x04

/* Request/Response packets */
#define BoloPacket_Request   13
#define BoloPacket_Response  14

typedef struct {
  BYTE signature[4];	  /* 'Bolo' */
  BYTE versionMajor;	  /* 0x01   */
  BYTE versionMinor;	  /* 0x00   */
  BYTE versionRevision;	  /* 0x09   */
  BYTE type;		  /* 13 for request, 14 for response */
} BOLOHEADER;

typedef struct {
  struct in_addr serveraddress;
  unsigned short serverport;
  u_long start_time;
  BYTE spare1;
  BYTE spare2;
} GAMEID;

typedef struct {
  BOLOHEADER h;

  u_char36 mapname;    /* Pascal string (first byte is length)         */
  GAMEID gameid;       /* 8 byte unique ID for game (combination       */
                       /* of starting machine address & timestamp)     */
  BYTE gametype;       /* Game type (1, 2 or 3: open, tourn. & strict) */
  BYTE allow_mines;    /* 0x80 for normal hidden mines                 */
                       /* 0xC0 for all mines visible                   */
  BYTE allow_AI;       /* 0 for no AI tanks, 1 for AI tanks allowed    */
  BYTE spare1;	       /* 0                                            */
  long start_delay;    /* if non zero, time until game starts, (50ths) */
  long time_limit;     /* if non zero, time until game ends, (50ths)   */

  WORD num_players;    /* number of players                            */
  WORD free_pills;     /* number of free (neutral) pillboxes           */
  WORD free_bases;     /* number of free (neutral) refuelling bases    */
  BYTE has_password;   /* non-zero if game has password set            */
  BYTE spare2;	       /* 0                                            */
} INFO_PACKET;

/* Sends a query packet to the command line argument specified */
static void sendquery(int s, char **argv) {
  static const BOLOHEADER h = {  /* Bolo request packet we are sending */
		{ 'B', 'o', 'l', 'o' },
    BoloVersion_Major,
    BoloVersion_Minor,
    BoloVersion_Revision,
    BoloPacket_Request
  };
  struct sockaddr_in rmtaddr; /* Remote address we are sending to */

  rmtaddr.sin_family      = AF_INET;
  rmtaddr.sin_port        = htons(atoi(argv[1]));
  rmtaddr.sin_addr.s_addr = inet_addr(argv[2]);
  
  if (rmtaddr.sin_addr.s_addr == -1) {
    struct hostent *hp = gethostbyname(argv[2]); /* Get the hostname and take its IP from that */
      if (!hp) { 
        fprintf(stderr, "%s: unknown host\n", argv[2]); 
        WSACleanup();
        exit(1);
      }
      rmtaddr.sin_family = hp->h_addrtype;
      memcpy(hp->h_addr, (char *) &rmtaddr.sin_addr, hp->h_length);
		}
  
  sendto(s, (char *) &h, sizeof(h), 0, (struct sockaddr *)&rmtaddr, sizeof(rmtaddr));
}

/* Prints out an IP address from an in_addr structure */
static void printip(struct in_addr address) {
	union { BYTE b[4]; struct in_addr address; } a;
  a.address = address;
  printf("%d.", a.b[0]);
  printf("%d.", a.b[1]);
  printf("%d.", a.b[2]);
  printf("%d" , a.b[3]);
}

int main(int argc, char **argv) {
  SOCKET s;                    /* This machines socket */
  WSADATA wsaData;             /* Used to setup Winsock */
  struct sockaddr_in lcladdr;  /* Local address for socket */
  int ret;                     /* Function returns */
  BYTE finish;                 /* Have we got our packet and should we quit? */

  finish = 0; /* Exit while loop guard */

  /* Check for arguments */
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <remote port> <remote machine>\n", argv[0]);
    exit(1);
  }
  
  ret = WSAStartup(MAKEWORD(2,0), &wsaData);
  if (ret != 0) {
    printf("Error Starting up Winsock\n",0);
    exit(1);
  }

  
  /* Open socket */
  s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s < 0) { 
    perror("socket");
    WSACleanup();
    exit(1);
  }

  /* Bind it */
  lcladdr.sin_family = AF_INET;
  lcladdr.sin_port = INADDR_ANY;
  lcladdr.sin_addr.s_addr = INADDR_ANY;
  ret = bind(s, (struct sockaddr *)&lcladdr, sizeof(lcladdr));
  if (ret < 0) {
    printf("Error Binding Socket\n");
    closesocket(s);
    WSACleanup();
    exit(1);
  }

  /* Send the query off */
  sendquery(s, argv);

  /* Wait to receive the data */
  while(finish == 0) {
    time_t timenow, gametime;   /* Game Time related things */
    char *timestring, *ptr;     /* Pointers to print out game time things */
    INFO_PACKET info;           /* Info packet */
    struct sockaddr_in from;    /* Address packet came from */
    int fromlen = sizeof(from); /* Size of that address */
    int packetlen = recvfrom(s, (char *) &info, sizeof(info), 0, (struct sockaddr *)&from, &fromlen);
    if (packetlen != sizeof(info)) continue;
    if (info.h.signature[0]    != 'B'  ||
        info.h.signature[1]    != 'o'  ||
        info.h.signature[2]    != 'l'  ||
        info.h.signature[3]    != 'o'  ||
        info.h.versionMajor    != BoloVersion_Major    ||
        info.h.versionMinor    != BoloVersion_Minor    ||
        info.h.type            != BoloPacket_Response   ) continue;
    /* Valid response packet - Process it */
    PtoCStr(info.mapname.c);
    info.num_players = info.num_players;
    info.free_pills  = info.free_pills;
    info.free_bases  = info.free_bases;
    
    time(&timenow);
    gametime = ntohl(info.gameid.start_time);
    printf("%s", ctime(&timenow));

    printf("Bolo player at ");
    printip(from.sin_addr);
    printf(" on map \"%s\" running since\n", info.mapname.c);

    ptr = timestring = asctime(gmtime(&gametime));
    while (*ptr && *ptr != '\n') {
      ptr++;
    }
    *ptr = 0;
    printf("%s", timestring);
    printf(" GMT (Server at ");
    printip(info.gameid.serveraddress);
    printf(" on port %d", info.gameid.serverport);
    printf(")\n");
    printf("Options: ");
    switch(info.gametype) {
    case 1:
      printf("Open Game");
      break;
    case 2:
      printf("Tournament");
      break;
    case 3:
      printf("Strict Tournament");
      break;
		}
    if (info.allow_mines == 0x80) {
      printf(", Hidden mines");
    }
    if (info.allow_AI) {
      printf(", AI tanks allowed");
    }
    if (info.has_password) {
      printf(", Password set");
    }
    printf("\nPlayers in game:%2d, Neutral pillboxes:%2d, Neutral Refueling Bases:%2d\n\n", info.num_players, info.free_pills, info.free_bases);
    finish = 1;
    fflush(stdout);
  }
  
  /* Winsock shutdown */
  WSACleanup();

  return 0;
}

