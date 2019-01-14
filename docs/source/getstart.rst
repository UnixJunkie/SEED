Installation
============

SEED makes use of the 
`BOOST C++ Libraries <https://www.boost.org/>`_
. The necessary header files are distributed along with SEED.

In order to install SEED, cd to directory ``src`` and make with the following command 
(you may have to modify the ``Makefile.local``):

.. code-block:: bash
    
    make seed
    
The binary is compiled into the ``bin`` directory.
After installation run SEED with the following command:
::
  
  ./seed_4 seed.inp >& log
