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

#endif /* UTIL_H_ */
