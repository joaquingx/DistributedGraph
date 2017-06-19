# Distributed Graph
### How to build
```sh
  $ git clone https://github.com/joaquingx/DistributedGraph.git
  $ cd DistributedGraph
  $ g++  -pthread auxiliar.cpp Master.cpp mainMaster.cpp -o mainMaster.o     
  $ g++  -pthread auxiliar.cpp Slave.cpp mainSlave.cpp -o mainSlave.o  
```

### Example
```sh
  $ ./mainMaster.o
  $ -S Hola!
  $ Mandado!
```
```sh
  $ ./mainSlave.o 127.0.0.1 9040 9034
  $ -S Hola!
  $ Mandado!
  $ Recibi : Hola!
```
