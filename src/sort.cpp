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
#include <stdio.h>
#include <math.h>
#include "funct.h"
#include <float.h> /*DEBUGCHECK*/ //clangini

void Sort(int N,int *IndArr,double *SorArr)
/* This function sorts an array of doubles SorArr and gives also the new order
   in the array IndArr. The result goes from the smallest to the largest. */
{
  int m,k,i,j,l,nn,tempin,loopva;
  double aln2i,lognb2,temp;

  aln2i=1.0/0.69314718;
  lognb2= (double) ((int) (logf(N)*aln2i+5.e-7));

  m=N;
  for (nn=1;nn<=lognb2;nn++) {
    m=m/2;
    k=N-m;
    for (j=1;j<=k;j++) {
      i=j;
      loopva=1;
      while (loopva) {
        loopva=0;
        l=i+m;

/* 	//DEBUG CHECK */
	if(std::isnan(SorArr[i]) || SorArr[i] != SorArr[i] )
	{
	    SorArr[i]=FLT_MAX;
	    printf("NAN sort() 1 : %f\n",SorArr[i]);
	}
	if(SorArr[l]!=SorArr[l] || std::isnan(SorArr[l]))
	{
	    SorArr[l]=FLT_MAX;
	    printf("NAN sort() 2 : %f\n",SorArr[l]);
	}
/* 	if(std::isnan(IndArr[i]) || IndArr[i] != IndArr[i] ) */
/* 	{ */
/* 	    printf("NAN sort() 3 : %lf\n",IndArr[i]); */
/* 	    IndArr[i]=FLT_MAX; */
/* 	} */
/* 	if(IndArr[l]!=IndArr[l] || std::isnan(IndArr[l])) */
/* 	{ */
/* 	    printf("NAN sort() 4 : %lf\n",IndArr[l]); */
/* 	    IndArr[l]=FLT_MAX;	     */
/* 	} */

        if (SorArr[l]<SorArr[i]) {
          temp=SorArr[i];
          SorArr[i]=SorArr[l];
          SorArr[l]=temp;
          tempin=IndArr[i];
          IndArr[i]=IndArr[l];
          IndArr[l]=tempin;
          i=i-m;
          if (i>=1)
            loopva=1;
        }
      }
    }
  }

}
