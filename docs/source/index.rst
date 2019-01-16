==============================================
SEED - Solvation Energy for Exhaustive Docking
==============================================

.. role:: raw-html(raw)
   :format: html

SEED is a program for fragment docking with force-field based evaluation of
binding energy.
:raw-html:`<br />`
The implemented docking approach determines optimal positions and orientations
of small-to-medium-sized molecular fragments in the binding site
of a rigid receptor, and ranks them according to their binding energy.
:raw-html:`<br />`
Polar fragments are positioned such that at least one hydrogen bond with
optimal distance to a protein polar group is made (polar docking).
Apolar fragments are docked are docked by matching hydrophobic regions on the
fragment and on the receptor (apolar docking).
These regions are efficiently and accurately determined
by evaluating electrostatic desolvation penalty and van der Waals
interactions with an uncharged probe sphere.
:raw-html:`<br />`
Two implicit solvation models
based on the Generalized Born methodology and developed in the
`Caflisch group <http://www.biochem-caflisch.uzh.ch/>`_
are employed to efficiently evaluate the protein and fragment desolvation
upon binding and the screened electrostatic interaction.

Contents:
=========

.. toctree::
   :maxdepth: 2

   getstart
   models
   docking
   params
   tutorials



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
