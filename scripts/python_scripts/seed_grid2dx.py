#!/usr/bin/env/python 

usage = '''
This script can convert a grid generated by SEED into a .dx map file 
that can be visualized e.g. with PyMOL.

python seed_grid2dx.py -g input.grid -b basename -t grid_type -v threshold

The grid_type should be coulomb, vanderwaals or desolvation. 
The output will be saved as basename.dx in the case of desolvation and coulomb
grids. In the case of coulomb grid, two files will be saved: basename_rep.dx and 
basename_att.dx containing the repulsive and attractive term of van der Waals 
interaction respectively.
The threshold value is relevant only for vanderwaals grids, where the 
huge range of energies may give problems for PyMOL visualization. If a threshold 
is set, all repulsive values > threshold will be set to threshold and all 
attractive values < -threshold will be set to -threshold. 
'''

import sys 
import getopt
import numpy as np
from gridData import OpenDX

def main(argv):
    
    grid_fn = ''
    basename = ''
    grid_type = ''
    th_value = -1.0
    do_thresholding = False
    
    try:
          opts, args = getopt.getopt(argv,"hg:b:t:v:",["grid=","basename=","type=","threshold="])
    except getopt.GetoptError:
        print(usage)
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print(usage)
            sys.exit()
        elif opt in ("-g", "--grid"):
            grid_fn = arg
            # print "Input grid file: %s" % grid_fn
        elif opt in ("-b", "--basename"):
            basename = arg
            # print "basename for output: %s" % basename
        elif opt in ("-t", "--type"):
            grid_type = arg
            # print "Grid type requested: %d" % grid_type
        elif opt in ("-v", "--threshold"):
            th_value = float(arg)
            do_thresholding = True 
    
    if (grid_fn == '' or basename == ''):
        print('Missing inputs.')
        sys.exit(2)
    
    if grid_type not in ['coulomb', 'vanderwaals', 'desolvation']:
        print('The specified grid type ' + grid_type + ' is not recognized.')
        sys.exit(2)
        
    elif grid_type in ['coulomb', 'vanderwaals']:
    
        # Reading main info
        with open(grid_fn, 'r') as f:
            line = f.readline()
            grid_shape = tuple([int(x) for x in line.split()])
            line = f.readline()
            grid_origin = np.array(line.split(), dtype = np.float64)
            line = f.readline()
            grid_spacing = np.array(line.split(), dtype=np.float64)
            grid_delta = np.diag(grid_spacing)
            n_skiprows = 3 # number of rows to be skipped to read the grid data
            
    elif grid_type == 'desolvation':
        # raise NotImplementedError("type " + grid_type + " is not yet implemented.")
        # Reading main info desolvation grid
        with open(grid_fn, 'r') as f:
            line = f.readline()
            grid_spacing = float(line.split()[2])
            grid_delta = np.diag(np.repeat(grid_spacing, 3))
            
            line = f.readline()
            line = f.readline()
            grid_origin = np.array(line.split(), dtype = np.float64)
            
            line = f.readline()
            line_split = np.array(line.split(), dtype=np.int32)
            grid_shape = tuple(line_split[3:6] - line_split[0:3] + 1)
            
            n_skiprows = 4 # number of rows to be skipped to read the grid data
    
    # Reading grid into ndarray:
    npoint = np.prod(grid_shape)
    grid_vals = np.loadtxt(grid_fn, dtype=np.float64, skiprows=n_skiprows)
    # print(grid_vals.shape)
    
    if grid_type in ['coulomb', 'desolvation']:
        grid_vals = grid_vals.reshape(grid_shape)
        # Now saving to dx file:
        dx = OpenDX.field(grid_type)
        dx.add('positions', OpenDX.gridpositions(1, grid_shape, grid_origin, grid_delta))
        dx.add('connections', OpenDX.gridconnections(2, grid_shape))
        dx.add('data', OpenDX.array(3, grid_vals))

        out_fn = basename + '.dx'
        dx.write(out_fn)
    
    elif grid_type == 'vanderwaals':
        grid_rep = grid_vals[:,1].reshape(grid_shape)
        grid_att = grid_vals[:,0].reshape(grid_shape)
        
        if do_thresholding:
            grid_rep[grid_rep > th_value] = th_value
            grid_att[grid_att < (-1.0*th_value)] = -1.0 * th_value
        
        dx = OpenDX.field('vdw_rep', components=dict(
          positions=OpenDX.gridpositions(1,grid_shape, grid_origin, grid_delta),
          connections=OpenDX.gridconnections(2, grid_shape),
          data=OpenDX.array(3, grid_rep)))
        out_fn = basename + '_rep.dx'
        dx.write(out_fn)
        
        dx = OpenDX.field('vdw_att', components=dict(
          positions=OpenDX.gridpositions(1,grid_shape, grid_origin, grid_delta),
          connections=OpenDX.gridconnections(2, grid_shape),
          data=OpenDX.array(3, grid_att)))
        out_fn = basename + '_att.dx'
        dx.write(out_fn)
    
    
if __name__ == '__main__':
    main(sys.argv[1:])
