#!/bin/python
"""Cassiano Langini May 2019
This script aims at generating conformers for a small molecule
Uses the ETDKG function of RDKit
It is the same of sdf_conformergen_outsplit.py, but 
takes a (single-molecule) mol2 file as input.
"""
from __future__ import print_function
import os
import sys
from rdkit import Chem
from rdkit.Chem import AllChem, TorsionFingerprints
from rdkit import DataStructs
from rdkit.ML.Cluster import Butina


if len(sys.argv) != 6:
	print("Usage = python script.py mol.mol2 nb_of_conf rms_threshold output_folder/ nb_thread")

output_folder = sys.argv[4]
if not os.path.exists(output_folder):
    os.makedirs(output_folder)

fn = sys.argv[1]
N = sys.argv[2]
RMS = sys.argv[3]
nbthread = sys.argv[5]
# Here new declarations
cutoff = 0.01

mol = Chem.MolFromMol2File(fn, removeHs=False)
if mol != None:
	mol = Chem.AddHs(mol)
	conf = AllChem.EmbedMultipleConfs(mol, numConfs=int(N),
								pruneRmsThresh=float(RMS),
								useExpTorsionAnglePrefs=True,
								useBasicKnowledge=True,
								numThreads=int(nbthread))
	if len(conf) > 0:
		Chem.rdMolAlign.AlignMolConformers(mol)
		AllChem.UFFOptimizeMoleculeConfs(mol, numThreads=int(nbthread))
		## Here new code to discard identical conformers around an axis of symmetry (not supported by pruneRmsThresh in the previous fct)
		matrix = TorsionFingerprints.GetTFDMatrix(
			mol, useWeights=False, maxDev='equal', symmRadius=2, ignoreColinearBonds=True)
		conf_clusters = Butina.ClusterData(matrix, len(conf), cutoff, True)
		confnb = 1
		for cluster in conf_clusters:
			writer = Chem.SDWriter(
				output_folder+"/"+mol.GetProp("_Name")+"_conf_"+str(confnb)+".sdf")
			writer.write(mol, confId=cluster[0])  # output only centroid
			writer.close()
			confnb += 1
	else:
		# not able to make conformers
		print("Could not generate any conformers for %s" % (mol.GetProp("_Name")))
		# confnb = 1
		# writer=Chem.SDWriter(output_folder+"/"+mol.GetProp("_Name")+"_conf_"+str(confnb)+".sdf")
		# writer.write(mol)
		# writer.close()
	## End new code
else:
	print("ERROR: not able to read molecule in %s"%(fn))
