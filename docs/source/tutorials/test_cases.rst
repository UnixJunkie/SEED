Bromodomains
============

BRD4 Bromodomain
----------------

In this tutorial we redock the ligand from crystal structure with PDB code 4PCI.
This is a 7-μM ligand of the N-terminal bromodomain of BRD4, abbreviated as BRD4(1) [Zhao2014]_.
We also try to dock an enantiomer of the original ligand.

In order to run this test case execute the following commands:
::

  cd test_cases/bromodomains/BRD4
  ../../../bin/seed_4 seed.inp >& log

The two enantiomers are docked by SEED which identifies the correct pose for the
enantiomer observed in the crystal structure while the other enantiomer
does not fit into the binding site.

.. raw:: html

  <script src="../_static/ngl.js"></script>
  <script>
  // old
  //  document.addEventListener("DOMContentLoaded", function () {
  //    var stage = new NGL.Stage("viewport");
  //    stage.loadFile("rcsb://1crn", {defaultRepresentation: true});
  //  });
  
  document.addEventListener("DOMContentLoaded", function () {
    // Create NGL Stage object
    var stage = new NGL.Stage( "viewport" );
    // Handle window resizing
    window.addEventListener( "resize", function( event ){
      stage.handleResize();
    }, false );
    // Load PDB entry 1CRN
    //stage.loadFile( "rcsb://1crn", { defaultRepresentation: true } );
    stage.loadFile("../_static/brd4_4pci_seed.mol2").then(function (o) {
      // add a "cartoon" representation to the structure component
      o.addRepresentation("cartoon", { color: "atomindex" });
      // provide a "good" view of the structure
      o.autoView();
    });
    stage.loadFile("../_static/4pci_brd4_ligand_and_mirror_image_best.mol2").then(function (o) {
      // color selection scheme 
      var schemeLig = NGL.ColormakerRegistry.addSelectionScheme([
        ["element", "not _C"],
        ["white", "LIG and _C"],
        ["red", "MIR and _C"],
      ], "ligand and enantiomer");
      // add a "cartoon" representation to the structure component
      o.addRepresentation("licorice", {color: schemeLig});
      //o.addRepresentation("licorice", { sele: "LIG and not _C", color: "element" });
      //o.addRepresentation("licorice", { sele: "LIG and _C", color: "white" });
      //o.addRepresentation("licorice", { sele: "MIR and not _C", color: "element" });
      //o.addRepresentation("licorice", { sele: "LIG and _C", color: "red" });
    });
  });
  </script>
  
  <div id="viewport" style="width:600px; height:400px;"></div>

Note also the gap in energy between the top pose of the correct enantiomer and all other poses. 

====================  =============  ================  =================  =================  =============  ===============  =================  
Name                  Tot\ :sup:`a`   ElinW\ :sup:`b`  rec_des\ :sup:`c`  frg_des\ :sup:`d`  vdW\ :sup:`e`  DElec\ :sup:`f`  DG_hydr\ :sup:`g`
====================  =============  ================  =================  =================  =============  ===============  =================  
4pci_ligand                  -25.97             -9.58               5.16               4.31         -25.86            -5.34              -4.24
4pci_ligand                  -20.67             -6.14               5.21               4.45         -24.19            -1.90              -4.24
4pci_ligand                  -19.62             -4.77               5.21               4.34         -24.41            -0.53              -4.24
4pci_ligand                  -19.43             -5.42               5.66               4.32         -24.00            -1.18              -4.24
4pci_ligand                  -17.81             -8.01               5.57               4.06         -19.43            -3.77              -4.24
4pci_ligand_inverted         -14.01             -0.91               2.94               0.43         -16.46             2.87              -3.78
4pci_ligand_inverted         -13.95             -0.86               2.77               0.41         -16.27             2.93              -3.78
4pci_ligand_inverted         -12.92             -0.78               2.68               0.41         -15.24             3.00              -3.78
4pci_ligand_inverted         -12.52              0.70               2.60               0.58         -16.39             4.49              -3.78
4pci_ligand_inverted         -13.05              0.31               5.62               0.95         -19.93             4.09              -3.78
====================  =============  ================  =================  =================  =============  ===============  =================  

| The table shown is an excerpt of SEED output table. The total energy term *Tot* 
  is the sum of the electrostatics and van der Waals energy terms: 
| :math:`Tot = ElinW + rec\_des + frg\_des + vdW`.
  See :ref:`output_files` for details about each term in the table.
  
|

.. [Zhao2014] Zhao H, et al.
  Discovery of BRD4 bromodomain inhibitors by fragment-based high-throughput docking,
  Bioorg Med Chem Lett. 24 (2014).
