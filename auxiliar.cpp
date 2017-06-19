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

bigramList openFile(char * path)
{
  int cnt = 0 ;
  bigramList mp;
  string fstWord,sndWord;
  bool isValid;
  FILE * pFile;
  char buffer [MAXN];
  pFile = fopen (path , "r");
  if (pFile == NULL) perror ("Error opening file");
  else
    {
      while ( ! feof (pFile) )
        {
          ++cnt;
          if(cnt > MAXELEMENTS)
            {
              break;
            }
          fstWord = sndWord =  "";
          isValid=1;
          if ( fgets (buffer , 100 , pFile) == NULL ) break;
          for(int i = 0 ; buffer[i] != 0 ; ++i )
            {
              if(int(uchar(buffer[i])) > 127)
                {
                  isValid=0;
                  break;
                }
            }
          if(isValid)
            {
              int i;
              for(i = 0 ; buffer[i] != ' ' ; ++i)
                fstWord += buffer[i];
              for(i = i + 1 ; buffer[i] != 10 ; ++i)
                sndWord += buffer[i];
              mp[fstWord].insert(sndWord);
              mp[sndWord].insert(fstWord);
            }
        }
      fclose (pFile);
    }
  return mp;
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

int fstHashFunction(int actualAdjacency,int stationNumbers)
{
  return (actualAdjacency % stationNumbers);
}

int sndHashFunction(int actualAdjacency, int stationNumbers)
{
  return ( (actualAdjacency+13) % stationNumbers);
}

void getInfo(struct sockaddr sa)
{
  char host[1024] , service[20];
  getnameinfo(&sa, sizeof sa, host, sizeof host, service, sizeof service, 0);
  printf("host: %s\n", host);    // e.g. "www.example.com"
  printf("service: %s\n", service); // e.g. "http"
}
