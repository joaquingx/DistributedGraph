#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <thread>
#include "auxiliar.h"

#ifndef SLAVE_H
#define SLAVE_H

struct Slave
{
  int yes = 1 , socketfd;
  char PORTLISTENER[MAXN],PORTSENDER[MAXN], IP_ADDR[MAXN];
  bigramList adjList;
  struct sockaddr * myInfo;
  Slave(char * ipAddr,  char * portSender);
  int linkWithSocket( char * ipAddr, char* port);
  void connectWithSocket(char * ipAddr, char * port, int & sockfd);
  void sendSomething();
  void recvSomething();
  void processing();
  void printEcho(string opciones);
  void recvEdge(string buffer);
  void printEdges();
  void printInfo();
  void sendRedundancy(string opciones);
  void sendAdjacency(string pattern);
};



#endif
