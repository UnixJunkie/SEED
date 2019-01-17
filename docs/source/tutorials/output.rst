Output analysis 
===============

Most of the times, after carrying out a docking run, we would like to 
rank the poses according to some criterion (for example the total binding energy)
and extract the structures corresponding to the best scoring ones.

To do this we first sort the SEED output table ``seed_best.dat`` with respect
to the total energy (note that we also remove the header): 

.. code-block:: bash
  
  tail -n -2 seed_best.dat | sort -n -k5 > seed_best_sorted.dat  

And then extract the best ``N`` poses (according to the sorted table) from the 
concatenated mol2 file ``seed_best.mol2`` into ``N`` separate mol2 files:

.. code-block:: bash 

  python ${PYTHONSCRIPTS}/separate_poses.py -i seed_best.mol2 -t seed_best_sorted.dat -n N
