/*
 * Util.h
 *
 *  Created on: 10 Oct 2011
 *      Author: buck
 */

#ifndef UTIL_H_
#define UTIL_H_


template<class F>
double dotProduct(const vector<F>& a, const vector<F>& b)
{
  size_t d = a.size();
  assert(b.size() == d);
  F p = 0;
  for (size_t i = 0; i < d; ++i)
  {
    p += a[i] * b[i];
  }
  return p;
}

ostream & operator <<(ostream &os, const Phrase& p)
{
  for (size_t i = 0; i < p.size(); i++)
  {
    if (i > 0)
      os << " ";
    os << p[i];
  }
  return os;
}


#endif /* UTIL_H_ */
