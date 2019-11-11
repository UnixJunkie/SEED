#!/bin/bash
# This script is part of SEED
# It extracts the atom type and LJ parameters from a Charmm .prm file (e.g. par_all36_na.prm)
#
# TODO: fix hard-coded variables

prmfile="par_all36_na.prm" 

sed -n -e '/Lennard-Jones/,/^$/{/^$/d; /^!/d; p; }' $prmfile | sed -e 's/!/#/' | awk '
  BEGIN{
    atommap["O"] = 8;
    atommap["H"] = 1;
    atommap["N"] = 7;
    atommap["C"] = 6;
    atommap["P"] = 15;
    startidx = 229;
  }
  {
    foundcomment = 0;
    printf "%-6d %-6s %-4d %7f %8f ", startidx, $1, atommap[substr($1,1,1)], $4, $3 * -1 ;
    for (i=1;i<NF;i++){
      if (substr($i,1,1) == "#"){
	foundcomment = 1;
	break;
      }
    }
    if (foundcomment == 1){
      s = ""; for (j = i; j <= NF; j++) s = s $j " "; printf "%s", s ; 
    }
    printf "\n";
    startidx++;
  }
'
