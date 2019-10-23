#!/bin/bash

## Jean-Remy Marchand 24th Oct. 2016
## This script recapitulates the commands from the tutorial Chapter 1, case study 2.

PYTHONSCRIPTS=/disk2b/jremy/CHARMM_energy_eval_tutorial/update/python_scripts

# Check protonation states (Chemaxon)
cxcalc dominanttautomerdistribution -H 7.2 -f sdf -t "tauto_occupancy" all.smiles > tautodistrib.sdf
# Extract dominant tautomers
python ${PYTHONSCRIPTS}/sdf_select_bytag.py tautodistrib.sdf maintauto.sdf
# Split files
[[ ! -d sdf_split ]] && mkdir sdf_split
python ${PYTHONSCRIPTS}/sdf_split.py maintauto.sdf sdf_split/
# Generate conformers
cd sdf_split
[[ ! -d conformers ]] && mkdir conformers
for i in *sdf ; do a=`echo $i | sed 's/.sdf//g'`; python ${PYTHONSCRIPTS}/sdf_conformergen.py $i 20 1.0 conformers/${a}_conformers ;done
# Clustering
cd conformers
[[ ! -d ../../sdf_separatedtauto_conformerclustered ]] && mkdir ../../sdf_separatedtauto_conformerclustered
for i in *sdf; do a=`echo $i | sed 's/.sdf//g'`; python ${PYTHONSCRIPTS}/clustering_discardredundantconformers_tfd.py ${i} ../../sdf_separatedtauto_conformerclustered/ ; done
# Convert to mol2
cd ../../sdf_separatedtauto_conformerclustered/
[[ ! -d ../mol2split ]] && mkdir ../mol2split
for i in *.sdf ; do a=`echo ${i} | sed 's/.sdf//g'` ; obabel -isdf ${i} -omol2 > ../mol2split/${a}.mol2 ; done
# Clean the mol2
cd ../mol2split ; for i in *mol2; do python ${PYTHONSCRIPTS}/mol2_cleaner.py ${i} ; done
# Generate the forcefield files
[[ ! -d ./param ]] && mkdir param
for i in *.mol2 ; do a=`echo ${i} | sed 's/.mol2//g'` ; cgenff ${i} > param/${a}.str ; done
# Put those which worked in a clean folder and create a list
cd param
[[ ! -d ./clean ]] && mkdir clean
for i in *.str ; do [[ `egrep "RESI" ${i}` ]] && sed -r 's/RESI ......../RESI LIG     /g' ${i} > clean/${i} ; done
cd clean
ls | sed 's/.str//g' > ../../../parametrized.list
cd ../../../
# Some clean up
[[ ! -d ./sdf_final ]] && mkdir sdf_final
for i in `cat parametrized.list` ; do mv sdf_separatedtauto_conformerclustered/${i}.sdf sdf_final/${i}.sdf ; done
[[ ! -d ./mol2_final ]] && mkdir mol2_final
for i in `cat parametrized.list` ; do mv mol2split/${i}.mol2 mol2_final/${i}.mol2 ; done

cd mol2split/param ; mv clean ../../parameters
cd ../../

# Combine the mol2 file and stream file in a mol2 for SEED
mkdir mol2seed ; for i in `cat parametrized.list` ; do python ${PYTHONSCRIPTS}/mol2ori_to_mol2seed4_cgenff4_singlefiles.py mol2_final/${i}.mol2 parameters/${i}.str mol2seed/${i}_seed.mol2 ; done

echo "don't forget to do some cleanup ;)"