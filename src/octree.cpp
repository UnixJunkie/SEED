#include <iostream>
#include "nrutil.h"
// #include "octree.hpp"

#define MINHALFDIM 0.1

using namespace std;

namespace octree {

  double distance2(Point3D center, double xcoor, double ycoor, double zcoor);
  
  class Octree {
    Point3D origin;
    Point3D halfdim;
    Octree *children[8]; // pointer to the four octants
    int is_solute; // if it is occupied or empty
    int is_frontier; // if it is on the frontier
    
  public:
    Octree(const Point3D& origin, const Point3D& halfdim, int is_solute)
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

    void set_children(int i, Octree *newborn){
      children[i] = newborn;
    }

    int count_subchildren(){
      int count = 0;

      if (isLeaf()){
        return 1;
      }

      for (auto i=0; i < 8; i++){
        count += children[i]->count_subchildren();
      }
      return count++;
    }
    
    void divide_to_leaf(const double *xcoor, const double *ycoor, const double *zcoor, double radius){
      int i;
      // bool is_split;
      bool within_radius;
      Point3D corner;
      double radius2;
      
      if (!is_solute){ // octree is not covering a solute region
        cout << "Not in a solute region" << endl;
        return;
      }

      if (halfdim.x < MINHALFDIM || halfdim.y < MINHALFDIM || halfdim.z < MINHALFDIM){ // minimum grid size reached
        cout << "MINHALFDIM reached" << endl;
        return;
      }

      if (!isLeaf()){
        cout << "Already divided. Moving down to children" << endl;
        for (i=0; i<8; i++){
          children[i]->divide_to_leaf(xcoor, ycoor, zcoor, radius);
        }
        return;
      }
      
      radius2 = radius * radius;
      // if (distance2(origin, *xcoor, *ycoor, *zcoor) > radius2)
      
      // Now we check if octree is at the frontier
      is_frontier = false;
      corner.x = origin.x + halfdim.x;
      corner.y = origin.y + halfdim.y;
      corner.z = origin.z + halfdim.z; 
      cout << "x,y,z = " << corner.x << " " << corner.y << " " << corner.z << endl;
      if (distance2(corner, *xcoor, *ycoor, *zcoor) <= radius2){
        within_radius = true;
      }
      
      for (i=1; i < 8; i++){ // loop over corners
        corner.x = origin.x + halfdim.x * (i&4 ? 1.0 : -1.0);
        corner.y = origin.y + halfdim.y * (i&2 ? 1.0 : -1.0);
				corner.z = origin.z + halfdim.z * (i&1 ? 1.0 : -1.0);
        cout << "x,y,z = " << corner.x << " " << corner.y << " " << corner.z << endl;
        if ((distance2(corner, *xcoor, *ycoor, *zcoor) <= radius2) != within_radius){
          is_frontier = true;
          break;
        }
      }
      
      if (!is_frontier){
        cout << "Not in a frontier!" << endl;
        return;
      }
      else { // we are in a frontier region
        cout << "In frontier region. Spawning children" << endl;
        for (i=0; i < 8; i++){
          Point3D neworigin(origin); // necessary to create a new one at each iteration
          neworigin.x += halfdim.x * (i&4 ? .5 : -.5);
					neworigin.y += halfdim.y * (i&2 ? .5 : -.5);
					neworigin.z += halfdim.z * (i&1 ? .5 : -.5);
          
          children[i] = new Octree(neworigin, halfdim*.5, is_solute); // ok to pass a temporary as passing by const & the lifetime of the temporary is extended to the lifetime of the ref.
          children[i]->divide_to_leaf(xcoor, ycoor, zcoor, radius);
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
          voxel_vol = (halfdim.x * 2.0) * (halfdim.y * 2.0) * (halfdim.z * 2.0);
          return ( voxel_vol/ (r2 * r2 * sqrt(r2)));
        } else {
          return 0.0;
        }
      }
      else {
        tmp_result = 0.0;
        for (auto i=0; i < 8; i++){
          tmp_result += children[i]->get_self_en_integral(xcoor, ycoor, zcoor, radius);
        }
        return(tmp_result);
      }
    }

    double get_atom_excl_volume(const double *xcoor, const double *ycoor, const double *zcoor, double radius){
        
        double tmp_result;
        double r2;
        double voxel_vol; // volume of the voxel
        
        if (isLeaf()){
          r2 = distance2(origin, *xcoor, *ycoor, *zcoor);
          if (is_solute && r2 >= (radius * radius)){
            voxel_vol = (halfdim.x * 2.0) * (halfdim.y * 2.0) * (halfdim.z * 2.0);
            return voxel_vol;
          } else {
            return 0.0;
          }
        }
        else {
          tmp_result = 0.0;
          for (auto i=0; i < 8; i++){
            tmp_result += children[i]->get_atom_excl_volume(xcoor, ycoor, zcoor, radius);
          }
          return(tmp_result);
        }
      }
    };

  
  
  double distance2(Point3D center, double xcoor, double ycoor, double zcoor){
    return( (center.x - xcoor)*(center.x - xcoor) +
            (center.y - ycoor)*(center.y - ycoor) +
            (center.z - zcoor)*(center.z - zcoor));
  }
  
}
