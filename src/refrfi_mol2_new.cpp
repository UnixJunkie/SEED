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
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "nrutil.h"
#include "funct.h"

//#include <typeinfo>
#ifndef _STRLENGTH
#define _STRLENGTH 500
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
  boost::char_separator<char> sep(" \t\n\r");

	char **FrAtEl_L,**FrAtTy_L,**FrBdTy_L, **SubNa_L, **FrSyAtTy_L;
  int i,j,**FrBdAr_L,/*FrAtNu_cn,FrBdNu_cn,*/ AtCount, CuAtNu /*insec isValid*/;
	bool AtNu_flag, OverlappingMol;
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
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "No @<TRIPOS>ATOM-tag found for fragment" << *CurFraTot
      << ". Skipping!\n";
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
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "Fragment " << *CurFraTot
                << " might have overlapping atoms. Skipping!\n";
      /* Once we will implement the resizing this part will not be needed any more */
      free_cmatrix(*FrAtEl, 1, *FrAtNu, 1, 5);
      free_dmatrix(*FrCoor, 1, *FrAtNu, 1, 3);
      free_cmatrix(*FrSyAtTy, 1, *FrAtNu, 1, 7);
      free_dvector(*FrPaCh, 1, *FrAtNu);
      free_cmatrix(*SubNa, 1, *FrAtNu, 1, 10);
      continue;
    }

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
		  (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "No @<TRIPOS>BOND-tag found for fragment " << *CurFraTot
      << ". Skipping!\n";
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
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "No @<TRIPOS>ALT_TYPE-tag found for fragment " << *CurFraTot
      << ". Skipping!\n";
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
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "No standard ALT_TYPE_SET signature find for fragment "<< *CurFraTot
      << "Skipping!\n";
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
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "Names of alternative atom type set do not coincide for fragment " << *CurFraTot
      << ". Skipping!\n";
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
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "List of alternative atom types for fragment " << *CurFraTot
                << " is not complete. "
                << "Skipping!\n";
      /* Once we will implement the resizing this part will not be needed any more */
      free_cmatrix(*FrAtEl, 1, *FrAtNu, 1, 5);
      free_dmatrix(*FrCoor, 1, *FrAtNu, 1, 3);
      free_cmatrix(*FrSyAtTy, 1, *FrAtNu, 1, 7);
      free_dvector(*FrPaCh, 1, *FrAtNu);
      free_cmatrix(*SubNa, 1, *FrAtNu, 1, 10);
      free_imatrix(*FrBdAr, 1, *FrBdNu, 1, 2);
      free_cmatrix(*FrBdTy, 1, *FrBdNu, 1, 4);
      free_cmatrix(*FrAtTy, 1, *FrAtNu, 1, 7);
      continue;
    }
    else if (AtCount > *FrAtNu){
      (*SkiFra)++;
      (*CurFraTot)++;
      std::cerr << "List of alternative atom types for fragment " << *CurFraTot
                << " is too long. There might be duplicates. "
                << "Skipping!\n";
      /* Once we will implement the resizing this part will not be needed any more */
      free_cmatrix(*FrAtEl, 1, *FrAtNu, 1, 5);
      free_dmatrix(*FrCoor, 1, *FrAtNu, 1, 3);
      free_cmatrix(*FrSyAtTy, 1, *FrAtNu, 1, 7);
      free_dvector(*FrPaCh, 1, *FrAtNu);
      free_cmatrix(*SubNa, 1, *FrAtNu, 1, 10);
      free_imatrix(*FrBdAr, 1, *FrBdNu, 1, 2);
      free_cmatrix(*FrBdTy, 1, *FrBdNu, 1, 4);
      free_cmatrix(*FrAtTy, 1, *FrAtNu, 1, 7);
      continue;
    }
    //StrLin = getline(inStream); /* This has to be checked! */
	  *strPos = inStream->tellg(); // Update the stream position indicator
    (*CurFraTot)++;
    //(*CurFra)++;
    return 0;
  } // end of while(true)
}
