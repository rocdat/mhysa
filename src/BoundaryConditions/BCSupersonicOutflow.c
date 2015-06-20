/*! @file BCSupersonicOutflow.c
    @author Debojyoti Ghosh
    @brief Supersonic outflow boundary conditions (specific to Euler/Navier-Stokes systems)
*/

#include <stdlib.h>
#include <basic.h>
#include <arrayfunctions.h>
#include <boundaryconditions.h>

#include <physicalmodels/euler2d.h>
#include <physicalmodels/navierstokes3d.h>

/*! Applies the supersonic outflow boundary condition: All flow variables
    (density, pressure, velocity) are extrapolated from the interior since
    the outflow is supersonic. This boundary condition is specific to two
    and three dimensional Euler/Navier-Stokes systems (#Euler2D, #NavierStokes2D,
    #NavierStokes3D).
    \n\n
    Note: The extrapolate boundary condition (#_EXTRAPOLATE_) can be used as well
    for this boundary. I am not entirely sure why I wrote the code for this boundary
    in such a complicated fashion.
*/
int BCSupersonicOutflowU(
                          void    *b,     /*!< Boundary object of type #DomainBoundary */
                          void    *m,     /*!< MPI object of type #MPIVariables */
                          int     ndims,  /*!< Number of spatial dimensions */
                          int     nvars,  /*!< Number of variables/DoFs per grid point */
                          int     *size,  /*!< Integer array with the number of grid points in each spatial dimension */
                          int     ghosts, /*!< Number of ghost points */
                          double  *phi,   /*!< The solution array on which to apply the boundary condition */
                          double  waqt    /*!< Current solution time */
                        )
{
  DomainBoundary *boundary = (DomainBoundary*) b;

  int dim   = boundary->dim;
  int face  = boundary->face;

  if (ndims == 2) {

    /* create a fake physics object */
    Euler2D physics; 
    double gamma;
    gamma = physics.gamma = boundary->gamma;
    double inv_gamma_m1 = 1.0/(gamma-1.0);

    if (boundary->on_this_proc) {
      int bounds[ndims], indexb[ndims], indexi[ndims];
      _ArraySubtract1D_(bounds,boundary->ie,boundary->is,ndims);
      _ArraySetValue_(indexb,ndims,0);
      int done = 0;
      while (!done) {
        int p1, p2;
        _ArrayCopy1D_(indexb,indexi,ndims);
        _ArrayAdd1D_(indexi,indexi,boundary->is,ndims);
        if      (face ==  1) indexi[dim] = ghosts-1-indexb[dim];
        else if (face == -1) indexi[dim] = size[dim]-indexb[dim]-1;
        else return(1);
        _ArrayIndex1DWO_(ndims,size,indexb,boundary->is,ghosts,p1);
        _ArrayIndex1D_(ndims,size,indexi,ghosts,p2);
        
        /* flow variables in the interior */
        double rho, uvel, vvel, energy, pressure;
        double rho_gpt, uvel_gpt, vvel_gpt, energy_gpt, pressure_gpt;
        _Euler2DGetFlowVar_((phi+nvars*p2),rho,uvel,vvel,energy,pressure,(&physics));
        /* set the ghost point values */
        rho_gpt       = rho;
        pressure_gpt  = pressure;
        uvel_gpt      = uvel;
        vvel_gpt      = vvel;
        energy_gpt    = inv_gamma_m1*pressure_gpt
                        + 0.5 * rho_gpt * (uvel_gpt*uvel_gpt + vvel_gpt*vvel_gpt);

        phi[nvars*p1+0] = rho_gpt;
        phi[nvars*p1+1] = rho_gpt * uvel_gpt;
        phi[nvars*p1+2] = rho_gpt * vvel_gpt;
        phi[nvars*p1+3] = energy_gpt;

        _ArrayIncrementIndex_(ndims,bounds,indexb,done);
      }
    }

  } else if (ndims == 3) {

    /* create a fake physics object */
    NavierStokes3D physics;
    double gamma;
    gamma = physics.gamma = boundary->gamma;
    double inv_gamma_m1 = 1.0/(gamma-1.0);

    if (boundary->on_this_proc) {
      int bounds[ndims], indexb[ndims], indexi[ndims];
      _ArraySubtract1D_(bounds,boundary->ie,boundary->is,ndims);
      _ArraySetValue_(indexb,ndims,0);
      int done = 0;
      while (!done) {
        int p1, p2;
        _ArrayCopy1D_(indexb,indexi,ndims);
        _ArrayAdd1D_(indexi,indexi,boundary->is,ndims);
        if      (face ==  1) indexi[dim] = ghosts-1-indexb[dim];
        else if (face == -1) indexi[dim] = size[dim]-indexb[dim]-1;
        else return(1);
        _ArrayIndex1DWO_(ndims,size,indexb,boundary->is,ghosts,p1);
        _ArrayIndex1D_(ndims,size,indexi,ghosts,p2);
        
        /* flow variables in the interior */
        double rho, uvel, vvel, wvel, energy, pressure;
        double rho_gpt, uvel_gpt, vvel_gpt, wvel_gpt, energy_gpt, pressure_gpt;
        _NavierStokes3DGetFlowVar_((phi+nvars*p2),rho,uvel,vvel,wvel,energy,pressure,(&physics));
        /* set the ghost point values */
        rho_gpt       = rho;
        pressure_gpt  = pressure;
        uvel_gpt      = uvel;
        vvel_gpt      = vvel;
        wvel_gpt      = wvel;
        energy_gpt    = inv_gamma_m1*pressure_gpt
                        + 0.5 * rho_gpt 
                        * (uvel_gpt*uvel_gpt + vvel_gpt*vvel_gpt + wvel_gpt*wvel_gpt);

        phi[nvars*p1+0] = rho_gpt;
        phi[nvars*p1+1] = rho_gpt * uvel_gpt;
        phi[nvars*p1+2] = rho_gpt * vvel_gpt;
        phi[nvars*p1+3] = rho_gpt * wvel_gpt;
        phi[nvars*p1+4] = energy_gpt;

        _ArrayIncrementIndex_(ndims,bounds,indexb,done);
      }
    }

  }
  return(0);
}

/*! Applies the supersonic outflow boundary condition to the delta-solution: 
    All flow variables are extrapolated from the interior since
    the outflow is supersonic. This boundary condition is specific to two
    and three dimensional Euler/Navier-Stokes systems (#Euler2D, #NavierStokes2D,
    #NavierStokes3D).
    \n\n
    Note: The code here is identical to BCExtrapolateDU().
*/
int BCSupersonicOutflowDU(
                          void    *b,       /*!< Boundary object of type #DomainBoundary */
                          void    *m,       /*!< MPI object of type #MPIVariables */
                          int     ndims,    /*!< Number of spatial dimensions */
                          int     nvars,    /*!< Number of variables/DoFs per grid point */
                          int     *size,    /*!< Integer array with the number of grid points in each spatial dimension */
                          int     ghosts,   /*!< Number of ghost points */
                          double  *phi,     /*!< The solution array on which to apply the boundary condition -
                                                 Note that this is a delta-solution \f$\Delta {\bf U}\f$.*/
                          double  *phi_ref, /*!< Reference solution */
                          double  waqt      /*!< Current solution time */
                         )
{
  DomainBoundary *boundary = (DomainBoundary*) b;

  int dim   = boundary->dim;
  int face  = boundary->face;

  if (boundary->on_this_proc) {
    int bounds[ndims], indexb[ndims], indexi[ndims];
    _ArraySubtract1D_(bounds,boundary->ie,boundary->is,ndims);
    _ArraySetValue_(indexb,ndims,0);
    int done = 0;
    while (!done) {
      _ArrayCopy1D_(indexb,indexi,ndims);
      _ArrayAdd1D_(indexi,indexi,boundary->is,ndims);
      if (face == 1)        indexi[dim] = 0;
      else if (face == -1)  indexi[dim] = size[dim]-1;
      else                  return(1);
      int p1,p2;
      _ArrayIndex1DWO_(ndims,size,indexb,boundary->is,ghosts,p1);
      _ArrayIndex1D_(ndims,size,indexi,ghosts,p2);
      _ArrayCopy1D_((phi+nvars*p2),(phi+nvars*p1),nvars);
      _ArrayIncrementIndex_(ndims,bounds,indexb,done);
    }
  }
  return(0);
}
