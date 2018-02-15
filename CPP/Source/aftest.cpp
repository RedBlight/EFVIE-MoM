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

#include <EFVIEMOM.hpp>

	//////////////////////////////////////////////////////////////////////////
	
	inline void GreenVolVolIntegralEdge(
		double& edgeGIVV,
		double& edgeGDVV,
		const LUV::LuVector3< double >& dirN,
		const LUV::LuVector3< double >& vecRho,
		const double& magD,
		const LUV::LuVector3< double >& vecObs,
		const LUV::LuVector3< double >& vecTetra,
		const LUV::LuVector3< double >& vecFace,
		const LUV::LuVector3< double >& vecV1,
		const LUV::LuVector3< double >& vecV2
	)
	{
		LUV::LuVector3< double > dirU = -LUV::LineNormalP( vecFace, vecV1, vecV2 );
		LUV::LuVector3< double > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector3< double > vecS = vecV2 + vecV1;
		LUV::LuVector3< double > vecM = vecV2 - vecV1;
		LUV::LuVector3< double > dirM = Unit( vecM );
		LUV::LuVector3< double > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector3< double > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector3< double > vecRhoP = ( vecS + vecB ) / 2;
		
		LUV::LuVector3< double > vecPM = vecRhoM - vecRho;
		LUV::LuVector3< double > vecPP = vecRhoP - vecRho;

		double magLM = LUV::Dot( vecPM, dirL );
		double magLP = LUV::Dot( vecPP, dirL );
		double magP0 = abs( LUV::Dot( vecPM, dirU ) );

		LUV::LuVector3< double > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		double magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		double magRM = LUV::Length( vecObs - vecRhoM );
		double magRP = LUV::Length( vecObs - vecRhoP );

		double magR0S = magR0 * magR0;
		double magAbsD = abs( magD );
		double magDS = magD * magD;
		double magAbsDC = magAbsD * magDS;

		double magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		double magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		double magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );
		double magA4 = ( magP0 * ( magR0S + 2.0 * magDS ) ) / 2.0;
		double magA5 = ( magP0 * ( magLP * magRP - magLM * magRM ) ) / 2.0;

		double magA6 = LUV::Dot( dirP0, dirU );
		double magA7 = magA1 - magA2;

		edgeGIVV += magA6 * ( magAbsD * magA7 - magP0 * magA3 );
		edgeGDVV += magA6 * ( magA3 * magA4 + magA5 - magAbsDC * magA7 );
	}

	inline void GreenVolVolIntegralFace(
		double& GIVV,
		LUV::LuVector3< double >& GDVV,
		const LUV::LuVector3< double >& vecObs,
		const LUV::LuVector3< double >& vecTetra,
		const LUV::LuVector3< double >& vecV1,
		const LUV::LuVector3< double >& vecV2,
		const LUV::LuVector3< double >& vecV3
	)
	{
		LUV::LuVector3< double > dirN = -LUV::PlaneNormalP( vecTetra, vecV1, vecV2, vecV3 );
		LUV::LuVector3< double > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		double magD = LUV::Dot( vecObs - vecRho, dirN );
		double magAbsD = abs( magD );
		
		double edgeGIVV = 0;
		double edgeGDVV = 0;
		
		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV1, vecV2, vecV3 );
		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV2, vecV3, vecV1 );
		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV3, vecV1, vecV2 );
		
		GIVV += magD * edgeGIVV;
		GDVV += dirN * edgeGDVV;
	}

	inline void GreenVolVolIntegral(
		double& GIVV,
		LUV::LuVector3< double >& GDVV,
		const LUV::LuVector3< double >& vecObs,
		const LUV::LuVector3< double >& vecV1,
		const LUV::LuVector3< double >& vecV2,
		const LUV::LuVector3< double >& vecV3,
		const LUV::LuVector3< double >& vecV4
	)
	{
		GreenVolVolIntegralFace( GIVV, GDVV, vecObs, vecV1, vecV2, vecV3, vecV4 );
		GreenVolVolIntegralFace( GIVV, GDVV, vecObs, vecV2, vecV3, vecV4, vecV1 );
		GreenVolVolIntegralFace( GIVV, GDVV, vecObs, vecV3, vecV4, vecV1, vecV2 );
		GreenVolVolIntegralFace( GIVV, GDVV, vecObs, vecV4, vecV1, vecV2, vecV3 );
		GIVV *= 1.0 / 2.0;
		GDVV *= 1.0 / 3.0;
	}

	//////////////////////////////////////////////////////////////////////////























		//////////////////////////////////////////////////////////////////////////
	
	inline void GreenVolIntegralEdge(
		double& edgeGISV,
		const LUV::LuVector3< double >& dirN,
		const LUV::LuVector3< double >& vecRho,
		const double& magAbsD,
		const LUV::LuVector3< double >& vecObs,
		const LUV::LuVector3< double >& vecTetra,
		const LUV::LuVector3< double >& vecFace,
		const LUV::LuVector3< double >& vecV1,
		const LUV::LuVector3< double >& vecV2
	)
	{
		LUV::LuVector3< double > dirU = -LUV::LineNormalP( vecFace, vecV1, vecV2 );
		LUV::LuVector3< double > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector3< double > vecS = vecV2 + vecV1;
		LUV::LuVector3< double > vecM = vecV2 - vecV1;
		LUV::LuVector3< double > dirM = Unit( vecM );
		LUV::LuVector3< double > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector3< double > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector3< double > vecRhoP = ( vecS + vecB ) / 2;
		
		LUV::LuVector3< double > vecPM = vecRhoM - vecRho;
		LUV::LuVector3< double > vecPP = vecRhoP - vecRho;

		double magLM = LUV::Dot( vecPM, dirL );
		double magLP = LUV::Dot( vecPP, dirL );
		double magP0 = abs( LUV::Dot( vecPM, dirU ) );

		LUV::LuVector3< double > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		double magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		double magRM = LUV::Length( vecObs - vecRhoM );
		double magRP = LUV::Length( vecObs - vecRhoP );

		double magR0S = magR0 * magR0;

		double magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		double magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		double magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );

		double magA6 = LUV::Dot( dirP0, dirU );
		double magA7 = magA1 - magA2;

		edgeGISV += magA6 * ( magAbsD * magA7 - magP0 * magA3 );
	}

	inline void GreenVolIntegralFace(
		double& GISV,
		const LUV::LuVector3< double >& vecObs,
		const LUV::LuVector3< double >& vecTetra,
		const LUV::LuVector3< double >& vecV1,
		const LUV::LuVector3< double >& vecV2,
		const LUV::LuVector3< double >& vecV3
	)
	{
		LUV::LuVector3< double > dirN = -LUV::PlaneNormalP( vecTetra, vecV1, vecV2, vecV3 );
		LUV::LuVector3< double > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		double magD = LUV::Dot( vecObs - vecRho, dirN );
		double magAbsD = abs( magD );
		
		double edgeGISV = 0;
		
		GreenVolIntegralEdge( edgeGISV, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV1, vecV2, vecV3 );
		GreenVolIntegralEdge( edgeGISV, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV2, vecV3, vecV1 );
		GreenVolIntegralEdge( edgeGISV, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV3, vecV1, vecV2 );
		
		GISV += magD * edgeGISV;
	}

	inline void GreenVolIntegral(
		double& GISV,
		const LUV::LuVector3< double >& vecObs,
		const LUV::LuVector3< double >& vecV1,
		const LUV::LuVector3< double >& vecV2,
		const LUV::LuVector3< double >& vecV3,
		const LUV::LuVector3< double >& vecV4
	)
	{
		GreenVolIntegralFace( GISV, vecObs, vecV1, vecV2, vecV3, vecV4 );
		GreenVolIntegralFace( GISV, vecObs, vecV2, vecV3, vecV4, vecV1 );
		GreenVolIntegralFace( GISV, vecObs, vecV3, vecV4, vecV1, vecV2 );
		GreenVolIntegralFace( GISV, vecObs, vecV4, vecV1, vecV2, vecV3 );
		GISV *= 1.0 / 2.0;
	}

	//////////////////////////////////////////////////////////////////////////












	
	//////////////////////////////////////////////////////////////////////////
	
	inline void GreenSurIntegralEdge(
		double& edgeGI_S,
		const LUV::LuVector3< double >& dirN,
		const LUV::LuVector3< double >& vecRho,
		const double& magAbsD,
		const LUV::LuVector3< double >& vecObs,
		const LUV::LuVector3< double >& vecTetra,
		const LUV::LuVector3< double >& vecFace,
		const LUV::LuVector3< double >& vecV1,
		const LUV::LuVector3< double >& vecV2
	)
	{
		LUV::LuVector3< double > dirU = -LUV::LineNormalP( vecFace, vecV1, vecV2 );
		LUV::LuVector3< double > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector3< double > vecS = vecV2 + vecV1;
		LUV::LuVector3< double > vecM = vecV2 - vecV1;
		LUV::LuVector3< double > dirM = Unit( vecM );
		LUV::LuVector3< double > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector3< double > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector3< double > vecRhoP = ( vecS + vecB ) / 2;
		
		LUV::LuVector3< double > vecPM = vecRhoM - vecRho;
		LUV::LuVector3< double > vecPP = vecRhoP - vecRho;

		double magLM = LUV::Dot( vecPM, dirL );
		double magLP = LUV::Dot( vecPP, dirL );
		double magP0 = abs( LUV::Dot( vecPM, dirU ) );

		LUV::LuVector3< double > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		double magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		double magRM = LUV::Length( vecObs - vecRhoM );
		double magRP = LUV::Length( vecObs - vecRhoP );

		double magR0S = magR0 * magR0;

		double magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		double magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		double magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );

		edgeGI_S += LUV::Dot( dirP0, dirU ) * ( magP0 * magA3 - magAbsD * ( magA1 - magA2 ) );
	}

	inline void GreenSurIntegral(
		double& GI_S,
		const LUV::LuVector3< double >& vecObs,
		const LUV::LuVector3< double >& vecTetra,
		const LUV::LuVector3< double >& vecV1,
		const LUV::LuVector3< double >& vecV2,
		const LUV::LuVector3< double >& vecV3
	)
	{
		LUV::LuVector3< double > dirN = -LUV::PlaneNormalP( vecTetra, vecV1, vecV2, vecV3 );
		LUV::LuVector3< double > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		double magAbsD = abs( LUV::Dot( vecObs - vecRho, dirN ) );

		GreenSurIntegralEdge( GI_S, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV1, vecV2, vecV3 );
		GreenSurIntegralEdge( GI_S, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV2, vecV3, vecV1 );
		GreenSurIntegralEdge( GI_S, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV3, vecV1, vecV2 );
	}

	//////////////////////////////////////////////////////////////////////////

void GreenIntegralUnitTest();

int main(int argc, char *argv[])
{
    try
	{
		// Select a device and display arrayfire info
        int device = argc > 1 ? atoi(argv[1]) : 0;
        af::setDevice( device );
        af::info();

		cout << endl;

		GreenIntegralUnitTest();

		cout << endl;


		//af::array B = af::constant( 2, 2, 2, f64 );

		////af_print( A * std::complex< double >( 0, 1 ) );

		//af::cdouble A(0,1);
		//B = B * A;
		//af_print( B );

		//std::complex< double > j(0,1);
		//LUV::LuVector3< std::complex< double > > A = 0;
		//LUV::LuVector3< double > B = 1.0/sqrt(3.0);
		//std::complex< double > C = std::exp( j * af::Pi / 4.0 );

		//std::cout << A << std::endl;

		//std::cout << C * B << std::endl;
		//std::cout << LUV::Length( C * B ) << std::endl;

		/*std::complex< float > A = std::complex< float >( 3, 5 ) * (float)(10.0); */

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

void GreenIntegralUnitTest()
{
	using namespace std;

	LUV::LuVector3d vecObs( 3.0, 3.0, 3.0 );
	LUV::LuVector3d vecV1( 0.0, 0.0, 0.0 );
	LUV::LuVector3d vecV2( 1.0, 0.0, 0.0 );
	LUV::LuVector3d vecV3( 0.0, 2.0, 0.0 );
	LUV::LuVector3d vecV4( 0.0, 0.0, 3.0 );
	
	double resfac(0);
	double result(0);
	LUV::LuVector3d resultVec(0,0,0);

	GreenVolVolIntegral( result, resultVec, vecObs, vecV1, vecV2, vecV3, vecV4 );
	GreenSurIntegral( resfac, vecObs, vecV1, vecV2, vecV3, vecV4 );
	cout << resfac << endl;
	cout << result << endl;
	cout << resultVec << endl;

	double vData[12] = {
		0.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 2.0, 0.0,
		0.0, 0.0, 3.0
	};

	uint64_t iData[4] = {
		0, 1, 2, 3
	};

	uint64_t fData[3] = {
		1, 2, 3
	};
	
	shared_ptr< uint64_t > tetraIndexData( new uint64_t[4], []( uint64_t* ptr ){ delete[] ptr; } );
	shared_ptr< double > tetraVertexData( new double[12], []( double* ptr ){ delete[] ptr; } );
	shared_ptr< double > tetraQuadData( new double[64*4], []( double* ptr ){ delete[] ptr; } );
	
	shared_ptr< uint64_t > triIndexData( new uint64_t[3], []( uint64_t* ptr ){ delete[] ptr; } );
	shared_ptr< double > triQuadData( new double[7*4], []( double* ptr ){ delete[] ptr; } );
	
	copy( iData, iData + 4, tetraIndexData.get() );
	copy( vData, vData + 12, tetraVertexData.get() );
	copy( fData, fData + 3, triIndexData.get() );

	TetraQuadGenerator64< FLOAT_T > tetQuadGen;
	tetQuadGen.Generate(
		1,
		tetraVertexData,
		tetraIndexData,
		tetraQuadData
	);

	TriQuadGenerator7< double > triQuadGen;
	triQuadGen.Generate(
		1,
		tetraVertexData,
		triIndexData,
		triQuadData
	);

	result = 0;
	resultVec = 0;
	for( uint64_t idq = 0; idq < 64; ++idq )
	{
		double* magW = reinterpret_cast< double* >( &(tetraQuadData.get())[4*idq] );
		LUV::LuVector3d* vecQ = reinterpret_cast< LUV::LuVector3d* >( &(tetraQuadData.get())[4*idq+1] );

		result += *magW * ( 1 / LUV::Length( vecObs - *vecQ ) );
		resultVec += *magW * ( ( vecObs - *vecQ ) / LUV::Length( vecObs - *vecQ ) );
	}

	resfac = 0;
	for( uint64_t idq = 0; idq < 7; ++idq )
	{
		double* magW = reinterpret_cast< double* >( &(triQuadData.get())[4*idq] );
		LUV::LuVector3d* vecQ = reinterpret_cast< LUV::LuVector3d* >( &(triQuadData.get())[4*idq+1] );

		resfac += *magW * ( 1 / LUV::Length( vecObs - *vecQ ) );
	}
	
	cout << resfac << endl;
	cout << result << endl;
	cout << resultVec << endl;
}

