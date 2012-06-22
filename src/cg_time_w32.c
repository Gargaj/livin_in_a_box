#include <windows.h>
#include "cg_time.h"

double cg_start_time  = 0.0;

LARGE_INTEGER pcv = { 0 };

double cg_previous_time  = 0.0;
double getTime() 
{
  LARGE_INTEGER count, freq;
  if (!pcv.QuadPart) {
    QueryPerformanceCounter(&pcv);
  }
  QueryPerformanceCounter(&count);
  QueryPerformanceFrequency(&freq);

  cg_previous_time += (double)(count.QuadPart-pcv.QuadPart) / (double)(freq.QuadPart);

  pcv = count;

  return cg_previous_time;
}

void startTime(){
  cg_start_time = getTime();
}

double getTimeInterval(){
  return getTime() - cg_start_time;
}
