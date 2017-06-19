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
#include "auxiliar.h"

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void printBigramList(bigramList mp)
{
  for(auto it = mp.begin() ; it != mp.end() ; ++it)
    {
      cout << it->first << " : ";
      for(auto adjIt = it->second.begin() ; adjIt != it->second.end() ; ++adjIt)
        {
          cout << (*adjIt) << " ";
        }
      cout << "\n";
    }
}

void getInfo(struct sockaddr sa)
{
  char host[1024] , service[20];
  getnameinfo(&sa, sizeof sa, host, sizeof host, service, sizeof service, 0);
  printf("host: %s\n", host);    // e.g. "www.example.com"
  printf("service: %s\n", service); // e.g. "http"
}


char getOption(string s)
{
  return s[s.find("-")+1];
}

string getArgument(string s)
{
  return  s.substr(s.find(" ") + 1);
}
