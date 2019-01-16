Running modes 
=============

SEED can run in two distinct modes: :ref:`dock-runmode` and :ref:`energy-runmode` 
that can be enabled by setting the first line of :ref:`i7<i7>` to ``d`` or ``e`` 
respectively. 

* In the docking running mode each fragment is docked into the receptor in multiple 
  positions and the best ones (in terms of total binding energy) are returned. This 
  is the usual setting for prospective docking campaigns.
  
* In the energy evaluation mode SEED will evaluate the total binding energy of 
  a fragment in the position provided by the input coordinates. This is the typical
  setting for rescoring poses generated beforehand by SEED or other software.

.. _dock-runmode:

Docking running mode 
--------------------

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
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Before starting the docking procedure the fragment is pre-aligned 
to a reference frame. The first atom of the fragment is put in the origin, 
the second is put along the positive *x* axis, the third in the *xy* plane and 
the others are aligned accordingly. If the third atom is collinear with the first two, 
the fourth atom is considered and so on. 

As the positioning of points on the Solvent Accessible Surface has a dependency 
on the absolute orientation of the fragment, 
the pre-alignment step is necessary to ensure consistency of results 
when docking (first line of :ref:`i7<i7>` set to ``d``) the same fragment 
provided in different spatial orientations or when carrying out an 
energy evaluation (first line of :ref:`i7<i7>` set to ``e``) of a fragment 
previously docked by SEED. Residual differences may be due to the 
limited precision of coordinates in usual mol2 files.

.. _fast-docking:

Fast docking scheme 
^^^^^^^^^^^^^^^^^^^

The fast docking scheme proceeds by applying the following filters sequentially:

* *Sphere for acceptance of the fragment position*.
  This filter is optional. The user can specify a sphere 
  (``y``/``n`` for enabling the filter and coordinates of the center and radius in 
  :ref:`i6<i6>`) in which the geometric center of the fragment pose must be 
  to be accepted.

* *Van der Waals interaction used as bad contacts detection*.
  The fast van der Waals energy is used to detect clashes: 
  a fragment is discarded if it is less favorable than the energy threshold 
  set in :ref:`p11<p11>`.

* *Van der Waals interaction for apolar docking*.
  To increase efficiency, apolar fragments are discarded without evaluation 
  of the electrostatic contribution if the fast van der Waals interaction is 
  less favorable than a threshold value. This value is 
  calculated by multiplying :ref:`p19<p19>` by the number of fragment atoms.

* *Total energy*.
  The fragment position is finally accepted if the total energy (fast model) 
  is smaller than a cutoff given in the third column of :ref:`i7<i7>`. 
  The total energy is the sum of the van der Waals interaction energy plus 
  the electrostatic contribution (screened intermolecular energy and 
  protein and fragment desolvation terms). These four terms can be 
  weighted by the scaling factors in :ref:`p23<p23>`.

.. _clustering:

Clustering
^^^^^^^^^^

The docking of a given fragment (with energy evaluation as 
described according to the :ref:`fast-docking`) is followed by sorting and clustering. 
Within each fragment type, poses are first sorted according to binding energy. 
Poses whose binding energy is lower than a user-specified threshold value 
(see last point in the :ref:`fast-docking`) are then clustered 
(a maximal number of positions can be set in :ref:`p27<p27>`) using the following 
similarity criterion between two fragment positions :math:`A` and :math:`B`: 

.. math::
  \begin{eqnarray}
  S(A,B)=\frac{S_{AB}}{\max (S_{AA},S_{BB})} & \mbox{ where } & 
    S_{XY}=\sum_{i \in X} \sum_{j \in Y} w_{t_i t_j} \exp (- \gamma r^2_{ij})
  \end{eqnarray}
  
where :math:`r_{ij}` is the distance between two atoms (:math:`i \in` fragment 
pose :math:`X`, :math:`j \in Y`), :math:`w_{t_i t_j}` is a user-controlled 
matrix whose coefficients reflect the similarity between element types 
(in most cases a unit matrix is used; otherwise the non-default coefficients 
have to be set in :ref:`p24<p24>` by giving the number of non-default 
values on the first line and two element types with the non-default value 
on the following lines) and :math:`\gamma` is a coefficient which acts on 
the broadness of the distribution
of the positions. :math:`B` is assigned to the cluster of 
:math:`A` if :math:`S(A,B)` is larger than a cutoff value 
:math:`\delta` with :math:`0 \leq \delta \leq 1`.
 
The clustering proceeds in two steps:

.. _clu1:

#.  A first clustering with :math:`\gamma=0.9` (first term of :ref:`p25<p25>`) 
    and :math:`\delta=0.4` (second term of :ref:`p25<p25>`) yields 
    large clusters which contain almost overlapping as well as more distant fragments; 
#.  A second clustering with :math:`\gamma=0.9` (first term of :ref:`p26<p26>`) 
    and `\delta=0.9` (second term of :ref:`p26<p26>`) is done on each 
    cluster found in :ref:`1.<clu1>` to eliminate fragments which are very close in space.
     
The second clustering is applied on the positions for which the binding energy 
of the cluster representative is smaller than a cutoff value specified 
in the 4th column of :ref:`i7<i7>`. A single clustering step 
with :math:`\gamma=0.9` and :math:`\delta=0.9` would generate too many 
small clusters. Hence, the first step is a real clustering while the 
second step is done only to discard redundant positions. 

.. _rescoring:

Accurate rescoring 
^^^^^^^^^^^^^^^^^^

The :math:`n` best binding modes within each cluster 
(:math:`n` is set in :ref:`p5<p5>`) are rescored and ranked according to the 
accurate energy model. Note that it is possible that poses passing the total
energy filter during fast docking result in a binding energy above the cutoff 
(set in the 3rd column of :ref:`i7<i7>`) with the accurate model. These poses 
appears in the docking summary in the ``seed.out`` log, but are not written to 
any other output files of the postprocess scheme.

.. _energy-runmode:

Energy evaluation mode 
----------------------

SEED allows the energy of a particular fragment position to be evaluated without 
generating new poses (as opposed to the :ref:`dock-runmode`). 
The fragment mol2 file (:ref:`i7<i7>`) must contain the coordinates of the 
relevant position, for which the binding energy has to be evaluated.
