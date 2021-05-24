/*
*-----------------------------------------------------------------------------------------------------------------------------------
*--------------------------------------------------------------- SERVER ------------------------------------------------------------
*-----------------------------------------------------------------------------------------------------------------------------------
*/

/* Send Multicast Datagram code example. */

#include <sys/types.h>

#include <sys/socket.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <stdio.h>

#include <stdlib.h>

struct in_addr localInterface;

struct sockaddr_in groupSock;

int sd;

char databuf[1024] = "Multicast test message lol!";

int datalen = sizeof(databuf);

int main(int argc, char *argv[])

{

  /* Create a datagram socket on which to send. */

  sd = socket(AF_INET, SOCK_DGRAM, 0);

  if (sd < 0)

  {

    perror("Opening datagram socket error");

    exit(1);
  }

  else

    printf("Opening the datagram socket...OK.\n");

  /* Initialize the group sockaddr structure with a */

  /* group address of 225.1.1.1 and port 5555. */

  memset((char *)&groupSock, 0, sizeof(groupSock));

  groupSock.sin_family = AF_INET;

  groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");

  groupSock.sin_port = htons(4321);

  // Disable loopback so you do not receive your own datagrams.

  {

    char loopch = 0;

    if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)

    {

      perror("Setting IP_MULTICAST_LOOP error");

      close(sd);

      exit(1);
    }

    else

      printf("Disabling the loopback...OK.\n");
  }

  /* Set local interface for outbound multicast datagrams. */

  /* The IP address specified must be associated with a local, */

  /* multicast capable interface. */

  localInterface.s_addr = inet_addr("203.106.93.94");

  if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)

  {

    perror("Setting local interface error");

    exit(1);
  }

  else

    printf("Setting the local interface...OK\n");

  /* Send a message to the multicast group specified by the*/

  /* groupSock sockaddr structure. */

  /*int datalen = 1024;*/

  if (sendto(sd, databuf, datalen, 0, (struct sockaddr *)&groupSock, sizeof(groupSock)) < 0)

  {
    perror("Sending datagram message error");
  }

  else

    printf("Sending datagram message...OK\n");

  /* Try the re-read from the socket if the loopback is not disable

if(read(sd, databuf, datalen) < 0)

{

perror("Reading datagram message error\n");

close(sd);

exit(1);

}

else

{

printf("Reading datagram message from client...OK\n");

printf("The message is: %s\n", databuf);

}

*/

  return 0;
}

/*
*-----------------------------------------------------------------------------------------------------------------------------------
*--------------------------------------------------------------- CLIENT ------------------------------------------------------------
*-----------------------------------------------------------------------------------------------------------------------------------
*/

/* Receiver/client multicast Datagram example. */

#include <sys/types.h>

#include <sys/socket.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <stdio.h>

#include <stdlib.h>

struct sockaddr_in localSock;

struct ip_mreq group;

int sd;

int datalen;

char databuf[1024];

int main(int argc, char *argv[])

{

  /* Create a datagram socket on which to receive. */

  sd = socket(AF_INET, SOCK_DGRAM, 0);

  if (sd < 0)

  {

    perror("Opening datagram socket error");

    exit(1);
  }

  else

    printf("Opening datagram socket....OK.\n");

  /* Enable SO_REUSEADDR to allow multiple instances of this */

  /* application to receive copies of the multicast datagrams. */

  {

    int reuse = 1;

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)

    {

      perror("Setting SO_REUSEADDR error");

      close(sd);

      exit(1);
    }

    else

      printf("Setting SO_REUSEADDR...OK.\n");
  }

  /* Bind to the proper port number with the IP address */

  /* specified as INADDR_ANY. */

  memset((char *)&localSock, 0, sizeof(localSock));

  localSock.sin_family = AF_INET;

  localSock.sin_port = htons(4321);

  localSock.sin_addr.s_addr = INADDR_ANY;

  if (bind(sd, (struct sockaddr *)&localSock, sizeof(localSock)))

  {

    perror("Binding datagram socket error");

    close(sd);

    exit(1);
  }

  else

    printf("Binding datagram socket...OK.\n");

  /* Join the multicast group 226.1.1.1 on the local 203.106.93.94 */

  /* interface. Note that this IP_ADD_MEMBERSHIP option must be */

  /* called for each local interface over which the multicast */

  /* datagrams are to be received. */

  group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");

  group.imr_interface.s_addr = inet_addr("203.106.93.94");

  if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)

  {

    perror("Adding multicast group error");

    close(sd);

    exit(1);
  }

  else

    printf("Adding multicast group...OK.\n");

  /* Read from the socket. */

  datalen = sizeof(databuf);

  if (read(sd, databuf, datalen) < 0)

  {

    perror("Reading datagram message error");

    close(sd);

    exit(1);
  }

  else

  {

    printf("Reading datagram message...OK.\n");

    printf("The message from multicast server is: \"%s\"\n", databuf);
  }

  return 0;
}



/*
*-----------------------------------------------------------------------------------------------------------------------------------
*--------------------------------------------------------------- CLIENT 2 ----------------------------------------------------------
*-----------------------------------------------------------------------------------------------------------------------------------
*/

//
// Simple sender.c program for UDP
//
// Adapted from:
// http://ntrg.cs.tcd.ie/undergrad/4ba2/multicast/antony/example.html
//
// Changes:
// * Compiles for Windows as well as Linux
// * Takes the port and group on the command line
//
// Note that what this program does should be equivalent to NETCAT:
//
//     echo "Hello World" | nc -u 239.255.255.250 1900

#ifdef _WIN32
    #include <Winsock2.h> // before Windows.h, else Winsock 1 conflict
    #include <Ws2tcpip.h> // needed for ip_mreq definition for multicast
    #include <Windows.h>
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h> // for sleep()
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
       printf("Command line args should be multicast group and port\n");
       printf("(e.g. for SSDP, `sender 239.255.255.250 1900`)\n");
       return 1;
    }

    char* group = argv[1]; // e.g. 239.255.255.250 for SSDP
    int port = atoi(argv[2]); // 0 if error, which is an invalid port

    // !!! If test requires, make these configurable via args
    //
    const int delay_secs = 1;
    const char *message = "Hello, World!";

#ifdef _WIN32
    //
    // Initialize Windows Socket API with given VERSION.
    //
    WSADATA wsaData;
    if (WSAStartup(0x0101, &wsaData)) {
        perror("WSAStartup");
        return 1;
    }
#endif

    // create what looks like an ordinary UDP socket
    //
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    // set up destination address
    //
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(group);
    addr.sin_port = htons(port);

      // now just sendto() our destination!
    //
    while (1) {
        char ch = 0;
        int nbytes = sendto(
            fd,
            message,
            strlen(message),
            0,
            (struct sockaddr*) &addr,
            sizeof(addr)
        );
        if (nbytes < 0) {
            perror("sendto");
            return 1;
        }

     #ifdef _WIN32
          Sleep(delay_secs * 1000); // Windows Sleep is milliseconds
     #else
          sleep(delay_secs); // Unix sleep is seconds
     #endif
     }

#ifdef _WIN32
    //
    // Program never actually gets here due to infinite loop that has to be
    // canceled, but since people on the internet wind up using examples
    // they find at random in their own code it's good to show what shutting
    // down cleanly would look like.
    //
    WSACleanup();
#endif

    return 0;
}