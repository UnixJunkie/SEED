#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Apr 21 12:06:09 2020

@author: clangi
"""
import os
import gc
import sys
import argparse
import pandas as pd

def get_parser():
    '''
    Returns a new argument parser 
    '''
    parser = argparse.ArgumentParser()
    parser.add_argument('table', type=str, 
                        help='SEED energy table')
    parser.add_argument('mol2', type=str,
                        help='SEED output mol2')
    parser.add_argument('query', type=str,
                        help='filtering query')
    parser.add_argument('-s', '--suffix', type=str, default='filt',
                        help='suffix to add to output table and mol2 files')
    return parser


def count_and_tell(in_fn):
    """Counts the molecules and tell the mol start and end positions in the file"""
    print("Now passing through the file to generate molecule index.")
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

    end_mol = start_mol[1:]
    end_mol.append(os.path.getsize(in_fn))
    return start_mol, end_mol, nmol

def main():
    
    parser = get_parser()
    opt = parser.parse_args()
    
    try:
        seedTable = pd.read_csv(opt.table, header=0, delim_whitespace=True)
    except:
        sys.exit("Error reading SEED table.")
    
    if (not os.access(opt.mol2, os.R_OK)):
        sys.exit("Cannot read mol2 input file.")
    
    
    mol2OutFn = os.path.splitext(opt.mol2)[0] + "_" + opt.suffix + ".mol2"
    tabOutFn = os.path.splitext(opt.table)[0] + "_" + opt.suffix + ".dat"
    
    if os.path.isfile(mol2OutFn) or os.path.isfile(tabOutFn):
        sys.exit('Output files already exist. Exiting.')

    try:
        filtTable = seedTable.query(opt.query) #seedTable[seedTable.Tot < th]
    except:
        sys.exit("Invalid pandas query. Exiting.")

    idx = filtTable.index
    # check that the index is sorted:
    idx_sort = idx.sort_values()
    if (not idx.equals(idx_sort)):
        sys.exit("The rows in the reduced table are not correctly sorted. Exiting.")

    start_mol, end_mol, nmol = count_and_tell(opt.mol2)

    in_f = open(opt.mol2, "r")
    out_f = open(mol2OutFn, 'w')
    for i in idx:
        in_f.seek(start_mol[i])
        out_mol = in_f.read(end_mol[i] - start_mol[i])
        out_f.write(out_mol)
    in_f.close()
    out_f.close()

    filtTable.to_csv(tabOutFn, sep=' ', index=False, header=True)

if __name__ == "__main__":
    main()


