/*#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

#include <itpp/itcomm.h>
*/
#include "itpp_ctrl.h"

using namespace std;
using namespace itpp;

//------------------for it++3.7.3 and above
complex <double> double_complex(double r, double i)
{
  return complex<double>(r, i);
}
double log(int x){
  return log((double)x);
}
double pow(int x, int y){
  return pow((double)x, (double)y);
}
double pow(int x, double y){
  return pow((double)x, y);
}
double sqrt(int x){
  return sqrt((double)x);
}

double log2(int &x){
  return log((double)x)/log(2);
}

//--END-------------for it++3.7.3 and above
