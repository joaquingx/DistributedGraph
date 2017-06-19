# Distributed Graph
### How to build
```sh
  $ git clone https://github.com/joaquingx/DistributedGraph.git
  $ cd DistributedGraph
  $ g++  -pthread auxiliar.cpp Master.cpp mainMaster.cpp -o mainMaster.o     
  $ g++  -pthread auxiliar.cpp Slave.cpp mainSlave.cpp -o mainSlave.o  
```

### Protocol
**-S Argument** Echo on all peers.

**-D Argument** Distribute file(argument) in all peers.

**-I Argument** Insert Bigram(argument).

**-P** Print Adjacency List on all peers.

### Example
```sh
  $ ./mainMaster.o
  $ -S Hola!
  $ -D prueba.txt
  $ -I chiclayo peru
  $ -P
```
```sh
  $ ./mainSlave.o 127.0.0.1 9034
```
