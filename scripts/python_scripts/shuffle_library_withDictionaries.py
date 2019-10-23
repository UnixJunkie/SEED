#!/usr/bin/env python
"""Script to shuffle a concatenated .mol2 file
Identical to shuffle_library_seekTell.py except that it makes use of a
dictionary for the correspondence (shuffled position) -> (original position)"""
import string
import sys
import os
import re
import random
import time

def print_usage():
    print "Usage = python shuffle_library library.mol2"

def count_and_tell(in_fn):
    """Counts the molecules and tell the mol start and end positions in the file"""
    start_mol = [] # Note that this is faster than start_mol = list()
    nmol = 0
    pattern = "@<TRIPOS>MOLECULE"

    f = open(in_fn, 'r')
    line = f.readline()
    prev_ptr = 0
    curr_ptr = f.tell()

    while line:
        if line.strip() == pattern:
            nmol += 1
            start_mol.append(prev_ptr)
        line = f.readline()
        prev_ptr = curr_ptr
        curr_ptr = f.tell()
    f.close()
    print nmol

    end_mol = start_mol[1:]
    end_mol.append(os.path.getsize(in_fn))
    return start_mol, end_mol, nmol

def main(argv):
    in_fn = argv[0]
    in_fn_split = in_fn.rsplit( ".", 1 )
    out_fn = in_fn_split[0] + "_shuffled." + in_fn_split[1]
    
    start_time = time.time()
    start_mol, end_mol, nmol = count_and_tell(in_fn)
    print nmol
    # print start_mol
    # print end_mol

    random.seed() # for comparison set a specific seed
    
    mol = range(nmol)
    mol_shuff = mol[:] # copy list by value (not by object reference)
    random.shuffle(mol_shuff) # shuffle sequence
    mol_dict = dict(zip(mol_shuff, mol))

    pattern = "@<TRIPOS>MOLECULE"

    in_f = open(in_fn, "r")
    out_f = open(out_fn, "w")
    for i in range(nmol):
        curr = mol_dict[i]
        in_f.seek(start_mol[curr])
        out_mol = in_f.read(end_mol[curr] - start_mol[curr])
        out_f.write(out_mol)

    in_f.close()
    out_f.close()
    print "Execution took", time.time() - start_time, "seconds."


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print_usage()
    main(sys.argv[1:])
