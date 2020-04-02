.. _parallel:

Running SEED on a cluster
=========================

SEED has a parallel version based on the Message Passing Interface (MPI) that 
can speed up docking campaigns on multicore cluster architectures. 

In order to install it you need a C-compiled MPI implementation (for example 
`OpenMPI <https://www.open-mpi.org/>`_) against which to link the code. For 
manually installed libraries you 
might also have to modify the ``Makefile.local`` and provide the name and path 
of the specific MPI library you want to use. 
You can compile SEED MPI implementation by running:

.. code-block:: bash
  
  cd SEED/src 
  make seed_mpi 
  
SEED MPI version implements a simple parallelization strategy in which each 
process screens a subset of the library against the same target. In order to 
start a SEED parallel run use:

.. code-block:: bash
  
  mpirun -np N seed_4_mpi seed.inp >& log
  
where *N* is the number of required MPI ranks.

Two reading modes are available in the parallel version, ``single`` and ``multi``,
which can be specified in the third line of :ref:`i7 <i7>`. 

The ``single`` mode is recommended when running on a cluster as it requires no pre-processing
of the library mol2 file and implements dynamic load balance. The master rank reads each molecule
and dispatches it to the first available rank. In this way as soon as a rank is idle after 
finishing the previous computation, it will receive a new molecule from the master rank.
Note that the master rank only reads the input file, but does not do any additional 
computation related to the screening.

The ``multi`` mode is the old reading mode originally implemented in SEED, without dynamic 
load balance; each process 
reads from a separate file, which means that the library has to be 
split beforehand into *N* mol2 files. Depending on the way the library was prepared, 
there might be some patterns (*e.g.*, the ligands are ordered by molecular weight) 
which could result in degraded performance as some processes will be faster than others. 
In order to avoid this problem we recommend shuffling the library before 
splitting it. We provide scripts for both shuffling and splitting the library: 

.. code-block:: bash

  python shuffle_library_withDictionaries.py library_name.mol2
  bash split_library_reciprocal.sh library_name_shuffled.mol2 N output_name
  
The shuffled library will be suffixed ``_shuffled.mol2``.
The *N* files will be named ``output_name_part${i}.mol2`` with *i* going from *0* to *N*-1.
Note that ``output_name`` could also contain a path as it might be desirable to save the 
split files into a different folder.
In the input file ``seed.inp`` you have to provide the basename of the library 
(in this case ``output_name.mol2``)
as input file, omitting the suffix.
Each MPI rank *i* will look for its corresponding library file ``output_name_part${i}.mol2``.

Note that, irrespective of the chosen reading mode, each rank will write log information, 
poses, and energies to separate output files. 
Each output file will be suffixed with ``_part${i}``, where *i* is the rank number 
(the master rank being 0).

If needed, the output stuctural files and energy tables 
can be easily recombined together by concatenation:

.. code-block:: bash 

    cat seed_best_part*.mol2 > seed_best_all.mol2
    awk 'FNR!=1' seed_best_part*.dat > seed_best_all.dat
    
where ``FNR!=1`` removes the header line.

The ``multi`` reading mode can be useful when reanalyzing SEED output poses, without having to 
concatenate the output files, or when running with a limited number of MPI processes, as with ``single`` 
the master rank only reads and dispatches molecules without doing any computation.
For the serial version the chosen reading mode is inconsequential as only one process will be started.
    
In general the SEED parallel execution is targeted towards the use on a cluster; 
We therefore provide a template submission script ``seed_sbatch.template.sh`` 
for systems using the `SLURM <https://slurm.schedmd.com/>`_ job scheduler. 
