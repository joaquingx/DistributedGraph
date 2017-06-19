#include <bits/stdc++.h>
#include "auxiliar.h"
#include "Slave.h"
using namespace std;

int main(int argc, char * argv[])
{
  if(argc < 3)
    {
      cout << "Usage:\n";
      cout << "./mainSlave.o address port\n";
      return 1;
    }
  Slave estacion(argv[1],argv[2]);
  estacion.processing();
  return 0;
}
