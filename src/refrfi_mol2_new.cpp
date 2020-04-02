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
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include "nrutil.h"
#include "funct.h"

//#include <typeinfo>
#ifndef _STRLENGTH
#define _STRLENGTH 500
#endif

#ifdef ENABLE_MPI
  #include <mpi.h>
  #define MASTERRANK 0
#endif

#ifdef ENABLE_MPI
/* MPI wrapper to read and dispatch the molecules. */
int MPI_ReFrFi_mol2(std::istream *inStream, std::streampos *strPos, MPI_Request *rkreqs,
                    int *readies, bool *firsttime)
{
  int nrks,aone,mol2tag[3], whichready, mlen,azero, nextplease;
  MPI_Status mstatus;
  MPI_Status *mstati;
  int rk;
  std::string StrLin, concat; // tmp line
  std::vector<std::string> mpi_strs; // vector to be dispatched
  bool endoffile, molstart;
  char * mpi_mess;
  
  endoffile = false;
  molstart = false;
  // maysend = false;
  aone = 1;
  azero = 0;
  whichready = -1;
  nextplease = -1;
  mol2tag[0] = 197;
  mol2tag[1] = 198;
  mol2tag[2] = 199;
  MPI_Comm_size(MPI_COMM_WORLD,&nrks);
  inStream->seekg(*strPos, std::ios_base::beg); //Move the get position to current location
  
  while(true) {
    *strPos = inStream->tellg();
    std::getline(*inStream, StrLin);
    boost::trim(StrLin);
    
    if (inStream->eof()){
      std::cerr << "\n\tEnd of fragment library was reached!" << std::endl;
      endoffile = true;
      mpi_strs.push_back(StrLin);
      break;
    } else if (StrLin == "@<TRIPOS>MOLECULE" && molstart == false) {
      molstart = true;
    } else if (StrLin == "@<TRIPOS>MOLECULE" && molstart == true) {
      molstart = false;
      break;
    }
    
    mpi_strs.push_back(StrLin);
  }
  
    // concat vector string:
    concat = boost::algorithm::join(mpi_strs,"\n"); // concat with newline
    boost::algorithm::trim_all(concat);
    
    if (*firsttime){
      for (rk=1; rk < nrks; rk++){
        MPI_Irecv(&readies[rk-1], aone, MPI_INT, rk, mol2tag[0], MPI_COMM_WORLD, &rkreqs[rk-1]);
      }
      *firsttime = false;
    }
    MPI_Waitany(nrks-1, rkreqs, &whichready, &mstatus);
    nextplease = whichready + 1; // index of the handle! NOTE: handle i corresponds to process i+1!
    mlen = concat.length() + 1; // +1 needed accounts for '\0'
    MPI_Send(&mlen, aone, MPI_INT, nextplease, mol2tag[1], MPI_COMM_WORLD); // send length of the message
    mpi_mess = new char[mlen]; 
    strcpy(mpi_mess, concat.c_str());
    
    //std::cerr << "Sending to rank " << nextplease << " message " << mlen << " long. or " << strlen(mpi_mess) << "\n";
    //std::cerr << mpi_mess << std::endl; 
    
    MPI_Send(mpi_mess, mlen, MPI_CHAR, nextplease, mol2tag[2], MPI_COMM_WORLD);
    delete [] mpi_mess;
    
    // open new request for the one we just used up
    MPI_Irecv(&readies[nextplease-1], aone, MPI_INT, nextplease, 
              mol2tag[0], MPI_COMM_WORLD, &rkreqs[nextplease-1]);    
  
  if (!endoffile){
    return 0;
  } else { // end-of-file: close all requests.
    mstati = new MPI_Status[nrks-1];
    MPI_Waitall(nrks-1, rkreqs, mstati);
    
    for (rk=1; rk < nrks; rk++){
      MPI_Send(&azero, aone, MPI_INT, rk, mol2tag[1], MPI_COMM_WORLD);
    }
    delete [] mstati;
    
    return 1;
  }
}

int MPI_slave_ReFrFi_mol2(int *SkiFra,int *CurFraTot,char *FragNa,
                std::string & FragNa_str,int *FrAtNu,int *FrBdNu,
                char ***FrAtEl,double ***FrCoor,char ***FrAtTy,char ***FrSyAtTy,
                double **FrPaCh,
                int ***FrBdAr,char ***FrBdTy,char *FrSubN,char *FrSubC,
                int *FrCoNu, char ***SubNa, std::string &AlTySp)
{
  typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(" \t\n\r");

	char **FrAtEl_L,**FrAtTy_L,**FrBdTy_L, **SubNa_L, **FrSyAtTy_L;
  int i,j,**FrBdAr_L, AtCount, CuAtNu, amready, aone, mol2tag[3], mlen;
	bool AtNu_flag, visitsecs[5];
  bool isValid_AlTySec, OverlappingMol; // is the alternative atom type section valid?
  double **FrCoor_L,*FrPaCh_L;
  char *mpi_mess;
  std::vector<std::string> mpi_strs;
  MPI_Request myreq;
  MPI_Status mstatus;
  int insec; // which mol2 section you are in 
  int seclc; // section line count 
  int curm; 
  bool skipit;
  
  // initialize pointers to null:
  // TODO replace with nullptr (also in nrutil)
  FrCoor_L = NULL;
  FrAtEl_L = NULL;
  FrAtTy_L = NULL;
  FrSyAtTy_L = NULL;
  SubNa_L = NULL;
  FrPaCh_L = NULL;
  FrBdAr_L = NULL;
  FrBdTy_L = NULL;
  
  skipit = true;
  mlen = -1;
  mol2tag[0] = 197;
  mol2tag[1] = 198;
  mol2tag[2] = 199;
  aone = 1;
  amready = 1;
  curm = 0;
	std::string StrLin, firstToken;
	std::size_t found;
  for (i = 0; i < 5; i++){
    visitsecs[i] = false;
  }
  
  MPI_Isend(&amready, aone, MPI_INT, MASTERRANK, mol2tag[0], MPI_COMM_WORLD, &myreq);
  MPI_Wait(&myreq, &mstatus);
  MPI_Recv(&mlen, aone, MPI_INT, MASTERRANK, mol2tag[1], MPI_COMM_WORLD, &mstatus);
  
  if (mlen == 0){
    // eof reached
    return 1;
  }
  
  
  mpi_mess = new char[mlen];
  MPI_Recv(mpi_mess, mlen, MPI_CHAR, MASTERRANK, mol2tag[2], MPI_COMM_WORLD, &mstatus);
  
  // int mycount = -1; 
  // MPI_Get_count(&mstatus, MPI_CHAR, &mycount); // enable if want to double check the size of the message

  boost::split(mpi_strs, mpi_mess, boost::is_any_of("\n")); // split by newline
  delete [] mpi_mess;
  
	tokenizer tokens(mpi_strs[0], sep); //Initialize tokenizer
	tokenizer::const_iterator itItem; // = tokens.begin();
    
  insec = -1;
  for (std::vector<std::string>::iterator s = mpi_strs.begin(); s != mpi_strs.end(); ++s) {
    /* Look for the next molecule section */
    // std::cerr << *s << std::endl;
    if (s->empty() || boost::starts_with(*s,"#")){ continue; } // cycle comments
    if (insec == -1 && *s != "@<TRIPOS>MOLECULE") continue; // cycle until molecule start
    if ( (*s).at(0) == '@') {
      if (insec != 0 && insec != -1) {
        std::cerr << "Encountered a new section while still processing input. Skipping." << std::endl;
        skipit = true;
        break;
      } 
      if (*s == "@<TRIPOS>MOLECULE") {
        // std::cerr << "Enter the mol" << std::endl;
        insec = 1;
        seclc = 0;
        curm = 0;
        visitsecs[insec] = true;
        continue;
      } else if (*s == "@<TRIPOS>ATOM") {
        // std::cerr << "Enter the atom section with curm " << curm << std::endl;
        if (curm == 0) {
          continue;
        } else if (visitsecs[2]){
          continue;
        } else {
          if (!visitsecs[1]){
            std::cerr << "Error: Trying to read an atom section not referring to any molecule. Skipping." 
                      << std::endl;
            skipit = true;
            break;
          }
          insec = 2;
          visitsecs[insec] = true;
          continue;
        } 
      } else if (*s == "@<TRIPOS>BOND"){
        // std::cerr << "Enter the bond section with curm " << curm << std::endl;
        if (curm == 0){
          continue;
        } else if (visitsecs[3]){
          continue;
        } else {
          if (!visitsecs[2])
          {
            std::cerr << "Error: Missing atom section in molecule. Skipping."
                      << std::endl;
            skipit = true;
            break;
          }
          insec = 3;
          visitsecs[insec] = true;
          continue;
        }
      } else if (*s == "@<TRIPOS>ALT_TYPE") {
        // std::cerr << "Enter the alt_type section with curm " << curm << std::endl;
        if (curm == 0){
          continue;
        } else if (visitsecs[4]) {
          continue;
        } else {
          if (!visitsecs[3])
          {
            std::cerr << "Error: Missing bond section in molecule. Skipping."
                      << std::endl;
            skipit = true;
            break;
          }
          insec = 4;
          visitsecs[insec] = true;
          continue;
        }
      }
    }
    
    if (insec == 1){ // MOLECULE section 
      seclc++;
      if (seclc == 1) { // read name 
        FragNa_str = *s;
        strcpy(FragNa, FragNa_str.c_str());
      } else if (seclc == 2) { // read mol info
        std::vector<std::string> s_split;
        boost::split(s_split, *s, boost::is_any_of(" \t"), boost::token_compress_on);
        // std::cerr << "dim of string " << s_split.size() << std::endl;
        if (s_split.size() < 2){
          skipit = true;
          std::cerr << "Either number of atoms or number of bonds is not specified. Skipping." << std::endl;
          break;
        }
        if (std::all_of(s_split[0].begin(), s_split[0].end(), ::isdigit) && std::all_of(s_split[1].begin(), s_split[1].end(), ::isdigit)){
          (*FrAtNu) = boost::lexical_cast<int>(s_split[0]);
          // std::cerr << "number of atoms " << *FrAtNu << std::endl;
          (*FrBdNu) = boost::lexical_cast<int>(s_split[1]);
          // std::cerr << "number of bonds " << *FrBdNu << std::endl;
        } else {
            skipit = true;
            std::cerr << "Non numeric values specifying number of atoms/bonds. Skipping." << std::endl;
            break;
        }
        //std::stringstream(*s) >> (*FrAtNu) >> (*FrBdNu); //>> (*FrCoNu); // OLD VERSION
        *FrCoNu = 1;
        // zero the indicators and set molecule as valid 
        insec = 0;
        seclc = 0;
        curm = 1;
      }
      // rest of molecule section is ignored
    } else if (insec == 2) {
      if (seclc == 0) { // first time I allocate the structures:
        FrAtEl_L=cmatrix(1,*FrAtNu,1,5);
      	FrCoor_L=dmatrix(1,*FrAtNu,1,3);
      	FrSyAtTy_L=cmatrix(1,*FrAtNu,1,7);
        SubNa_L =cmatrix(1,*FrAtNu,1,10);/*can be simplified. same for all fragment atoms*/
      	FrPaCh_L=dvector(1,*FrAtNu);
      	*FrAtEl=FrAtEl_L;
      	*FrCoor=FrCoor_L;
      	*FrSyAtTy=FrSyAtTy_L;
      	*FrPaCh=FrPaCh_L;
        *SubNa = SubNa_L;
      }
      seclc++;  
      
      tokens.assign(*s, sep);
  		itItem = tokens.begin();
	  	++itItem;
      strcpy(&FrAtEl_L[seclc][1],(*itItem).c_str());
  		++itItem;
      FrCoor_L[seclc][1] = boost::lexical_cast<double> (*itItem);
      ++itItem;
      FrCoor_L[seclc][2] = boost::lexical_cast<double> (*itItem);
      ++itItem;
      FrCoor_L[seclc][3] = boost::lexical_cast<double> (*itItem);
      ++itItem;
      strcpy(&FrSyAtTy_L[seclc][1],(*itItem).c_str());
  		++itItem; ++itItem;
      strcpy(&SubNa_L[seclc][1],(*itItem).c_str());
      ++itItem;
  		FrPaCh_L[seclc] = boost::lexical_cast<double> (*itItem);
      
      if (seclc == *FrAtNu) {
        // Check for Overlapping atoms
        OverlappingMol = false;
        for (i = 1; i <= (*FrAtNu); i++)
        {
          for (j = i + 1; j <= (*FrAtNu); j++)
          {
            if (DistSq(FrCoor_L[i][1], FrCoor_L[i][2], FrCoor_L[i][3],
                       FrCoor_L[j][1], FrCoor_L[j][2], FrCoor_L[j][3]) < 0.04)
            {
              OverlappingMol = true;
              goto OverlapOut;
            }
          }
        }
        OverlapOut: ;
        if (OverlappingMol){
          std::cerr << "Fragment " << *CurFraTot
                    << " might have overlapping atoms. Skipping!\n"
                    << std::endl;
          skipit = true;
          break;
        }

        insec = 0;
        seclc = 0;
      }
    } else if (insec == 3){
      if (seclc == 0){
        FrBdAr_L=imatrix(1,*FrBdNu,1,2);
      	FrBdTy_L=cmatrix(1,*FrBdNu,1,4);
      	*FrBdAr=FrBdAr_L;
      	*FrBdTy=FrBdTy_L;
      }
      seclc++;
      tokens.assign(*s, sep);
		  itItem = tokens.begin();
		  ++itItem; // skip bond_id
		  FrBdAr_L[seclc][1] = boost::lexical_cast<int>(*itItem);
		  ++itItem;
		  FrBdAr_L[seclc][2] = boost::lexical_cast<int>(*itItem);
		  ++itItem;
      strcpy(&FrBdTy_L[seclc][1],(*itItem).c_str());
      
      if (seclc == *FrBdNu){
        insec = 0;
        seclc = 0;
      }
    } else if (insec == 4){
      if (seclc == 0){
        found = s->find("ALT_TYPE_SET");
  	    if (found == std::string::npos){
          std::cerr << "Alternative atom type set not found" << std::endl;
          skipit = true;
          break;
        } else {
          AlTySp = s->substr(0,(found-1));
          FrAtTy_L=cmatrix(1,*FrAtNu,1,7);
      	  *FrAtTy=FrAtTy_L;
        }
      } else {
        if (seclc == 1) {      
          tokens.assign(*s, sep);
          itItem = tokens.begin();
          isValid_AlTySec = true;
          firstToken = *(itItem);
      	  if (firstToken != AlTySp){
            std::cerr << "Names of alternative atom type set do not coincide for fragment "
                      << *CurFraTot
                      << ". Skipping!\n"
                      << std::endl;
            skipit = true;
            break;
      	   }
    	     ++itItem; // skip the alternative atom type set name
           AtCount = 0;
           AtNu_flag = false;
        } else if (seclc > 1){
          tokens.assign(*s, sep);
          itItem = tokens.begin();
          isValid_AlTySec = true;
        } 
    	  while (AtCount < *FrAtNu){
          if (itItem == tokens.end())
          {
            isValid_AlTySec = false;
            break;
          }
          // while (itItem != tokens.end()){
    		  if (*itItem != "\\"){
    			  if (!AtNu_flag){
    				  CuAtNu =  boost::lexical_cast<int>(*itItem); // Current atom number
    				  AtNu_flag = true;
    				  ++itItem;
    			  } else {
              strcpy(&FrAtTy_L[CuAtNu][1],(*itItem).c_str());
              ++AtCount;
              // std::cerr << "read alt data type " << CuAtNu << " type " << (*itItem).c_str() << " AtCount " << AtCount << std::endl;
              ++itItem;
    				  AtNu_flag = false;
            } 
    		  } else {
            break;
          }
          // if ((*itItem != "\\") && (itItem == tokens.end()))
          // {
          //   isValid_AlTySec = false;
          //   break;
          // }   
    	  }
        if (isValid_AlTySec){        
          if (AtCount == *FrAtNu)
          {
            skipit = false; // correct termination
            break;
          } 
        } else {  
          // either too few or too many alternative atom types
          std::cerr << "Either to few or too many alternative atom types. Skipping." 
                    << std::endl;
          skipit = true;
          break;
        }
      }
      seclc++;
    } // end of insec switch
  }
  
  if (skipit){
    (*SkiFra)++;
    (*CurFraTot)++;
    // clean-up memory:
    // check for NULL is taken care by function free_*
    release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                    SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                    *FrAtNu, *FrBdNu);
    // free_cmatrix(FrAtEl_L,1,*FrAtNu,1,5);
    // free_dmatrix(FrCoor_L,1,*FrAtNu,1,3);
    // free_cmatrix(FrSyAtTy_L,1,*FrAtNu,1,7);
    // free_dvector(FrPaCh_L,1,*FrAtNu);
    // free_cmatrix(SubNa_L,1,*FrAtNu,1,10);
    // free_imatrix(FrBdAr_L,1,*FrBdNu,1,2);
    // free_cmatrix(FrBdTy_L,1,*FrBdNu,1,4);
    // free_cmatrix(FrAtTy_L,1,*FrAtNu,1,7);
    return -1;
  } else {
    (*CurFraTot)++;
    return 0;
  }
}
#endif


/* The code for the mol2 reader was partially inspired by RDKIT http://www.rdkit.org/ */
/* 	We reimplemented this function in C++ to take advantage of the more advanced
	parsing functions of this language (in particular we make extensive use of boost/tokenizer)
*/

int ReFrFi_mol2(std::istream *inStream, std::streampos *strPos,
                int *SkiFra,int *CurFraTot,char *FragNa,
                std::string & FragNa_str,int *FrAtNu,int *FrBdNu,
                char ***FrAtEl,double ***FrCoor,char ***FrAtTy,char ***FrSyAtTy,
                double **FrPaCh,
                int ***FrBdAr,char ***FrBdTy,char *FrSubN,char *FrSubC,
                int *FrCoNu, char ***SubNa, std::string &AlTySp)
/* This function reads the file of the current fragment (CurFra) in the mol2
   format :
   inStream pointer to the input file stream
   strPos  position in the input stream	// can be cast to/from an int
   SkiFra  counter of skipped fragments
   FrFiNa  names of the files containing the fragments
   FragNa  name of the fragment
   FragNa_str  name of the fragment as C++ string
   FrAtNu  number of atoms in the fragment (for one conformation)
   FrBdNu  number of bonds in the fragment (for one conformation)
   FrAtEl  fragment atoms elements
   FrCoor  fragment coordinates
   FrAtTy  fragment atoms types
   FrPaCh  fragment partial charges
   FrBdAr  fragment bonds array
   FrBdTy  fragment bonds type
   FrSubN  fragment substructure name
   FrSubC  fragment substructure chain
   FrCoNu  number of conformations of the current fragment type
   FrAtNu_cn  number of atoms for all conformations of the current
              fragment type
   FrBdNu_cn  number of bonds for all conformations of the current
              fragment type

   AlTySp   alternative atom type specification */
{
	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(" \t\n\r");

	char **FrAtEl_L,**FrAtTy_L,**FrBdTy_L, **SubNa_L, **FrSyAtTy_L;
  int i,j,**FrBdAr_L,/*FrAtNu_cn,FrBdNu_cn,*/ AtCount, CuAtNu /*insec isValid*/;
	bool AtNu_flag, OverlappingMol; 
  // bool skipit, end_of_lib;
  double **FrCoor_L,*FrPaCh_L;

  /* skipit and end_of_lib are mainly necessary to release memory in case of skip/eof */
  // skipit = true;
  // end_of_lib = false;

  /* setting to NULL for safety */
  FrCoor_L = NULL;
  FrAtEl_L = NULL;
  FrAtTy_L = NULL;
  FrSyAtTy_L = NULL;
  SubNa_L = NULL;
  FrPaCh_L = NULL;
  FrBdAr_L = NULL;
  FrBdTy_L = NULL;

  std::string StrLin, /*AlTySp,*/ firstToken;
	std::size_t found;

  inStream->seekg(*strPos, std::ios_base::beg); //Move the get position to current location
  std::getline(*inStream, StrLin);
  boost::trim(StrLin);

	tokenizer tokens(StrLin,sep); //Initialize tokenizer
	tokenizer::const_iterator itItem = tokens.begin();

  //insec = 0;
  //isValid = 1; 	/* Did we find a valid molecule? */

  while (true){

    /* Look for the next molecule section */
	  while (!inStream->eof() && StrLin != "@<TRIPOS>MOLECULE"){
      std::getline(*inStream, StrLin);
      boost::trim(StrLin);
	  }

	  if (inStream->eof()){
		  std::cerr << "\n\tEnd of fragment library was reached!" << std::endl;
  		return 1;
	  } //else {
      //insec = 1; /* Entering molecule section */
    //}

    /* Read fragment name */
    std::getline(*inStream,FragNa_str);
	  boost::trim(FragNa_str);
	  strcpy(FragNa, FragNa_str.c_str()); 
    /* Read atom and bond number */
    std::getline(*inStream,StrLin);
	  std::stringstream(StrLin) >> (*FrAtNu) >> (*FrBdNu); //>> (*FrCoNu);
    /*if (*FrCoNu != 1){
      std::cerr << "WARNING! Number of substructures/conformations is " << *FrCoNu << " (!=1)" << std::endl;
      std::cerr << "This number is ignored and set to 1." << std::endl;
      *FrCoNu = 1;
    } Always put FrCoNu to 1 */
    *FrCoNu = 1; // hard-coded

	  /* Move to the @<TRIPOS>ATOM block */
	  while (!inStream->eof() && StrLin[0] != '@'){ // should use StrLin.c_str()? clangini
      std::getline(*inStream,StrLin);
	  }
	  if (inStream->eof()){
      std::cerr << "End of fragment library was reached before expected! Last fragment was skipped \n"
                << std::endl;
      return 1;
	  }

	  boost::trim(StrLin);
	  if (StrLin != "@<TRIPOS>ATOM"){
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "No @<TRIPOS>ATOM-tag found for fragment" << *CurFraTot
                << ". Skipping!\n"
                << std::endl;
      continue;
	  }
    /* <TRIPOS>ATOM-tag found */
	  FrAtEl_L=cmatrix(1,*FrAtNu,1,5);
  	FrCoor_L=dmatrix(1,*FrAtNu,1,3);
  	FrSyAtTy_L=cmatrix(1,*FrAtNu,1,7);
    SubNa_L =cmatrix(1,*FrAtNu,1,10);/*can be simplified. same for all fragment atoms*/
  	FrPaCh_L=dvector(1,*FrAtNu);
  	*FrAtEl=FrAtEl_L;
  	*FrCoor=FrCoor_L;
  	*FrSyAtTy=FrSyAtTy_L;
  	*FrPaCh=FrPaCh_L;
    *SubNa = SubNa_L;
  /* We read here also the coordinates */
    for (i=1; i<=(*FrAtNu); i++ ){
      std::getline(*inStream,StrLin);
      // std::cout << "atom line "<<i<<": "<< StrLin << std::endl;
		  tokens.assign(StrLin, sep);
  		itItem = tokens.begin();
	  	++itItem;
      //std::cout << "atom element "<<i<<": "<< *itItem << std::endl;
  		//FrAtEl_L[i][1] = (*itItem).c_str();
      strcpy(&FrAtEl_L[i][1],(*itItem).c_str());
  		++itItem;
      // std::cout << "X-coordinate "<<i<<": "<< *itItem << std::endl;
      FrCoor_L[i][1] = boost::lexical_cast<double> (*itItem);
      // std::cout << "X-coordinate "<<i<<": "<<FrCoor_L[i][1]<< std::endl;
      ++itItem;
      // std::cout << "Y-coordinate "<<i<<": "<< *itItem << std::endl;
      FrCoor_L[i][2] = boost::lexical_cast<double> (*itItem);
      // std::cout << "Y-coordinate "<<i<<": "<<FrCoor_L[i][2]<< std::endl;
      ++itItem;
      // std::cout << "Z-coordinate "<<i<<": "<< *itItem << std::endl;
      FrCoor_L[i][3] = boost::lexical_cast<double> (*itItem);
      // std::cout << "Z-coordinate "<<i<<": "<<FrCoor_L[i][3]<< std::endl;
      ++itItem;
      // std::cout << "Sybyl atom type "<<i<<": "<< *itItem << std::endl;
  		// FrSyAtTy_L[i][1] = (*itItem).c_str();
      strcpy(&FrSyAtTy_L[i][1],(*itItem).c_str());
  		++itItem; ++itItem;
      // std::cout << "Substructure name "<<i<<": "<< *itItem << std::endl;
      // SubNa_L[i][1] = (*itItem).c_str();
      strcpy(&SubNa_L[i][1],(*itItem).c_str());
      ++itItem;
      // std::cout << "Partial charge "<<i<<": "<< (*itItem) << "end" << std::endl;
      // std::cout << typeid(itItem).name() << "\n" << typeid(*itItem).name() << std::endl;
  		FrPaCh_L[i] = boost::lexical_cast<double> (*itItem);
      // std::cout << "Charge "<<i<<": "<<FrPaCh_L[i]<< std::endl;
	  }
    //std::cout << "Atom Block was read!" << std::endl; /* clangini */
    /* check there are no overlapping atoms */
    OverlappingMol = false;
    for (i=1; i<=(*FrAtNu); i++ ){
      for (j=i+1; j <= (*FrAtNu); j++){
        if (DistSq(FrCoor_L[i][1], FrCoor_L[i][2], FrCoor_L[i][3],
                   FrCoor_L[j][1], FrCoor_L[j][2], FrCoor_L[j][3]) < 0.04){
                    OverlappingMol = true;
                   }
      }
    }
    if (OverlappingMol) {
      std::cerr << "Fragment " << *CurFraTot
                << " might have overlapping atoms. Skipping!\n"
                << std::endl;
      // skipit = true;
      // break;
      (*SkiFra)++;
      (*CurFraTot)++;
      // /* Once we will implement the resizing this part will not be needed any more */
      // free_cmatrix(*FrAtEl, 1, *FrAtNu, 1, 5);
      // free_dmatrix(*FrCoor, 1, *FrAtNu, 1, 3);
      // free_cmatrix(*FrSyAtTy, 1, *FrAtNu, 1, 7);
      // free_dvector(*FrPaCh, 1, *FrAtNu);
      // free_cmatrix(*SubNa, 1, *FrAtNu, 1, 10);
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);
      continue;
    }

    /* Move to the @<TRIPOS>BOND block */
	  while (!inStream->eof() && StrLin[0] != '@'){
		  //StrLin = getline(inStream);
      std::getline(*inStream,StrLin);
	  }
	  if (inStream->eof()){
      // end_of_lib = true;
      // break;
      std::cerr << "End of fragment library was reached before expected! Last fragment was skipped!\n"
                << std::endl;
      // /* Once we will implement the resizing this part will not be needed any more */
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);
      // free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      // free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      // free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      // free_dvector(*FrPaCh,1,*FrAtNu);
      // free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      return 1;
  	}
	  boost::trim(StrLin);
	  if (StrLin != "@<TRIPOS>BOND"){
      std::cerr << "No @<TRIPOS>BOND-tag found for fragment " << *CurFraTot
                << ". Skipping!\n"
                << std::endl;
      // skipit = true;
      // break;
		  (*SkiFra)++;
      (*CurFraTot)++;
      // /* Once we will implement the resizing this part will not be needed any more */
		  // free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      // free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      // free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      // free_dvector(*FrPaCh,1,*FrAtNu);
      // free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);

      continue;
    }
    FrBdAr_L=imatrix(1,*FrBdNu,1,2);
  	FrBdTy_L=cmatrix(1,*FrBdNu,1,4);
  	*FrBdAr=FrBdAr_L;
  	*FrBdTy=FrBdTy_L;

	  for (i = 1; i <= *FrBdNu; i++) {
		  //StrLin = getline(isStream);
      std::getline(*inStream,StrLin);
      // std::cout << "bond line " << i << ": " << StrLin << std::endl;
      //tokenizer tokens(StrLin, sep);
		  tokens.assign(StrLin,sep);
		  //tokenizer::const_iterator itItem = tokens.begin();
		  itItem = tokens.begin();
		  ++itItem; // skip bond_id
		  FrBdAr_L[i][1] = boost::lexical_cast<int>(*itItem);
      // std::cout << "1st bond partner "<<i<<": "<<FrBdAr_L[i][1]<< std::endl;
		  ++itItem;
		  FrBdAr_L[i][2] = boost::lexical_cast<int>(*itItem);
      // std::cout << "2nd bond partner "<<i<<": "<<FrBdAr_L[i][2]<< std::endl;
		  ++itItem;
		  //FrBdTy_L[i][1] = (*itItem).c_str();
      strcpy(&FrBdTy_L[i][1],(*itItem).c_str());
	  }
    //std::cout << "Bond Block was read!" << std::endl; /* clangini */
	  /* We do not read the SUBSTRUCTURE section any more */
	  while (!inStream->eof() && StrLin[0] != '@'){
		  //StrLin = getline(inStream);
      std::getline(*inStream,StrLin);
	  }
    boost::trim(StrLin);
    if (StrLin == "@<TRIPOS>SUBSTRUCTURE"){// if find @<TRIPOS>SUBSTRUCTURE skip and go on
      std::getline(*inStream,StrLin);
      while (!inStream->eof() && StrLin[0] != '@'){
        std::getline(*inStream,StrLin);
  	  }
    }
	  if (inStream->eof()){
      // end_of_lib = true;
      // break;
      std::cerr << "End of fragment library was reached before expected! Last fragment was skipped!\n"
                << std::endl;
      // /* Once we will implement the resizing this part will not be needed any more */
		  // free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      // free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      // free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      // free_dvector(*FrPaCh,1,*FrAtNu);
      // free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      // free_imatrix(*FrBdAr,1,*FrBdNu,1,2);
      // free_cmatrix(*FrBdTy,1,*FrBdNu,1,4);
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);
      return 1;
	  }

	  boost::trim(StrLin);
	  if (StrLin != "@<TRIPOS>ALT_TYPE"){
      std::cerr << "No @<TRIPOS>ALT_TYPE-tag found for fragment " << *CurFraTot
                << ". Skipping!\n" << std::endl;
      // skipit = true;
      // break;

      (*SkiFra)++;
      (*CurFraTot)++;
		  // /* Once we will implement the resizing this part will not be needed any more */
		  // free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      // free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      // free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      // free_dvector(*FrPaCh,1,*FrAtNu);
      // free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      // free_imatrix(*FrBdAr,1,*FrBdNu,1,2);
      // free_cmatrix(*FrBdTy,1,*FrBdNu,1,4);
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);

      continue;
	  }
    /* Read the @<TRIPOS> ALT_TYPE section */
    //StrLin = getline(inStream);
    std::getline(*inStream,StrLin);
	  boost::trim(StrLin);
	  //boost::algorithm::to_upper(StrLin); //not needed
    /* std::transform(StrLin.begin(), StrLin.end(),StrLin.begin(), ::toupper); */
	  found = StrLin.find("ALT_TYPE_SET");
	  if (found == std::string::npos){
      std::cerr << "No standard ALT_TYPE_SET signature find for fragment " << *CurFraTot
                << "Skipping!\n"
                << std::endl;
      // skipit = true;
      // break;

      (*SkiFra)++;
      (*CurFraTot)++;
      // /* Once we will implement the resizing this part will not be needed any more */
		  // free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      // free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      // free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      // free_dvector(*FrPaCh,1,*FrAtNu);
      // free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      // free_imatrix(*FrBdAr,1,*FrBdNu,1,2);
      // free_cmatrix(*FrBdTy,1,*FrBdNu,1,4);
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);

      continue;
	  }
    AlTySp = StrLin.substr(0,(found-1)); // Save the ALT_TYPE_SET name (for example CHARMM)
    //std::cout <<"Alternative atom type specification is: "<<AlTySp<<std::endl;
	  //StrLin = getline(inStream);
    std::getline(*inStream,StrLin);
	  tokens.assign(StrLin, sep);
    itItem = tokens.begin();
	  firstToken = *(itItem);
    //std::cout <<"Alternative atom type specification is: "<<firstToken<<std::endl;
	  //boost::algorithm::to_upper(firstToken); // not needed
	  if (firstToken != AlTySp){
      std::cerr << "Names of alternative atom type set do not coincide for fragment " << *CurFraTot
                << ". Skipping!\n"
                << std::endl;
      // skipit = true;
      // break;

      (*SkiFra)++;
      (*CurFraTot)++;
      //  /* Once we will implement the resizing this part will not be needed any more */
		  // free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      // free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      // free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      // free_dvector(*FrPaCh,1,*FrAtNu);
      // free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      // free_imatrix(*FrBdAr,1,*FrBdNu,1,2);
      // free_cmatrix(*FrBdTy,1,*FrBdNu,1,4);
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);

      continue;
	  }
    FrAtTy_L=cmatrix(1,*FrAtNu,1,7);
	  *FrAtTy=FrAtTy_L;
	  ++itItem; // skip the alternative atom type set name
	  AtCount = 0;
	  AtNu_flag = false;
	  while (itItem != tokens.end()){
		  if (*itItem != "\\"){
			  if (!AtNu_flag){
				  CuAtNu =  boost::lexical_cast<int>(*itItem); // Current atom number
				  AtNu_flag = true;
				  ++itItem;
			  } else {
				  //FrAtTy_L[CuAtNu][1] = (*itItem).c_str();
          //std::cout<<"Atom type for atom "<<AtCount+1<<": "<<(*itItem)<<std::endl;
          strcpy(&FrAtTy_L[CuAtNu][1],(*itItem).c_str());
          //std::cout<<"Atom type for atom "<<CuAtNu<<": "<<(*FrAtTy)[CuAtNu][1]<<std::endl;
          //std::cout<<"Atom type for atom "<<AtCount+1<<": "<<FrAtTy_L[CuAtNu][1]<<std::endl;
				  ++AtCount;
				  ++itItem;
				  AtNu_flag = false;
			  }
		  } else {
		    //StrLin = getline(inStream);
        std::getline(*inStream,StrLin);
			  //tokenizer tokens(StrLin, sep);
			  tokens.assign(StrLin, sep);
			  //tokenizer::const_iterator itItem = tokens.begin();
			  itItem = tokens.begin();
		  }
	  }
    if (AtCount < *FrAtNu){
      std::cerr << "List of alternative atom types for fragment " << *CurFraTot
                << " is not complete. "
                << "Skipping!\n"
                << std::endl;
      // skipit = true;
      // break;

      (*SkiFra)++;
      (*CurFraTot)++;
      // /* Once we will implement the resizing this part will not be needed any more */
      // free_cmatrix(*FrAtEl, 1, *FrAtNu, 1, 5);
      // free_dmatrix(*FrCoor, 1, *FrAtNu, 1, 3);
      // free_cmatrix(*FrSyAtTy, 1, *FrAtNu, 1, 7);
      // free_dvector(*FrPaCh, 1, *FrAtNu);
      // free_cmatrix(*SubNa, 1, *FrAtNu, 1, 10);
      // free_imatrix(*FrBdAr, 1, *FrBdNu, 1, 2);
      // free_cmatrix(*FrBdTy, 1, *FrBdNu, 1, 4);
      // free_cmatrix(*FrAtTy, 1, *FrAtNu, 1, 7);
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);

      continue;
    }
    else if (AtCount > *FrAtNu){
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "List of alternative atom types for fragment " << *CurFraTot
                << " is too long. There might be duplicates. "
                << "Skipping!\n"
                << std::endl;
      // skipit = true;
      // break;
      // /* Once we will implement the resizing this part will not be needed any more */
      // free_cmatrix(*FrAtEl, 1, *FrAtNu, 1, 5);
      // free_dmatrix(*FrCoor, 1, *FrAtNu, 1, 3);
      // free_cmatrix(*FrSyAtTy, 1, *FrAtNu, 1, 7);
      // free_dvector(*FrPaCh, 1, *FrAtNu);
      // free_cmatrix(*SubNa, 1, *FrAtNu, 1, 10);
      // free_imatrix(*FrBdAr, 1, *FrBdNu, 1, 2);
      // free_cmatrix(*FrBdTy, 1, *FrBdNu, 1, 4);
      // free_cmatrix(*FrAtTy, 1, *FrAtNu, 1, 7);
      release_mol_mem(FrAtEl_L, FrCoor_L, FrSyAtTy_L, FrPaCh_L,
                      SubNa_L, FrBdAr_L, FrBdTy_L, FrAtTy_L,
                      *FrAtNu, *FrBdNu);

      continue;
    }
    //StrLin = getline(inStream); /* This has to be checked! */
	  *strPos = inStream->tellg(); // Update the stream position indicator
    (*CurFraTot)++;
    //(*CurFra)++;
    return 0;
  } // end of while(true)
}

void release_mol_mem(char **FrAtEl, double **FrCoor, char **FrSyAtTy, double *FrPaCh,
                     char **SubNa, int **FrBdAr, char **FrBdTy, char **FrAtTy, 
                     int FrAtNu, int FrBdNu)
{
  /* This function releases the memory allocated for 
     a molecule (read from mol2). 
     Check for NULL is taken care by function free_* */
  free_cmatrix(FrAtEl, 1, FrAtNu, 1, 5);
  free_dmatrix(FrCoor, 1, FrAtNu, 1, 3);
  free_cmatrix(FrSyAtTy, 1, FrAtNu, 1, 7);
  free_dvector(FrPaCh, 1, FrAtNu);
  free_cmatrix(SubNa, 1, FrAtNu, 1, 10);
  free_imatrix(FrBdAr, 1, FrBdNu, 1, 2);
  free_cmatrix(FrBdTy, 1, FrBdNu, 1, 4);
  free_cmatrix(FrAtTy, 1, FrAtNu, 1, 7);
  
  return;
}
