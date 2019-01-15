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
      o.addRepresentation("ball+sticks", { color: "element" });
    });
  });
  </script>
  
  <div id="viewport" style="width:400px; height:300px;"></div>

Note also the gap in energy between the top pose of the correct enantiomer and all other poses.

.. [Zhao2014] Zhao H, et al.
  Discovery of BRD4 bromodomain inhibitors by fragment-based high-throughput docking,
  Bioorg Med Chem Lett. 24 (2014).
