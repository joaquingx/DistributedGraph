#include <bits/stdc++.h>
using namespace std;

typedef map< string , vector< string > > Adjacency;


void graphInput(Adjacency Graph)
{
  int n, m ; cin >> n >> m;
  for(int i = 0 ; i < n ; ++i)
    {
      string l,r; cin >> l >> r;
      Graph[l] = r;
      Graph[r] = l;
    }
}


