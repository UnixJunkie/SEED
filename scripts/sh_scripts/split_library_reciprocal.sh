#!/bin/bash
# clangini 31/07/2017
USAGE="\nScript for splitting a concatenated mol2 file input_library.mol2 into N files\n\
Usage: bash split_library_reciprocal.sh input_library.mol2 output_name N\n\
The output files are suffixed _partX with X going from 0 to N-1.
part1 contains molecules 1, M+1, 2M+1, ... , part2 contains 2, M+2, 2M+2, ...\n"

# Different from split_library.sh, where the first M molecules goes into
# part0, the second M into part1, etc...

if [ "$1" == "--help" ] || [ "$1" == "-h" ];
then
  echo -e $USAGE;
  exit 1
fi

if [[ "$1" == -* ]] || [[ "$2" == -* ]] || [[ "$3" == -* ]];
then
  echo -e "\nFatal. Unrecognized option. Use --help for usage information.\n";
  exit -1
fi

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ];
then 
  echo -e "\nFatal. Too few arguments. Use --help for usage information.\n";
  exit -1
fi 

if [ -r "$1" ] && [ -f "$1" ];
then
  dummy="dummy";# everything ok
else
  echo -e "\nFatal. Cannot open/read input library. Use --help for usage information.\n";
  exit -1
fi

re='^[0-9]+$'
if ! [[ $3 =~ $re ]] ; then
   echo -e "\nFatal: $3 is not a number.\n"
   exit -11
fi

library_fn_full=$1
output_fn_full=$2
npart=$3

# library_fn_full=${library##*/}
library_fn_noext=${library_fn_full%.*}
output_fn_noext=${output_fn_full%.*}
output_path=$(dirname $output_fn_full)
extension=${library_fn_full##*.}


echo -e "\nLibrary ${library_fn_full} will be split into ${npart} parts."
echo -e "Output files will be saved as ${output_fn_noext}_partX.${extension} in folder $output_path \n"

if [ -w "${output_fn_noext}_part0.${extension}" ];
then 
  echo -e "\nOutput files already exist and will be overwritten.\n"
fi 

if [ ! -d $output_path ];
then 
  echo -e "\nFatal. Output directory does not exist.\n"
  exit -1
fi

# echo $library_path
# echo $library_fn_full
# echo $library_fn_noext
# echo $extension

# nmol=$(grep "@<TRIPOS>MOLECULE" $library_fn_full | wc -l )
# nmol_part=$((($nmol + $npart - 1)/$npart))
# echo $nmol
# echo $nmol_part

awk -v fname=${library_fn_noext} -v pat="@<TRIPOS>MOLECULE" -v ext=$extension -v npart=$npart -v outfn=$output_fn_noext '
BEGIN {
  filenum = -1
}
{
  if ($0 ~ pat){
    filenum++
  }
  if (filenum < npart){
    print > outfn"_part"filenum"."ext
  } else {
    filenum = 0
    print > outfn"_part"filenum"."ext
  }
}
' ${library_fn_full}
