.. _all_param:

Parameters 
==========

SEED user-modifiable parameters are contained in two main input files.
| The file ``seed.inp`` contains the most frequently modified :ref:`input_param`
as they regard a specific SEED run (path and name of structural input files, 
list of residues forming the binding pocket, 
switch between polar and apolar docking, ...).

The :ref:`par_param` ``seed.par`` contains 
less frequently modified input/output options, parameters for docking, 
energy and clustering. Modification of most of these parameters is recommended only 
to advanced users who wish to fine tune the energy model.

.. _input_param:

Input Parameters
----------------

Here we define all the parameters of the ``seed.inp`` file.

.. _i1:

**i1**
  | first line: name of parameter file (``seed.par``) 
  | second line: name of the keyword-based parameter file (``seed_kw.par``)
  
.. _i2:

**i2**
  name of coordinate file for the receptor (in SYBYL mol2 format)

.. _i3:

**i3**
  | Binding site residue list.
  | First line: number of residues in the binding site.
  | Following lines: residue indices (one per line). 
  | Note that residues are renumbered sequentially starting from 1 within SEED 
    and the residue index refers to this new numbering; if for example 
    ARG38 is the first residue of the protein, its index is 1 and not 38. The SEED 
    residue indices can be retrieved from ``seed.out`` after the line 
    ``Data for the receptor :``.
    To avoid ambiguity we recommend to renumber the 
    residues starting from 1 in the input file. Binding site metal ions have to 
    be in the list as well. 

.. _i4:
  
**i4**
  | List of points (*e.g.* ligand heavy atoms of a known ligand-receptor 
    complex structure) in the binding site used to select polar and apolar 
    receptor vectors which satisfy the angle criterion (see :ref:`angle_criterion`) 
    and discard vectors pointing outside of the binding site.
  | First line:  number of points (``0``: no removal of vectors using the angle criterion).
  | Following lines: coordinates of the points (one point per line).

.. _i5:

**i5**
  | Vectors for the metal ions in the binding site.
  | Make sure that the residue number of the metal is in the
    binding site residue list. 
  | First line:  total number of coordination points.
  | Following lines:  atom number of metal / x y z of coordination point (vector extremity)

.. _i6:

**i6**
  | Spherical cutoff for docking:
  | coordinates of the center and radius of a sphere in which the geometry center of
    the fragment position must be in order to be accepted. 
    This filter can be discarded by selecting ``n`` instead of ``y`` as first value.
  | ``y``, ``n`` / sphere center / sphere radius

.. _i7:
  
**i7**
  | Fragment library specifications
  | **First line**: one character specifying the running mode of SEED: 
    :ref:`dock-runmode` (``d``) or only :ref:`energy-runmode` (``e``).
  | **Second line**: the first column contains the path of the fragment mol2 file 
    and the second column allows the selection of apolar, polar docking or both 
    (``a``, ``p``, ``b``). The fragment position is accepted if the total energy 
    (according to the fast energy model) is smaller than a cutoff given in the third column. 
    The second :ref:`clustering` is applied on the poses
    for which the binding energy of the cluster representative is smaller than a cutoff value
    specified in the 4th column. In summary:
  
  | Fragment library filename - 
    apolar docking, polar docking, or both (``a``, ``p``, ``b``) -
    energy cutoff in kcal/mol - 2nd clustering cutoff in kcal/mol

  | **Third line**: Reading mode, either ``single`` or ``multi``. This option is only relevant
    when using the MPI parallel version and only concerns the way the input mol2 library is read. 
    With ``single`` SEED expects a single mol2 input
    file; molecules are read from this file by the master rank, which dispatches them to the
    first available rank, balancing the computational load among the processes. 
    This is especially important when running Monte Carlo minimization as the variance of the
    running time per molecule can be large.
    With the ``multi`` option each rank reads from a separate mol2 file. This requires the user
    to preemptively split the library into a number of parts equal to the number of ranks. In order
    to relieve the possible load imbalace, whe recommend shuffling the library file before splitting it
    (scripts are provided).
    The ``multi`` option can be useful when reanalyzing SEED output poses, as each rank writes to a 
    separate output mol2 file, or when running with a limited number of MPI ranks, as with ``single`` 
    the master rank only reads and dispatches molecules without doing any conmputation.
    For the serial version the chosen reading mode is inconsequential as only one process will be started.
    
As you do not need to modify all the parameters and in most of the cases 
default values will give good results, we recommend not to write an input 
file from scratch, but to modify a default template. You can do this here through 
the :ref:`par_generator`.

.. _par_param:
  
Parameter File
--------------

Here we define all the parameters of the ``seed.par`` file.

.. _p1:

**p1**
  Dielectric constant of the solute (receptor and fragment)

.. _p2:
  
**p2**
  Ratio of kept vectors for docking : polar / apolar

.. _p3:

**p3**
  | Output control for structure files (two values on the same line).
  | First value: write \*_clus.mol2 file (y/n)
  | Second value: write \*_best.mol2 file (y/n)

.. _p4:

**p4**
  | Output control for energy table files (two values on the same line).
  | First value: write \*_clus.dat summary table file (y/n)
  | Second value: write \*_best.dat summary table file (y/n)

.. _p5:

**p5**
  | Maximum number of saved clusters and poses (two values on the same line).
  | First value: maximum number of cluster members saved in \*_clus\* 
    output files. Note that this value determines the maximum number
    of poses per cluster that go through slow energy evaluation.
    Second value: maximum number of poses saved in \*_best\* output files.
    
.. _p6:

**p6**
  | Filename for output log file. This is the main SEED output file (``seed.out``).
  | The docked fragments are saved in the directory ./outputs         

.. _p7:

**p7**
  write (w) or read (r) Coulombic grid / grid filename

.. _p8:

**p8**
  write (w) or read (r) van der Waals grid / grid filename

.. _p9:

**p9**
  write (w) or read (r) receptor desolvation grid / grid filename 

.. _p10:

**p10**
  | Bump checking: used only for slow energy evaluation (three values)
  | n x atoms = maximum tolerated bumps / 
  | scaling factor for interatomic distance /
  | severe overlap factor (beta factor in PROTEINS paper)

.. _p11:

**p11**
  van der Waals energy cutoff (kcal/mol): 
  this is used as bump checking for the fast energy model.

.. _p12:

**p12**
  Angle (deg) and number of points on the sphere around the ideal 
  hydrogen bonding vector direction.

.. _p13:

**p13**
  Number of fragment rotations around each axis.

.. _p14:

**p14**
  Settings for the reduction of the seeding vectors (four values).
  
  * angle_rmin  if distance <= (multipl_fact_rmin\*minDist)
  * angle_rmax  if distance >= (multipl_fact_rmax\*maxDist)
  * linear dependence (range between angle_rmin and angle_rmax)
    for other distances

.. _p15:

**p15**
  Van der Waals probe radius for removal of the receptor polar vectors.

.. _p16:

**p16**
  | Settings for the Coulombic term in the fast energy model (three values).
  | ``1`` = distance dependent dielectric / grid margin / grid spacing

.. _p17:

**p17**
  | Settings for the van der Waals term in the fast energy model (two values). 
  | grid margin / grid spacing

.. _p18:

**p18**
  | Settings for the van der Waals accurate energy model (two values).
  | nonbonding cutoff / grid spacing 
  
  | Note that the Coulombic cutoff for formal charges is automatically
    set to 1.3 x van_der_Waals_cutoff

.. _p19:

**p19**
  | Multiplicative factor (k) for apolar docking to skip evaluation of 
    electrostatics. The van der Waals energy cutoff is:
  |  k x Number of fragment atoms, including hydrogen atoms

.. _p20:

**p20**
  | Settings for the solvation grid (two values): 
  | grid margin / grid spacing

.. _p21:

**p21**
  | Settings for the solvation term evaluation (three values):
  | water radius for solvation / number of points per sphere to generate SAS / 
    solvent dielectric constant

.. _p22:

**p22**
  | Setting for the Hydrophobicity maps (five values): 
  | point densities (A^-2) on the SAS for apolar vectors on the receptor / on the fragment /
    probe radius to generate SAS for apolar vectors /
    scaling factor for desolvation and / van der Waals interactions

.. _p23:

**p23**
  Scaling factors for fast and also accurate energy evaluation (four values):
  van der Waals / electrostatic interaction / receptor desolvation /
  fragment desolvation

Clustering parameters
^^^^^^^^^^^^^^^^^^^^^

The clustering with GSEAL proceeds in two steps: the
first clustering yields large clusters which contain almost 
overlapping as well as more distant fragments; the second
clustering is done on each cluster found in the first clustering
to eliminate fragments which are very close in space.

.. _p24:

**p24**
  | Non-default similarity weight factors (150 atom elements) for GSEAL: 
  | First line: 0 or number of non-default elements
  | Following lines: list (first element number / second element number / value )

.. _p25:

**p25**
  | Parameters for first clustering (overall clustering):
  | GSEAL similarity exponential factor / cutoff factor

.. _p26:

**p26**
  | Parameters for second clustering (to discard redundant positions):
  | GSEAL similarity exponential factor / cutoff factor

.. _p27:

**p27**
  Maximal number of poses to be clustered

.. _p28:

**p28**
  | Setting for the amount of information to be written to the output ``seed.out``:
  | Maximum number of lines to be written in the output file for the sorted 
    energies and the two clustering procedures / 
  | print level (``0`` = lean, ``1`` = adds sorting before postprocessing, 
    ``2`` = adds 2nd clustering).

Force field parameters
^^^^^^^^^^^^^^^^^^^^^^

.. _p29:

**p29**
  | Van der Waals radius and energy minimum (absolute value).
  | First line: number of records
  | Following lines: each record contains five values:
  | sequential index / atom type / element number / van der Waals radius / 
    van der Waals energy minimum

.. _p30:

**p30**
  | Hydrogen bond distances between donor and acceptor.
  | First  line: Default distance for all atom and element types.
  | First block:
  
  * First line: number of records
  * Following lines: element number i / element number j / donor-acceptor distance 
  
  | Second block:
  
  * First line: number of records
  * Following lines: atom type i / atom type j / donor-acceptor distance

.. _p31:

**p31**
  | List of relative atomic weights.
  | First line: number of elements (without element 0)
  | element name / element number / atomic weight

.. _KW_param:

Keyword-based parameter file
----------------------------

In order to allow more flexibility and easier addition of SEED parameters, 
we have decided to move from the original sequential format of the ``seed.par`` 
to a keyword based format. This, for legacy reasons, only involves the newly 
added settings, so that an older ``seed.par`` can be used as it is without the need 
of modifications or rewritings.
The new keyword based parameters should be specified in the format 
``<keyword> = <value>`` as for example:
::

  # Additional parameters 
  do_mc = y # activates MCSA sampling 
  mc_temp = 500
  mc_max_xyz_step = 0.7 0.1

Comments can be introduced by *#* and will be ignored. Note that some keywords require 
multiple values. If the same keyword is repeated multiple times in the file, the last 
instance will be used.
The additional keyword-based parameter file, that we will refer to as ``seed_kw.par`` 
should always be present (even if blank) and its path has to be specified in the second 
line of :ref:`i1<i1>`.
The keywords that can be set are the following:

.. _MC_param:

Monte Carlo parameters
^^^^^^^^^^^^^^^^^^^^^^

The following parameters are needed for running a Monte Carlo Simulated Annealing
minimization of the top poses.
This option can be enabled by setting :ref:`do_mc<do_mc>` to ``y`` (yes) and adding 
the following related keywords.
If :ref:`do_mc<do_mc>` is set to ``n`` (no), all the additional MC parameters in this section 
play no role.

.. _do_mc:

**do_mc**
  | Perform MC refinement? (``y`` / ``n``)

.. _mc_temp:

**mc_temp**
  | Starting temperature of MC run.

.. _mc_max_xyz_step:

**mc_max_xyz_step**
  | Maximum rigid body translation step (in Angstrom): coarse (1st value) 
  | and fine (2nd value) moves.

.. _mc_max_rot_step:

**mc_max_rot_step**
  | Maximum rigid body rotation step (in degrees): coarse (1st value) 
  | and fine (2nd value) moves.

.. _mc_rot_freq:

**mc_rot_freq**
  | MC move set frequencies:
  | Frequency :math:`p` of rigid body rotation moves (the frequency of 
  | rigid body translation move will be :math:`q = 1 - p`).

.. _mc_xyz_fine_freq:

**mc_xyz_fine_freq**
  | Relative frequency (w.r.t. the number of translation move) of fine translation moves.

.. _mc_rot_fine_freq:

**mc_rot_fine_freq**
  | Relative frequency (w.r.t. the number of rotation moves) of fine rotation moves.

.. _mc_niter:

**mc_niter**
  | Number of steps :math:`N_{out}` of the outer MC chain (1st value).
  | Number of steps :math:`N_{in}` of the inner MC chain (2nd value).

.. _mc_sa_alpha:

**mc_sa_alpha**
  | Annealing parameter :math:`\alpha`.

.. _mc_rseed:

**mc_rseed**
  | Seed for the pseudo-random number generator used by the MC sampler. A value of ``-1`` uses 
    the current CPU time.


.. _par_generator:

Parameter File Generator
------------------------

The parameter file generator helps you preparing the input parameter files 
for a SEED run (``seed.inp``, ``seed.par``, and ``seed_kw.par``).
You can load a template with predefined default values, edit the user-specific 
information and save it. The template for ``seed_kw.par`` shows example settings for 
a run with additional MCSA minimization of the poses.

  .. <script>
  ..   //var filename = "./_static/seed.inp";
  ..   //var fileblob = new Blob([filename], {type : 'text/plain'});
  ..   //var reader = new FileReader();
  ..   //reader.onload = function (e) {
  ..   //  var textArea = document.getElementById("param_area");
  ..   //  textArea.value = e.target.result;
  ..   //};
  ..   //reader.addEventListener("loadend", function() {
  ..     // reader.result contains the contents of blob as a typed array
  ..   //});
  ..   //reader.readAsText(fileblob);
  ..   var xhr = new XMLHttpRequest();
  ..   xhr.open("GET", "./_static/seed.inp");
  ..   //although we can get the remote data directly into an arraybuffer
  ..   //using the string "arraybuffer" assigned to responseType property.
  ..   //For the sake of example we are putting it into a blob and then copying the blob data into an arraybuffer.
  ..   xhr.responseType = "blob";
  ..
  ..   function analyze_data(blob)
  ..   {
  ..       var myReader = new FileReader();
  ..       myReader.readAsText(blob)
  ..
  ..       var textArea = document.getElementById("param_area");
  ..       textArea.value = "sticazzi"//e.srcElement.result;
  ..
  ..       myReader.addEventListener("loadend", function(e)
  ..       {
  ..           //var buffer = e.srcElement.result;//arraybuffer object
  ..           //var textArea = document.getElementById("param_area");
  ..           //textArea.value = "sticazzi"//e.srcElement.result;
  ..       });
  ..   }
  ..
  ..   xhr.onload = function()
  ..   {
  ..       //analyze_data(xhr.response);
  ..       document.getElementsByTagName("body")[0].innerHTML = xhr.response;
  ..   }
  ..   xhr.send();
  .. </script>

.. raw:: html

  <button id="btn-inp">Load default seed.inp</button>
  <button id="btn-par">Load default seed.par</button>
  <button id="btn-kw">Load default seed_kw.par</button>
  
  <div>
    <p>Here you can edit the file with user-specific information. 
    Fields you necessarily have to edit are marked by XXXX</p>
  </div>
  
  <textarea id="input-area" cols="100" style="resize:none" rows="20" placeholder="Enter your parameters">
  </textarea>
  
  
  <div class="form-group">
    <label for="input-fileName">File name</label>
    <input type="text" class="form-control" id="input-fileName" value="" placeholder="Enter file name">
  </div>
  
  <div>
    <button id="btn-save">Save input file</button>
  </div>
  
  <script src="./_static/FileSaver.js"></script>
  <script>
  
  jQuery("#btn-inp").click( function() {
    jQuery( "#input-area" ).load("_static/seed.inp");
    jQuery("#input-fileName").val("seed.inp")
  });
  jQuery("#btn-par").click( function() {
    jQuery( "#input-area" ).load("_static/seed4_cgenff4.par");
    jQuery("#input-fileName").val("seed.par")
  });
  jQuery("#btn-kw").click( function() {
    jQuery( "#input-area" ).load("_static/seed4_kw.par");
    jQuery("#input-fileName").val("seed_kw.par")
  });
  
  //jQuery(document).ready(function(){
  //  jQuery( "#input-area" ).load("_static/seed.inp");
  //});
  
  jQuery("#btn-save").click( function(){
    var text = jQuery("#input-area").val();
    var filename = jQuery("#input-fileName").val()
    var blob = new Blob([text], {type: "text/plain;charset=utf-8"});
    saveAs(blob, filename);
  });
  </script>
  
  
  
