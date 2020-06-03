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
Apolar fragments are docked by matching hydrophobic regions on the
fragment and on the receptor (apolar docking).
These regions are efficiently and accurately determined
by evaluating electrostatic desolvation penalty and van der Waals
interactions with an uncharged probe sphere.
:raw-html:`<br />`
Two implicit solvation models based on the Generalized Born methodology
are employed to efficiently evaluate the protein and fragment desolvation
upon binding and the screened electrostatic interaction.

**NEW!** The best generated poses can now be stochastically minimized with 
Monte Carlo simulated annealing. Beware that this new feature is experimental and
has not been tested thoroughly yet. See :ref:`mc_minimization` for details.

**NEW!** A new keyword-based parameter file has been introduced. This allows more flexibility in 
the way settings are specified, but only involves newly added settings (e.g. the ones of :ref:`mc_minimization`),
so that legacy ``seed.par`` files keep working as expected. See :ref:`KW_param` for details.

|

SEED is developed in the `Caflisch Lab <http://www.biochem-caflisch.uzh.ch/>`_ at 
the `University of Zurich <https://www.uzh.ch/de.html>`_
and hosted on `GitLab <https://gitlab.com/CaflischLab/SEED>`_ under the 
GNU General Public License version 3 (GPLv3).

If you use SEED in your work, please cite the original SEED paper:

* | N. Majeux, M. Scarsi, J. Apostolakis, C. Ehrhardt, and A. Caflisch. Exhaustive docking of
    molecular fragments on protein binding sites with electrostatic solvation.
    Proteins: Structure, Function and Genetics, 37:88-105, 1999.
  | :raw-html:`<a class="reference external" href="http://dx.doi.org/doi:10.1002/(SICI)1097-0134(19991001)37:1<88::AID-PROT9>3.0.CO;2-O">doi: 10.1002/(SICI)1097-0134(19991001)37:1<88::AID-PROT9>3.0.CO;2-O</a>`

Go to :ref:`publications` for an overview of articles related to SEED.

Contents:
=========

.. toctree::
   :maxdepth: 2

   getstart
   parallel
   docking
   vectors
   models
   params
   tutorials
   publications
   restricted



Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
