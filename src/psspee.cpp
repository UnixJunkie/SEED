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

using namespace std;

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

        else{
          std::cerr << "Two atoms are too close to each other" << std::endl;
          *VWEnEv_ps=*VWEnEv_ps+(1.e+12);
        }

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
  double dEdr;
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
          std::cerr << "Two atoms are too close to each other" << std::endl; 
          RFSqDi = 1.e-4; // capping
        }

        RFSqDi_p3 = RFSqDi * RFSqDi * RFSqDi;
        SumRad = ReVdWR[j] + FrVdWR[i];
        SumRad_p6 = SumRad * SumRad * SumRad * SumRad * SumRad * SumRad;

        dEdr = - 12.0 / RFSqDi * ReVdWE_sr[j] * FrVdWE_sr[i] * 
                SumRad_p6 * ((SumRad_p6/(RFSqDi_p3 * RFSqDi_p3)) - (1.0 / RFSqDi_p3));

        e_ij[1] = ReCoor[j][1] - RoSFCo[i][1]; 
        e_ij[2] = ReCoor[j][2] - RoSFCo[i][2]; 
        e_ij[3] = ReCoor[j][3] - RoSFCo[i][3];

        F_i[1] += dEdr * e_ij[1];
        F_i[2] += dEdr * e_ij[2];
        F_i[3] += dEdr * e_ij[3];

        // *VWEnEv_ps = *VWEnEv_ps + ReVdWE_sr[j] * FrVdWE_sr[i] *
        //                               SumRad_p6 * ((SumRad_p6 / (RFSqDi_p3 * RFSqDi_p3)) - (2 / RFSqDi_p3));
        // }

        //else
          //*VWEnEv_ps = *VWEnEv_ps + (1.e+12);
      }
    }

    FvdW[1] += F_i[1];
    FvdW[2] += F_i[2];
    FvdW[3] += F_i[3];

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

double calc_grms(double *F, double *T, double alpha_xyz, double alpha_rot){
  /* This function calculates the root mean squared gradient for vdW minimization */
  double grms;
  grms = sqrtf(F[1] * F[1] * alpha_xyz * alpha_xyz +
               F[2] * F[2] * alpha_xyz * alpha_xyz +
               F[3] * F[3] * alpha_xyz * alpha_xyz +
               T[1] * T[1] * alpha_rot * alpha_rot +
               T[2] * T[2] * alpha_rot * alpha_rot + 
               T[3] * T[3] * alpha_rot * alpha_rot);
  return grms;
}

void check_gradient_vdw(int FrAtNu, int ReAtNu, double *ReVdWE_sr, double *FrVdWE_sr,
                        double *ReVdWR, double *FrVdWR, double *FvdW, double *TvdW,
                        double **RoSFCo, double **ReCoor,
                        double *ReMinC,
                        double GrSiCu_en, int *CubNum_en, int ***CubFAI_en, int ***CubLAI_en,
                        int *CubLiA_en, int PsSpNC, int ***PsSphe,
                        double PsSpRa, int ReReNu, int *AtReprRes,
                        int *FiAtRes, int *LaAtRes, int *FrAtEl_nu, double *AtWei)
{
  /* This function checks the gradients */
  int i,j,k;
  double **SDFrRe_ps;
  double num_grad[6]; // numerical gradient
  double eps = 0.00001;
  double eps_rot = eps * 0.0174533; 
  double **eps_coords;
  double vdw_plus, vdw_minus;
  double COM[4];
  Quaternion<double> q_rb;

  eps_coords = zero_dmatrix(1,FrAtNu, 1, 3);
  copy_dmatrix(RoSFCo, eps_coords, 1, FrAtNu, 1, 3);
  SDFrRe_ps = zero_dmatrix(1, FrAtNu, 1, ReAtNu);

  // numerical forces:
  for (j=1; j <= 3; j++){
    // x + eps
    for (i=1; i <= FrAtNu; i++)
    {
      eps_coords[i][j] += eps;  
    }
    SqDisFrRe_ps_vdW(FrAtNu, eps_coords, ReCoor, ReMinC, GrSiCu_en,
                    CubNum_en, CubFAI_en, CubLAI_en, CubLiA_en,
                    PsSpNC, PsSphe, SDFrRe_ps, ReAtNu, PsSpRa,
                    ReReNu, AtReprRes, FiAtRes, LaAtRes);
    PsSpEE(FrAtNu, ReAtNu, ReVdWE_sr, FrVdWE_sr,
          ReVdWR, FrVdWR, &vdw_plus, SDFrRe_ps);
    // copy_dmatrix(RoSFCo, eps_coords, 1, FrAtNu, 1, 3);
    // x - eps
    for (i = 1; i <= FrAtNu; i++)
    {
      eps_coords[i][j] -= 2*eps;
    }
    SqDisFrRe_ps_vdW(FrAtNu, eps_coords, ReCoor, ReMinC, GrSiCu_en,
                    CubNum_en, CubFAI_en, CubLAI_en, CubLiA_en,
                    PsSpNC, PsSphe, SDFrRe_ps, ReAtNu, PsSpRa,
                    ReReNu, AtReprRes, FiAtRes, LaAtRes);
    PsSpEE(FrAtNu, ReAtNu, ReVdWE_sr, FrVdWE_sr,
          ReVdWR, FrVdWR, &vdw_minus, SDFrRe_ps);
    copy_dmatrix(RoSFCo, eps_coords, 1, FrAtNu, 1, 3);
    // numerical gradient:
    num_grad[j] = (vdw_plus - vdw_minus) / (2 * eps);
  }
  std::cout << "Grad check" << std::endl;
  std::cout << -FvdW[1] << " " << num_grad[1] << std::endl;
  std::cout << -FvdW[2] << " " << num_grad[2] << std::endl;
  std::cout << -FvdW[3] << " " << num_grad[3] << std::endl;
  //numerical torques:

  while (k<=3){
    vdw_plus = 0.0;
    vdw_minus = 0.0;
    CenterOfMass(COM, RoSFCo, FrAtNu, AtWei, FrAtEl_nu);
    if (k == 1)
    {
      q_rb.fromXYZrot(eps_rot, 0.0, 0.0);
    } 
    else if (k == 2)
    {
      q_rb.fromXYZrot(0.0, eps_rot, 0.0);
    } 
    else if (k == 3)
    {
      q_rb.fromXYZrot(0.0, 0.0, eps_rot);
    }
    for (i = 1; i <= FrAtNu; i++)
    {
      q_rb.quatConjugateVecRef(eps_coords[i], COM[1], COM[2], COM[3]);
    }
    SqDisFrRe_ps_vdW(FrAtNu, eps_coords, ReCoor, ReMinC, GrSiCu_en,
                     CubNum_en, CubFAI_en, CubLAI_en, CubLiA_en,
                     PsSpNC, PsSphe, SDFrRe_ps, ReAtNu, PsSpRa,
                     ReReNu, AtReprRes, FiAtRes, LaAtRes);
    PsSpEE(FrAtNu, ReAtNu, ReVdWE_sr, FrVdWE_sr,
           ReVdWR, FrVdWR, &vdw_plus, SDFrRe_ps);
    if (k == 1)
    {
      q_rb.fromXYZrot(-2 * eps_rot, 0.0, 0.0);
    }
    else if (k == 2)
    {
      q_rb.fromXYZrot(0.0, -2 * eps_rot, 0.0);
    }
    else if (k == 3)
    {
      q_rb.fromXYZrot(0.0, 0.0, -2 * eps_rot);
    }
    for (i = 1; i <= FrAtNu; i++)
    {
      q_rb.quatConjugateVecRef(eps_coords[i], COM[1], COM[2], COM[3]);
    }
    SqDisFrRe_ps_vdW(FrAtNu, eps_coords, ReCoor, ReMinC, GrSiCu_en,
                     CubNum_en, CubFAI_en, CubLAI_en, CubLiA_en,
                     PsSpNC, PsSphe, SDFrRe_ps, ReAtNu, PsSpRa,
                     ReReNu, AtReprRes, FiAtRes, LaAtRes);
    PsSpEE(FrAtNu, ReAtNu, ReVdWE_sr, FrVdWE_sr,
           ReVdWR, FrVdWR, &vdw_minus, SDFrRe_ps);
    copy_dmatrix(RoSFCo, eps_coords, 1, FrAtNu, 1, 3);
    // numerical gradient:
    num_grad[k+3] = (vdw_plus - vdw_minus) / (2 * eps_rot);
    k++;
  }
  std::cout << -TvdW[1] << " " << num_grad[4] << std::endl;
  std::cout << -TvdW[2] << " " << num_grad[5] << std::endl;
  std::cout << -TvdW[3] << " " << num_grad[6] << std::endl;

  free_dmatrix(eps_coords, 1, FrAtNu, 1, 3);
  free_dmatrix(SDFrRe_ps, 1, FrAtNu, 1, ReAtNu);
}


