#ifndef KOBJECT_HEADER
#define KOBJECT_HEADER

#include <iostream>

template <int N>
class KObject {

public:
  KObject();
  ~KObject() {};

  void print();

protected:
  int  pts[N];
  bool flag;
};

template <int N>
KObject<N>::KObject()
{
  flag   = false;
  for(int i=0; i<N; i++)
    pts[i] = 2*i;
}

template <int N>
void KObject<N>::print()
{
  for(int i=0; i<N; i++)
    std::cout << "[" << i << "]: " << pts[i] << std::endl;
}


#endif

