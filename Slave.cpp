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
#include "Slave.h"

int Slave::linkWithSocket(char * ipAddr, char* port)
{
  struct addrinfo hints, *ai, *p;
  int rv;
  int socketName;
  // get us a socket and bind it
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo((char * ) ipAddr, (char *)port, &hints, &ai)) != 0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }
  //falta para el sender

  for(p = ai; p != NULL; p = p->ai_next) {
    socketName = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (socketName < 0) {
      continue;
    }
    struct sockaddr * ipv4 = p->ai_addr;
    getInfo((*ipv4));
    // lose the pesky "address already in use" error message
    setsockopt(socketName, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(socketName, p->ai_addr, p->ai_addrlen) < 0) {
      close(socketName);
      continue;
    }

    break;
  }

  // if we got here, it means we didn't get bound
  if (p == NULL) {
    fprintf(stderr, "selectserver: failed to bind\n");
    exit(2);
  }
  freeaddrinfo(ai); // all done with this
  return socketName;
}

void Slave::connectWithSocket(char * ipAddr, char * port, int & sockfd)
{
  int numbytes;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(ipAddr, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return ;
  }

  // loop through all the results and connect to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
                         p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }
    myInfo = p->ai_addr;

    if ( (connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
      close(sockfd);
      perror("client: connect");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
  printf("client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure
}


Slave::Slave(char * ipAddr,  char * portSender)
{
  strcpy(this->PORTSENDER,portSender);
  strcpy(this->IP_ADDR, ipAddr);
  connectWithSocket(ipAddr,portSender, socketfd);
}

void Slave::sendSomething()
{
  while(1)
    {
      string opciones;
      getline(cin,opciones);
      int idxOpcion = opciones.find("-");
      int idxArgument = opciones.find(" ");
      string argument = opciones.substr(idxArgument+1);
      char buffer[100];
      strcpy(buffer,argument.c_str());
      if(opciones[idxOpcion+1] == 'S') // Send Something
        {
          if (send(socketfd, buffer, 100, 0) == -1)
            perror("send\n");
        }
      cout << "Mandado!\n";
    }
}

void Slave::printEcho(string opciones)
{
  cout << getArgument(opciones) << "\n";
}
void Slave::recvEdge(string buffer)
{
  // cout << "recibi esto : "<<buffer << "\n";
  buffer = getArgument(buffer);
  string fstWord,sndWord;
  int i;
  for(i = 0 ; buffer[i] != ' ' ; ++i)
    fstWord += buffer[i];
  for(i = i + 1 ; buffer[i] != 10 and buffer[i] != '\0'; ++i)
    sndWord += buffer[i];
  adjList[fstWord].insert(sndWord);
  adjList[sndWord].insert(fstWord);
}

void Slave::printEdges()
{
  printBigramList(adjList);
}

void Slave::printInfo()
{
  getInfo((*myInfo));
  cout << "Nodos almacenados: " << adjList.size() << "\n";
}

void Slave::sendRedundancy(string opciones)
{
  string arg = getArgument(opciones), stOk,stError;
  stOk = "-R " + arg;
  stError = "-R -1" ;
  char bufOk[100],bufError[100];
  strcpy(bufOk,stOk.c_str());
  strcpy(bufError,stError.c_str());
  if(adjList.count(arg))
    {
      if (send(socketfd, bufOk, 100, 0) == -1)
        perror("send\n");
    }
  else
    {
      if (send(socketfd, bufError, 100, 0) == -1)
        perror("send\n");
    }

}

void Slave::recvSomething()
{
  while(1)
    {
      char buf[256];    // buffer for client data
      int nbytes;
      // handle data from a client
      if ((nbytes = recv(socketfd, buf, sizeof buf, 0)) <= 0) // se fue
        {
          // got error or connection closed by client
          if (nbytes == 0)
            {
              // connection closed
              printf("selectserver: socket %d hung up\n", socketfd);
            }
          else
            {
              perror("recv:::::");
            }
          // close(socketfd); // bye!
        }
      else
        {
          string opciones(buf);
          char switcher = getOption(buf);
          switch(switcher)
            {
            case 'S':
              printLines();
              printEcho(opciones);
              printLines();
              break;
            case 'I': //Insertar Artistas
              recvEdge(opciones);
              break;
            case 'P': // Imprimir Aristas
              printLines();
              printEdges();
              printLines();
              break;
            case 'C':
              printLines();
              printInfo();
              printLines();
              break;
            case 'R':
              sendRedundancy(opciones);
            }
        }
    }
}
void Slave::processing()
{
  thread controlSend(&Slave::sendSomething,this);
  thread controlRcv(&Slave::recvSomething,this);
  controlSend.join();
  controlRcv.join();
}
