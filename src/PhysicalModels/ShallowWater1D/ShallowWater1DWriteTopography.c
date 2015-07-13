/*! @file ShallowWater1DWriteTopography.c
    @author Debojyoti Ghosh
    @brief Function to write out the topography
*/
#include <stdlib.h>
#include <basic.h>
#include <arrayfunctions.h>
#include <mpivars.h>
#include <hypar.h>
#include <physicalmodels/shallowwater1d.h>

int WriteArray(int,int,int*,int*,int,double*,double*,void*,void*,char*);

/*! Write out the topography data to file */
int ShallowWater1DWriteTopography(
                                    void* s, /*!< Solver object of type #HyPar */
                                    void *m  /*!< MPI object of type #MPIVariables */
                                 )
{
  HyPar           *solver = (HyPar*)          s;
  MPIVariables    *mpi    = (MPIVariables*)   m;
  ShallowWater1D  *params = (ShallowWater1D*) solver->physics;
  _DECLARE_IERR_;

  IERR WriteArray(solver->ndims,1,solver->dim_global,solver->dim_local,
                  solver->ghosts,solver->x,params->b,solver,mpi,
                  "topography"); CHECKERR(ierr);

  return(0);
}