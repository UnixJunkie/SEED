# SEED (Solvation Energy for Exhaustive Docking) version 4.0.0

SEED is a program for fragment docking with force-field based evaluation of binding energy.

The new SEED documentation is now online at: [https://caflischlab-seed.readthedocs.io/en/latest/](https://caflischlab-seed.readthedocs.io/en/latest/)!

For details about the energy models implemented in SEED please refer to the original publications and to the pdf user manual (seed_4.0.0_doc.pdf).

### Installation ###
cd to directory src and make SEED with the following command (you may have to modify the Makefile.local):
```sh
make seed
```
The binary is compiled into the bin directory.
### Run SEED ###
Run SEED with the following command:
```sh
./seed_4 seed.inp >& log
```
You can find an example seed.inp and seed.par, along with the results of simple study cases,
in the test_cases folder.

### Citations ###
Review of the applications of SEED in the period 1999-2018, includes a discussion on its strenghts/weaknesses in light of our experience:
 * J.-R. Marchand, and A. Caflisch. In silico fragment-based drug design with SEED.
European Journal of Medicinal Chemistry, 156:907-917, 2018.

Original paper describing SEED:
 * N. Majeux, M. Scarsi, J. Apostolakis, C. Ehrhardt, and A. Caflisch. Exhaustive docking of
molecular fragments on protein binding sites with electrostatic solvation.
Proteins: Structure, Function and Genetics, 37:88-105, 1999.

The description of the fast energy evaluation is in the second SEED paper:
 * N. Majeux, M. Scarsi, and A. Caflisch. Efficient electrostatic solvation model for protein-
fragment docking.
Proteins: Structure, Function and Genetics, 42:256-268, 2001.
