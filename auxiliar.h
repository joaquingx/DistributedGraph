#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <bits/stdc++.h>
using namespace std;
#ifndef AUXILIAR_H
#define AUXILIAR_H
#define MAXN 100
#define MAXELEMENTS int(1e5+100)
typedef unsigned char uchar;
typedef map<string , set<string>  > bigramList;

bigramList openFile(char * path);
void printBigramList(bigramList mp);
int fstHashFunction(int actualAdjacency,int stationNumbers);
int sndHashFunction(int actualAdjacency, int stationNumbers);
void getInfo(struct sockaddr sa);
void *get_in_addr(struct sockaddr *sa);
#endif
