#!/bin/bash

if [[ -z "$1" ]] || [[ -z "$2" ]] || [[ -n "$3" ]]; 
then
  echo -e "Usage:\n\n/bin/bash convert_CAMP_BIO_to_CGENFF.sh MAP MOL2FILE > OUTPUTFILE\n"
  exit
fi

awk -v fn1=$1 '{if ($1 == "CAMP_BIO") {while (( getline line<fn1) > 0) {split(line,ele," "); xx[ele[1]]=ele[3]}; printf("CGenFF_4.0 "); for (k=2;k<=NF;k=k+2) {printf ("%d %s ",$k,xx[$(k+1)])}; printf("\n")} else if ($1 == "CAMP_BIO_ALT_TYPE_SET") {print "CGenFF_4.0_ALT_TYPE_SET"} else {print}}' $2

# how to get the map
# cat charmm36.prm | awk '{if ($1 == "biotype") {print}}' | cut -b8-15,50-80 | awk '{if (($2 >= 57) && ($2 <=59)) {print $2+57} else if (($1 == 344) || ($1 == 364) || ($1 == 6) || ($1 == 1200)) {print "117"} else {print $4}}' > tmplst

# then paste this (with "#" to top of "tmplst")
# bonded_type    0      ! XXX  dummy
# bonded_type    1  	! H    polar H
# bonded_type    2	! HC   N-ter H
# bonded_type    3	! HA   nonpolar H
# bonded_type    4	! HT   TIPS3P WATER HYDROGEN
# bonded_type    5	! HP   aromatic H
# bonded_type    6	! HB1  backbone H
# bonded_type    7	! HR1  his he1, (+) his HG,HD2
# bonded_type    8	! HR2  (+) his HE1
# bonded_type    9	! HR3  neutral his HG, HD2
# bonded_type   10	! HS   thiol hydrogen
# bonded_type   11	! HE1  for alkene; RHC=CR
# bonded_type   12	! HE2  for alkene; H2C=CR
# bonded_type   13      ! C    carbonyl C, peptide backbone         
# bonded_type   14      ! CA   aromatic C
# bonded_type   15      ! CT1  aliphatic sp3 C for CH
# bonded_type   16      ! CT2  aliphatic sp3 C for CH2
# bonded_type   17      ! CT3  aliphatic sp3 C for CH3
# bonded_type   18	! CPH1 his CG and CD2 carbons
# bonded_type   19	! CPH2 his CE1 carbon
# bonded_type   20	! CPT  trp C between rings
# bonded_type   21	! CY   TRP C in pyrrole ring
# bonded_type   22	! CP1  tetrahedral C (proline CA)
# bonded_type   23	! CP2  tetrahedral C (proline CB/CG)
# bonded_type   24	! CP3  tetrahedral C (proline CD)
# bonded_type   25	! CC   carbonyl C, asn,asp,gln,glu,cter,ct2
# bonded_type   26	! CD   carbonyl C, pres aspp,glup,ct1
# bonded_type   27	! CS   thiolate carbon
# bonded_type   28	! CE1  for alkene; RHC=CR
# bonded_type   29	! CE2  for alkene; H2C=CR
# bonded_type   30	! CST  CO2 carbon 
# bonded_type   31	! N    proline N
# bonded_type   32      ! NR1  neutral his protonated ring nitrogen
# bonded_type   33      ! NR2  neutral his unprotonated ring nitrogen
# bonded_type   34      ! NR3  charged his ring nitrogen
# bonded_type   35      ! NH1  peptide nitrogen
# bonded_type   36	! NH2  amide nitrogen
# bonded_type   37	! NH3  ammonium nitrogen
# bonded_type   38	! NC2  guanidinium nitroogen
# bonded_type   39	! NY   TRP N in pyrrole ring
# bonded_type   40      ! NP   Proline ring NH2+ (N-terminal)
# bonded_type   41      ! NPH  heme pyrrole N
# bonded_type   42      ! O    carbonyl oxygen
# bonded_type   43      ! OB   carbonyl oxygen in acetic acid
# bonded_type   44      ! OC   carboxylate oxygen
# bonded_type   45      ! OH1  hydroxyl oxygen
# bonded_type   46      ! OS   ester oxygen
# bonded_type   47      ! OT   TIPS3P WATER OXYGEN
# bonded_type   48	! OM   heme CO/O2 oxygen
# bonded_type   49	! OST  CO2 oxygen
# bonded_type   50	! S    sulphur
# bonded_type   51      ! SM   sulfur C-S-S-C type
# bonded_type   52	! SS   thiolate sulfur
# bonded_type   53      ! HN1  Nucleic acid amine proton
# bonded_type   54	! HN2  Nucleic acid ring nitrogen proton
# bonded_type   55	! HN3  Nucleic acid aromatic carbon proton
# bonded_type   56	! HN3B NAD+ aromatic hydrogen
# bonded_type   57      ! HN4  Nucleic acid phosphate hydroxyl proton
# bonded_type   58	! HN5  Nucleic acid ribose hydroxyl proton
# bonded_type   59	! HN6  Nicotinamide aliphatic proton
# bonded_type   60	! HN7  Nucleic acid proton (equivalent to protein HA)
# bonded_type   61	! HN8  Bound to CN8 in nucleic acids/model compounds
# bonded_type   62	! HN9  Bound to CN9 in nucleic acids/model compounds
# bonded_type   63      ! CN1  Nucleic acid carbonyl carbon
# bonded_type   64      ! CN1A NAD+/NADH amide carbonyl carbon
# bonded_type   65	! CN1T Nucleic acid carbonyl carbon (T/U C2)
# bonded_type   66	! CN2  Nucleic acid aromatic carbon to amide
# bonded_type   67	! CN3  Nucleic acid aromatic carbon 
# bonded_type   68	! CN3A NAD+ aromatic carbon
# bonded_type   69	! CN3B NAD+ aromatic carbon
# bonded_type   70	! CN3C NADH aromatic carbon
# bonded_type   71	! CN3D Nucleic acid aromatic carbon for 5MC
# bonded_type   72      ! CN3T Nucleic acid aromatic carbon, Thy C5
# bonded_type   73      ! CN4  Nucleic acid purine C8 and ADE C2  
# bonded_type   74      ! CN5  Nucleic acid purine C4 and C5
# bonded_type   75      ! CN5G Nucleic acid guanine C5
# bonded_type   76      ! CN7  Nucleic acid carbon (equivalent to protein CT1)
# bonded_type   77      ! CN7B Nucleic acid aliphatic carbon for C1'
# bonded_type   78	! CN7C C2' in arabinose 
# bonded_type   79      ! CN8  Nucleic acid carbon (equivalent to protein CT2)
# bonded_type   80      ! CN8B Nucleic acid carbon (equivalent to protein CT2)
# bonded_type   81      ! CN9  Nucleic acid carbon (equivalent to protein CT3)
# bonded_type   82      ! NN1  Nucleic acid amide nitrogen
# bonded_type   83      ! NN2  Nucleic acid protonated ring nitrogen
# bonded_type   84	! NN2B From NN2, for N9 in GUA different from ADE
# bonded_type   85	! NN2U Nucleic acid protonated ring nitrogen, ura N3
# bonded_type   86	! NN2G Nucleic acid protonated ring nitrogen, gua N1
# bonded_type   87	! NN3  Nucleic acid unprotonated ring nitrogen 
# bonded_type   88	! NN3A Nucleic acid unprotonated ring nitrogen, ade N1 and N3
# bonded_type   89	! NN3I Nucleic acid unprotonated ring nitrogen, inosine N3
# bonded_type   90	! NN3G Nucleic acid unprotonated ring nitrogen, gua N3
# bonded_type   91	! NN4  Nucleic acid purine N7
# bonded_type   92	! NN5  Nucleic acid sp2 amine nitrogen
# bonded_type   93	! NN6  Nucleic acid sp3 amine nitrogen (equiv to protein nh3)
# bonded_type   94	! ON1  Nucleic acid carbonyl oxygen
# bonded_type   95	! ON1C Nucleic acid carbonyl oxygen, cyt O2
# bonded_type   96	! ON2  Nucleic acid phosphate ester oxygen
# bonded_type   97	! ON3  Nucleic acid =O in phosphate 
# bonded_type   98	! ON4  Nucleic acid phosphate hydroxyl oxygen
# bonded_type   99	! ON5  Nucleic acid ribose hydroxyl oxygen
# bonded_type  100	! ON6  Nucleic acid deoxyribose ring oxygen
# bonded_type  101	! ON6B Nucleic acid ribose ring oxygen
# bonded_type  102      ! P    phosphorus
# bonded_type  103      ! HN2 in GUA
# bonded_type  104      ! CL in perchlorate
# bonded_type  105      ! O in perchlorate
# bonded_type  106      ! N in nitrate
# bonded_type  107      ! O in nitrate
# bonded_type  108	! ON2B Aryl phosphate ester oxygen
# bonded_type  109	! CAI Special bonded type CAI (indol aromatic carbon adjacent to CPT1)
# bonded_type  110      ! CTA2 Special bonded type CTA2 (polypeptide side chains GLU, ASP, HSP)
# bonded_type  111      ! P2   Phosphorous in phosphate diester linkages in DNA only
# bonded_type  112      ! NG3PO quat. ammonium nitrogen (partially from CGENFF NG3P0)
# bonded_type  113      ! NG3P2 sec. ammonium nitrogen but not N-terminal PRO (partially from CGENFF NG3P2
# bonded_type  114      ! HA1
# bonded_type  115      ! HA2
# bonded_type  116      ! HA3
# bonded_type  117      ! HB2

# then:
# awk '{if ($2 == "bonded_type") {btn[$3] = $5;} else {k=k+1; print k,$1,btn[$1]}}' tmplst > CAMP_BIO_to_CGENFF.map
