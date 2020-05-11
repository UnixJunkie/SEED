/*
*    This file is part of SEED.
*
*    Copyright (C) 2017, Caflisch Lab, University of Zurich
*
*    SEED is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    SEED is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _QUATERNION_H
#define _QUATERNION_H

#include <iostream>
#include "point.h"
//#include <math.h>
//#include "nrutil.h"

template <class T>
class Quaternion {
  private:
    T w,x,y,z;
  public:
    // Constructors
    Quaternion(); // Default constructor: creates a zero quaternion.
    Quaternion(T w_i,T x_i,T y_i, T z_i);
    Quaternion(T *v);
    Quaternion(const Quaternion<T>& q); //copy constructor
    //default destructor
    ~Quaternion();
    //Overloaded operators
    const Quaternion<T>& operator=(const Quaternion<T>& rhs);
    const Quaternion<T>& operator+=(const Quaternion<T>& q);
    const Quaternion<T>& operator-=(const Quaternion<T>& q);
    Quaternion<T> operator*(const Quaternion<T>& q) const;
    Quaternion<T> operator/(Quaternion<T>& q);
    Quaternion<T> operator/(const T s);
    //const Quaternion<T>& operator*=(const Quaternion<T>& q);
    bool operator==(const Quaternion<T>& q) const;
    bool operator!=(const Quaternion<T>& q) const;

    friend Quaternion operator+(const Quaternion& lhs,
            const Quaternion& rhs){
      return Quaternion(lhs) += rhs;
    }
    friend Quaternion operator-(const Quaternion& lhs,
            const Quaternion& rhs){
      return Quaternion(lhs) -= rhs;
    }
    friend Quaternion operator-(const Quaternion& q){
      return Quaternion()-q;
    }
    friend Quaternion operator*(const Quaternion& q, const T s){
      return Quaternion(q.w*s,q.x*s,q.y*s,q.z*s);
    }
    friend Quaternion operator*(const T s, const Quaternion& q){
      return Quaternion(q.w*s,q.x*s,q.y*s,q.z*s);
    }
    //friend Quaternion<T> operator*(const Quaternion<T>& lhs,
    //                                const Quaternion<T>& rhs);
    friend std::ostream& operator<<(std::ostream& output,
                                    const Quaternion& q){
      output << "[" << q.w << ", " << "(" << q.x <<
               ", " << q.y << ", " << q.z << ")]";
      return output;
    }
    void print_quat(void) const;
    Quaternion<T> conj(void);
    T norm2(void); //Squared norm
    T norm(void);
    Quaternion<T> inverse(void);
    Quaternion<T> normalize(void);
    void norm_inplace(void);

    template<typename V>
    void quatConjugateVec(V *v);
    template<typename V>
    void quatConjugateVec(V *v1,V *v2,V *v3);//Overloaded
    template<typename V>
    void quatConjugateVecRef(V *v,V *ref);
    template<typename V>
    void quatConjugateVecRef(V *v,V ref1,V ref2,V ref3);
    template<typename V>
    void quatConjugateVec(V *v1,V *v2,V *v3,V *ref);//Overloaded
    //template<typename V>
    //void quatConjugateVec(V v1,V v2,V v3,V ref1,V ref2,V ref3);//Overloaded
    //Methods for setting quaternion components
    const Quaternion<T>& Set(T, T, T, T);
    const Quaternion<T>& W(T);
    const Quaternion<T>& fromAngleAxis(T angle,const T *axis);
    const Quaternion<T>& fromAngleAxis(T angle,T ax1,T ax2,T ax3);
    const Quaternion<T>& fromXYZrot(T w1, T w2, T w3);

    void get_AngleAxis(point& axis, T& theta);

    static Quaternion<T> unitRandom();
    static Quaternion<T> unitRandom(T mc_rot_step);
};

#include "quaternion.cpp"
#endif
