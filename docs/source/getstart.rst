Getting Started
===============

Installation 
------------

SEED makes use of the 
`BOOST C++ Libraries <https://www.boost.org/>`_
. The necessary header files are distributed along with SEED.

In order to install SEED, cd to directory ``src`` and make with the following command 
(you may have to modify the ``Makefile.local``):

.. code-block:: bash
    
    make seed
    
The binary is compiled into the ``bin`` directory.
After installation run SEED with the following command:
::
  
  ./seed_4 seed.inp >& log

Input Files 
-----------

The following is a list of the input files required for a SEED run:

* The file ``seed.inp`` contains the most frequently modified input values 
  (path and name of structural input files, list of residues forming the 
  binding pocket, switch between polar and apolar docking, ...).
* The parameter file ``seed.par`` contains less frequently modified 
  input/output options, parameters for docking, energy and clustering. Modification
  of some of these parameters is recommended only to advanced users who wish 
  to fine tune the energy model.

  The both ``seed.inp`` and ``seed.par`` have comment lines which start with a ``#`` 
  and terminate with the word ``end``. All the other lines are information read by the program.
  It is important that this information is provided in the correct order for parsing by the program. 
  In the online documentation and the pdf user manual, parameters referring to the input 
  and parameter files are indicated by **i** and **p** respectively. 
  For a full working example of the parameter file refer to the tutorial test cases provided.
* A standard SYBYL mol2 format file containing all the fragments to dock.
  This file is simply the concatenation of all the fragments, expressed in mol2 format. 
  Note that different conformations of the same fragment are treated as different fragments.
  Partial charges are written in the 9th column in the ``@<TRIPOS>ATOM`` record as for the receptor.
  
  In order to assign the correct Van der Waals parameters from the file ``seed.par``, 
  the CHARMM atom types should be specified in the mol2 file. 
  This is done using the alternative atom type specified by the record ``@<TRIPOS>ALT_TYPE``, 
  which takes the following form:
  ::

    @<TRIPOS>ALT_TYPE
    CGenFF_4.0_ALT_TYPE_SET
    CGenFF_4.0 1 CG331 2 CG301 3 CG331 4 CG324 ...

  Where ``CGenFF_4.0_ALT_TYPE_SET`` sets a user-defined name 
  (for example ``CGenFF_4.0``) for the alternative atom type set. 
  This name is repeated on the next line, followed by the list of atom number-atom type pairs 
  for each atom in the molecule. This list should span a single line, 
  but can be broken by using ``\\``.
  It is recommended to keep the SYBYL atom types on the 6th column of the 
  record ``@<TRIPOS>ATOM`` as they are recognized by most cheminformatics 
  and visualization software. 
  The first line of the SYBYL record ``@<TRIPOS>MOLECULE`` specifies the fragment name. 
  It is convenient (but not necessary) to have unique names for each fragment. 
  In case fragments with duplicate names are found in the input, 
  they will be renamed in all the output files appending to their name the dollar sign $ 
  and an incremental index.
  As the fragment mol2 input file is read sequentially, 
  the number of fragments in it does not have to be specified a priori.

* A standard SYBYL mol2 file for the receptor with partial charges on the 
  9th column in the ``@<TRIPOS>ATOM`` record and CHARMM atom types specified 
  by the ``@<TRIPOS>ALT_TYPE`` record (refer to the fragment file description for details).
  
Output Files 
------------

The main SEED output file, whose filename is specified in **p6** (by default ``seed.out``), 
contains the energy values and results of clustering.

A directory ``outputs`` in which all the output files are written is 
automatically created by the program. Note that if a directory named ``outputs`` is
already present, it will be overwritten by the SEED run.

``<FragmentMol2FileName>_clus.mol2`` contains the fragment positions with 
best energy after the postprocessing step. This file is the concatenation 
of a mol2 file for each saved pose. The maximum number of poses to be saved per cluster 
can be set in **p5** (first value). The comment line of the SYBYL mol2 
record ``@<TRIPOS>MOLECULE`` (6th line after the record identifier) contains 
some useful information about the pose,
*i.e.* increasing pose index, cluster number, total energy and fragment number
(``Fr_nu``). The latter represents the program internal numbering of the pose and 
it is not interesting *per se*, but it can be used to match the pose 
to docking information written in the main output file.

``seed_clus.dat`` is a summary table containing the separate energy terms for 
each fragment position saved to ``<FragmentMol2FileName>_clus.mol2``. 
This information can be also retrieved from the main output file. 
Columns are organized as follows:

* **Name**: Fragment name.
* **Pose**: Incremental pose number. This index restarts at 1 for each new fragment.
* **Cluster**: Cluster number.
* **Fr_nu**: Fragment number. This is SEED internal pose number.
* **Tot**: Total binding energy.
* **ElinW**: Electrostatic interaction in water.
* **rec_des**: Desolvation of the receptor upon complex formation.
* **frg_des**: Desolvation of the fragment upon complex formation.
* **vdW**: Van der Waals interaction energy.
* **DElec**: Electrostatic difference upon fragment binding. It is given by *ElinW-DG_hydr*. 
  It roughly represents how good the fragment feels in the protein compared to 
  how good it feels in water. 
* **DG_hydr**: Free energy of hydration of the fragment.
* **Tot_eff**: *Tot/HAC*.
* **vdW_eff**: *vdW/HAC*.
* **Elec_eff**: *ElinW/HAC*. 
* **HAC**: Heavy atom count. It is the total number of non-hydrogen atoms in the fragment.
* **MW**: Molecular weight of the fragment.

``<FragmentMol2FileName>_best.mol2`` contains the best fragment positions, 
according to the total energy, irrespective of the cluster they belong to 
(parameter **p5**, second value). The difference with respect to 
``<FragmentMol2FileName>_clus.mol2`` is that the user can set the total number 
of poses to be saved instead of the number of cluster members.

``seed_best.dat`` is the same as ``seed_clus.dat`` but matching 
``<FragmentMol2FileName>_best.mol2``.

Note that the number of cluster members to be saved (first value of **p5**) determines 
the maximum number of poses for which to evaluate the slow energy during postprocessing. 
Thus in general it is advisable to set this number to a value higher than one, 
in order to be sure to consider a meaningful number of poses, 
and to suppress the corresponding mol2 file output (first value of **p3** set to *no*) 
as it may quickly become big.


Starting a New Project
----------------------

When a new project is started, it is useful to first generate the vectors 
without docking any fragment (**i7** set to *d*). 
Of the six files listed below one should visualize the two files 
``polar_rec_reduc.mol2`` and ``apolar_rec_reduc.mol2``. It is 
useful to modify the appropriate parameters if the vector distributions 
do not meet the user's expectation, since fragments are docked using the vectors 
present in the two aforementioned files. 
After this test one has just to read the maps (**p7** and **p8** set to *r*) 
instead of generating them again.

* ``polar_rec.mol2`` contains vectors distributed uniformly on a spherical region 
  around each ideal H-bond direction. The deviation from ideal hydrogen bond geometry 
  and the number of additional vectors to distribute uniformly on the spherical region 
  are set in **p12**.

* ``polar_rec_reduc_angle.mol2`` contains vectors of ``polar_rec.mol2`` which 
  are selected according to an angle criterion (**i4**, **p14**). Vectors pointing 
  outside of the binding site are discarded. The file ``polar_rec_reduc_angle.mol2`` 
  exists only if the angle criterion has been activated by the user (**i4**).

* ``polar_rec_reduc.mol2`` contains vectors of ``polar_rec.mol2`` 
  (or of ``polar_rec_reduc_angle.mol2`` if the angle criterion has been activated (**i4**)) 
  which are selected according to favorable van der Waals interaction between 
  all the receptor atoms and a spherical probe on the vector extremity. 
  The aim is to discard receptor vectors that point into region of space occupied 
  by other atoms of the receptor and select preferentially vectors in the concave 
  regions of the receptor. The van der Waals radius of the probe is specified 
  in **p15**. The number of selected vectors is controlled with **p2**. 

* ``apolar_rec.mol2`` contains points distributed uniformly on the solvent-accessible 
  surface of the receptor. The density of surface points is set in **p22**.

* ``apolar_rec_reduc_angle.mol2`` contains vectors of ``apolar_rec.mol2`` which 
  are selected according to an angle criterion (**i4**, **p14**). Vectors pointing 
  outside of the binding site are discarded. The file ``apolar_rec_reduc_angle.mol2`` 
  exists only if the angle criterion has been activated by the user (**i4**).

* ``apolar_rec_reduc.mol2`` contains points of ``apolar_rec.mol2``. 
  (or of ``apolar_rec_reduc_angle.mol2`` if the angle criterion has been 
  activated (**i4**)) which are selected according to their hydrophobicity. 
  For this purpose a low dielectric sphere is placed on each of these points. 
  The hydrophobicity is defined as the weighted sum of the receptor desolvation 
  energy due to the presence of the probe and the probe/receptor van der Waals interaction. 
  The weighting factors and the probe radius are set in **p22**. The number of 
  selected apolar points is controlled with **p2**.
