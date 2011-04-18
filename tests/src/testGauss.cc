#include <iostream>
#include <fstream>

#include <cfit/parameter.hh>
#include <cfit/variable.hh>
#include <cfit/dataset.hh>
#include <cfit/chi2.hh>
#include <cfit/pdf.hh>

#include <cfit/models/gauss.hh>

#include <Minuit/MnPrint.h>

#ifdef MPI_ON
#include <mpi.h>
#endif

#define MIN   ( -10. )
#define MAX   (  10. )
#define NBIN  ( 100. )

#define RANGE ( MAX - MIN )

// Auxiliary functions to compute the bin number from a point, the center of
//    a bin, the (approximate) Poisson error, and to read a data file.
int    bin      ( double x ) { return int( NBIN / RANGE * ( x - MIN ) ); }
double binCenter( int    b ) { return RANGE / NBIN * ( b + .5 ) + MIN;   }
double error    ( double y ) { return y == 0 ? 1. : sqrt( y ); }
void   readData ( std::string fileName, Dataset& data )
{
  double x;
  double y[ 100 ];

  for ( int b = 0; b < 100; b++ )
    y[ b ] = 0;

  std::ifstream file( fileName.c_str() );
  while ( file >> x )
    y[ bin( x ) ]++;

  for ( int b = 0; b < 100; b++ )
    {
      data.push( "x", binCenter( b ) );
      data.push( "y", y[ b ], error( y[ b ] ) );
    }

  file.close();

  return;
}



int main( int argc, char** argv )
{
  // If working with MPI, initialize it.
#ifdef MPI_ON
  MPI::Init( argc, argv );
  const int rank = MPI::COMM_WORLD.Get_rank();
#endif

  // Data container.
  Dataset data;

  // Read the data. If working with MPI, only the root
  //    process reads the data, then it gets scattered.
#ifdef MPI_ON
  if ( rank == 0 )
    readData( "data/gauss.dat", data );
  data.scatter();
#else
  readData( "data/gauss.dat", data );
#endif

  // Variables the model depends on.
  Variable x( "x" );
  Variable y( "y" );

  // Parameters of the model.
  Parameter area1 ( "area1" , 2.e5, 300. );
  Parameter area2 ( "area2" , 2.e5, 300. );
  Parameter mean1 ( "mean1" , -1. , .003 ); 
  Parameter mean2 ( "mean2" , 2.  , .003 );
  Parameter sigma1( "sigma1", 2.  , .003 );
  Parameter sigma2( "sigma2", 1.  , .003 );

  // Definition of the pdf.
  Gauss g1( x, mean1, sigma1 );
  Gauss g2( x, mean2, sigma2 );

  Pdf sum = area1 * g1 + area2 * g2;

  // Definition of the minimizer from the pdf.
  Chi2 chi2( sum, y, data );

  // Compute the minimum.
  FunctionMinimum min = chi2.minimize();

  // If working with MPI, only the root process outputs the result.
#ifdef MPI_ON
  if ( rank == 0 )
    std::cout << min << std::endl;

  MPI::Finalize();
#else
  std::cout << min << std::endl;
#endif

  return 0;
}