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
    document.addEventListener("DOMContentLoaded", function () {
      var stage = new NGL.Stage("viewport");
      stage.loadFile("rcsb://1crn", {defaultRepresentation: true});
    });
  </script>
  <div id="viewport" style="width:400px; height:300px;"></div>

Note also the gap in energy between the top pose of the correct enantiomer and all other poses.

.. [Zhao2014] Zhao H, et al.
  Discovery of BRD4 bromodomain inhibitors by fragment-based high-throughput docking,
  Bioorg Med Chem Lett. 24 (2014).
