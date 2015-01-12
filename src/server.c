/* 
 * Copyright (c) 1997, 2014 University of Michigan, Ann Arbor.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of Michigan, Ann Arbor. The name of the University 
 * may not be used to endorse or promote products derived from this 
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Sugih Jamin (jamin@eecs.umich.edu)
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#ifdef _WIN32
#define close(sockdep) closesocket(sockdep)
#define perror(errmsg) { fprintf(stderr, "%s: %d\n", (errmsg), WSAGetLastError()); }
#endif

#define net_assert(err, errmsg) { if ((err)) { perror(errmsg); assert(!(err)); } }

#define PORT 4897
#define QLEN 100
#define BLEN 256

int visits;

int
main(int argc, char *argv[])
{
  struct sockaddr_in self, client;
  struct hostent *cp;
  int sd, td;
  int len;
  char buf[BLEN];
#ifdef _WIN32
  int err;
  WSADATA wsa;

  err = WSAStartup(MAKEWORD(2,2), &wsa);  // winsock 2.2
  net_assert(err, "sample server: WSAStartup");
#endif

  sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  
  memset((char *) &self, 0, sizeof(struct sockaddr_in));
  self.sin_family = AF_INET;
  self.sin_addr.s_addr = INADDR_ANY;
  self.sin_port = htons((u_short) PORT);

  bind(sd, (struct sockaddr *) &self, sizeof(struct sockaddr_in));

  listen(sd, QLEN);

  while (1) {
    len = sizeof(struct sockaddr_in);
    td = accept(sd, (struct sockaddr *) &client, (socklen_t *)&len);

    cp = gethostbyaddr((char *) &client.sin_addr, sizeof(struct in_addr), AF_INET);
    printf("Connected from %s\n", 
          ((cp && cp->h_name) ? cp->h_name : inet_ntoa(client.sin_addr)));

    visits++;
    sprintf(buf, "This server has been contacted %d time(s).\n", visits);
    send(td, buf, strlen(buf), 0);

    close(td);
  }

#ifdef _WIN32
  WSACleanup();
#endif
  exit(0);
}
