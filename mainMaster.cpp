#include <bits/stdc++.h>
#include "auxiliar.h"
#include "Master.h"
using namespace std;

int main(int argc, char * argv[])
{
  if(argc < 3)
    {
      cout << "Usage:\n";
      cout << "./mainMaster.o address port\n";
      return 1;
    }
  Master estacion(argv[1],argv[2]);
  estacion.processing();
  return 0;
}
