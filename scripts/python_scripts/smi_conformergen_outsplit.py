#!/bin/python
"""Jean-Remy Marchand 24th Oct. 2016
Updated with integration of clustering on dihedral fingerprints in March 2018
This script aims at generating conformers for a small molecule
Uses the ETDKG function of RDKit
http://pubs.acs.org/doi/abs/10.1021/acs.jcim.5b00654
http://pubs.acs.org/doi/full/10.1021/acs.jcim.6b00613
"""

import os, sys
from rdkit import Chem
from rdkit.Chem import AllChem, TorsionFingerprints
from rdkit import DataStructs 
from rdkit.ML.Cluster import Butina


if len(sys.argv)!=6:
	print "Usage = python script.py mol.sdf nb_of_conf rms_threshold output_folder/ nb_thread"

output_folder=sys.argv[4]
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

file=sys.argv[1]
N=sys.argv[2]
RMS=sys.argv[3]
nbthread=sys.argv[5]
# Here new declarations
cutoff=0.01

mols=Chem.SmilesMolSupplier(file, titleLine=False)
for mol in mols:
	if mol != None:
		mol=Chem.AddHs(mol)
		conf=AllChem.EmbedMultipleConfs(mol, numConfs=int(N), pruneRmsThresh=float(RMS), useExpTorsionAnglePrefs=True, useBasicKnowledge=True, numThreads=int(nbthread))
		Chem.rdMolAlign.AlignMolConformers(mol)
		AllChem.UFFOptimizeMoleculeConfs(mol, numThreads=int(nbthread))
		## Here new code to discard identical conformers around an axis of symmetry (not supported by pruneRmsThresh in the previous fct)
		matrix=TorsionFingerprints.GetTFDMatrix(mol, useWeights=False, maxDev='equal', symmRadius=2, ignoreColinearBonds=True)
		conf_clusters=Butina.ClusterData(matrix, len(conf), cutoff, True) 
		confnb=1
		for cluster in conf_clusters:
			writer=Chem.SDWriter(output_folder+"/"+mol.GetProp("_Name")+"_conf_"+str(confnb)+".sdf")
			writer.write(mol, confId=cluster[0]) # output only centroid
			writer.close()
			confnb+=1
		## End new code

