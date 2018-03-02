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
#include "nrutil.h"

int FragSolvEn_Rbound(int FrAtNu,double **FrCoor,double *FrPaCh,
               double *FrVdWR, double *FrEffRad_bound, double *FrRadOut,
               double *FrRadOut2,double **Frdist2,int Nsurfpt_fr,
               struct point *surfpt_fr_orig,double WaMoRa,double GrSiSo,
               double Ksolv,double pi4,double *PFrSolvEn,char *EmpCorrB,FILE*FPaOut)
/*##########################################
Calculate the solvation energy of the isolated frag
according to the GB formula.
This is the same as FragSolvEn(...) but it determines the lower boundaries
on the Born radii for each atom.
###########################################*/

/*##########################################
int FrAtNu -------------- Tot # frag atoms
double **FrCoor ---------- Frag coordinates in the original location
double *FrPaCh ----------- Frag partial charges
double *FrVdWR ----------- Frag vdW radii
double *FrRadOut -------- Frag charge radii + WaMoRa
double *FrRadOut2 ------- (Frag charge radii + WaMoRa)^2
double *FrEffRad_bound --- lower bound on effective Born radius
double **Frdist2 --------- Squared interatomic frag distances
int Nsurfpt_fr ---------- Tot # of points over the frag SAS1
struct point *surfpt_fr_orig - Coor of points over frag SAS1 (obtained from
                               FrCoor coordinates)
double WaMoRa ----------- Radius of the water molecule
double GrSiSo ----------- Size of the 3D grid used for cont. electrostatics
double Ksolv ------------ Constant
double pi4 -------------- 4 * greekpi
double *PFrSolvEn ------- Frag solvation energy
###########################################*/
{
  int iat,ix,iy,iz,nn,NGridx,NGridy,NGridz,NGrid;
  double *FrSelfVol,*FrEffRad,FrSelfEn,FrIntEn,UnitVol,*XGrid,*YGrid,*ZGrid,
         *FrSelfVol_corrB;
  struct point Min,Max;
  char ***FrGridMat;
/*##########################################
int iat,ix,iy,iz,nn ----- Multipurpose variables
int NGridx -------------- Tot # of frag grid points along x
int NGridy -------------- Tot # of frag grid points along y
int NGridz -------------- Tot # of frag grid points along z
double *FrSelfVol ------- FrSelfVol[iat] = integral of 1/r^4 over the frag
                          volume for atom iat
double *FrEffRad -------- FrEffRad[n] = effective radius of frag atom n
double FrSelfEn --------- Total self energy of the isolated frag
double FrIntEn ---------- Total intramolecular interaction energy
                          of the isolated frag
double UnitVol ---------- Volume of the grid element for cont. elec.
double *XGrid ----------- X coor of the grid points
double *YGrid ----------- Y coor of the grid points
double *ZGrid ----------- Z coor of the grid points
struct point Min -------- Min coor of the frag grid box
struct point Max -------- Max coor of the frag grid box
char ***FrGridMat ------- Matrix telling if a grid point is occupied by the
                          frag (o), empty (e), or if it belongs to the interface
                          between SAS and MS (s)
###########################################*/

  /* Get the # of grid pts needed to cover the frag molecule along the
     three axis (no grid increase) */
  nn = get_Grid_Dim(FrAtNu,FrCoor,FrVdWR,WaMoRa,GrSiSo,0.,&NGridx,&NGridy,
                    &NGridz,&NGrid,&Min,&Max,&UnitVol);

  XGrid=dvector(1,NGridx);
  YGrid=dvector(1,NGridy);
  ZGrid=dvector(1,NGridz);
  /* Get the cartesian coor for the grid points */
  nn = Coor_Grid_Pts(GrSiSo,NGridx,NGridy,NGridz,Min,XGrid,YGrid,ZGrid);

  FrSelfVol=dvector(1,FrAtNu);
  FrSelfVol_corrB=dvector(1,FrAtNu);
  FrEffRad=dvector(1,FrAtNu);
  FrGridMat = c3tensor(1,NGridx+1,1,NGridy+1,1,NGridz+1);
  for (ix=1;ix<=NGridx+1;ix++)
    for (iy=1;iy<=NGridy+1;iy++)
      for (iz=1;iz<=NGridz+1;iz++)
        FrGridMat[ix][iy][iz] = 'e';

/* Make the map (FrGridMat) of the 3D grid points occupied by the volume
   enclosed by the frag SAS  */
  nn = SAS_Volume(FrAtNu,FrCoor,FrRadOut,FrRadOut2,Min,GrSiSo,
                  1,1,1,NGridx,NGridy,NGridz,FrGridMat);
  for (iat=1;iat<=FrAtNu;iat++)
  {
    FrSelfVol[iat] = 0.;
    FrSelfVol_corrB[iat] = 0.;
  }
/* Place a sphere of radius WaMoRa on every SAS surface
   grid point (*surfpt_fr_orig) and mark as empty all the volume grid
   points falling inside the sphere */
  nn = Excl_Grid(FrAtNu,FrCoor,Min,FrRadOut,FrRadOut2,WaMoRa,GrSiSo,
                 1,1,1,NGridx,NGridy,NGridz,UnitVol,
                 FrGridMat,Nsurfpt_fr,surfpt_fr_orig,FrSelfVol,FrSelfVol_corrB,
		             EmpCorrB);
  for (int jj=1;jj<=FrAtNu;jj++){
    std::cout << "FrSelfVol_corrB[" << jj << "]= " << FrSelfVol_corrB[jj] << '\n';
    std::cout << "FrSelfVol[" << jj << "]= " << FrSelfVol[jj] << '\n';
  }
/* Calculate the frag self energy */
  nn = Get_Self_En_Fr(FrAtNu,FrCoor,FrPaCh,FrRadOut,FrRadOut2,
                      XGrid,YGrid,ZGrid,UnitVol,Ksolv,pi4,FrGridMat,
                      1,1,1,NGridx,NGridy,NGridz,FrSelfVol,FrEffRad,&FrSelfEn,
		                  FrSelfVol_corrB,EmpCorrB,FPaOut);
  /* Set the calculated FrEffRad as a lower bound of Effective Born radius */
  copy_dvector(FrEffRad, FrEffRad_bound, 1, FrAtNu);

  /*for (int jj=1;jj<=FrAtNu;jj++){
    std::cout << "FrSelfVol_corrB[" << jj << "]= " << FrSelfVol_corrB[jj] << '\n';
    std::cout << "FrSelfVol[" << jj << "]= " << FrSelfVol[jj] << '\n';
  }*/

/* Calculate the frag interaction energy */
  nn = GB_int_fr(FrAtNu,Frdist2,FrPaCh,FrEffRad,Ksolv,&FrIntEn);

  *PFrSolvEn = FrSelfEn + FrIntEn;
/*  printf("%lf\n",*PFrSolvEn); */

/* Free memory */
  free_dvector(XGrid,1,NGridx);
  free_dvector(YGrid,1,NGridy);
  free_dvector(ZGrid,1,NGridz);
  free_dvector(FrEffRad,1,FrAtNu);
  free_dvector(FrSelfVol,1,FrAtNu);
  free_c3tensor(FrGridMat,1,NGridx+1,1,NGridy+1,1,NGridz+1);
  free_dvector(FrSelfVol_corrB,1,FrAtNu);

  if ( iat == FrAtNu+1 )
    return 1;
  else
    return 0;
}
