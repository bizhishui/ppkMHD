#ifndef HYDRO_INIT_FUNCTORS_2D_H_
#define HYDRO_INIT_FUNCTORS_2D_H_

#include <limits> // for std::numeric_limits
#ifdef __CUDA_ARCH__
#include <math_constants.h> // for cuda math constants, e.g. CUDART_INF
#endif // __CUDA_ARCH__

#include "shared/kokkos_shared.h"
#include "HydroBaseFunctor2D.h"

// init conditions
#include "shared/problems/BlastParams.h"
#include "shared/problems/IsentropicVortexParams.h"
#include "shared/problems/RayleighTaylorInstabilityParams.h"
#include "shared/problems/initRiemannConfig2d.h"

namespace ppkMHD { namespace muscl {

/*************************************************/
/*************************************************/
/*************************************************/
class InitImplodeFunctor2D : public HydroBaseFunctor2D {

public:
  InitImplodeFunctor2D(HydroParams params,
		       DataArray2d Udata) :
    HydroBaseFunctor2D(params), Udata(Udata)  {};

  // static method which does it all: create and execute functor
  static void apply(HydroParams params,
                    DataArray2d Udata,
		    int         nbCells)
  {
    InitImplodeFunctor2D functor(params, Udata);
    Kokkos::parallel_for(nbCells, functor);
  }

  KOKKOS_INLINE_FUNCTION
  void operator()(const int& index) const
  {

    const int isize = params.isize;
    const int jsize = params.jsize;
    const int ghostWidth = params.ghostWidth;
    
#ifdef USE_MPI
    const int i_mpi = params.myMpiPos[IX];
    const int j_mpi = params.myMpiPos[IY];
#else
    const int i_mpi = 0;
    const int j_mpi = 0;
#endif

    const int nx = params.nx;
    const int ny = params.ny;

    const real_t xmin = params.xmin;
    const real_t ymin = params.ymin;
    const real_t dx = params.dx;
    const real_t dy = params.dy;
    
    const real_t gamma0 = params.settings.gamma0;
    
    int i,j;
    index2coord(index,i,j,isize,jsize);
    
    real_t x = xmin + dx/2 + (i+nx*i_mpi-ghostWidth)*dx;
    real_t y = ymin + dy/2 + (j+ny*j_mpi-ghostWidth)*dy;
    
    real_t tmp = x+y*y;
    if (tmp > 0.5 && tmp < 1.5) {
      Udata(i  ,j  , ID) = 1.0;
      Udata(i  ,j  , IP) = 1.0/(gamma0-1.0);
      Udata(i  ,j  , IU) = 0.0;
      Udata(i  ,j  , IV) = 0.0;
    } else {
      Udata(i  ,j  , ID) = 0.125;
      Udata(i  ,j  , IP) = 0.14/(gamma0-1.0);
      Udata(i  ,j  , IU) = 0.0;
      Udata(i  ,j  , IV) = 0.0;
    }
    
  } // end operator ()

  DataArray2d Udata;

}; // InitImplodeFunctor2D
  
/*************************************************/
/*************************************************/
/*************************************************/
class InitBlastFunctor2D : public HydroBaseFunctor2D {

public:
  InitBlastFunctor2D(HydroParams params,
		     BlastParams bParams,
		     DataArray2d Udata) :
    HydroBaseFunctor2D(params), bParams(bParams), Udata(Udata)  {};
  
  // static method which does it all: create and execute functor
  static void apply(HydroParams params,
		    BlastParams bParams,
                    DataArray2d Udata,
		    int         nbCells)
  {
    InitBlastFunctor2D functor(params, bParams, Udata);
    Kokkos::parallel_for(nbCells, functor);
  }

  KOKKOS_INLINE_FUNCTION
  void operator()(const int& index) const
  {

    const int isize = params.isize;
    const int jsize = params.jsize;
    const int ghostWidth = params.ghostWidth;
    
#ifdef USE_MPI
    const int i_mpi = params.myMpiPos[IX];
    const int j_mpi = params.myMpiPos[IY];
#else
    const int i_mpi = 0;
    const int j_mpi = 0;
#endif

    const int nx = params.nx;
    const int ny = params.ny;

    const real_t xmin = params.xmin;
    const real_t ymin = params.ymin;
    const real_t dx = params.dx;
    const real_t dy = params.dy;
    
    const real_t gamma0 = params.settings.gamma0;

    // blast problem parameters
    const real_t blast_radius      = bParams.blast_radius;
    const real_t radius2           = blast_radius*blast_radius;
    const real_t blast_center_x    = bParams.blast_center_x;
    const real_t blast_center_y    = bParams.blast_center_y;
    const real_t blast_density_in  = bParams.blast_density_in;
    const real_t blast_density_out = bParams.blast_density_out;
    const real_t blast_pressure_in = bParams.blast_pressure_in;
    const real_t blast_pressure_out= bParams.blast_pressure_out;
  
    int i,j;
    index2coord(index,i,j,isize,jsize);
    
    real_t x = xmin + dx/2 + (i+nx*i_mpi-ghostWidth)*dx;
    real_t y = ymin + dy/2 + (j+ny*j_mpi-ghostWidth)*dy;

    real_t d2 = 
      (x-blast_center_x)*(x-blast_center_x)+
      (y-blast_center_y)*(y-blast_center_y);    
    
    if (d2 < radius2) {
      Udata(i  ,j  , ID) = blast_density_in;
      Udata(i  ,j  , IP) = blast_pressure_in/(gamma0-1.0);
      Udata(i  ,j  , IU) = 0.0;
      Udata(i  ,j  , IV) = 0.0;
    } else {
      Udata(i  ,j  , ID) = blast_density_out;
      Udata(i  ,j  , IP) = blast_pressure_out/(gamma0-1.0);
      Udata(i  ,j  , IU) = 0.0;
      Udata(i  ,j  , IV) = 0.0;
    }
    
  } // end operator ()
  
  BlastParams bParams;
  DataArray2d Udata;
  
}; // InitBlastFunctor2D
  
/*************************************************/
/*************************************************/
/*************************************************/
class InitFourQuadrantFunctor2D : public HydroBaseFunctor2D {

public:
  InitFourQuadrantFunctor2D(HydroParams params,
			    DataArray2d Udata,
			    int configNumber,
			    HydroState U0,
			    HydroState U1,
			    HydroState U2,
			    HydroState U3,
			    real_t xt,
			    real_t yt) :
    HydroBaseFunctor2D(params), Udata(Udata),
    U0(U0), U1(U1), U2(U2), U3(U3), xt(xt), yt(yt)
  {};
  
  // static method which does it all: create and execute functor
  static void apply(HydroParams params,
                    DataArray2d Udata,
		    int configNumber,
		    HydroState U0,
		    HydroState U1,
		    HydroState U2,
		    HydroState U3,
		    real_t xt,
		    real_t yt,
		    int    nbCells)
  {
    InitFourQuadrantFunctor2D functor(params, Udata, configNumber,
				      U0, U1, U2, U3, xt, yt);
    Kokkos::parallel_for(nbCells, functor);
  }

  KOKKOS_INLINE_FUNCTION
  void operator()(const int& index) const
  {

    const int isize = params.isize;
    const int jsize = params.jsize;
    const int ghostWidth = params.ghostWidth;
    
#ifdef USE_MPI
    const int i_mpi = params.myMpiPos[IX];
    const int j_mpi = params.myMpiPos[IY];
#else
    const int i_mpi = 0;
    const int j_mpi = 0;
#endif

    const int nx = params.nx;
    const int ny = params.ny;

    const real_t xmin = params.xmin;
    const real_t ymin = params.ymin;
    const real_t dx = params.dx;
    const real_t dy = params.dy;
    
    int i,j;
    index2coord(index,i,j,isize,jsize);
    
    real_t x = xmin + dx/2 + (i+nx*i_mpi-ghostWidth)*dx;
    real_t y = ymin + dy/2 + (j+ny*j_mpi-ghostWidth)*dy;
    
    if (x<xt) {
      if (y<yt) {
	// quarter 2
	Udata(i  ,j  , ID) = U2[ID];
	Udata(i  ,j  , IP) = U2[IP];
	Udata(i  ,j  , IU) = U2[IU];
	Udata(i  ,j  , IV) = U2[IV];
      } else {
	// quarter 1
	Udata(i  ,j  , ID) = U1[ID];
	Udata(i  ,j  , IP) = U1[IP];
	Udata(i  ,j  , IU) = U1[IU];
	Udata(i  ,j  , IV) = U1[IV];
      }
    } else {
      if (y<yt) {
	// quarter 3
	Udata(i  ,j  , ID) = U3[ID];
	Udata(i  ,j  , IP) = U3[IP];
	Udata(i  ,j  , IU) = U3[IU];
	Udata(i  ,j  , IV) = U3[IV];
      } else {
	// quarter 0
	Udata(i  ,j  , ID) = U0[ID];
	Udata(i  ,j  , IP) = U0[IP];
	Udata(i  ,j  , IU) = U0[IU];
	Udata(i  ,j  , IV) = U0[IV];
      }
    }
    
  } // end operator ()

  DataArray2d Udata;
  HydroState2d U0, U1, U2, U3;
  real_t xt, yt;
  
}; // InitFourQuadrantFunctor2D

/*************************************************/
/*************************************************/
/*************************************************/
class InitIsentropicVortexFunctor2D : public HydroBaseFunctor2D {

public:
  InitIsentropicVortexFunctor2D(HydroParams params,
				IsentropicVortexParams iparams,
				DataArray2d Udata) :
    HydroBaseFunctor2D(params), iparams(iparams), Udata(Udata)  {};
  
  // static method which does it all: create and execute functor
  static void apply(HydroParams params,
		    IsentropicVortexParams iparams,
                    DataArray2d Udata,
		    int         nbCells)
  {
    InitIsentropicVortexFunctor2D functor(params, iparams, Udata);
    Kokkos::parallel_for(nbCells, functor);
  }
  
  KOKKOS_INLINE_FUNCTION
  void operator()(const int& index) const
  {

    const int isize = params.isize;
    const int jsize = params.jsize;
    const int ghostWidth = params.ghostWidth;
    
#ifdef USE_MPI
    const int i_mpi = params.myMpiPos[IX];
    const int j_mpi = params.myMpiPos[IY];
#else
    const int i_mpi = 0;
    const int j_mpi = 0;
#endif

    const int nx = params.nx;
    const int ny = params.ny;

    const real_t xmin = params.xmin;
    const real_t ymin = params.ymin;
    const real_t dx = params.dx;
    const real_t dy = params.dy;
    
    const real_t gamma0 = params.settings.gamma0;
  
    int i,j;
    index2coord(index,i,j,isize,jsize);
    
    real_t x = xmin + dx/2 + (i+nx*i_mpi-ghostWidth)*dx;
    real_t y = ymin + dy/2 + (j+ny*j_mpi-ghostWidth)*dy;

    // ambient flow
    const real_t rho_a = this->iparams.rho_a;
    //const real_t p_a   = this->iparams.p_a;
    const real_t T_a   = this->iparams.T_a;
    const real_t u_a   = this->iparams.u_a;
    const real_t v_a   = this->iparams.v_a;
    //const real_t w_a   = this->iparams.w_a;
    
    // vortex center
    const real_t vortex_x = this->iparams.vortex_x;
    const real_t vortex_y = this->iparams.vortex_y;

    // relative coordinates versus vortex center
    real_t xp = x - vortex_x;
    real_t yp = y - vortex_y;
    real_t r  = sqrt(xp*xp + yp*yp);
    
    const real_t beta = this->iparams.beta;

    real_t du = - yp * beta / (2 * M_PI) * exp(0.5*(1.0-r*r));
    real_t dv =   xp * beta / (2 * M_PI) * exp(0.5*(1.0-r*r));
    
    real_t T = T_a - (gamma0-1)*beta*beta/(8*gamma0*M_PI*M_PI)*exp(1.0-r*r);
    real_t rho = rho_a*pow(T/T_a,1.0/(gamma0-1));
    
    Udata(i  ,j  , ID) = rho;
    Udata(i  ,j  , IU) = rho*(u_a + du);
    Udata(i  ,j  , IV) = rho*(v_a + dv);
    //Udata(i  ,j  , IP) = pow(rho,gamma0)/(gamma0-1.0) +
    Udata(i  ,j  , IP) = rho*T/(gamma0-1.0) +
      0.5*rho*(u_a + du)*(u_a + du) +
      0.5*rho*(v_a + dv)*(v_a + dv) ;
    
  } // end operator ()
  
  IsentropicVortexParams iparams;
  DataArray2d Udata;
  
}; // InitIsentropicVortexFunctor2D

/*************************************************/
/*************************************************/
/*************************************************/
/**
 * Test of the Rayleigh-Taylor instability.
 * See
 * http://www.astro.princeton.edu/~jstone/Athena/tests/rt/rt.html
 * for a description of such initial conditions
 */
class RayleighTaylorInstabilityFunctor2D : public HydroBaseFunctor2D {

public:
  RayleighTaylorInstabilityFunctor2D(HydroParams params,
				     RayleighTaylorInstabilityParams rtiparams,
				     DataArray2d Udata,
				     VectorField2d gravity) :
    HydroBaseFunctor2D(params),
    rtiparams(rtiparams),
    Udata(Udata),
    gravity(gravity)
  {};

  KOKKOS_INLINE_FUNCTION
  void operator()(const int& index) const
  {

    const int isize = params.isize;
    const int jsize = params.jsize;
    const int ghostWidth = params.ghostWidth;
    
#ifdef USE_MPI
    const int i_mpi = params.myMpiPos[IX];
    const int j_mpi = params.myMpiPos[IY];
#else
    const int i_mpi = 0;
    const int j_mpi = 0;
#endif

    const int nx = params.nx;
    const int ny = params.ny;

    const real_t xmin = params.xmin;
    const real_t ymin = params.ymin;

    const real_t xmax = params.xmax;
    const real_t ymax = params.ymax;

    const real_t Lx = xmax-xmin;
    const real_t Ly = ymax-ymin;

    const real_t dx = params.dx;
    const real_t dy = params.dy;
    
    const real_t gamma0 = params.settings.gamma0;
  
    int i,j;
    index2coord(index,i,j,isize,jsize);
    
    real_t x = xmin + dx/2 + (i+nx*i_mpi-ghostWidth)*dx;
    real_t y = ymin + dy/2 + (j+ny*j_mpi-ghostWidth)*dy;

    /* initialize perturbation amplitude */
    real_t amplitude = rtiparams.amplitude;
    real_t        d0 = rtiparams.d0;
    real_t        d1 = rtiparams.d1;
    
    //bool  randomEnabled = rti.randomEnabled;


    /* uniform static gravity field */
    const real_t gravity_x = rtiparams.gx;
    const real_t gravity_y = rtiparams.gy;
    const real_t gravity_z = rtiparams.gz;
    
    real_t         P0 = 1.0/(gamma0-1.0);
      
  
    // the initial condition must ensure the condition of
    // hydrostatic equilibrium for pressure P = P0 - 0.1*\rho*y
	
    // Athena initial conditions are
    // if ( y > 0.0 ) {
    //   h_U(i,j,ID) = 2.0;
    // } else {
    //   h_U(i,j,ID) = 1.0;
    // }
    // h_U(i,j,IP) = P0 + gravity_x*x + gravity_y*y;
    // h_U(i,j,IU) = 0.0;
    // h_U(i,j,IV) = amplitude*(1+cos(2*M_PI*x))*(1+cos(0.5*M_PI*y))/4;
    
    if ( y > (ymin+ymax)/2 ) {
      Udata(i,j,ID) = d1;
    } else {
      Udata(i,j,ID) = d0;
    }
    Udata(i,j,IU) = 0.0f;
    // if (randomEnabled)
    //   Udata(i,j,IV) = amplitude * ( rand() * 1.0 / RAND_MAX - 0.5);
    // else
    Udata(i,j,IV) = amplitude * 
      (1+cos(2*M_PI*x/Lx))*
      (1+cos(2*M_PI*y/Ly))/4;

    Udata(i,j,IE) = P0 + Udata(i,j,ID)*(gravity_x*x + gravity_y*y);

    // init gravity field
    gravity(i,j,IX) = gravity_x;
    gravity(i,j,IY) = gravity_y;
    
  } // end operator ()

  RayleighTaylorInstabilityParams rtiparams;
  DataArray2d Udata;
  VectorField2d gravity;
  
}; // class RayleighTaylorInstabilityFunctor2D

} // namespace muscl

} // namespace ppkMHD

#endif // HYDRO_INIT_FUNCTORS_2D_H_
