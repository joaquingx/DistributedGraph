#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <thread>
#include "Master.h"
#include "auxiliar.h"
#define INITIAL 4


int Master::fstHashFunction(int actualAdjacency,int stationNumbers)
{
  return (actualAdjacency % stationNumbers);
}

int Master::sndHashFunction(int actualAdjacency, int stationNumbers)
{
  return ( (actualAdjacency+13) % stationNumbers);
}

void Master::openFile(char * path)
{
  int cnt = 0 ;
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
          isValid=1;
          if ( fgets (buffer , MAXN , pFile) == NULL ) break;
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
              string msg = "-I ";
              msg += string(buffer);
              int getStation = fstHashFunction(cnt,cntPeers)+INITIAL;
              int getReplication = sndHashFunction(cnt,cntPeers)+INITIAL;
              // cout << msg << "\n";
              // cout << getStation << " " << getReplication << " " << msg  << "\n";
              if (send(getStation, (char*)msg.c_str() , MAXN, 0) == -1)
                {
                  perror("send");
                }
              if(getStation != getReplication)
                {
                  if (send(getReplication, (char*)msg.c_str() , MAXN, 0) == -1)
                    {
                      perror("send");
                    }
                }
              sleep(0.5); // ??? How to overcome?????
            }
        }
      fclose (pFile);
    }
}

void Master::echoSomething(char * buffer)
{
  for(int j = 0; j <= fdmax; j++)
    {
      // send to everyone!
      if (FD_ISSET(j, &master))
        {
          ++activePeers;
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

void Master::distributeBetweenPeers(char * buffer)
{
  string nuevo(buffer);
  openFile((char*)getArgument(buffer,1).c_str());
}


void Master::keepAlive()
{
  string kipi = "-S You are alive! Great!";
  time_t secondsPast = time(NULL),secondsNow;
  while(1)
    {
      secondsNow = time(NULL);
      if(secondsNow - secondsPast > 60)
        {
          secondsPast = secondsNow;
          activePeers = 0;
          echoSomething( (char*)kipi.c_str());
          cntPeers = activePeers-1;
          cout << "Keep Alive Detects " << activePeers << " stations active\n";
          cout << "Estadod el cntEnd: " << cntEnd << "\n";
        }
    }
}

void Master::exitAll()
{
  read_fds = master; // copy it
  if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
    {
      perror("select");
      exit(4);
    }
  // run through the existing connections looking for data to read
  for(int i = 0; i <= fdmax; i++)
    {
      if (FD_ISSET(i, &read_fds))
        { // we got one!!
          close(i); // bye!
        } // END got new incoming connection
    } // END looping through file descriptors
}

void Master::printInfo()
{
  cout << "We have " << cntPeers << " active Peers\n";
  getInfo((*myInfo));
  getProtocol();
}


// void Master::printDebug()
// {

// }

void Master::controlMaster()
{
  string opciones;
  while(1)
    {
      // if(cntRedundancy == getArgument(opciones))
      char buffer[100];
      getline(cin,opciones);
      strcpy(buffer,opciones.c_str());
      char switchter = getOption(opciones,1);
      printLines();
      switch(switchter)
        {
        // case '*':
        //   printDebug();
        case 'D':
          distributeBetweenPeers(buffer);
          break;
        case 'E':
          exitAll();
          break;
        case 'R':
          cntEnd = 0;
          actualState = 1;
          actualRedundancy = getArgument(opciones,1);
          redundancyMap[getArgument(opciones,1)] = 0;
          echoSomething(buffer);
          break;
        case 'Q':
          cntEnd = 0;
          actualState = 2;
          faltaQ = int(char(getArgument(opciones,2)[0])-'0');
          adjacent.clear();
          echoSomething(buffer);
          break;
        case 'C':
          printInfo();
        default:
          echoSomething(buffer);
        }
      printLines();
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
    myInfo = p->ai_addr;
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
      ++cntPeers;
      printf("selectserver: new connection from %s on "
             "socket %d\n",
             inet_ntop(remoteaddr.ss_family,
                       get_in_addr((struct sockaddr*)&remoteaddr),
                       remoteIP, INET6_ADDRSTRLEN),
             newfd);
    }
  return 1;
}

void Master::getRedundancy(string buffer)
{
  string nodo = getArgument(buffer,1);
  // cout << nodo << "<-\n";
  if(nodo != "-1")
    {
      ++redundancyMap[nodo];
    }
}

void Master::updateAdjacent(string buffer)
{
  string argument = getArgument(buffer,1);
  adjacent.insert(argument);
}


void Master::stateControl()
{
  // opcion 1 redundancy
  // opcion 2 depth
  // opcion 3 sinonimos
  switch(actualState)
    {
    case 1:
      cout << "Redundancy of " << actualRedundancy<< ": " << redundancyMap[actualRedundancy] << " \n";
      break;
    case 2:
      depthSearchRecv();
      adjacent.clear();
      break;
    }
  cntEnd=0;
}



void Master::recvControl(char * buffer)
{
  cout << "Esto es lo que estoy recibiendo : "<< buffer << "\n";
  string opciones(buffer);
  strcpy(buffer,opciones.c_str());
  char switchter = getOption(opciones,1);
  switch(switchter)
    {
    case 'R':
      getRedundancy(opciones);
      break;
    case 'Q':
      cout << "Llegando : "<<opciones << "\n";
      updateAdjacent(opciones);
      break;
    case 'E':
      cout << "Estado del cntEnd : " << cntEnd << "\n";
      ++cntEnd;
      if(cntEnd == cntPeers)
        stateControl();
      break;
      // case 'E':
      //   exitAll();
      //   break;
      // case 'C':
      //   printInfo();
    default:
      echoSomething(buffer);
    }
}

bool Master::recvSomething(int i)
{
  char buf[100];    // buffer for client data
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
      --cntPeers;
      FD_CLR(i, &master); // remove from master set
    }
  else
    {
      recvControl(buf);
      // cout << buf << "\n";
      // // we got some data from a client
      // for(int j = 0; j <= fdmax; j++)
      //   {
      //     // send to everyone!
      //     if (FD_ISSET(j, &master))
      //       {
      //         // except the listener and ourselves
      //         if  (j != listener && j != i )
      //           {
      //             if (send(j, buf, nbytes, 0) == -1)
      //               {
      //                 perror("send mesg");
      //               }
      //           }
      //       }
      //   }
    }
  return 1;
}


void Master::depthSearchRecv()
{
  cout << "faltaQ: "<<faltaQ << "\n";
  faltaQ--;
  if(faltaQ <= 0)
    {
      cout << "Nodos Adyacente : " << adjacent.size()  << "\n";
      for(auto it = adjacent.begin() ; it != adjacent.end() ; ++it)
        cout << (*it) << " ";
      cout << "\n";
    }
  else
    {
      string mandar = "-Q " + char(faltaQ+'0');
      string cad;
      for(auto it = adjacent.begin() ; it != adjacent.end() ; ++it)
        {
          cad = "-Q  " + (*it);
          cout << cad << "<-- esto se mandara\n";
          echoSomething((char*)cad.c_str());
        }
    }
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
  thread control(&Master::controlMaster,this);
  thread kipilivi(&Master::keepAlive,this);
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
  kipilivi.join();
}
