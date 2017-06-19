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
  int yes = 1 , listener, sender ;
  char PORTLISTENER[100],PORTSENDER[100], IP_ADDR[100];
  bigramList myPart;
  Slave(char * ipAddr, char * portListener, char * portSender);
  int linkWithSocket( char * ipAddr, char* port);
  void connectWithSocket(char * ipAddr, char * port, int & sockfd);
  void sendSomething();
  void recvSomething();
  void processing();
};



#endif
