#include <iostream>
#include <chrono>
#include <random>
#include "../src/nrutil.h"
#include "../src/nrutil.cpp"
#include "../src/octree.cpp"

using namespace octree;

int main(int argc, char *argv[]){
  double atrad,atrad2, size, halfdim_scalar, volume, volume_regular, x, y, z;
  // double ***grid;
  int is_solute, numvoxel, i,j,k, num_vox_lin; 
  Point3D atcenter, boxcenter, halfdim;
  Octree *octagrid;
  
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator(seed);
  std::uniform_real_distribution<double> udistribution(0.0,1.0);
  
  is_solute = 1;
  size = 5.0;
  halfdim_scalar = size * 0.5;
  atrad = 1.5;
  atrad2 = atrad * atrad;
  
  halfdim.x = halfdim_scalar;
  halfdim.y = halfdim_scalar;
  halfdim.z = halfdim_scalar;
  
  boxcenter.x = halfdim.x;
  boxcenter.y = halfdim.y;
  boxcenter.z = halfdim.z;
  
  atcenter.x = atrad + udistribution(generator) * (size - 2*atrad);
  atcenter.y = atrad + udistribution(generator) * (size - 2*atrad);
  atcenter.z = atrad + udistribution(generator) * (size - 2*atrad);
  
  octagrid = new Octree(boxcenter, halfdim, is_solute);
  
  // we do first division manually:
  for (i = 0; i < 8; i++){
    Point3D neworigin(boxcenter); // necessary to create a new one at each iteration
    neworigin.x += halfdim.x * (i&4 ? .5 : -.5);
    neworigin.y += halfdim.y * (i&2 ? .5 : -.5);
    neworigin.z += halfdim.z * (i&1 ? .5 : -.5);
    
    octagrid->set_children(i, new Octree(neworigin, halfdim*0.5, is_solute)); // ok to pass a temporary as passing by const & the lifetime of the temporary is extended to the lifetime of the ref.
    // children[i]->divide_to_leaf(xcoor, ycoor, zcoor, radius);
  }

  numvoxel = octagrid->count_subchildren();
  std::cout << "octagrid has " << numvoxel << " voxels" << std::endl;

  octagrid->divide_to_leaf(&atcenter.x, &atcenter.y, &atcenter.z, atrad);


  numvoxel = octagrid->count_subchildren();
  std::cout << "octagrid has " << numvoxel << " voxels" << std::endl;

  volume = 0.0; // initialization
  volume = octagrid->get_atom_excl_volume(&atcenter.x,&atcenter.y,&atcenter.z, atrad);
  
  std::cout << "Expected volume is: " << (size*size*size - 4.0/3.0 * 3.14159265358979323846 * atrad * atrad * atrad) << std::endl;
  std::cout << "Calculated volume is: " << volume << std::endl;

  num_vox_lin = (int) (size/(MINHALFDIM*2.0));
  volume_regular = 0.0;
  std::cout << "Num voxel per dimension: " << num_vox_lin << std::endl;
  
  for (i=0; i < num_vox_lin; i++){
    x = (double) i * (MINHALFDIM*2.) + MINHALFDIM;
    for (j = 0; j < num_vox_lin; j++){
      y = (double)j * (MINHALFDIM*2.) + MINHALFDIM;
      for (k = 0; k < num_vox_lin; k++){
        z = (double)k * (MINHALFDIM*2.) + MINHALFDIM;
        if (distance2(atcenter, x, y, z) >= atrad2){
          volume_regular += (MINHALFDIM*2) * (MINHALFDIM*2) * (MINHALFDIM*2);
        }
      }
    }
  }
  std::cout << "Calculated volume with regular grid is: " << volume_regular << std::endl;

}
