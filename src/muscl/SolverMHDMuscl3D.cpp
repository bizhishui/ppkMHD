#include <string> 
#include <cstdio>
#include <cstdbool>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "SolverMHDMuscl3D.h"
#include "HydroParams.h"

// the actual computational functors called in SolverMHDMuscl3D
#include "MHDRunFunctors3D.h"

// Kokkos
#include "kokkos_shared.h"

// for init condition
#include "BlastParams.h"

static bool isBigEndian()
{
  const int i = 1;
  return ( (*(char*)&i) == 0 );
}

namespace ppkMHD {

using namespace muscl::mhd3d;

// =======================================================
// ==== CLASS SolverMHDMuscl3D IMPL ======================
// =======================================================

// =======================================================
// =======================================================
/**
 *
 */
SolverMHDMuscl3D::SolverMHDMuscl3D(HydroParams& params, ConfigMap& configMap) :
  SolverBase(params, configMap),
  U(), U2(), Q(),
  Qm_x(), Qm_y(), Qm_z(),
  Qp_x(), Qp_y(), Qp_z(),
  QEdge_RT(),  QEdge_RB(),  QEdge_LT(),  QEdge_LB(),
  QEdge_RT2(), QEdge_RB2(), QEdge_LT2(), QEdge_LB2(),
  QEdge_RT3(), QEdge_RB3(), QEdge_LT3(), QEdge_LB3(),
  Fluxes_x(), Fluxes_y(), Fluxes_z(),
  Emf(),
  ElecField(),
  DeltaA(), DeltaB(), DeltaC(),
  isize(params.isize),
  jsize(params.jsize),
  ksize(params.ksize),
  ijsize(params.isize*params.jsize),
  ijksize(params.isize*params.jsize*params.ksize)
{

  m_nCells = ijksize;

  /*
   * memory allocation (use sizes with ghosts included)
   */
  U     = DataArray("U", ijksize, nbvar);
  Uhost = Kokkos::create_mirror_view(U);
  U2    = DataArray("U2",ijksize, nbvar);
  Q     = DataArray("Q", ijksize, nbvar);

  if (params.implementationVersion == 0) {
  
    Qm_x = DataArray("Qm_x", ijksize, nbvar);
    Qm_y = DataArray("Qm_y", ijksize, nbvar);
    Qm_z = DataArray("Qm_z", ijksize, nbvar);
    
    Qp_x = DataArray("Qp_x", ijksize, nbvar);
    Qp_y = DataArray("Qp_y", ijksize, nbvar);
    Qp_z = DataArray("Qp_z", ijksize, nbvar);

    QEdge_RT  = DataArray("QEdge_RT", ijksize, nbvar);
    QEdge_RB  = DataArray("QEdge_RB", ijksize, nbvar);
    QEdge_LT  = DataArray("QEdge_LT", ijksize, nbvar);
    QEdge_LB  = DataArray("QEdge_LB", ijksize, nbvar);

    QEdge_RT2 = DataArray("QEdge_RT2", ijksize, nbvar);
    QEdge_RB2 = DataArray("QEdge_RB2", ijksize, nbvar);
    QEdge_LT2 = DataArray("QEdge_LT2", ijksize, nbvar);
    QEdge_LB2 = DataArray("QEdge_LB2", ijksize, nbvar);

    QEdge_RT3 = DataArray("QEdge_RT3", ijksize, nbvar);
    QEdge_RB3 = DataArray("QEdge_RB3", ijksize, nbvar);
    QEdge_LT3 = DataArray("QEdge_LT3", ijksize, nbvar);
    QEdge_LB3 = DataArray("QEdge_LB3", ijksize, nbvar);

    Fluxes_x  = DataArray("Fluxes_x", ijksize, nbvar);
    Fluxes_y  = DataArray("Fluxes_y", ijksize, nbvar);
    Fluxes_z  = DataArray("Fluxes_z", ijksize, nbvar);

    Emf       = DataArrayVector3("Emf", ijksize);

    ElecField = DataArrayVector3("ElecField", ijksize); 

    DeltaA    = DataArrayVector3("DeltaA", ijksize);
    DeltaB    = DataArrayVector3("DeltaB", ijksize);
    DeltaC    = DataArrayVector3("DeltaC", ijksize);
    
  }
  
  // default riemann solver
  // riemann_solver_fn = &SolverMHDMuscl3D::riemann_approx;
  // if (!riemannSolverStr.compare("hllc"))
  //   riemann_solver_fn = &SolverMHDMuscl3D::riemann_hllc;
  
  /*
   * initialize hydro array at t=0
   */
  if ( !m_problem_name.compare("blast") ) {

    init_blast(U);
    
  } else if ( !m_problem_name.compare("orszag_tang") ) {
    
    init_orszag_tang(U);
    
  } else {
    
    std::cout << "Problem : " << m_problem_name
	      << " is not recognized / implemented."
	      << std::endl;
    std::cout <<  "Use default - orszag_tang" << std::endl;
    init_orszag_tang(U);

  }
  std::cout << "##########################" << "\n";
  std::cout << "Solver is " << m_solver_name << "\n";
  std::cout << "Problem (init condition) is " << m_problem_name << "\n";
  std::cout << "##########################" << "\n";

  // print parameters on screen
  params.print();
  std::cout << "##########################" << "\n";

  // initialize time step
  compute_dt();

  // initialize boundaries
  make_boundaries(U);

  // copy U into U2
  Kokkos::deep_copy(U2,U);

} // SolverMHDMuscl3D::SolverMHDMuscl3D


// =======================================================
// =======================================================
/**
 *
 */
SolverMHDMuscl3D::~SolverMHDMuscl3D()
{

} // SolverMHDMuscl3D::~SolverMHDMuscl3D

// =======================================================
// =======================================================
/**
 * Compute time step satisfying CFL condition.
 *
 * \return dt time step
 */
double SolverMHDMuscl3D::compute_dt_local()
{

  real_t dt;
  real_t invDt = ZERO_F;
  DataArray Udata;
  
  // which array is the current one ?
  if (m_iteration % 2 == 0)
    Udata = U;
  else
    Udata = U2;

  // call device functor
  ComputeDtFunctor computeDtFunctor(params, Udata);
  Kokkos::parallel_reduce(ijksize, computeDtFunctor, invDt);
    
  dt = params.settings.cfl/invDt;

  return dt;

} // SolverMHDMuscl3D::compute_dt_local

// =======================================================
// =======================================================
void SolverMHDMuscl3D::next_iteration_impl()
{

  if (m_iteration % 10 == 0) {
    std::cout << "time step=" << m_iteration << std::endl;
  }
    
  // output
  if (params.enableOutput) {
    if ( should_save_solution() ) {
      
      std::cout << "Output results at time t=" << m_t
		<< " step " << m_iteration
		<< " dt=" << m_dt << std::endl;
      
      save_solution();
      
    } // end output
  } // end enable output
  
  // compute new dt
  timers[TIMER_DT]->start();
  compute_dt();
  timers[TIMER_DT]->stop();
  
  // perform one step integration
  godunov_unsplit(m_dt);
  
} // SolverMHDMuscl3D::next_iteration_impl

// =======================================================
// =======================================================
// ///////////////////////////////////////////
// Wrapper to the actual computation routine
// ///////////////////////////////////////////
void SolverMHDMuscl3D::godunov_unsplit(real_t dt)
{
  
  if ( m_iteration % 2 == 0 ) {
    godunov_unsplit_cpu(U , U2, dt);
  } else {
    godunov_unsplit_cpu(U2, U , dt);
  }
  
} // SolverMHDMuscl3D::godunov_unsplit

// =======================================================
// =======================================================
// ///////////////////////////////////////////
// Actual CPU computation of Godunov scheme
// ///////////////////////////////////////////
void SolverMHDMuscl3D::godunov_unsplit_cpu(DataArray data_in, 
				 DataArray data_out, 
				 real_t dt)
{

  real_t dtdx;
  real_t dtdy;
  real_t dtdz;
  
  dtdx = dt / params.dx;
  dtdy = dt / params.dy;
  dtdz = dt / params.dz;

  // fill ghost cell in data_in
  timers[TIMER_BOUNDARIES]->start();
  make_boundaries(data_in);
  timers[TIMER_BOUNDARIES]->stop();
    
  // copy data_in into data_out (not necessary)
  // data_out = data_in;
  Kokkos::deep_copy(data_out, data_in);
  
  // start main computation
  timers[TIMER_NUM_SCHEME]->start();

  // convert conservative variable into primitives ones for the entire domain
  convertToPrimitives(data_in);

  if (params.implementationVersion == 0) {

    // compute electric field
    computeElectricField(data_in);

    // compute magnetic slopes
    computeMagSlopes(data_in);
    
    // trace computation: fill arrays qm_x, qm_y, qm_z, qp_x, qp_y, qp_z
    computeTrace(data_in, dt);

    // Compute flux via Riemann solver and update (time integration)
    computeFluxesAndStore(dt);

    // Compute Emf
    computeEmfAndStore(dt);
    
    // actual update with fluxes
    {
      UpdateFunctor functor(params, data_out,
			    Fluxes_x, Fluxes_y, Fluxes_z, dtdx, dtdy, dtdz);
      Kokkos::parallel_for(ijksize, functor);
    }

    // actual update with emf
    {
      UpdateEmfFunctor functor(params, data_out,
			       Emf, dtdx, dtdy, dtdz);
      Kokkos::parallel_for(ijksize, functor);
    }
    
  }
  timers[TIMER_NUM_SCHEME]->stop();
  
} // SolverMHDMuscl3D::godunov_unsplit_cpu

// =======================================================
// =======================================================
// ///////////////////////////////////////////////////////////////////
// Convert conservative variables array U into primitive var array Q
// ///////////////////////////////////////////////////////////////////
void SolverMHDMuscl3D::convertToPrimitives(DataArray Udata)
{

  // call device functor
  ConvertToPrimitivesFunctor functor(params, Udata, Q);
  Kokkos::parallel_for(ijksize, functor);
  
} // SolverMHDMuscl3D::convertToPrimitives

// =======================================================
// =======================================================
// ///////////////////////////////////////////////////////////////////
// Compute electric field
// ///////////////////////////////////////////////////////////////////
void SolverMHDMuscl3D::computeElectricField(DataArray Udata)
{

  // call device functor
  ComputeElecFieldFunctor functor(params, Udata, Q, ElecField);
  Kokkos::parallel_for(ijksize, functor);
  
} // SolverMHDMuscl3D::computeElectricField

// =======================================================
// =======================================================
// ///////////////////////////////////////////////////////////////////
// Compute magnetic slopes
// ///////////////////////////////////////////////////////////////////
void SolverMHDMuscl3D::computeMagSlopes(DataArray Udata)
{

  // call device functor
  ComputeMagSlopesFunctor functor(params, Udata, DeltaA, DeltaB, DeltaC);
  Kokkos::parallel_for(ijksize, functor);
  
} // SolverMHDMuscl3D::computeMagSlopes

// =======================================================
// =======================================================
// ///////////////////////////////////////////////////////////////////
// Compute trace (only used in implementation version 2), i.e.
// fill global array qm_x, qmy, qp_x, qp_y
// ///////////////////////////////////////////////////////////////////
void SolverMHDMuscl3D::computeTrace(DataArray Udata, real_t dt)
{

  // local variables
  real_t dtdx;
  real_t dtdy;
  real_t dtdz;
  
  dtdx = dt / params.dx;
  dtdy = dt / params.dy;
  dtdz = dt / params.dz;

  // call device functor
  ComputeTraceFunctor functor(params, Udata, Q,
			      DeltaA, DeltaB, DeltaC, ElecField,
			      Qm_x, Qm_y, Qm_z,
			      Qp_x, Qp_y, Qp_z,
			      QEdge_RT,  QEdge_RB,  QEdge_LT,  QEdge_LB,
			      QEdge_RT2, QEdge_RB2, QEdge_LT2, QEdge_LB2,
			      QEdge_RT3, QEdge_RB3, QEdge_LT3, QEdge_LB3,
			      dtdx, dtdy, dtdz);
  Kokkos::parallel_for(ijksize, functor);

} // SolverMHDMuscl3D::computeTrace

// =======================================================
// =======================================================
// //////////////////////////////////////////////////////////////////
// Compute flux via Riemann solver and store
// //////////////////////////////////////////////////////////////////
void SolverMHDMuscl3D::computeFluxesAndStore(real_t dt)
{
   
  real_t dtdx = dt / params.dx;
  real_t dtdy = dt / params.dy;
  real_t dtdz = dt / params.dz;

  // call device functor
  ComputeFluxesAndStoreFunctor
    functor(params,
	    Qm_x, Qm_y, Qm_z,
	    Qp_x, Qp_y, Qp_z,
	    Fluxes_x, Fluxes_y, Fluxes_z,
	    dtdx, dtdy, dtdz);
  Kokkos::parallel_for(ijksize, functor);
  
} // computeFluxesAndStore

// =======================================================
// =======================================================
// //////////////////////////////////////////////////////////////////
// Compute EMF via 2D Riemann solver and store
// //////////////////////////////////////////////////////////////////
void SolverMHDMuscl3D::computeEmfAndStore(real_t dt)
{
   
  real_t dtdx = dt / params.dx;
  real_t dtdy = dt / params.dy;
  real_t dtdz = dt / params.dz;

  // call device functor
  ComputeEmfAndStoreFunctor functor(params,
				    QEdge_RT,  QEdge_RB,  QEdge_LT,  QEdge_LB,
				    QEdge_RT2, QEdge_RB2, QEdge_LT2, QEdge_LB2,
				    QEdge_RT3, QEdge_RB3, QEdge_LT3, QEdge_LB3,
				    Emf,
				    dtdx, dtdy, dtdz);
  Kokkos::parallel_for(ijksize, functor);
  
} // computeEmfAndStore

// =======================================================
// =======================================================
// //////////////////////////////////////////////////
// Fill ghost cells according to border condition :
// absorbant, reflexive or periodic
// //////////////////////////////////////////////////
void SolverMHDMuscl3D::make_boundaries(DataArray Udata)
{
  const int ghostWidth=params.ghostWidth;
  
  int max_size = std::max(isize,jsize);
  max_size = std::max(max_size,ksize);

  // call device functor
  {
    int nbIter = ghostWidth * jsize * ksize;
    MakeBoundariesFunctor<FACE_XMIN> functor(params, Udata);
    Kokkos::parallel_for(nbIter, functor);
  }
  {
    int nbIter = ghostWidth * jsize * ksize;
    MakeBoundariesFunctor<FACE_XMAX> functor(params, Udata);
    Kokkos::parallel_for(nbIter, functor);
  }
  {
    int nbIter = isize * ghostWidth * ksize;
    MakeBoundariesFunctor<FACE_YMIN> functor(params, Udata);
    Kokkos::parallel_for(nbIter, functor);
  }
  {
    int nbIter = isize * ghostWidth * ksize;
    MakeBoundariesFunctor<FACE_YMAX> functor(params, Udata);
    Kokkos::parallel_for(nbIter, functor);
  }
  {
    int nbIter = isize * jsize * ghostWidth;
    MakeBoundariesFunctor<FACE_ZMIN> functor(params, Udata);
    Kokkos::parallel_for(nbIter, functor);
  }
  {
    int nbIter = isize * jsize * ghostWidth;
    MakeBoundariesFunctor<FACE_ZMAX> functor(params, Udata);
    Kokkos::parallel_for(nbIter, functor);
  }
  
} // SolverMHDMuscl3D::make_boundaries

// =======================================================
// =======================================================
/**
 * Hydrodynamical Implosion Test.
 * http://www.astro.princeton.edu/~jstone/Athena/tests/implode/Implode.html
 */
// void SolverMHDMuscl3D::init_implode(DataArray Udata)
// {

//   InitImplodeFunctor functor(params, Udata);
//   Kokkos::parallel_for(ijksize, functor);
  
// } // init_implode

// =======================================================
// =======================================================
/**
 * Hydrodynamical blast Test.
 * http://www.astro.princeton.edu/~jstone/Athena/tests/blast/blast.html
 */
void SolverMHDMuscl3D::init_blast(DataArray Udata)
{

  BlastParams blastParams = BlastParams(configMap);
  
  InitBlastFunctor functor(params, blastParams, Udata);
  Kokkos::parallel_for(ijksize, functor);

} // SolverMHDMuscl3D::init_blast

// =======================================================
// =======================================================
/**
 * Orszag-Tang vortex test.
 * http://www.astro.princeton.edu/~jstone/Athena/tests/orszag-tang/pagesource.html
 */
void SolverMHDMuscl3D::init_orszag_tang(DataArray Udata)
{
  
  // init all vars but energy
  {
    InitOrszagTangFunctor<INIT_ALL_VAR_BUT_ENERGY> functor(params, Udata);
    Kokkos::parallel_for(ijksize, functor);
  }

  // init energy
  {
    InitOrszagTangFunctor<INIT_ENERGY> functor(params, Udata);
    Kokkos::parallel_for(ijksize, functor);
  }  
  
} // init_orszag_tang

// =======================================================
// =======================================================
void SolverMHDMuscl3D::save_solution_impl()
{

  timers[TIMER_IO]->start();
  if (m_iteration % 2 == 0)
    saveVTK(U, m_times_saved, "U");
  else
    saveVTK(U2, m_times_saved, "U");
  
  timers[TIMER_IO]->stop();
    
} // SolverMHDMuscl3D::save_solution_impl()

// =======================================================
// =======================================================
// ///////////////////////////////////////////////////////
// output routine (VTK file format, ASCII, VtkImageData)
// Take care that VTK uses row major (i+j*nx+k*nx*ny)
// To make sure OpenMP and CUDA version give the same
// results, we transpose the OpenMP data.
// ///////////////////////////////////////////////////////
void SolverMHDMuscl3D::saveVTK(DataArray Udata,
		     int iStep,
		     std::string name)
{

  const int nx = params.nx;
  const int ny = params.ny;
  const int nz = params.nz;

  const int imin = params.imin;
  const int imax = params.imax;

  const int jmin = params.jmin;
  const int jmax = params.jmax;

  const int kmin = params.kmin;
  const int kmax = params.kmax;

  const int ghostWidth = params.ghostWidth;
  
  // copy device data to host
  Kokkos::deep_copy(Uhost, Udata);
  
  // local variables
  int i, j, k, iVar;
  std::string outputDir    = configMap.getString("output", "outputDir", "./");
  std::string outputPrefix = configMap.getString("output", "outputPrefix", "output");
    
  // check scalar data type
  bool useDouble = false;

  if (sizeof(real_t) == sizeof(double)) {
    useDouble = true;
  }
  
  // write iStep in string stepNum
  std::ostringstream stepNum;
  stepNum.width(7);
  stepNum.fill('0');
  stepNum << iStep;
  
  // concatenate file prefix + file number + suffix
  std::string filename     = outputDir + "/" + outputPrefix+"_"+stepNum.str() + ".vti";
  
  // open file 
  std::fstream outFile;
  outFile.open(filename.c_str(), std::ios_base::out);
  
  // write header
  outFile << "<?xml version=\"1.0\"?>\n";
  if (isBigEndian()) {
    outFile << "<VTKFile type=\"ImageData\" version=\"0.1\" byte_order=\"BigEndian\">\n";
  } else {
    outFile << "<VTKFile type=\"ImageData\" version=\"0.1\" byte_order=\"LittleEndian\">\n";
  }

  // write mesh extent
  outFile << "  <ImageData WholeExtent=\""
	  << 0 << " " << nx << " "
	  << 0 << " " << ny << " "
	  << 0 << " " << nz  << " "
	  <<  "\" Origin=\"0 0 0\" Spacing=\"1 1 1\">\n";
  outFile << "  <Piece Extent=\""
	  << 0 << " " << nx << " "
	  << 0 << " " << ny << " "
	  << 0 << " " << nz << " "    
	  << "\">\n";
  
  outFile << "    <PointData>\n";
  outFile << "    </PointData>\n";
  outFile << "    <CellData>\n";

  // write data array (ascii), remove ghost cells
  for ( iVar=0; iVar<nbvar; iVar++) {
    outFile << "    <DataArray type=\"";
    if (useDouble)
      outFile << "Float64";
    else
      outFile << "Float32";
    outFile << "\" Name=\"" << m_variables_names[iVar] << "\" format=\"ascii\" >\n";
    
    for (int index=0; index<ijksize; ++index) {
      //index2coord(index,i,j,k,isize,jsize,ksize);

      // enforce the use of left layout (Ok for CUDA)
      // but for OpenMP, we will need to transpose
      k = index / ijsize;
      j = (index - k*ijsize) / isize;
      i = index - j*isize - k*ijsize;

      if (k>=kmin+ghostWidth and k<=kmax-ghostWidth and
	  j>=jmin+ghostWidth and j<=jmax-ghostWidth and
	  i>=imin+ghostWidth and i<=imax-ghostWidth) {
#ifdef CUDA
    	outFile << Uhost(index , iVar) << " ";
#else
	int index2 = k+ksize*j+ksize*jsize*i;
    	outFile << Uhost(index2 , iVar) << " ";
#endif
      }
    }
    outFile << "\n    </DataArray>\n";
  } // end for iVar

  outFile << "    </CellData>\n";

  // write footer
  outFile << "  </Piece>\n";
  outFile << "  </ImageData>\n";
  outFile << "</VTKFile>\n";
  
  outFile.close();

} // SolverMHDMuscl3D::saveVTK

} // namespace ppkMHD
