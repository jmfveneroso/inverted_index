/**
 * Utility functions.
 * @author: jmfveneroso@gmail.com.br
 */

#ifndef __UTIL_H__
#define __UTIL_H__

#include <iostream>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <map>
#include <time.h>
#include <unistd.h>

using namespace std;

namespace TP1 {

static inline void startbar () {
  cout << setw(9) << "Completed";
  cout << setw(20) << "Time Ellapsed";
  cout << setw(20) << "Time Remaining";
  cout << setw(30) << "Object count";
  cout << endl;
}

/**
 * Usage: 
 *   startbar();
 *   for (int i = 0; i < 100000000; i++) {
 *     // Do stuff
 *     loadbar(i, 100000000 - 1);
 *   }
 */
static inline void loadbar (unsigned int x, unsigned int n) {
  static clock_t start_t = clock();
  
  if (n == 0) n = 0;
  
  float ratio  = (n == 0) ? 1 : (x / (float) n);
  
  cout << setw(8) << (int)((float)(ratio * 10000) / 100) << "%";
  
  clock_t t = clock() - start_t;
  int time_ellapsed = (float) t * 1000 / CLOCKS_PER_SEC;
  
  cout << setw(17) << time_ellapsed << " ms";
  
  int time_remaining = (float) (1 / ratio) * time_ellapsed - time_ellapsed;
  
  if (x != 0)
    cout << setw(17) << time_remaining << " ms";
  else
    cout << setw(17) << "---" << " ms";
  
  cout << "\r" << flush;
  
  // usleep(1000000);
}
  
} // End of namespace Tp1.

#endif
