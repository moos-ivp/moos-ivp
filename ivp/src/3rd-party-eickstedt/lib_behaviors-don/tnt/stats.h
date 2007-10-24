
#ifndef STATS_H
#define STATS_H

template <class T>
T
min(Vector<T> x) {
  Subscript n = x.dim();
  T min = x[0];
  for (int i=1; i<n; i++)
    if (min>x[i]) min = x[i];
  return min;
}

template <class T>
T
max(Vector<T> x) {
  Subscript n = x.dim();
  T max = x[0];
  for (int i=1; i<n; i++)
    if (max<x[i]) max = x[i];
  return max;
}

template <class T>
T
mean(Vector<T> x) {
  T m = 0;
  for (int i=0; i<x.dim(); i++) m += x[i];
  m /= x.dim();
  return m;
}

/* template <class T>
T
square(T x) {
  return x*x;
  }*/

#endif
