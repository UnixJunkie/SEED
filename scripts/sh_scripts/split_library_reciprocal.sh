#!/bin/bash
# clangini 31/07/2017
# script for splitting a concatenated mol2 file library.mol2 into N files
# Usage: bash split_library_reciprocal.sh library.mol2 N
# Differently from split_library.sh, where the first M molecules goes into
# part0, the second M into part1, etc... Here part1 contains molecules 1, M+1,
# 2M+1, ... , part2 contains 2, M+2, 2M+2, ...

library_fn_full=$1
npart=$2

# library_path=${library%/*}
# library_fn_full=${library##*/}
library_fn_noext=${library_fn_full%.*}
extension=${library_fn_full##*.}

# echo $library_path
echo $library_fn_full
echo $library_fn_noext
echo $extension


# nmol=$(grep "@<TRIPOS>MOLECULE" $library_fn_full | wc -l )
# nmol_part=$((($nmol + $npart - 1)/$npart))
# echo $nmol
# echo $nmol_part

awk -v fname=${library_fn_noext} -v pat="@<TRIPOS>MOLECULE" -v ext=$extension -v npart=$npart '
BEGIN {
  filenum = -1
}
{
  if ($0 ~ pat){
    filenum++
  }
  if (filenum < npart){
    print > fname"_part"filenum"."ext
  } else {
    filenum = 0
    print > fname"_part"filenum"."ext
  }
}
' ${library_fn_full}
