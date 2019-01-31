.. raw:: html
  
  <head>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/crypto-js/3.1.2/rollups/aes.js"></script>
  <script language="JavaScript">
  <!--hide
  
  var password;
  var pass_crypt;
  var pass1="38a7744f5523335db845ff1976bf4747";

  password=prompt('Please enter your password to view this page!',' ');
  pass_crypt = CryptoJS.MD5(password).toString();
  
  if (pass_crypt == pass1){
    alert('Password Correct! You are entering restricted content!');
  }
  else
     {
       window.location="../restricted.html";
      }

  //-->
  </script>
  </head>


Protein preparation with CAMPARI 
================================

With the latest implemented features it is now straightforward to use CAMPARI 
to directly generate a receptor mol2 file for SEED. As CAMPARI's pdb parser is 
fairly robust, in the vast majority of the cases it will be able to read a raw 
pdb without the need of any preprocessing. 

The input files required by CAMPARI to describe the receptor are a sequence file 
listing all the residues in the system and a pdb or another structural file to 
read the atom coordinates (the atoms for which no coordinates are provided will 
be rebuilt by CAMPARI). For details please refer to 
`CAMPARI documentation <http://campari.sourceforge.net/V3/index.html>`_.

The sequence file can be extracted from the ``SEQRES`` of the pdb using the script 
``convert_SEQRES_toseq.sh`` provided by CAMPARI.

Before starting the preparation with CAMPARI be sure to have run:

.. code-block:: bash

  cvs update
  
If you specify in the key file the following keyword

.. code-block:: bash

  FMCSC_SYBYLLJMAP  ljmap_for_abs4.2_charmm36.prm
  
CAMPARI will write structural mol2 files with the ``ALT_TYPE_SET`` specification 
as required by SEED. Be sure also to use the correct CHARMM parameters:

.. code-block:: bash

  PARAMETERS  campariv3/params/abs4.2_charmm36.prm
  
The written mol2 files will contain the CAMPARI biotypes which will have to be 
remapped to the correct CHARMM atom types by using: 

.. code-block:: bash

  bash convert_CAMP_BIO_to_CGENFF.sh CAMP_BIO_to_CGENFF.map MOL2FILE > OUTPUTFILE
  
A protein receptor can be prepared with CAMPARI using the two keyfiles ``tmd_build.template.key`` and 
``cons_mini_abs_internal.template.key`` (make sure to correct filenames and paths).

``tmd_build.template.key`` rebuilds the system starting from the provided pdb. 
Missing residues are added and missing sidechains are build in random conformations.
After that a specific relaxation protocol (refer to keyword ``TMD_RELAX``) is carried out 
to remove any major steric clash. 

The relaxed output (pdb with suffix ``_RELAXED.pdb``) can be used as structural 
input for the minimization, which is performed in internal 
coordinates by ``cons_mini_abs_internal.template.key``. 
We recommend the use of a freeze file (keyword ``FRZFILE``, using mode ``A``) to allow only 
side-chain terminal dihedrals (those ending with a hydrogen) 
to move during minimization. This can be accomplished 
for example by extracting the indices of the non-terminal ones (the ones we 
would like to freeze) from a CAMPARI log file (keyword ``TMDREPORT`` enabled), with a 
command like the following: 

.. code-block:: bash 
  
  sed -n '/Summary of Rotation/,/End of Summary of Rotation/p' log | sed '/Mol.  #/,/Atom/d' | tail -n +2 | head -n -1 | awk '$3 > 10 {print $1}'

If the system is made up by multiple chains or molecules, make sure to include 
also constraints for rigid body translations and rotations in the ``FRZFILE``.
