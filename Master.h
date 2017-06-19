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
using namespace std;

#ifndef MASTER_H
#define MASTER_H
struct Master
{
  char PORTLISTENER[100], IP_ADDR[100];
  fd_set master;    // master file descriptor list
  fd_set read_fds;  // temp file descriptor list for select()
  int fdmax;        // maximum file descriptor number
  int listener ,  newfd;
  int yes=1;
  Master(char * ipAddr, char * portListener);
  void processing();
  bool newConnection();
  bool sendSomething();
  bool recvSomething(int i);
  int linkWithSocket( char * ipAddr, char* port);
  void echoSomething();
};

#endif
