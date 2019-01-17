#!/bin/bash
# Meta script and text file from JRM 29th of March 2018
# All related python scripts by JRM, at diverse dates
# "Licence agreement": Feel free to reproduce it, modify it, add things to it
# But if you do so, keep the original reference and most importantly, continue to share it with your colleagues

######## IMPORTANT PRELIMINARY STATEMENTS #######
#JR#### READ EVERYTHING CAREFULLY!!! There's a lot of comments, that's on purpose. Don't ask questions that 
#JR#### can be answered by the comments in this file. In addition, everything is time benchmarked. Always nice to keep an eye on that
#JR#### 1 ####
#JR# Choose carefully your library: tailor it to your needs. What do you want? Interaction with what side chains?
#JR# Most of the time, there's many filters to apply to the library before running the docking.
#JR# The less noise = the better results. Does it make sense to have compounds with 7 rotatable bonds?
#JR# With 5 chiral centers? With a logP of 6? With only 3 heavy atoms? Without a ring?
#JR# With aggregator structures, or PAINS?
#JR# I would advise, arbitrarily: logP <= 3, rotatable bonds <= 5, N ring > 1, HAC > 5 and < 40
#JR# Keep in mind aggregator for compound purchase => http://advisor.bkslab.org/
#JR# Keep in mind PAINS for purchase => http://mobyle.rpbs.univ-paris-diderot.fr/cgi-bin/portal.py?form=FAF-Drugs4#forms::FAF-Drugs4
#JR#### 2 ####
#JR# Pay attention that according to your source for the chemical library
#JR# Are your compounds are named properly in the SDF file? It's easier to solve that at the beginning than after (script shared)
#JR# Is there unwanted things, such as counter salts or mixtures in a single molecule? (easy to do with the RDKit)
#JR# Do you have chiral centers? If yes, is the chemical library selling enantiopur compounds or solely mixtures?
#JR# If it's a mixture, do you have all stereoisomers in the origine library or should you generate them?
#JR#### 3 #### 
#JR# Altho this is a "meta script", I would rather do it interactively, or at least test 
#JR# each command on a representative subset of your library.
#JR#### 4 ####
#JR# Similarly, never trust what you've prepared. Painfully check as much as you can.
#JR# That includes opening the output file in a text file and verify it complies to 
#JR# the format you decided to output (traditionnally SDF for a normal library or MOL2 for a SEED library)
#JR# That includes extracting n random molecules (10^2) and vizualise them in pymol. If it 
#JR# doesn't display them properly, they're not compliant (pymol is very flexible for reading molecules
#JR# and accepts much more mistakes than most docking software)
######## END OF PRELIMINARY STATEMENTS, DON'T HESITATE TO ADD REMARKS (with your own reference) #######

######## SOFTWARE REQUIREMENTS #######
#JR# generation of protomers: ChemAxon, with a working cxcalc binary
#JR# generation of conformers: RDKit > 2017 
#JR# proper mol2 file conversion: corina
#JR# generation of parameters: cgenff, rdkit (for absinth)
#JR# all side files in a joined python script folder
#JR# and other files, such as the FES.csv files

PYTHONDIR=/disk3/jremy/caflisch_git/chem_library_preparation/pythonscripts/
ABSINTHDIR=/disk3/jremy/caflisch_git/chem_library_preparation/otherfiles/


echo "

"
date
echo "OK, let's start preparing that library"
##CL## Before starting the protocol, remove counterions from the molecules.
##CL## We use molconvert for this -> corina 3.10 does not support SD V3000 files (but the corina 4.0 does)
molconvert -F sdf library.sdf -o library_nosalt.sdf
##CL## can also use corina for this:
##CL## corina -d rs
##CL## (corina -h all => rs - Remove small fragments (counter ions, solvent molecules, etc.))

##JR## Step 1: generation of tautomers with ChemAxon and select the main ones with a custom python script (minimum occupancy set in the script)
time cxcalc dominanttautomerdistribution -H 7.2 -f sdf -t "tauto_occupancy" library_nosalt.sdf > tautodistrib.sdf
echo "

"
date
echo "tautomers generated"
python ${PYTHONDIR}sdf_select_bytag_nordkit.py tautodistrib.sdf maintauto.sdf 24.9
##JR# Note that this python script adds "tauto_number" to molecules so that their name is unique
##CL# modified, now the occupancy threshold in no longer hard-coded

##JR## Step 2: generation of conformers
mkdir 100conf_075rmsd/
time python ${PYTHONDIR}sdf_conformergen_outsplit.py maintauto.sdf 100 0.75 100conf_075rmsd/ 4
echo "

"
date
echo "conformers generated"
cd 100conf_075rmsd/ ; ls | sed 's/.sdf//g' > ../conformers_tautomers_original_full.list ; cd ..
##JR# Note that this script output each structure separately, to avoid a succession of scripts.
##JR# This will be horrible for your file system, but there's not much smarter to do.

##JR## Step 3: Convert to mol2
mkdir mol2_split
time for i in `cat conformers_tautomers_original_full.list`; do
	 corina -i t=sdf 100conf_075rmsd/${i}.sdf -o t=mol2 -d no3d -d newtypes -o fcharges |egrep -v "\#" | awk 'NF'  > mol2_split/${i}.mol2
done
echo "

"
date
echo "Files have been converted to mol2"

# Tar temporary folder
tar -zcf 100conf_075rmsd_split.tgz 100conf_075rmsd/ && rm -r 100conf_075rmsd/

##JR## Step 4: Generate CGenFF parameters
##JR## Here to save time we want to generate just the parameter for ONE conformer for each tautomer.
##JR## Side note, ofc different tautomers of the same molecules need different parameters.
##JR## Let's generate the list of uniq tautomers
egrep "_conf_1$" conformers_tautomers_original_full.list > original_tautomers_firstconf.list
mkdir cgenff_param
time for i in `cat original_tautomers_firstconf.list`; do
	a=`echo $i | sed 's/_conf_1//g'`
	cgenff mol2_split/${i}.mol2 > cgenff_param/${a}.str 
done &> /dev/null
##JR## Some molecules didn't go through, however, CGenFF generates a non empty file,
##JR## with empty parameters. We should get rid of them.
##JR## In addition, for further putative use in CHARMM, it may turn useful to rename the
##JR## residue name in standard identifier "LIG"
cd cgenff_param
mkdir ../cgenff_clean
time for i in *.str ; do [[ `egrep "RESI" ${i}` ]] && sed -r 's/RESI ......../RESI LIG     /g' ${i} > ../cgenff_clean/${i} ; done &> /dev/null
cd ../cgenff_clean ; ls | sed 's/.str//g' > ../tautomers_firstconf_cgenffparam.list ; cd ../
for i in `cat tautomers_firstconf_cgenffparam.list` ; do grep $i conformers_tautomers_original_full.list ; done > tautomers_conformers_cgenffparam.list 

# Clean temporary folder
rm -r cgenff_param

echo "

"
date
echo "CGenFF parameters have been generated"

##JR## Step 5: Create SEED files
mkdir mol2seed
time for i in `cat tautomers_conformers_cgenffparam.list` ; do
	a=`echo $i | sed -r 's/_conf_[0-9]*//g' `
	python ${PYTHONDIR}mol2ori_to_mol2seed4_cgenff4_singlefiles.py mol2_split/${i}.mol2 cgenff_clean/${a}.str mol2seed/${i}_seed.mol2
done

##CL## At this point different conformer of the same fragment (or tautomer)
##CL## have the same name. So we rename them in the mol2 file.
cd mol2seed ; for i in *; do sed -i "s/${i%_conf*}/${i%_seed.mol2}/" $i; done; cd ..

cd mol2seed ; for i in * ; do cat $i >> ../library_seed.mol2 ; done ; cd ..

# Clean temporary folder
rm -r mol2seed

echo "

"
date
echo "SEED library is ready!"

#The library file for seed is library_seed.mol2 "
