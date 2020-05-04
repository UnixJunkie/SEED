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
#include <funct.h>
#include <nrutil.h>

void PsSpEE(int FrAtNu,int ReAtNu,double *ReVdWE_sr,double *FrVdWE_sr,
            double *ReVdWR,double *FrVdWR,double *VWEnEv_ps,double **SDFrRe_ps)
/* This function evaluates the energy using a pseudo-sphere approach as a
   cutoff :
   RFSqDi  squared distance between one atom of the receptor and one atom
           of the fragment
   SumRad  sum of the van der Waals radii */
{
  int i,j;
  double SumRad,SumRad_p6,RFSqDi,RFSqDi_p3;

  *VWEnEv_ps=0;

  for (i=1;i<=FrAtNu;i++) {

    for (j=1;j<=ReAtNu;j++) {

      RFSqDi=SDFrRe_ps[i][j];

      if (RFSqDi>=0.0) {

        if (RFSqDi>(1.e-4)) { // is it necessary? maybe to avoid overflow. clangini.

          RFSqDi_p3=RFSqDi*RFSqDi*RFSqDi;
          SumRad=ReVdWR[j]+FrVdWR[i];
          SumRad_p6=SumRad*SumRad*SumRad*SumRad*SumRad*SumRad;

          *VWEnEv_ps=*VWEnEv_ps+ReVdWE_sr[j]*FrVdWE_sr[i]*
                     SumRad_p6*((SumRad_p6/(RFSqDi_p3*RFSqDi_p3))-
                     (2/RFSqDi_p3));
        }

        else
          *VWEnEv_ps=*VWEnEv_ps+(1.e+12);

      }

    }

  }

}

void PsSpFE(int FrAtNu, int ReAtNu, double *ReVdWE_sr, double *FrVdWE_sr,
            double *ReVdWR, double *FrVdWR, double *FvdW, double *TvdW, 
            double *maxFvdW, double *maxTvdW, double **SDFrRe_ps, 
            double **RoSFCo, double **ReCoor, double **RelCOMCo)
/* This function evaluates the vdw forces using a pseudo-sphere approach as a
   cutoff :
   RFSqDi  squared distance between one atom of the receptor and one atom
           of the fragment. -1.0 if the atoms are too far (out of pseudo-sphere).
   SumRad  sum of the van der Waals radii */
{
  int i, j;
  double SumRad, SumRad_p6, RFSqDi, RFSqDi_p3;
  double dEdr, RFDi;
  double e_ij[4]; // unit vector from i to j
  double F_i[4]; // force on atom i
  double T_i[4]; // torque on atom i

  // *VWEnEv_ps = 0.0;
  FvdW[1] = 0.0;
  FvdW[2] = 0.0;
  FvdW[3] = 0.0;
  TvdW[1] = 0.0;
  TvdW[2] = 0.0;
  TvdW[3] = 0.0;
  *maxFvdW = 0.0;
  *maxTvdW = 0.0;

  for (i = 1; i <= FrAtNu; i++)
  {
    F_i[1] = 0.0;
    F_i[2] = 0.0;
    F_i[3] = 0.0;

    for (j = 1; j <= ReAtNu; j++)
    {

      dEdr = 0.0; // module of the force
      RFSqDi = SDFrRe_ps[i][j];
      // RFDi = sqrt(RFSqDi);

      if (RFSqDi >= 0.0)
      {

        if (RFSqDi < (1.e-4))
        { // is it necessary? maybe to avoid overflow. clangini.
          RFSqDi = 1.e-4; // capping
        }

        RFSqDi_p3 = RFSqDi * RFSqDi * RFSqDi;
        SumRad = ReVdWR[j] + FrVdWR[i];
        SumRad_p6 = SumRad * SumRad * SumRad * SumRad * SumRad * SumRad;

        dEdr = - 12.0 / RFSqDi * ReVdWE_sr[j] * FrVdWE_sr[i] * 
                SumRad_p6 * ((SumRad_p6/(RFSqDi_p3 * RFSqDi_p3) - (1.0 / RFSqDi_p3)));

        e_ij[1] = ReCoor[j][1] - RoSFCo[i][1]; 
        e_ij[2] = ReCoor[j][2] - RoSFCo[i][2]; 
        e_ij[3] = ReCoor[j][3] - RoSFCo[i][3];

        F_i[1] = F_i[1] + dEdr * e_ij[1];
        F_i[2] = F_i[2] + dEdr * e_ij[2];
        F_i[3] = F_i[3] + dEdr * e_ij[3];

        // *VWEnEv_ps = *VWEnEv_ps + ReVdWE_sr[j] * FrVdWE_sr[i] *
        //                               SumRad_p6 * ((SumRad_p6 / (RFSqDi_p3 * RFSqDi_p3)) - (2 / RFSqDi_p3));
        // }

        //else
          //*VWEnEv_ps = *VWEnEv_ps + (1.e+12);
      }
    }

    FvdW[1] = FvdW[1] + F_i[1];
    FvdW[2] = FvdW[2] + F_i[2];
    FvdW[3] = FvdW[3] + F_i[3];

    VectPr(RelCOMCo[i][1], RelCOMCo[i][2], RelCOMCo[i][3],
           F_i[1], F_i[2], F_i[3],
           &T_i[1], &T_i[2], &T_i[3]);
    TvdW[1] = TvdW[1] + T_i[1];
    TvdW[2] = TvdW[2] + T_i[2];
    TvdW[3] = TvdW[3] + T_i[3];
  }

  *maxFvdW = max_abs_el(FvdW, 1, 3);
  *maxTvdW = max_abs_el(TvdW, 1, 3);

  return;
}
