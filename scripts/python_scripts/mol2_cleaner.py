#!/usr/bin/python
"""Jean-Remy Marchand 24th Oct. 2016
This script aims at cleaning a mol2 file from obabel
ie it gives unique atom names and set the residue nb to 1
and residue name to LIG
"""
import sys, os
if len(sys.argv)!=2:
	print "Usage = python script.py lig.mol2"

atoms=0
bonds=0
cline=0
outmol=""
currline=[]
nbatoms=""
nbbonds=""
outline=""

# Read file
orifile=sys.argv[1]
mol=open(orifile, 'r')
type=""
for line in mol:
	currline=line.split()
	if '@<TRIPOS>ATOM' in line:
		atoms=0
		bonds=0
		outmol+=line
	elif '@<TRIPOS>BOND' in line:
		bonds=0
		outmol+=line
	elif len(currline)==9:
		type = ''.join(i for i in currline[1] if not i.isdigit() and i!="'")[0:2]
		outline='%7s %-9s %8s %9s %9s %-7s %2s %-9s %8s\n'%(currline[0], type+currline[0], currline[2], currline[3], currline[4], currline[5], "1", "LIG", currline[8])
		outmol+=outline
		atoms+=1
	elif len(currline)==5:
		nbatoms=currline[0]
		nbbonds=currline[1]
		outmol+=line
	elif len(currline)==4:
		bonds+=1
		outmol+=line
	else:
		outmol+=line

mol.close()

if int(atoms)!=int(nbatoms):
	print "Problem in number of atoms"
if int(bonds)!=int(nbbonds):
	print "Problem in number of (James) bonds"


writefile=open(sys.argv[1], 'w')
writefile.write(outmol)
writefile.close()

