Getting Started
===============

After the :ref:`installation` and familiarizing with the most important 
:ref:`input_files` and :ref:`output_files`, the best way to get started with 
SEED is to try to replicate some :ref:`Test Cases <test_cases>` and start 
understanding the main parameters with the help of the :ref:`par_generator`.

.. _installation:

Installation 
------------
  
SEED code is hosted on `GitLab <https://gitlab.com/CaflischLab/SEED>`_.

In order to build the latest development version of SEED do the following 
(you may have to modify the ``Makefile.local`` in the ``src`` folder):

.. code-block:: bash
    
    git clone https://gitlab.com/CaflischLab/SEED
    cd SEED/src 
    make seed
    
SEED makes use of the 
`BOOST C++ Libraries <https://www.boost.org/>`_. 
The necessary header files are distributed along with SEED.
The binary is compiled into the ``SEED/bin`` folder.

After installation run SEED with the following command:

.. code-block:: bash
  
  seed_4 seed.inp >& log

.. _input_files:

Input Files 
-----------

The following is a list of the input files required for a SEED run:

* The file ``seed.inp`` contains the most frequently modified input values 
  (path and name of structural input files, list of residues forming the 
  binding pocket, switch between polar and apolar docking, ...). For detailed 
  information see :ref:`input_param`.
* The parameter file ``seed.par`` (filename can be specified in :ref:`i1<i1>`) 
  contains less frequently modified 
  input/output options, parameters for docking, energy and clustering. Modification
  of some parameters in this file is recommended only to advanced users who wish 
  to fine tune the SEED energy model.

  Both ``seed.inp`` and ``seed.par`` allows comment lines (starting with a ``#``) 
  and the files must terminate with the word ``end``. All the other lines hold information read by the program.
  It is important that this information is provided in the correct order for parsing by the program. 
  In this documentation and in the pdf user manual, parameters referring to the input 
  and parameter files are indicated by **i** and **p** respectively.
  For a detailed description refer to :ref:`all_param`. 
  For a full working example of the input and parameter file try out the 
  :ref:`par_generator` and the test cases in the :ref:`tutorials` section.
* A standard SYBYL mol2 format file containing all the fragments to dock.
  This file is simply the concatenation of all the fragments, expressed in mol2 format. 
  Note that different conformations of the same fragment are treated as different fragments.
  Partial charges are written in the 9th column in the ``@<TRIPOS>ATOM`` record.
  
  In order to retrieve the correct van der Waals parameters from ``seed.par`` file (block :ref:`p29<p29>`), 
  the CHARMM atom types should be specified in the mol2 file. 
  This is done using the alternative atom type specified by the record ``@<TRIPOS>ALT_TYPE``, 
  which takes the following form:
  ::

    @<TRIPOS>ALT_TYPE
    CGenFF_4.0_ALT_TYPE_SET
    CGenFF_4.0 1 CG331 2 CG301 3 CG331 4 CG324 ...

  Where ``CGenFF_4.0_ALT_TYPE_SET`` sets a user-defined name 
  (for example ``CGenFF_4.0``) for the alternative atom type set. 
  This name is repeated on the next line, followed by the list of "atom number-atom type" pairs 
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
  9th column in the ``@<TRIPOS>ATOM`` record (as for the fragments) and CHARMM atom types specified 
  by the ``@<TRIPOS>ALT_TYPE`` record (refer to the fragment file description for details).

.. _output_files:
  
Output Files 
------------

The main SEED output file, whose filename is specified in :ref:`p6<p6>` (by default ``seed.out``), 
contains detailed information about the energy values 
(with both fast and accurate model) and results of clustering. 
The first term of :ref:`p28<p28>` is the maximal number
of lines that can be written in the main output file for each docking step of each fragment
type. The second term of :ref:`p28<p28>` gives control on which information may be discarded in
the output file (print level).

A directory ``outputs`` in which all the output files are written is 
automatically created by the program. Note that if a directory named ``outputs`` is
already present, it will be overwritten by the SEED run.

``<FragmentMol2FileName>_clus.mol2`` contains the fragment top poses 
per cluster ranked by accurate energy after the postprocessing step. This file is the concatenation 
of a mol2 file for each saved pose. The maximum number of poses to be saved per cluster 
can be set in :ref:`p5<p5>` (first value). The comment line of the SYBYL mol2 
record ``@<TRIPOS>MOLECULE`` (6th line after the record identifier) contains 
some useful information about the pose,
*i.e.* increasing pose index, cluster number, total energy and fragment number
(``Fr_nu``). The latter represents the program internal numbering of the pose and 
it is not interesting *per se*, but it can be used to match the pose 
to docking information written in ``seed.out``.

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
according to the total binding energy, irrespective of the cluster they belong to 
(maximum number of saved poses set by :ref:`p5<p5>`, second value). The difference with respect to 
``<FragmentMol2FileName>_clus.mol2`` is that the user can set the total number 
of poses to be saved instead of the number of cluster members.

``seed_best.dat`` is the same as ``seed_clus.dat`` but matching 
``<FragmentMol2FileName>_best.mol2``.

The writing of the above ``*_clus.mol2`` and ``*_best.mol2`` files is activated or deacti-
vated by :ref:`p3<p3>` (first and second value respectively). The writing of the ``seed_clus.dat`` and
``seed_best.dat`` summary table is activated or deactivated by :ref:`p4<p4>` 
(first and second value respectively). Note that the maximum number of poses and 
poses per cluster to be saved (:ref:`p5<p5>`) are upper bounds as the number of generated poses
may be smaller than the number of poses requested in output. The four parameters
for writing the output files (:ref:`p3<p3>` and :ref:`p4<p4>`) can be switched on/off independently.

Note that the number of cluster members to be saved (first value of :ref:`p5<p5>`) 
implicitly determines the maximum number of poses for which to evaluate the accurate binding energy. 
Thus in general it is advisable to set this number to a value higher than one, 
in order to be sure to consider a meaningful number of poses, 
and to suppress the corresponding mol2 file output (first value of :ref:`p3<p3>` set to ``n``) 
as it may quickly become big.

Other output files 
^^^^^^^^^^^^^^^^^^

Besides the docking output files containing structural information and energy values,
SEED generates some additional output files. 

The grids for the evaluation of fast van der Waals energy, 
fast screened interaction energy and receptor desolvation can be saved on disk and 
reused for a subsequent run (see :ref:`p7<p7>`, :ref:`p8<p8>`, :ref:`p9<p9>`). 
The grid files are saved by default in the ```scratch`` subfolder.

When a new project is started, it can be very useful to first generate and 
visualize the vectors used for ligand placement, before performing any 
docking (see :ref:`vectors` for details).
Vectors are saved in the following mol2 files and can be opened in a molecular 
viewer:

* ``polar_rec.mol2`` contains vectors distributed uniformly on a spherical region 
  around each ideal H-bond direction. The deviation from ideal hydrogen bond geometry 
  and the number of additional vectors to distribute uniformly on the spherical region 
  are set in :ref:`p12<p12>`.

* ``polar_rec_reduc_angle.mol2`` contains vectors of ``polar_rec.mol2`` which 
  are selected according to an angle criterion (:ref:`i4<i4>`, :ref:`p14<p14>`). Vectors pointing 
  outside of the binding site are discarded. The file ``polar_rec_reduc_angle.mol2`` 
  exists only if the angle criterion has been activated by the user (:ref:`i4<i4>`).

* ``polar_rec_reduc.mol2`` contains vectors of ``polar_rec.mol2`` 
  (or of ``polar_rec_reduc_angle.mol2`` if the angle criterion has been activated (:ref:`i4<i4>`)) 
  which are selected according to favorable van der Waals interaction between 
  all the receptor atoms and a spherical probe on the vector extremity. 
  The aim is to discard receptor vectors that point into region of space occupied 
  by other atoms of the receptor and select preferentially vectors in the concave 
  regions of the receptor. The van der Waals radius of the probe is specified 
  in :ref:`p15<p15>`. The number of selected vectors is controlled with :ref:`p2<p2>`. 

* ``apolar_rec.mol2`` contains points distributed uniformly on the solvent-accessible 
  surface of the receptor. The density of surface points is set in :ref:`p22<p22>`.

* ``apolar_rec_reduc_angle.mol2`` contains vectors of ``apolar_rec.mol2`` which 
  are selected according to an angle criterion (:ref:`i4<i4>`, :ref:`p14<p14>`). Vectors pointing 
  outside of the binding site are discarded. The file ``apolar_rec_reduc_angle.mol2`` 
  exists only if the angle criterion has been activated by the user (:ref:`i4<i4>`).

* ``apolar_rec_reduc.mol2`` contains points of ``apolar_rec.mol2``. 
  (or of ``apolar_rec_reduc_angle.mol2`` if the angle criterion has been 
  activated (**i4**)) which are selected according to their hydrophobicity. 
  For this purpose a low dielectric sphere is placed on each of these points. 
  The hydrophobicity is defined as the weighted sum of the receptor desolvation 
  energy due to the presence of the probe and the probe/receptor van der Waals 
  interaction. 
  The weighting factors and the probe radius are set in :ref:`p22<p22>`. The number of 
  selected apolar points is controlled with :ref:`p2<p2>`.

Of the six files listed above one should visualize 
``polar_rec_reduc.mol2`` and ``apolar_rec_reduc.mol2``. It is 
useful to modify the appropriate parameters if the vector distributions 
do not meet the user's expectation, since fragments are docked using the vectors 
present in these files. 
As soon as the you are happy with the generated vectors, you can just read the maps 
(first value of :ref:`p7<p7>`, :ref:`p8<p8>`, :ref:`p9<p9>` set to ``r``) 
instead of generating and writing them again (first value set to ``w``).

The file ``sas_apolar.pdb`` contains points defining the solvent accessible 
surface of the binding site, which can be visualized with a molecular viewer.
  
Troubleshooting
---------------

If after starting a SEED run the program exits unexpectedly, the 
keyword ``WARNING`` should be looked for in the main output file 
(``seed.out``, :ref:`p6 <p6>`) to find hints on possible problems 
(wrong path for filenames, unknown value for some parameters, ...).

The docking workflow implemented in SEED involves many filtering steps, hence, 
if the main output file does not contain any fragment position 
for a given fragment type, it can be due to several reasons: 
the center of the spherical cutoff (:ref:`i6<i6>`) might be misplaced 
(outside the binding site), the checking of steric clashes (:ref:`p10<p10>` and 
:ref:`p11<p11>`) too strict, 
the van der Waals energy cutoff (:ref:`p19<p19>`) for apolar fragments too severe, 
the total energy cutoff (third value of :ref:`i7<i7>`), or the 
energy cutoff for the second clustering (fourth value of :ref:`i7<i7>`) too stringent. 
To find out what the reason could be, the following part of the main output file should 
be investigated:

| ``Total number of generated fragments of type 1 (BENZ) : 118800``
| ``Fragments that passed the sphere checking : 102894``
| ``Fragments that passed the bump checking : 49007``
| ``Fragments that passed the vdW energy cutoff : 22100``
| ``Fragments that passed the total energy cutoff : 17794``

Parallel SEED
-------------

SEED has an MPI-parallel version. If you want to run a SEED screening campaign 
on a cluster, refer to :ref:`parallel`.

MC minimization
---------------

Stochastic minimization with a rigid-body Monte Carlo Simulated Annealing scheme can
be enabled to run on the top generated poses; these are the poses for which 
the accurate binding energy is evaluated and their maximum number (per cluster) 
of can be tuned with the first value of :ref:`p5<p5>`.

MC minimization can be used in both :ref:`dock-runmode` and :ref:`energy-runmode`. 
The latter can be useful when rescoring poses not generated by SEED. In this case, 
if you had to run rescoring without minimizing the poses, you might get very unfavourable
energy values due to the ruggedness of the van der Waals energy landscape.

If you want to know more details, refer to :ref:`mc_minimization`.
