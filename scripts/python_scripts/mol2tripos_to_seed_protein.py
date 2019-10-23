#!/usr/bin/python
"""Jean-Remy Marchand 24th Apr. 2017
This script aims at converting a mol2 file to a mol2 "seed" format
ie it takes atom types and partial charges from the CHARMM
forcefield file top_all36_cgenff.rtf
It has been validated on an input mol2 file from obabel
SEED 4 modification => Sybyl atom types are kept, CGenFF atom types
are added under the tag "@<TRIPOS>ALT_TYPE"
"""
import sys, os
if len(sys.argv)!=4:
	print "Usage = python script.py lig.mol2 top_all36_prot.rtf out.mol2"

# Open data
a=open(sys.argv[1], "r")
orimol2=a.readlines()
a.close()
a=open(sys.argv[2], "r")
topology=a.readlines()
a.close()

# Process the CHARMM topology file
# Creation of a dictionary of dictionaries
# dict[KEY] where KEY = residue ; value = 2nd dictionary
# 2nd dictionaries are key = atom name, value = atom type
# or key = atom name, value = partial charge
parse=False
resi=False
dict_types={}
dict_charges={}
for line in topology:
	if "BOND" in line:
		parse=False
		resi=False
	if parse==True:
		if "GROUP" in line:
			None
		elif "ATOM" in line:
			tmpdict={}
			atom_name=line.split()[1]
			atom_type=line.split()[2]
			atom_charge=line.split()[3]
			tmpdict[atom_name]=atom_type
			try: # Check if key is initialized
				if dict_types[resi] != None:
					dict_types[resi].update(tmpdict)
					tmpdict={}
					tmpdict[atom_name]=atom_charge
					dict_charges[resi].update(tmpdict)
			except: # If key isn't initialized, set it
				dict_types[resi]=tmpdict
				tmpdict={}
				tmpdict[atom_name]=atom_charge
				dict_charges[resi]=tmpdict
	if "RESI" in line:
		parse=True
		resi=line.split()[1]

# Add by hand stuffs here => TIP3P water for example
# Or terminii
tmpdict={}
resi="TIP"
tmpdict["OH2"]="OT"
dict_types[resi]=tmpdict
tmpdict={}
tmpdict["H1"]="HT"
dict_types[resi].update(tmpdict)
tmpdict={}
tmpdict["H2"]="HT"
dict_types[resi].update(tmpdict)
tmpdict={}
tmpdict["OH2"]="-0.834"
dict_charges[resi]=tmpdict
tmpdict={}
tmpdict["H1"]="0.417"
dict_charges[resi].update(tmpdict)
tmpdict={}
tmpdict["H2"]="0.417"
dict_charges[resi].update(tmpdict)

## Process the mol2 file
# The default atom charges need to be updated to CHARMM charges
# We also need to generate a dict of ALT_TYPE with atom indice
# as the key and the value is the CHARMM atom type
whereami_mol2=False
start=False
james_bond=False
dict_atom_types={}
res=False
out=""
# Doing a counter to renumber residues in order to avoid
# Artifacts from Obabel (restarts residue numbering when chain break)
# (and that is not good with SEED because there you specify the binding pocket by
# residue index)
# /!\ I did the choice to renumber from 1 ! /!\
counter_resid=0
old_resn=""
old_resid=0
for line in orimol2:
	if len(line.split())==0:
		continue
	elif "#" in line:
		continue
	else:
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
				out+=line
				continue
			if whereami_mol2==False:
				out+=line
			elif whereami_mol2==True:
			## Residue number check and modification
				if old_resn!=str(line.split()[7]) or old_resid!=int(line.split()[6]):
					counter_resid+=1
				old_resid=int(line.split()[6])
				old_resn=str(line.split()[7])
				if len(line) > 2:
					atom_indice=int(line.split()[0])
					res=line.split()[7][0:3]
					name=line.split()[1]
					try:
						if dict_types[res][name]!=None:
							dict_atom_types[atom_indice]=dict_types[res][name]
							out+=("%7s %-8s%10.4f%10.4f%10.4f %-7s%5s %-11s%7.4f\n") %(int(atom_indice), line.split()[1], float(line.split()[2]), float(line.split()[3]), float(line.split()[4]), line.split()[5], counter_resid, line.split()[7], float(dict_charges[res][name]))	
							atom_indice+=1
					except:
						dict_atom_types[atom_indice]="XXX"
						out+=("%7s %-8s%10.4f%10.4f%10.4f %-7s%5s %-11s%s\n") %(int(atom_indice), line.split()[1], float(line.split()[2]), float(line.split()[3]), float(line.split()[4]), line.split()[5], counter_resid, line.split()[7], "XXX")	
						atom_indice+=1
		else:
			out+=line

# Now we add the CGenFF alternative atom types
out+="@<TRIPOS>ALT_TYPE\nCGenFF_4.0_ALT_TYPE_SET\nCGenFF_4.0 "
for key, value in dict_atom_types.iteritems():
	out+=str(str(key)+" "+str(value)+" ")

writefile=open(sys.argv[3], "w")
writefile.write(out)
writefile.close()


print "WATCH OUT! This script handles only charged CTER/NTER!!!"
print "Modify the NTER/CTER charges/atom types by hand!"
print "grep for 'XXX'"
print "Watch out again, you need to modify MORE charges"
print "than just the XXX ones! The parameters for N, CA, HA..."
print "There's some both in the 'ATOM' and 'ALT_TYPE' sections!!"
