#include <iostream>
#include <random>
#include "../src/nrutil.h"
#include "../src/octree.cpp"

using namespace octree;

int main(int argc, char *argv[]){
  double atrad, size, halfdim_scalar; 
  point atcenter, boxcenter, halfdim;
  Octree *octagrid;
  
  std::default_random_engine generator;
  std::uniform_real_distribution<double> udistribution(0.0,1.0);
  
  size = 5.0;
  halfdim_scalar = size / 2.0;
  atrad = 1.5;
  
  halfdim.x = halfdim_scalar;
  halfdim.y = halfdim_scalar;
  halfdim.z = halfdim_scalar;
  
  boxcenter.x = halfdim.x;
  boxcenter.y = halfdim.y;
  boxcenter.z = halfdim.z;
  
  atcenter.x = atrad + udistribution(generator) * (size - 2*atrad);
  atcenter.y = atrad + udistribution(generator) * (size - 2*atrad);
  atcenter.z = atrad + udistribution(generator) * (size - 2*atrad);
  
  octagrid = new Octree(boxcenter, halfdim, )
  
}
