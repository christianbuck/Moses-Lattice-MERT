/*
 * LatticeMERT
 * Copyright (C)  2010-2012 
 *   Christian Buck
 *   Kārlis Goba <karlis.goba@gmail.com> 
 * 
 * LatticeMERT is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * LatticeMERT is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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

/*
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
*/

// From: Effective STL, item 7
struct DeleteObject
{
  template<typename T>
  void operator()(const T* ptr) const
  {
    delete ptr;
  }
};


#endif /* UTIL_H_ */
