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
      // add a "cartoon" representation to the structure component
      o.addRepresentation("ball+stick", { color: "element" });
    });
  });
  </script>
  
  <div id="viewport" style="width:600px; height:400px;"></div>

Note also the gap in energy between the top pose of the correct enantiomer and all other poses.

======================   =======   =======   =======   =======   =======   =======   =======  
Name                         Tot     ElinW   rec_des   frg_des       vdW     DElec   DG_hydr
======================   =======   =======   =======   =======   =======   =======   =======  
4pci_ligand               -25.97     -9.58      5.16      4.31    -25.86     -5.34     -4.24
4pci_ligand               -20.67     -6.14      5.21      4.45    -24.19     -1.90     -4.24
4pci_ligand               -19.62     -4.77      5.21      4.34    -24.41     -0.53     -4.24
4pci_ligand               -19.43     -5.42      5.66      4.32    -24.00     -1.18     -4.24
4pci_ligand               -17.81     -8.01      5.57      4.06    -19.43     -3.77     -4.24
4pci_ligand               -15.10     -3.90      5.60      4.45    -21.26      0.34     -4.24
4pci_ligand               -13.60     -0.95      7.09      1.73    -21.47      3.29     -4.24
4pci_ligand               -12.61      0.94      6.73      2.47    -22.76      5.18     -4.24
4pci_ligand               -12.14      0.49      6.16      2.58    -21.37      4.73     -4.24
4pci_ligand               -11.65      0.94      6.41      2.26    -21.26      5.18     -4.24
4pci_ligand               -12.79     -0.91      2.44      1.53    -15.85      3.33     -4.24
4pci_ligand               -12.78     -0.75      2.60      1.72    -16.35      3.49     -4.24
4pci_ligand               -11.94     -0.31      2.79      2.08    -16.50      3.93     -4.24
4pci_ligand_inverted      -14.01     -0.91      2.94      0.43    -16.46      2.87     -3.78
4pci_ligand_inverted      -13.95     -0.86      2.77      0.41    -16.27      2.93     -3.78
4pci_ligand_inverted      -12.92     -0.78      2.68      0.41    -15.24      3.00     -3.78
4pci_ligand_inverted      -12.52      0.70      2.60      0.58    -16.39      4.49     -3.78
4pci_ligand_inverted      -13.05      0.31      5.62      0.95    -19.93      4.09     -3.78
4pci_ligand_inverted      -12.76      0.30      5.00      1.83    -19.88      4.08     -3.78
4pci_ligand_inverted      -12.29      1.33      4.88      1.56    -20.06      5.11     -3.78
4pci_ligand_inverted      -11.58      0.58      6.05      0.85    -19.07      4.36     -3.78
4pci_ligand_inverted      -13.00     -0.42      3.75      0.45    -16.77      3.36     -3.78
4pci_ligand_inverted      -12.97     -1.48      7.22      2.85    -21.56      2.30     -3.78
4pci_ligand_inverted      -12.72     -0.54      7.82      2.86    -22.86      3.24     -3.78
4pci_ligand_inverted      -11.71     -0.82      8.07      2.78    -21.75      2.96     -3.78
4pci_ligand_inverted      -11.64     -1.12      7.12      2.98    -20.62      2.66     -3.78
4pci_ligand_inverted      -10.85     -0.42      7.02      2.54    -19.99      3.36     -3.78
4pci_ligand_inverted      -12.66      0.70      4.57      1.76    -19.69      4.48     -3.78
4pci_ligand_inverted      -12.02     -0.69      2.43      3.25    -17.01      3.09     -3.78
4pci_ligand_inverted      -11.72     -0.57      4.56      2.64    -18.36      3.21     -3.78
======================   =======   =======   =======   =======   =======   =======   =======  

|

.. [Zhao2014] Zhao H, et al.
  Discovery of BRD4 bromodomain inhibitors by fragment-based high-throughput docking,
  Bioorg Med Chem Lett. 24 (2014).
