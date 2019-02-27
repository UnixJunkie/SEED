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
#include <boost/lexical_cast.hpp>
#include "nrutil.h"
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
  
  std::cerr << "MPI read main " << std::endl;
  
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
  
  if (!endoffile){
    // concat vector string:
    concat = boost::algorithm::join(mpi_strs,"\n");
    boost::replace_all(concat, "\0", "");
    boost::algorithm::trim_all(concat);
    
    if (*firsttime){
      std::cerr << "First call Master" << std::endl;
      for (rk=1; rk < nrks; rk++){
        MPI_Irecv(&readies[rk-1], aone, MPI_INT, rk, mol2tag[0], MPI_COMM_WORLD, &rkreqs[rk-1]);
      }
      *firsttime = false;
    }
    MPI_Waitany(nrks-1, rkreqs, &whichready, &mstatus);
    nextplease = whichready; // index of the handle
    mlen = concat.length() + 1; // +1 needed accounts for '\0'
    MPI_Send(&mlen, aone, MPI_INT, nextplease+1, mol2tag[1], MPI_COMM_WORLD); // send length of the message
    mpi_mess = new char[mlen]; 
    strcpy(mpi_mess, concat.c_str()); 
    
    // if (mpi_mess[mlen] == '\0'){
    //   std::cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
    // }
    
    //std::cerr << "Sending to rank " << nextplease+1 << " message " << mlen << " long. or " << strlen(mpi_mess) << "\n";
    //std::cerr << mpi_mess << std::endl; 
    
    MPI_Send(mpi_mess, mlen, MPI_CHAR, nextplease+1, mol2tag[2], MPI_COMM_WORLD);
    delete [] mpi_mess;
    // open new request for the one we just used up
    MPI_Irecv(&readies[nextplease], aone, MPI_INT, nextplease+1, mol2tag[0], MPI_COMM_WORLD, &rkreqs[nextplease]);
    
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
  boost::char_separator<char> sep(" \t\n");

	char **FrAtEl_L,**FrAtTy_L,**FrBdTy_L, **SubNa_L, **FrSyAtTy_L;
  int i,**FrBdAr_L, AtCount, CuAtNu, amready, aone, mol2tag[3], mlen;
	bool AtNu_flag, visitsecs[5];
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
  FrAtEl_L = NULL;
  FrAtTy_L = NULL;
  FrBdTy_L = NULL;
  SubNa_L = NULL;
  FrSyAtTy_L = NULL;
  FrBdAr_L = NULL;
  FrCoor_L = NULL;
  FrPaCh_L = NULL;
  
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
  
  std::cerr << "MPI read slave " << std::endl;
  MPI_Isend(&amready, aone, MPI_INT, MASTERRANK, mol2tag[0], MPI_COMM_WORLD, &myreq);
  MPI_Wait(&myreq, &mstatus);
  MPI_Recv(&mlen, aone, MPI_INT, MASTERRANK, mol2tag[1], MPI_COMM_WORLD, &mstatus);
  
  if (mlen == 0){
    // eof reached
    return 1;
  }
  
  
  mpi_mess = new char[mlen];
  MPI_Recv(mpi_mess, mlen, MPI_CHAR, MASTERRANK, mol2tag[2], MPI_COMM_WORLD, &mstatus);
  
  int mycount = -1;
  MPI_Get_count(&mstatus, MPI_CHAR, &mycount);
  
  //std::cerr << "Receiving message from " << MASTERRANK << " message " << mlen << " long.\n";
  //std::cerr << mpi_mess << std::endl; 
  // std::cerr << "ggggggggg " << mlen << "   " << mycount << "   " << strlen(mpi_mess) << " gggggggggggggggg"<< std::endl;
  // if (mpi_mess[strlen(mpi_mess)] == '\0'){
  //   std::cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
  // }
  
  boost::split(mpi_strs, mpi_mess, boost::is_any_of("\n"));
  delete [] mpi_mess;
  
  // for(auto const& ll: mpi_strs) {
  //    std::cerr << ll << std::endl;
  // }
  if (mpi_strs[0] == "@<TRIPOS>MOLECULE"){
    std::cerr << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
  }
  //return -1;
  
	tokenizer tokens(mpi_strs[0], sep); //Initialize tokenizer
	tokenizer::const_iterator itItem; // = tokens.begin();
    
  insec = -1;
  for (std::vector<std::string>::iterator s = mpi_strs.begin(); s != mpi_strs.end(); ++s) {
    std::cerr << "looping" << std::endl;
    /* Look for the next molecule section */
    if (insec == -1 && *s != "@<TRIPOS>MOLECULE") continue; // cycle until molecule start
    if ( (*s).at(0) == '@') {
      if (insec != 0 && insec != -1) {
        std::cerr << "Encountered a new section while still processing input. \
                      Skipping." << std::endl;
        //insec = -1;
        skipit = true;
        break;
      } 
      if (*s == "@<TRIPOS>MOLECULE") {
        std::cerr << "Enter the mol" << std::endl;
        insec = 1;
        seclc = 0;
        curm = 0;
        visitsecs[insec] = true;
        continue;
      } else if (*s == "@<TRIPOS>ATOM") {
        std::cerr << "Enter the atom section with curm " << curm << std::endl;
        if (curm == 0) {
          continue;
        } else if (visitsecs[2]){
          continue;
        } else {
          insec = 2;
          visitsecs[insec] = true;
          continue;
        } 
      } else if (*s == "@<TRIPOS>BOND"){
        std::cerr << "Enter the bond section with curm " << curm << std::endl;
        if (curm == 0){
          continue;
        } else if (visitsecs[3]){
          continue;
        } else {
          insec = 3;
          visitsecs[insec] = true;
          continue;
        }
      } else if (*s == "@<TRIPOS>ALT_TYPE") {
        std::cerr << "Enter the alt_type section with curm " << curm << std::endl;
        if (curm == 0){
          continue;
        } else if (visitsecs[4]) {
          continue;
        } else {
          insec = 4;
          //seclc = 0;
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
        std::stringstream(*s) >> (*FrAtNu) >> (*FrBdNu); //>> (*FrCoNu);
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
        std::cerr << *s << std::endl;
        found = s->find("ALT_TYPE_SET");
  	    if (found == std::string::npos){
          std::cerr << "Alternative atom type set not found" << std::endl;
          skipit = true;
          break;
        } else {
          std::cerr << "Alternative atom type set FOUND" << std::endl;
          AlTySp = s->substr(0,(found-1));
          FrAtTy_L=cmatrix(1,*FrAtNu,1,7);
      	  *FrAtTy=FrAtTy_L;
        }
      } else {
        if (seclc == 1) {      
          tokens.assign(*s, sep);
          itItem = tokens.begin();
    	    firstToken = *(itItem);
      	  if (firstToken != AlTySp){
      		  std::cerr << "Names of alternative atom type set do not coincide for fragment " << *CurFraTot
                      << ". Skipping!\n";
            skipit = true;
            break;
      	   }
    	     ++itItem; // skip the alternative atom type set name
           AtCount = 0;
           AtNu_flag = false;
        } else if (seclc > 1){
          tokens.assign(*s, sep);
          itItem = tokens.begin();
        } 
    	  while (AtCount < *FrAtNu){
    		  if (*itItem != "\\"){
    			  if (!AtNu_flag){
    				  CuAtNu =  boost::lexical_cast<int>(*itItem); // Current atom number
    				  AtNu_flag = true;
    				  ++itItem;
    			  } else {
              strcpy(&FrAtTy_L[CuAtNu][1],(*itItem).c_str());
    				  ++AtCount;
              std::cerr << "read alt data type " << CuAtNu << " type " << (*itItem).c_str() << " AtCount " << AtCount << std::endl;
    				  ++itItem;
    				  AtNu_flag = false;
    			  }
    		  } else {
            break;
          }   
    	  }
        if (AtCount == *FrAtNu){
          skipit = false; // correct termination
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
    free_cmatrix(FrAtEl_L,1,*FrAtNu,1,5);
    free_dmatrix(FrCoor_L,1,*FrAtNu,1,3);
    free_cmatrix(FrSyAtTy_L,1,*FrAtNu,1,7);
    free_dvector(FrPaCh_L,1,*FrAtNu);
    free_cmatrix(SubNa_L,1,*FrAtNu,1,10);
    free_imatrix(FrBdAr_L,1,*FrBdNu,1,2);
    free_cmatrix(FrBdTy_L,1,*FrBdNu,1,4);
    free_cmatrix(FrAtTy_L,1,*FrAtNu,1,7);
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
	//boost::char_separator<char> sep={" \t\n"};
  boost::char_separator<char> sep(" \t\n");

	char **FrAtEl_L,**FrAtTy_L,**FrBdTy_L, **SubNa_L, **FrSyAtTy_L;
  int i,**FrBdAr_L,/*FrAtNu_cn,FrBdNu_cn,*/ AtCount, CuAtNu /*insec isValid*/;
	bool AtNu_flag;
  double **FrCoor_L,*FrPaCh_L;

	std::string StrLin, /*AlTySp,*/ firstToken;
	std::size_t found;

  inStream->seekg(*strPos, std::ios_base::beg); //Move the get position to current location
  std::getline(*inStream, StrLin);
  boost::trim(StrLin);

  //std::cout << "First read line is: " << StrLin << std::endl; /* clangini OK */

	tokenizer tokens(StrLin,sep); //Initialize tokenizer
	tokenizer::const_iterator itItem = tokens.begin();

  //insec = 0;
  //isValid = 1; 	/* Did we find a valid molecule? */

  while (true){

    /* Look for the next molecule section */
	  while (!inStream->eof() && StrLin != "@<TRIPOS>MOLECULE"){
		  //StrLin = std::getline(*inStream);
      std::getline(*inStream, StrLin);
      boost::trim(StrLin);
	  }
    //std::cout << "Out of while StrLin is: " << StrLin << std::endl; /* clangini OK */

	  if (inStream->eof()){
		  std::cerr << "\n\tEnd of fragment library was reached!" << std::endl;
  		return 1;
	  } //else {
      //insec = 1; /* Entering molecule section */
    //}

    //std::cout << "No eof reached" << std::endl; /* clangini */

    //StrLin = getline(inStream);
    std::getline(*inStream,FragNa_str);
	  boost::trim(FragNa_str);
	  strcpy(FragNa, FragNa_str.c_str()); /* Read fragment name */
    //std::cout << "FragNa is: " << FragNa << std::endl; /* clangini */
	  //StrLin = getline(inStream);     /* Read FrAtNu, FrBdNu, FrCoNu */
    std::getline(*inStream,StrLin);
	  std::stringstream(StrLin) >> (*FrAtNu) >> (*FrBdNu); //>> (*FrCoNu);
    /*if (*FrCoNu != 1){
      std::cerr << "WARNING! Number of substructures/conformations is " << *FrCoNu << " (!=1)" << std::endl;
      std::cerr << "This number is ignored and set to 1." << std::endl;
      *FrCoNu = 1;
    } Always put FrCoNu to 1 */
    *FrCoNu = 1;

	  /* Move to the @<TRIPOS>ATOM block */
	  while (!inStream->eof() && StrLin[0] != '@'){ // should use StrLin.c_str()? clangini
		  //StrLin = getline(inStream);
      std::getline(*inStream,StrLin);
	  }
	  if (inStream->eof()){
		  std::cerr << "End of fragment library was reached before expected! Last fragment was skipped \n";
		  return 1;
	  }

    //std::cout << "StrLin is: " << StrLin << std::endl; /* clangini */
	  boost::trim(StrLin);
	  if (StrLin != "@<TRIPOS>ATOM"){
		  std::cerr << "No @<TRIPOS>ATOM-tag found for fragment" << *CurFraTot
			    << ". Skipping!\n";
      (*SkiFra)++;
      (*CurFraTot)++;
		  continue;
	  }
    //std::cout << "TRIPOS ATOM tag was found" << std::endl; /* clangini */
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
    //std::cout << "Memory initialized" << std::endl; /* clangini */
  /* We read here also the coordinates */
    for (i=1; i<=(*FrAtNu); i++ ){
		  //StrLin = getline(inStream);
      std::getline(*inStream,StrLin);
      //std::cout << "atom line "<<i<<": "<< StrLin << std::endl;
		  tokens.assign(StrLin, sep);
  		itItem = tokens.begin();
	  	++itItem;
      //std::cout << "atom element "<<i<<": "<< *itItem << std::endl;
  		//FrAtEl_L[i][1] = (*itItem).c_str();
      strcpy(&FrAtEl_L[i][1],(*itItem).c_str());
  		++itItem;
      //std::cout << "X-coordinate "<<i<<": "<< *itItem << std::endl;
      FrCoor_L[i][1] = boost::lexical_cast<double> (*itItem);
      //std::cout << "X-coordinate "<<i<<": "<<FrCoor_L[i][1]<< std::endl;
      ++itItem;
      //std::cout << "Y-coordinate "<<i<<": "<< *itItem << std::endl;
      FrCoor_L[i][2] = boost::lexical_cast<double> (*itItem);
      //std::cout << "Y-coordinate "<<i<<": "<<FrCoor_L[i][2]<< std::endl;
      ++itItem;
      //std::cout << "Z-coordinate "<<i<<": "<< *itItem << std::endl;
      FrCoor_L[i][3] = boost::lexical_cast<double> (*itItem);
      //std::cout << "Z-coordinate "<<i<<": "<<FrCoor_L[i][3]<< std::endl;
      ++itItem;
      //std::cout << "Sybyl atom type "<<i<<": "<< *itItem << std::endl;
  		//FrSyAtTy_L[i][1] = (*itItem).c_str();
      strcpy(&FrSyAtTy_L[i][1],(*itItem).c_str());
  		++itItem; ++itItem;
      //std::cout << "Substructure name "<<i<<": "<< *itItem << std::endl;
      //SubNa_L[i][1] = (*itItem).c_str();
      strcpy(&SubNa_L[i][1],(*itItem).c_str());
      ++itItem;
      //std::cout << "Partial charge "<<i<<": "<< (*itItem)<<"ebbasta" << std::endl;
  		FrPaCh_L[i] = boost::lexical_cast<double> (*itItem);
      //std::cout << "Charge "<<i<<": "<<FrPaCh_L[i]<< std::endl;
	  }
    //std::cout << "Atom Block was read!" << std::endl; /* clangini */
	  /* Move to the @<TRIPOS>BOND block */
	  while (!inStream->eof() && StrLin[0] != '@'){
		  //StrLin = getline(inStream);
      std::getline(*inStream,StrLin);
	  }
	  if (inStream->eof()){
		  std::cerr << "End of fragment library was reached before expected! Last fragment was skipped!\n";
      /* Once we will implement the resizing this part will not be needed any more */
		  free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      free_dvector(*FrPaCh,1,*FrAtNu);
      free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      return 1;
  	}
	  boost::trim(StrLin);
	  if (StrLin != "@<TRIPOS>BOND"){
		  std::cerr << "No @<TRIPOS>BOND-tag found for fragment " << *CurFraTot
			    << ". Skipping!\n";
		  (*SkiFra)++;
      (*CurFraTot)++;
      /* Once we will implement the resizing this part will not be needed any more */
		  free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      free_dvector(*FrPaCh,1,*FrAtNu);
      free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      continue;
	  }
    FrBdAr_L=imatrix(1,*FrBdNu,1,2);
  	FrBdTy_L=cmatrix(1,*FrBdNu,1,4);
  	*FrBdAr=FrBdAr_L;
  	*FrBdTy=FrBdTy_L;

	  for (i = 1; i <= *FrBdNu; i++) {
		  //StrLin = getline(isStream);
      std::getline(*inStream,StrLin);
		  //tokenizer tokens(StrLin, sep);
		  tokens.assign(StrLin,sep);
		  //tokenizer::const_iterator itItem = tokens.begin();
		  itItem = tokens.begin();
		  ++itItem; // skip bond_id
		  FrBdAr_L[i][1] = boost::lexical_cast<int>(*itItem);
		  ++itItem;
		  FrBdAr_L[i][2] = boost::lexical_cast<int>(*itItem);
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
		  std::cerr << "End of fragment library was reached before expected! Last fragment was skipped!\n";
		  /* Once we will implement the resizing this part will not be needed any more */
		  free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      free_dvector(*FrPaCh,1,*FrAtNu);
      free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      free_imatrix(*FrBdAr,1,*FrBdNu,1,2);
      free_cmatrix(*FrBdTy,1,*FrBdNu,1,4);
      return 1;
	  }

	  boost::trim(StrLin);
	  if (StrLin != "@<TRIPOS>ALT_TYPE"){
	    std::cerr << "No @<TRIPOS>ALT_TYPE-tag found for fragment " << *CurFraTot
		            << ". Skipping!\n";
      (*SkiFra)++;
      (*CurFraTot)++;
		  /* Once we will implement the resizing this part will not be needed any more */
		  free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      free_dvector(*FrPaCh,1,*FrAtNu);
      free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      free_imatrix(*FrBdAr,1,*FrBdNu,1,2);
      free_cmatrix(*FrBdTy,1,*FrBdNu,1,4);
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
		  std::cerr << "No standard ALT_TYPE_SET signature find for fragment "<< *CurFraTot
                << "Skipping!\n";
      (*SkiFra)++;
      (*CurFraTot)++;
      /* Once we will implement the resizing this part will not be needed any more */
		  free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      free_dvector(*FrPaCh,1,*FrAtNu);
      free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      free_imatrix(*FrBdAr,1,*FrBdNu,1,2);
      free_cmatrix(*FrBdTy,1,*FrBdNu,1,4);
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
                << ". Skipping!\n";
      (*SkiFra)++;
      (*CurFraTot)++;
       /* Once we will implement the resizing this part will not be needed any more */
		  free_cmatrix(*FrAtEl,1,*FrAtNu,1,5);
      free_dmatrix(*FrCoor,1,*FrAtNu,1,3);
      free_cmatrix(*FrSyAtTy,1,*FrAtNu,1,7);
      free_dvector(*FrPaCh,1,*FrAtNu);
      free_cmatrix(*SubNa,1,*FrAtNu,1,10);
      free_imatrix(*FrBdAr,1,*FrBdNu,1,2);
      free_cmatrix(*FrBdTy,1,*FrBdNu,1,4);
      continue;
	  }
    FrAtTy_L=cmatrix(1,*FrAtNu,1,7);
	  *FrAtTy=FrAtTy_L;
	  ++itItem; // skip the alternative atom type set name
	  AtCount = 0;
	  AtNu_flag = false;
	  while (AtCount < *FrAtNu){
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
    //StrLin = getline(inStream); /* This has to be checked! */
	  *strPos = inStream->tellg(); // Update the stream position indicator
    (*CurFraTot)++;
    //(*CurFra)++;
    return 0;
  }
}
