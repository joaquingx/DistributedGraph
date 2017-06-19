#include <bits/stdc++.h>
#include "auxiliar.h"
#include "Slave.h"
using namespace std;

int main(int argc, char * argv[])
{
  Slave estacion(argv[1],argv[2],argv[3]);
  estacion.processing();
  return 0;
}
