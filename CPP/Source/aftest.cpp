/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <_BitDepthDefines.hpp>

#include <arrayfire.h>
#include <cstdio>
#include <cstdlib>
#include <complex>
#include <thread>
#include <iostream>

#include <_LuVector.hpp>

int main(int argc, char *argv[])
{
    try
	{
		// Select a device and display arrayfire info
        int device = argc > 1 ? atoi(argv[1]) : 0;
        af::setDevice( device );
        af::info();

		std::complex< float > A = std::complex< float >( 3, 5 ) * (float)(10.0); 

		//af::array A = af::randu( 1, 1, c64 );
		//af_print( A );

		//std::complex< double > z;
		//A.host( &z );

		//std::cout << z << std::endl;

		//af::array A = af::randu( 5, 5, c64 );
		//A( 0, 0 ) = af::cdouble( 0, 0 );
		//A( 2, 2 ) = af::cdouble( 0, 0 );
		//af_print( A );

		//A( af::iszero( af::real( A ) ) && af::iszero( af::imag( A ) ) ) = af::cdouble( 99, 99 );
		//af_print( A );

		
		//double jTemp[] = { 0, 1 };
		//af::array j = af::array( 1, reinterpret_cast< AF_CFLOAT* >( jTemp ) );

		//af::array A = af::constant( 2, 2, 2, f64 );
		//af::array B = AF_CFLOAT( 3, 5 ) * A;
		//af_print( B );
		
		
		//af::array A = af::constant( 2, 2, 2, f64 );
		//af::array B = af::constant( 3, 2, 2, f64 );
		//af::array C = af::constant( 5, 2, 2, f64 );
		//af_print( A );
		//af_print( B );
		//af_print( C );

		//LUV::LuVector< 3, af::array > KKK( A, B, C );

		//LUV::LuVector< 3, af::array > LLL( KKK );
		//
		//af_print( LUV::Dot( KKK, KKK ) );
		//
		//af_print( KKK[0] );

		//af::array RES = LUV::Dot( KKK, KKK );
		//af_print( RES );







		//std::complex< double > arr[6] = {{1,2},{3,4},{5,6},{7,8},{9,10},{11,12}};

		//array afarr = array( 3, 2, c64 );
		//afarr.write( arr, 8 * 12, afHost );
		//af_print( afarr );


		//unsigned int n = std::thread::hardware_concurrency();
		//std::cout << n << " concurrent threads are supported.\n";

		//af::array afTetra2ComputeMatrix = af::array( 3, quadCount, AF_COMPLEX_T );
		//afTetra2ComputeMatrix.write( reinterpret_cast< T* >( tetra2QuadFieldE ), SIZEOF_CT * 3 * quadCount, afHost );

		//array A = randu( 2, 2, f64 );
  //      af_print( A );

		//array B = randu( 2, 2, c64 );
  //      af_print( B );
		//af_print( sum(B,0) );
		//B = sum(B,0);
  //      af_print( B );
		//B = sum(B,1);
  //      af_print( B );

		//std::complex< double > a;

		//B.host( &a );

		//std::cout << a << std::endl;


		//A *= B;
  //      af_print( A );

		//array C = A * B;
  //      af_print( C );

		//array D = C.row( 0 );
		//D *= 1000;
  //      af_print( C );



		/*
        printf("Create a 5-by-3 matrix of random floats on the GPU\n");
        array A = randu(5,3, f32);
        af_print(A);

        printf("Element-wise arithmetic\n");
        array B = sin(A) + 1.5;
        af_print(B);

        printf("Negate the first three elements of second column\n");
        B(seq(0, 2), 1) = B(seq(0, 2), 1) * -1;
        af_print(B);

        printf("Fourier transform the result\n");
        array C = fft(B);
        af_print(C);

        printf("Grab last row\n");
        array c = C.row(end);
        af_print(c);

        printf("Scan Test\n");
        dim4 dims(16, 4, 1, 1);
        array r = constant(2, dims);
        af_print(r);

        printf("Scan\n");
        array S = af::scan(r, 0, AF_BINARY_MUL);
        af_print(S);

        printf("Create 2-by-3 matrix from host data\n");
        float d[] = { 1, 2, 3, 4, 5, 6 };
        array D(2, 3, d, afHost);
        af_print(D);

        printf("Copy last column onto first\n");
        D.col(0) = D.col(end);
        af_print(D);

        // Sort A
        printf("Sort A and print sorted array and corresponding indices\n");
        array vals, inds;
        sort(vals, inds, A);
        af_print(vals);
        af_print(inds);
		*/

    } catch (af::exception& e) {

        fprintf(stderr, "%s\n", e.what());
        throw;
    }

	system( "pause" );

    return 0;
}
