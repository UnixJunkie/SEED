#include "nrutil.h"
#include "octree.hpp"

#define MINHALFDIM 0.005

namespace octree {
  
  class Octree {
    point origin;
    point halfdim;
    Octree *children[8]; // pointer to the four octants
    int is_solute; // if it is occupied or empty
    int is_frontier; // if it is on the frontier
    
  public:
    Octree(const point& origin, const point& halfdim, int is_solute)
    : origin(origin), halfdim(halfdim), is_solute(is_solute), is_frontier(-1) {
      for (auto i=0; i < 8; i++){
        children[i] = nullptr;
      }
    }
    // TODO Add copy constructor
    ~Octree(){ // destructor
      // delete data;
      for (auto i=0; i < 8; i++){
        delete children[i];
      }
    }
    
    void divide_to_leaf(const double *xcoor, const double *ycoor, const double *zcoor, double radius){
      int i;
      // bool is_split;
      bool within_radius;
      point corner;
      double radius2;
      
      if (!is_solute){ // octree is not covering a solute region
        return;
      }
      if (halfdim.x < MINHALFDIM || halfdim.y < MINHALFDIM || halfdim.z < MINHALFDIM){ // minimum grid size reached
        return;
      }
      
      radius2 = radius * radius;
      // if (distance2(origin, *xcoor, *ycoor, *zcoor) > radius2)
      
      // Now we check if octree is at the frontier
      is_frontier = false;
      corner.x = origin.x + halfdim.x;
      corner.y = origin.y + halfdim.y;
      corner.z = origin.z + halfdim.z; 
      if (distance2(corner, *xcoor, *ycoor, *zcoor) <= radius2)
        within_radius = true;
      
      for (i=1; i < 8; i++){ // loop over corners
        corner.x = origin.x + halfdim.x * (i&4 ? 1.0 : -1.0);
        corner.y = origin.y + halfdim.y * (i&2 ? 1.0 : -1.0);
				corner.z = origin.z + halfdim.z * (i&1 ? 1.0 : -1.0);
        if ((distance2(corner, *xcoor, *ycoor, *zcoor) <= radius2) != within_radius)){
          is_frontier = true;
          break;
        }
      }
      
      if (!is_frontier){
        return;
      }
      else { // we are in a frontier region
        for (i=0; i < 8; i++){
          point neworigin; // necessary to create a new one at each iteration
          neworigin.x += halfdim.x * (i&4 ? .5 : -.5);
					neworigin.y += halfdim.y * (i&2 ? .5 : -.5);
					neworigin.z += halfdim.z * (i&1 ? .5 : -.5);
          
          children[i] = new Octree(neworigin, halfdim*.5f, is_solute); // ok to pass a temporary as passing by const & the lifetime of the temporary is extended to the lifetime of the ref.
          children[i].divide_to_leaf();
        }
        return;
      }
      
    }
    
    // void setData(){ // This is Seed specific
    // }
    
    bool isLeaf(){
      return (children[0] == nullptr);
    }
    
    double get_self_en_integral(const double *xcoor, const double *ycoor, const double *zcoor, double radius){
      
      double tmp_result;
      double r2;
      double voxel_vol; // volume of the voxel
      
      if (isLeaf()){
        r2 = distance2(origin, *xcoor, *ycoor, *zcoor);
        if (is_solute && r2 >= (radius * radius)){
          voxel_vol = (halfdim.x * 2) * (halfdim.y * 2) * (halfdim.z * 2);
          return ( voxel_vol/ (r2 * r2 * sqrt(r2)));
        } else {
          return 0.0;
        }
      }
      else {
        tmp_result = 0.0;
        for (auto i=0; i < 8; i++){
          tmp_result += children[i].get_self_en_integral(xcoor, ycoor, zcoor);
        }
        return(tmp_result);
      }
    }
  }
  
  double distance2(point center, double xcoor, double ycoor, double zcoor){
    return( (center.x - xcoor)*(center.x - xcoor) +
            (center.y - ycoor)*(center.y - ycoor) +
            (center.z - zcoor)*(center.z - zcoor));
  }
  
}
