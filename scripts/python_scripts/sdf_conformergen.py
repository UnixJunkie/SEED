#!/bin/python
"""Jean-Remy Marchand 30th Mar. 2017
This script aims at generating conformers for a small molecule
Uses the ETDKG function of RDKit
http://pubs.acs.org/doi/abs/10.1021/acs.jcim.5b00654
http://pubs.acs.org/doi/full/10.1021/acs.jcim.6b00613
"""

import os, sys
from rdkit import Chem
from rdkit.Chem import AllChem

if len(sys.argv)!=5:
	print "Usage = python script.py mol.sdf nb_of_conf rms_threshold output"

file=sys.argv[1]
N=sys.argv[2]
RMS=sys.argv[3]
writer=Chem.SDWriter(sys.argv[4]+".sdf")

mols=Chem.SDMolSupplier(file, removeHs=False)
for mol in mols:
	if mol != None:
		mol=Chem.AddHs(mol)
		conf=AllChem.EmbedMultipleConfs(mol, numConfs=int(N), pruneRmsThresh=float(RMS), useExpTorsionAnglePrefs=True, useBasicKnowledge=True)
		Chem.rdMolAlign.AlignMolConformers(mol)
		AllChem.UFFOptimizeMoleculeConfs(mol)
		for m in conf:
			writer.write(mol, confId=m)

writer.close()
