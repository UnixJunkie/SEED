#!/usr/bin/python
"""Jean-Remy Marchand 24th Oct. 2016
This script aims at converting a mol2 file to a mol2 "seed" format
ie it takes atom types and partial charges from a CHARMM stream file
Generated with CGenFF
Modification CGenFF 4.0 => It adds the lone pair dummy atom 
for halogen bonding
It has been validated on an input mol2 file from obabel
SEED 4 modification => Sybyl atom types are kept, CGenFF atom types
are added under the tag "@<TRIPOS>ALT_TYPE"
"""
import sys, os
from math import sqrt
if len(sys.argv)!=4:
	print "Usage = python script.py lig.mol2 lig.str out.mol2"

# Function to calculate the position of the LP
# Takes as input a tuple with coordinates of the C atom, X atom and distance from
# the X to the LP   
def calc_position_LP(C, X, dist):
	V=(float(X[0])-float(C[0]), float(X[1])-float(C[1]), float(X[2])-float(C[2]))
	normalizateur=sqrt((V[0]**2)+(V[1]**2)+(V[2]**2))
	V_norm=(V[0]/normalizateur, V[1]/normalizateur, V[2]/normalizateur)
	Translateur=(float(dist)*V_norm[0], float(dist)*V_norm[1], float(dist)*V_norm[2])
	LP=(float(X[0])+Translateur[0], float(X[1])+Translateur[1], float(X[2])+Translateur[2])
	return LP

# Open data
a=open(sys.argv[1], "r")
orimol2=a.readlines()
a.close()
a=open(sys.argv[2], "r")
topology=a.readlines()
a.close()
good=False
for lin in topology:
	if "RESI" in lin:
		good=True
if good==False:
	print "No parameters for", orimol2
	sys.exit()

# Process the CHARMM stream file (dictionaries of atom types; charges;
# and halogen particles, referred to as "lone pairs"
# Check lone pairs in stream file
# LP_dict: key = LP identifier
# value = (name of halogen, name of C, distance to halogen)
lonepair=0
LP_dict={}
interesting_atoms=[]
indice=0
dict_atom_types={}
dict_charges={}
for line in topology:
	if "ATOM" in line:
		indice+=1
		dict_atom_types[indice]=line.split()[2]
		dict_charges[indice]=line.split()[3]
	if "LONEPAIR" in line:
		lonepair+=1
		LP_dict[line.split()[2]]=(line.split()[3], line.split()[4], line.split()[6])
		interesting_atoms.append(line.split()[3])
		interesting_atoms.append(line.split()[4])
# Now let's create a correspondence table of atom indices and coordinates
# involved in halogen bonding
# key = atom name, value = atom indice
dict_indices={}
dict_interesting_coordinates={}
for line in orimol2:
	if len(line.split())==0:
		continue
	elif "#" in line:
		continue
	if len(line.split()) >= 6:
		for atom in interesting_atoms:
			if line.split()[1]==atom:
				dict_indices[atom]=line.split()[0]
				dict_interesting_coordinates[atom]=(line.split()[2], line.split()[3], line.split()[4])
	elif "BOND" in line:
		break
out=""
# Process the header of the mol2 file and add the new atoms/bonds
for line in orimol2:
	if len(line.split())==0:
		continue
	elif "#" in line:
		continue
	if len(line.split())==5:
		out+=(" %s %s 1 0 0\n") % (int(line.split()[0])+lonepair, int(line.split()[1])+lonepair)
	elif "ATOM" in line:
		break
	else:
		out+=line

# Now it's time to process the atom and bond blocks
whereami_mol2=False
start=False
atom_indice=0
bond_indice=0
dict_LP_indices={}
james_bond=False
for line in orimol2:
	if len(line.split())==0:
		continue
	elif "#" in line:
		continue
	if "@<TRIPOS>ATOM" in line:
		start=True
		whereami_mol2=True
		out+=line
		continue
	if start==True:
		if "TRIPOS" in line and james_bond==True:
			break
			# Breaks if any new section after the BOND one
		if "@<TRIPOS>BOND" in line:
			whereami_mol2=False
			james_bond=True
			# Calculate position of halogen bonding particle(s)
			for key, value in LP_dict.iteritems():
				coor=calc_position_LP(dict_interesting_coordinates[LP_dict[key][1]], dict_interesting_coordinates[LP_dict[key][0]], LP_dict[key][2])		
				out+=("%7s %-8s%10.4f%10.4f%10.4f %-7s%5s %-11s%7.4f\n") %(atom_indice, key, coor[0], coor[1], coor[2], "Du", "1", "LIG", 0.0500)
				dict_LP_indices[key]=atom_indice
				atom_indice+=1
			out+=line
			continue
		if whereami_mol2==False:
			out+=line
			if len(line.split())==4:
				bond_indice=int(line.split()[0])
		elif whereami_mol2==True:
			if len(line) > 2:
				atom_indice=int(line.split()[0])
				out+=("%7s %-8s%10.4f%10.4f%10.4f %-7s%5s %-11s%7.4f\n") %(int(atom_indice), line.split()[1], float(line.split()[2]), float(line.split()[3]), float(line.split()[4]), line.split()[5], "1", "LIG", float(dict_charges[atom_indice]))	
				atom_indice+=1
			else:
				out+=line


# Add bonds for the halogen bonding particle(s)
for key, value in dict_LP_indices.iteritems():
    bond_indice+=1
    out+=("%6s%6s%6s%5s\n") % (bond_indice, dict_indices[LP_dict[key][0]], value, 1)

# Now we add the CGenFF alternative atom types
out+="@<TRIPOS>ALT_TYPE\nCGenFF_4.0_ALT_TYPE_SET\nCGenFF_4.0 "
for key, value in dict_atom_types.iteritems():
	out+=str(str(key)+" "+str(value)+" ")

out+="\n"
writefile=open(sys.argv[3], "w")
writefile.write(out)
writefile.close()
