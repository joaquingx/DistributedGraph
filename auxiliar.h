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

char getOption(string s, int cual);
string getArgument(string s, int cual);
void printBigramList(bigramList mp);
void getInfo(struct sockaddr sa);
void getProtocol();
void *get_in_addr(struct sockaddr *sa);
void printLines();
#endif
