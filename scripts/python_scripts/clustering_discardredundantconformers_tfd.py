#!/bin/python
"""Jean-Remy Marchand 30th Mar. 2017
This script aims at discarding redundant small molecules (axis of symmetry)
Based on Torsional Fingerprints to calculate the distances
"""

import sys
import os
from rdkit import Chem
from rdkit.Chem import AllChem, TorsionFingerprints
from rdkit import DataStructs 
from rdkit.ML.Cluster import Butina

if len(sys.argv)!=3:
    print "Usage = python script.py sdf outputfoldername"

outputfoldername=sys.argv[2]
cutoff=0.001
distances=[]
bah=open(sys.argv[1], "r")
molname=str(sys.argv[1])[0:-4]
print molname
smi=bah.readlines()
bah.close()
allmol=[]
name=[]
indice=0

allmol=Chem.SDMolSupplier(sys.argv[1], removeHs=False)

# Create fingerprints & Calculate distance (it's a dissimilarity => 0 = identical)
fingouz=[AllChem.GetMorganFingerprintAsBitVect(x, 2, 1024) for x in allmol]
nbofsmi = len(allmol)
distances=[]
for i in range(1, nbofsmi):
	sims=DataStructs.BulkTanimotoSimilarity(fingouz[i], fingouz[:i])
	distances.extend([1-x for x in sims])

# Clustering
clusterz=Butina.ClusterData(distances, nbofsmi, cutoff, isDistData=True)

center=""
nb=[]
clust_size=0
#Loop over number of clusters
for a in range(0, len(clusterz)):
	refmol=allmol[clusterz[a][0]]
	# Loop over molecules of cluster a. Iterates from 1 and not 0 because 0 has already been added to refmol
	for b in range(1, len(clusterz[a])):
		refmol.AddConformer(allmol[clusterz[a][b]].GetConformer(), True)
	matrix=TorsionFingerprints.GetTFDMatrix(refmol, useWeights=False, maxDev='equal', symmRadius=2, ignoreColinearBonds=True)
	conf_clusters=Butina.ClusterData(matrix, len(clusterz[a]), cutoff, True)
	for i in range(0, len(conf_clusters)):
		writer = Chem.SDWriter(str(outputfoldername)+"/"+str(molname)+"_tauto"+str(a)+"conf_"+str(i)+".sdf")
		writer.write(allmol[clusterz[a][conf_clusters[i][0]]])
		writer.close()
	refmol=0

