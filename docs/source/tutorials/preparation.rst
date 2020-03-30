Preliminary words
=================

This set of tutorials constitutes our "recipe cookbook" explaining how to 
prepare the input files (receptor and fragment library) for SEED. 
This is not by any means the only way to do it, 
and you probably can find better ways yourself, especially if you are familiar with 
preparing structural inputs for docking or Molecular Dynamics software. 

In the following we will make use of many custom scripts (you can find them 
in the ``scripts`` folder) and rely on external software, most of which is
open source or free for academics. In particular, for specific tasks, we will use:

* `ChemAxon <https://chemaxon.com/>`_ calculators
* `obabel <https://openbabel.org/docs/dev/Command-line_tools/babel.html>`_ 
* `RDKit <http://www.rdkit.org/docs/GettingStartedInPython.html>`_ Python API 
* `CGenFF <http://silcsbio.com/>`_ for parametrizing molecules with the CHARMM General Force Field
* `CHARMM <http://charmm.chemistry.harvard.edu/charmm_lite.php>`_ 
* The `psfgen <http://www.ks.uiuc.edu/Research/vmd/plugins/psfgen/>`_ plugin of VMD 
* `UCSF Chimera <https://www.cgl.ucsf.edu/chimera/download.html>`_
* `CORINA <https://www.mn-am.com/products/corina>`_
* `CAMPARI <http://campari.sourceforge.net/>`_ software package

Most of the tasks for which we suggest one of these softwares can be carried out 
with alternative ones that you might be more comfortable with. Thus you do not 
need to have all the software from the list installed on your system. 

Before starting the input preparation tutorials read about the :ref:`input_files` to 
understand the format for structural input read by SEED.

To run the code provided in this tutorial as is, you should set the 
``PYTHONSCRIPTS`` variable to ``"SEED/scripts/python"``.

**Note** that the Python scripts we provide were written in Python 2. A few of them
will require minor modifications to be run with Python 3.

.. _lig_from_pdb:

Ligand Preparation from a PDB 
=============================

In this tutorial we will learn how to prepare a ligand for SEED docking 
starting from a PDB file. This can be useful in a typical redocking experiment
scenario.
Let us assume for this case that you know the protonation state and the conformation 
of your ligand and you don't want to change them.

After you have extracted the ligand from the pdb file, convert it to mol2 format:

.. code-block:: bash
  
  obabel -ipdb lig.pdb -p 7.4 -omol2 > lig.mol2
  
Check carefully the protonation state and verify that you are really getting 
the molecule you want as the automatic detection of bonds from a pdb file is not 
always correct.

Now clean the mol2 file (the script gives unique names to atoms, set the 
residue number to 1 and the name to *LIG*):

.. code-block:: bash
  
  python ${PYTHONSCRIPTS}/mol2_cleaner.py lig.mol2

Generate the CGenFF parameters for the ligand:

.. code-block:: bash
  
  cgenff lig.mol2 > lig.str

Combine ``lig.mol2`` with the charges and atom types from the CGenFF stream file
``lig.str``:

.. code-block:: bash
  
  python ${PYTHONSCRIPTS}/mol2ori_to_mol2seed4_cgenff4_singlefiles.py lig.mol2 lig.str out.mol2

Protein Preparation
===================

In this tutorial we will learn how to prepare a receptor for SEED docking.
We assume here that the starting point is a protein from the PDB 
with a ligand inside the protein binding pocket and some structural water molecules 
we want to keep as part of the receptor. 
In order to start, select the chains that interest you, and save the different 
components in your system in three separate files: 

#.  ``protein.pdb`` 
#.  ``waters.pdb`` containing explicit waters you want to keep
#.  ``ligand.pdb`` containing a ligand, in order to minimize the protein 
    in presence of the ligand.
    
Prepare the ligand mol2 and stream file as in Tutorial :ref:`lig_from_pdb`.  Here we also reconvert 
the mol2 used for generating the stream file to a PDB 
(so that the residue name, indices, atom names, ... are identical in the PDB and 
STR we will give to CHARMM)

.. code-block:: bash
    
  obabel -imol2 lig.mol2 -opdb > lig.pdb

As in this tutorial we will use CHARMM for the protein preparation we first need 
to have a CHARMM-proof PDB file (CHARMM naming convention for atom and residues): 

* Extract the protein chain of interest and run ``psfgen`` on it.
  psfgen is a plugin for VMD and information about it can be found at `<http://www.ks.uiuc.edu/Research/vmd/plugins/psfgen/>`_ 

  The input file for psfgen ``psf_protein.gen`` (I/O names and paths to be adapted) 
  is in ``scripts/psfgen_files``.
  To avoid any problems with CHARMM, renumber residues from 1. 
  (you can use the python script ``PDB_cleaner_protein.py`` for this task).

* Prepare a CHARMM PDB file of the explicit structural water molecules: 
  this is the same as for the protein, but using ``psf_waters.gen`` as input to psfgen.
  Once again, to avoid any problems, renumber residues from 1.

The described steps can be done by running:

.. code-block:: bash
  
  python ${PYTHONSCRIPTS}/PDB_cleaner_protein.py prot.pdb out.pdb
  psfgen psf_protein.gen > log_protein
  psfgen psf_waters.gen > log_waters
  
Good alternatives to prepare a CHARMM-proof receptor file are to use the 
`CHARMM-GUI <http://www.charmm-gui.org/>`_ web server 
or the `CAMPARI <http://campari.sourceforge.net/>`_ software package (using keyword
PDB_W_CONV 3 to write CHARMM compatible pdb files).

Now run the minimization with CHARMM (see file ``H_min_rdie.inp``).
Watch out to replace all the I/O names and paths properly in the file.
Also pay attention to set the correct number of water molecules.

.. code-block:: bash
  
  charmm < H_min_rdie.inp > log

Extract the protein and water chains from ``receptor_min.pdb`` and convert it 
to a mol2 file (we use UCSF Chimera for this task, as up to our knowledge, 
UCSF Chimera is one of the only few free tools that can "perfectly" handle a mol2 
file of a protein).

.. code-block:: bash
  
  chimera prot_water.pdb 

In the GUI of Chimera click on File => Save Mol2. Be sure to check 
"Use untransformed coordinates" and nothing else;
then enter the file name and save.

Always check that you have what you expected and wanted in your output!

Now run the script to add the atom types and charges taken from
CHARMM force field and contained in the file ``top_all36_prot.rtf``.

.. code-block:: bash
  
  python ${PYTHONSCRIPTS}/mol2tripos_to_seed_protein.py prot_water.mol2 ${CHARMMFILES}/top_all36_prot.rtf out_forseed.mol2

Pay attention that this script retrieves atom types and charges 
ONLY if the atom names in the mol2 file are consistent with the CHARMM topology file.
TIP3 waters are also recognized, but the termini are not. 
This means that for the moment you have to fix by hand the few atom types 
and atom charges corresponding to your termini (charged, capped...). 
An alternative is to copy atom names and partial charges to the mol2 file taking 
them directly from the psf topology file generated by psfgen.

Library Preparation
===================

In this tutorial we will learn how to prepare a chemical library for a 
prospective docking campaign with SEED. 
Before starting with the actual steps of the preparation there are a few 
preliminary points which should be taken into account:

*   Choose your library carefully and tailor it to your needs. 
    Consider what you are interested in: small fragments? 
    interaction with which side chains? charged molecules? 
    |  Most of the times it makes sense to pre-filter the library before running the docking, as the less 
    noise, the better results. Does it make sense to have compounds with 7 rotatable bonds? 
    With 5 chiral centers? With a logP of 6? With only 3 heavy atoms? Without any rings?
    With aggregator structures, or PAINS?
    
    In general, for the use with SEED, we suggest to choose compounds with: 
    logP <= 3, rotatable bonds <= 5, N ring > 1, HAC > 5 and < 40.
    
    It is important to note that SEED can perform only rigid fragment docking, 
    hence it does not account for ligand flexibility. The workaround to this is to 
    pre-generate multiple conformers for each ligand and dock all of them separately. Of course
    this can work reasonably well for molecules with only a few rotatable bonds.

*   The most common format for chemical libraries is SDF and there are a few 
    things to carefully check, according to the source of your library:
    Are your compounds named properly in the SDF file? It is easier to solve 
    this before preparing the library than afterwards. Are there any unwanted elements, such 
    as counter salts or mixtures in a single molecule? 
    Do you have chiral centers? If yes, is the chemical library selling enantiopure compounds or solely mixtures?
    If it is a mixture, do you have all stereoisomers in the original library or should you generate them?

*   Never trust what you have prepared. Painfully double check as much as you can.
    That includes opening the output file in a text editor and verify it complies to 
    the format you have decided to output (traditionally SDF for a normal library 
    or MOL2 for screening with SEED); extracting n random molecules (10:sup:`2`) and 
    visualizing them in PyMOL. If it does not display them properly, they are probably not compliant 
    with the format. Always remember that most docking software are not very 
    tolerant to mistakes in the input format.

In order to make it easier to trace back results from output to input,
we highly recommend to give unique names to the fragments in the input library.
Our convention (which we will follow in this tutorial) is to append to the fragment 
name an index for the tautomer/protomer and an index for the conformer.

As starting point we assume to have a collection of ligands in a single SDF 
file without defined conformations and protonation states.

#.  We first use ChemAxon `MolConverter <https://chemaxon.com/marvin-archive/latest/help/applications/molconvert.html>`_ 
    to remove counterions from the molecules.

    .. code-block:: bash
  
      molconvert -F sdf library.sdf -o library_nosalt.sdf
  
#.  Then we proceed by generating tautomers with ChemAxon and keep only 
    the ones with occupancy above a threshold (in this example 24.9%):

    .. code-block:: bash
      
      cxcalc dominanttautomerdistribution -H 7.2 -f sdf -t "tauto_occupancy" library_nosalt.sdf > tautodistrib.sdf
      python ${PYTHONSCRIPTS}/sdf_select_bytag_nordkit.py tautodistrib.sdf maintauto.sdf 24.9
      
    Note that the Python script appends "tauto_number" to the molecule name so that it is unique.
#.  Now we generate the conformers and prune the ones which are too similar 
    in terms of RMSD:

    .. code-block:: bash
      
      mkdir 100conf_075rmsd/
      python ${PYTHONSCRIPTS}/sdf_conformergen_outsplit.py maintauto.sdf 100 0.75 100conf_075rmsd/ 4
      cd 100conf_075rmsd/ ; ls | sed 's/.sdf//g' > ../conformers_tautomers_original_full.list ; cd ..
    
    Note that this script outputs each structure separately.
#.  Convert the SDF files to MOL2. We use CORINA for this task but you can choose 
    any alternatives you prefer:

    .. code-block:: bash
       
      mkdir mol2_split
      for i in `cat conformers_tautomers_original_full.list`; do
        corina -i t=sdf 100conf_075rmsd/${i}.sdf -o t=mol2 -d no3d -d newtypes -o fcharges |egrep -v "\#" | awk 'NF'  > mol2_split/${i}.mol2
      done
      
    We now tar the temporary folder as we will not need it in what follows:

    .. code-block:: bash
      
      tar -zcf 100conf_075rmsd_split.tgz 100conf_075rmsd/ && rm -r 100conf_075rmsd/
      
#.  We can now generate the  CGenFF parameters. In order to save time we generate just the parameters 
    for ONE conformer of each tautomer, as in the fixed charge model we use, parameters 
    do not depend on the conformation 
    (but of course different tautomers of the same molecules need different parameters).
    First we write the list of unique tautomers:

    .. code-block:: bash
      
      egrep "_conf_1$" conformers_tautomers_original_full.list > original_tautomers_firstconf.list
      
    Then we can generate the parameters:

    .. code-block:: bash
      
      mkdir cgenff_param
      for i in `cat original_tautomers_firstconf.list`; do
        a=`echo $i | sed 's/_conf_1//g'`
        cgenff mol2_split/${i}.mol2 > cgenff_param/${a}.str
      done &> /dev/null
      
    For molecules which did not go through parametrization succesfully, 
    CGenFF generates non-empty files with empty parameters, and we should get rid of them.
    In addition, for further putative use in CHARMM, it may turn useful to rename the
    residue name to the standard identifier "LIG".

    .. code-block:: bash
      
      cd cgenff_param
      mkdir ../cgenff_clean
      for i in *.str ; do 
        [[ `egrep "RESI" ${i}` ]] && sed -r 's/RESI ......../RESI LIG     /g' ${i} > ../cgenff_clean/${i}
      done &> /dev/null
      
      cd ../cgenff_clean ; ls | sed 's/.str//g' > ../tautomers_firstconf_cgenffparam.list ; cd ../
      for i in `cat tautomers_firstconf_cgenffparam.list` ; do 
        grep $i conformers_tautomers_original_full.list ; 
      done > tautomers_conformers_cgenffparam.list
      
#.  Finally we create the MOL2 library file for SEED:

    .. code-block:: bash
      
      mkdir mol2seed
      for i in `cat tautomers_conformers_cgenffparam.list` ; do
        a=`echo $i | sed -r 's/_conf_[0-9]*//g' `
        python ${PYTHONSCRIPTS}/mol2ori_to_mol2seed4_cgenff4_singlefiles.py mol2_split/${i}.mol2 cgenff_clean/${a}.str mol2seed/${i}_seed.mol2
      done
      
    At this point different conformer of the same fragment (or tautomer)
    have the same name . To avoid any ambiguity we rename them in the MOL2 file and 
    as a final step we reconcatenate all the files into a unique one.

    .. code-block:: bash
      
      cd mol2seed ; for i in *; do sed -i "s/${i%_conf*}/${i%_seed.mol2}/" $i; done; cd ..
      cd mol2seed ; for i in * ; do cat $i >> ../library_seed.mol2 ; done ; cd ..

The chemical library is now ready to be docked by SEED.
The steps for this tutorial can be run all together using the bash script ``library_preparation.sh``.


      
    
      
    
