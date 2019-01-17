#!/usr/bin/python
"""Jean-Remy Marchand 2014
This script aims at cleaning a protein PDB file 
restarting the residue index of a PDB file at 1
for not having a column like "A1084" but "A   1"
"""
import sys, os
if len(sys.argv)!=3:
	print "Usage = python script.py prot.pdb out.pdb"

# Define variables
data=[]
counter=0
a=0
resid=0
outline=[]
old=0

# Read original file
oriPDB=open(sys.argv[1], "r")

# Loop where you check if your residue index changed => increment your counter
#(which is actually the new residue index) and print out the PDB line
for line in oriPDB:
	if "REMARK" in line:
		continue
	data=line.split()
	if data[0]=="END":
		break
	counter+=1
	if line[22:27]!=old:
		resid+=1
		old=line[22:27]
	outline.append('%-6s%5s %-4s%1s%3s %1s%4s%1s   %8s%8s%8s%6s%6s      %3s%2s' %("ATOM", counter, data[2], " ", data[3], "A", resid, " ", line[30:38], line[38:46], line[46:54], "0.00", "0.00", "PRO", ""))
oriPDB.close()

# Just to add the "END" at the end of the PDB file
outline.append("END")

writefile=open(sys.argv[2], "w")
writefile.write("\n".join(outline))
writefile.close()
