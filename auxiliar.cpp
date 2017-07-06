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
  printf("Host: %s\n", host);    // e.g. "www.example.com"
  printf("Port: %s\n", service); // e.g. "http"
}


void getProtocol()
{
  cout << "Usage: \n";
  cout << "-S Argument : Echo Argument on all peers\n";
  cout << "-D Argument : Distribute the argument among peers\n";
  cout << "-I Argument : Insert Bigram in Graph\n";
  cout << "-P : Print Adjacency List on all peers\n";
  cout << "-C : Get this page\n";
}

char getOption(string s, int cual)
{
  string saved = s;
  int pos = 0;
  for(int i = 0 ; i < cual ; ++i)
    {
      pos = s.find("-") + 1;
      s = s.substr(pos);
    }
  return saved[pos];
}

string getArgument(string s, int cual)
{
  reverse(s.begin(), s.end() );
  int pos=0;
  for(int i = 0 ; i < cual; ++i)
    {
      s = s.substr(pos);
      pos = s.find(" ")+1;
    }
  pos--;
  // cout << s << "\n";
  // cout << pos << "\n";
  reverse(s.begin(), s.end() );
  return  s.substr(s.size()-pos);
}

void printLines()
{
  cout << "------------------------------------------------\n";
  cout << "------------------------------------------------\n";
}
