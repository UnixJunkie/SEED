Docking workflow
================

In order to efficiently speed up the screening of fragments, SEED relies on a two-step docking 
workflow we will refer to as postprocess scheme.

In the first step, after the initial alignment of the fragment to a reference frame 
(pre-alignment of the fragment :ref:`prealign`), 
the generated poses are screened and filtered according to the fast energy model 
(:ref:`fast-docking`). Poses passing all the filters are 
sorted according to their total binding energy and clustered in order to find 
the most representative ones and discard redundant 
positions (clustering procedure :ref:`clustering`). 
In the second step the best binding modes within each cluster 
are rescored and ranked with the more accurate solvation model (:ref:`rescoring`).

.. _prealign:

Pre-alignment of the fragment
-----------------------------

.. _fast-docking:

Fast docking scheme 
-------------------

.. _clustering:

Clustering
----------

.. _rescoring:

Accurate rescoring 
------------------
