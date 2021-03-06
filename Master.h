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
  char PORTLISTENER[MAXN], IP_ADDR[MAXN];
  fd_set master;    // master file descriptor list
  fd_set read_fds;  // temp file descriptor list for select()
  int fdmax;        // maximum file descriptor number
  int listener ,  newfd;
  int yes=1;
  int cntPeers=0,activePeers=0,cntEnd=0, cntRedundancy,cntAdjacent=0, cntCombinations=0, getSize=0;
  int faltaQ = 0;
  int actualState=0;
  string actualRedundancy;
  vector< vector < string > > combinations;
  map<string , int > redundancyMap;
  set<string> adjacent;
  struct sockaddr * myInfo;
  Master(char * ipAddr, char * portListener);
  /* void getQuery(char * buffer); */
  void processing();
  bool newConnection();
  bool sendSomething();
  bool recvSomething(int i);
  int linkWithSocket( char * ipAddr, char* port);
  void echoSomething();
  void controlMaster();
  int fstHashFunction(int actualAdjacency,int stationNumbers);
  int sndHashFunction(int actualAdjacency, int stationNumbers);
  void openFile(char * path);
  void echoSomething(char * buffer);
  void distributeBetweenPeers(char * buffer);
  void stateControl();
  void updateAdjacent(string buffer);
  void keepAlive();
  vector<string> depthSearch(int falta , map<string , int > mark );
  void exitAll();
  void printInfo();
  void getRedundancy(string buffer);
  void recvControl(char * buffer);
  void depthSearchRecv();
  void printDebug();
  void fillCombinate();
  void printCombinations();
  void combinationsProcessing( string buffer);
  void assignStation(char * buffer);
};

#endif
