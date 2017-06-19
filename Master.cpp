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
#include "Master.h"
#include "auxiliar.h"

void Master::echoSomething()
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
          for(int j = 0; j <= fdmax; j++)
            {
              // send to everyone!
              if (FD_ISSET(j, &master))
                {
                  // except the listener and ourselves
                  if  (j != listener)
                    {
                      if (send(j, buffer, 100, 0) == -1)
                        {
                          perror("send");
                        }
                    }
                }
            }
        }
      cout << "Mandado!\n";
    }
}

int Master::linkWithSocket( char * ipAddr, char* port)
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

Master::Master(char * ipAddr, char * portListener)
{
  strcpy(this->PORTLISTENER,portListener);
  strcpy(this->IP_ADDR, ipAddr);

  FD_ZERO(&master);    // clear the master and temp sets
  FD_ZERO(&read_fds);
  listener = linkWithSocket(ipAddr,portListener);

  // listen
  if (listen(listener, 10) == -1) {
    perror("listen");
    exit(3);
  }

  // add the listener  and sender to the master set
  FD_SET(listener, &master);

  // keep track of the biggest file descriptor
  fdmax = listener; // so far, it's this one
}


bool Master::newConnection()
{
  char remoteIP[INET6_ADDRSTRLEN];
  struct sockaddr_storage remoteaddr; // client address
  socklen_t addrlen;
  // handle new connections
  addrlen = sizeof remoteaddr;
  newfd = accept(listener,
                 (struct sockaddr *)&remoteaddr,
                 &addrlen);
  if (newfd == -1)
    {
      perror("accept");
    }
  else
    {
      FD_SET(newfd, &master); // add to master set
      if (newfd > fdmax)
        {    // keep track of the max
          fdmax = newfd;
        }
      printf("selectserver: new connection from %s on "
             "socket %d\n",
             inet_ntop(remoteaddr.ss_family,
                       get_in_addr((struct sockaddr*)&remoteaddr),
                       remoteIP, INET6_ADDRSTRLEN),
             newfd);
    }
  return 1;
}

bool Master::recvSomething(int i)
{
  char buf[256];    // buffer for client data
  int nbytes;

  // handle data from a client
  if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) // se fue
    {
      // got error or connection closed by client
      if (nbytes == 0)
        {
          // connection closed
          printf("selectserver: socket %d hung up\n", i);
        }
      else
        {
          perror("recv:");
        }
      close(i); // bye!
      FD_CLR(i, &master); // remove from master set
    }
  else
    {
      cout << buf << "\n";
      // we got some data from a client
      for(int j = 0; j <= fdmax; j++)
        {
          // send to everyone!
          if (FD_ISSET(j, &master))
            {
              // except the listener and ourselves
              if  (j != listener && j != i )
                {
                  if (send(j, buf, nbytes, 0) == -1)
                    {
                      perror("send");
                    }
                }
            }
        }
    }
  return 1;
}

bool Master::sendSomething()
{
  return 1;
}
void Master::processing()
{
  //aux variables
  int i, j, rv;
  // aux variables
  thread control(&Master::echoSomething,this);
  for(;;) {
    read_fds = master; // copy it
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
      {
        perror("select");
        exit(4);
      }
    // run through the existing connections looking for data to read
    for(i = 0; i <= fdmax; i++)
      {
        if (FD_ISSET(i, &read_fds))
          { // we got one!!
            if (i == listener) // coneccion nueva
              newConnection();
            else
              recvSomething(i);
          } // END got new incoming connection
      } // END looping through file descriptors
  } // END for(;;)--and you thought it would never end!
  control.join();
}
