#!/bin/bash -l
#SBATCH --job-name=myseed
#SBATCH --output=myseed_%j.out
#SBATCH --error=myseed_%j.err
#SBATCH --nodes=1
#SBATCH --ntasks=36
#SBATCH --cpus-per-task=1 
#SBATCH --ntasks-per-node=36
#SBATCH --ntasks-per-core=2
#SBATCH --constraint=myconstraint
#SBATCH --account=myaccount
#SBATCH --partition=mypartition
#SBATCH --time=02:00:00

INPUTDIR=inpfls
seedscrpt=myseed #do not put .inp

SEEDDIR=/software/seed_4/bin
seedexe=seed_4_mpi

RUNDIR=myseeddir

if [[ ! -d ${RUNDIR} ]] ; then
  mkdir -p ${RUNDIR}
fi

cd ${RUNDIR}

#export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
srun --ntasks-per-node $SLURM_NTASKS_PER_NODE -n $SLURM_NTASKS -c $SLURM_CPUS_PER_TASK  ${SEEDDIR}/${seedexe} ${INPUTDIR}/${seedscrpt}.inp >& log_${seedscrpt}.log
