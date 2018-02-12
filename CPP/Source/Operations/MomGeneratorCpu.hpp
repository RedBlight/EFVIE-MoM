#ifndef MOM_GENERATOR_CPU_INCLUDED
#define MOM_GENERATOR_CPU_INCLUDED

// generatin' yo mama
// yo mama so fat, she (sometimes) doesn't fit into a 24 GB ram.

#include <_BitDepthDefines.hpp>

#include <fstream>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <utility>
#include <memory>
#include <map>
#include <complex>
#include <thread>

#include <arrayfire.h>

#include <_LuVector.hpp>

using namespace std;

template< class T >
class MomGeneratorCpu
{
public:
	UINT_T threadCount_;
	vector< thread* > threadList_;
	
	complex< T > j_;

	T eps0_;
	T mu0_;
	T c0_;

	T waveNumber_;
	complex< T > waveNumberJ_;

	//temporary list of pointers to use across functions.
	UINT_T faceCount_;
	UINT_T tetraQuadCount_;
	UINT_T faceQuadCount_;
	UINT_T tetraQuadBlockSize_;
	UINT_T faceQuadBlockSize_;

	T* vertexDataPtr_;
	T* tetraQuadDataPtr_;
	T* faceQuadDataPtr_;
	T* emPropDataPtr_;

	UINT_T* tetraVertexIndexPtr_;
	UINT_T* faceVertexIndexPtr_;
	UINT_T* faceTetraIndexPtr_;
	UINT_T* tetraFaceIndexPtr_;

	complex< T >* momDataPtr_;

public:
	MomGeneratorCpu(
		const T& waveNumber,
		const UINT_T& faceCount,
		const UINT_T& tetraQuadCount,
		const UINT_T& faceQuadCount,
		const shared_ptr< T >& vertexData,
		const shared_ptr< T >& tetraQuadData,
		const shared_ptr< T >& faceQuadData,
		const shared_ptr< T >& emPropData,
		const shared_ptr< UINT_T >& tetraVertexIndex,
		const shared_ptr< UINT_T >& faceVertexIndex,
		const shared_ptr< UINT_T >& faceTetraIndex,
		const shared_ptr< UINT_T >& tetraFaceIndex,
		shared_ptr< complex< T > >& momData // preallocated
	) :
		threadCount_( thread::hardware_concurrency() * 2 ),
		threadList_(),
		j_( 0.0, 1.0 ),
		eps0_( 8.85418782E-12 ),
		mu0_( 1.25663706E-6 ),
		c0_( 299792458.0 ),
		waveNumber_( waveNumber ),
		waveNumberJ_( waveNumber * j_ ),
		faceCount_( faceCount ),
		tetraQuadCount_( tetraQuadCount ),
		faceQuadCount_( faceQuadCount ),
		tetraQuadBlockSize_( tetraQuadCount_ * 4 ),
		faceQuadBlockSize_( faceQuadCount_ * 4 ),
		vertexDataPtr_( vertexData.get() ),
		tetraQuadDataPtr_( tetraQuadData.get() ),
		faceQuadDataPtr_( faceQuadData.get() ),
		emPropDataPtr_( emPropData.get() ),
		tetraVertexIndexPtr_( tetraVertexIndex.get() ),
		faceVertexIndexPtr_( faceVertexIndex.get() ),
		faceTetraIndexPtr_( faceTetraIndex.get() ),
		tetraFaceIndexPtr_( tetraFaceIndex.get() ),
		momDataPtr_( momData.get() )
	{
		threadList_.reserve( threadCount_ );
	}

	~MomGeneratorCpu()
	{

	}

	inline UINT_T GetSwgVert( const UINT_T* faceVerts, const UINT_T* tetraVerts ) const
	{
		return
		tetraVerts[0] != faceVerts[0] && tetraVerts[0] != faceVerts[1] && tetraVerts[0] != faceVerts[2] ? tetraVerts[0] : (
		tetraVerts[1] != faceVerts[0] && tetraVerts[1] != faceVerts[1] && tetraVerts[1] != faceVerts[2] ? tetraVerts[1] : (
		tetraVerts[2] != faceVerts[0] && tetraVerts[2] != faceVerts[1] && tetraVerts[2] != faceVerts[2] ? tetraVerts[2] : (
		tetraVerts[3]
		)));
	}

	inline complex< T > GreenWhole( const T& R )
	{
		return exp( waveNumberJ_ * R ) / R;
	}

	inline complex< T > GreenRegular( const T& R )
	{
		return ( exp( waveNumberJ_ * R ) - 1.0 ) / R;
	}

	inline T GreenSingular( const T& R )
	{
		return 1.0 / R;
	}

	inline T GreenVolumeIntegralScalar_Edge(
		const T& magD,
		const LUV::LuVector3< T >& dirN,
		const LUV::LuVector3< T >& vecRho,
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecTetra,
		const LUV::LuVector3< T >& vecFace,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2
	)
	{
		LUV::LuVector3< T > dirU = -LUV::LineNormalP( vecV3, vecV1, vecV2 );
		LUV::LuVector3< T > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector3< T > vecS = vecV2 + vecV1;
		LUV::LuVector3< T > vecM = vecV2 - vecV1;
		LUV::LuVector3< T > dirM = Unit( vecM );
		LUV::LuVector3< T > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector3< T > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector3< T > vecRhoP = ( vecS + vecB ) / 2;

		LUV::LuVector3< T > vecL = vecRhoP - vecRhoM;
		T magL = LUV::Length( vecL );
		
		LUV::LuVector3< T > vecPM = vecRhoM - vecRho;
		LUV::LuVector3< T > vecPP = vecRhoP - vecRho;

		T magLM = LUV::Dot( vecPM, dirL );
		T magLP = LUV::Dot( vecPP, dirL );
		T magP0 = af::abs( LUV::Dot( vecPM, dirU ) );
		T magPM = LUV::Length( vecPM );
		T magPP = LUV::Length( vecPM );

		LUV::LuVector3< T > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		T magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		T magRM = LUV::Length( vecObs - vecRhoM );
		T magRP = LUV::Length( vecObs - vecRhoP );

		//

		T magR0S = magR0 * magR0;
		T magAbsD = af::abs( magD );
		T magDS = magD * magD;

		///

		T magA1 = af::atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		T magA2 = af::atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		T magA3 = af::log( ( magRP + magLP ) / ( magRM + magLM ) );

		return magD * ( LUV::Dot( dirP0, dirU ) * ( magAbsD * ( magA1 - magA2 - magP0 * magA3 ) ) );
	}

	inline T GreenVolumeIntegralScalar_Face(
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecTetra,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2,
		const LUV::LuVector3< T >& vecV3
	)
	{
		LUV::LuVector3< T > dirN = -LUV::PlaneNormalP( vecTetra, vecV1, vecV2, vecV3 );
		LUV::LuVector3< T > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		T magD = LUV::Dot( vecObs - vecRho, dirN );
		T magAbsD = abs( magD );
		T magDS = magD * magD;

		return GreenVolumeIntegralScalar_Edge( magD, dirN, vecRho, vecObs, vecTetra, vecV1, vecV2, vecV3 )
			+ GreenVolumeIntegralScalar_Edge( magD, dirN, vecRho, vecObs, vecTetra, vecV2, vecV3, vecV1 )
			+ GreenVolumeIntegralScalar_Edge( magD, dirN, vecRho, vecObs, vecTetra, vecV3, vecV1, vecV2 );

	}

	inline T GreenVolumeIntegralScalar(
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2,
		const LUV::LuVector3< T >& vecV3,
		const LUV::LuVector3< T >& vecV4  )
	{
		return GreenVolumeIntegralScalar_Face( vecObs, vecV1, vecV2, vecV3, vecV4 )
			+ GreenVolumeIntegralScalar_Face( vecObs, vecV2, vecV3, vecV4, vecV1 )
			+ GreenVolumeIntegralScalar_Face( vecObs, vecV3, vecV4, vecV1, vecV2 )
			+ GreenVolumeIntegralScalar_Face( vecObs, vecV4, vecV1, vecV2, vecV3 );



		LUV::LuVector3< T > dirN1 = -LUV::PlaneNormalP( vecV4, vecV1, vecV2, vecV3 );



		LUV::LuVector< 3, af::array > dirN = -LUV::PlaneNormalP( vecV4, vecV1, vecV2, vecV3 );
		LUV::LuVector< 3, af::array > dirU = -LUV::LineNormalP( vecV3, vecV1, vecV2 );
		LUV::LuVector< 3, af::array > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector< 3, af::array > vecS = vecV2 + vecV1;
		LUV::LuVector< 3, af::array > vecM = vecV2 - vecV1;
		LUV::LuVector< 3, af::array > dirM = Unit( vecM );
		LUV::LuVector< 3, af::array > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector< 3, af::array > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector< 3, af::array > vecRhoP = ( vecS + vecB ) / 2;
		LUV::LuVector< 3, af::array > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		
		af::array magD = LUV::Dot( vecObs - vecRho, dirN );

		LUV::LuVector< 3, af::array > vecL = vecRhoP - vecRhoM;
		af::array magL = LUV::Length( vecL );
		
		LUV::LuVector< 3, af::array > vecPM = vecRhoM - vecRho;
		LUV::LuVector< 3, af::array > vecPP = vecRhoP - vecRho;

		af::array magLM = LUV::Dot( vecPM, dirL );
		af::array magLP = LUV::Dot( vecPP, dirL );
		af::array magP0 = af::abs( LUV::Dot( vecPM, dirU ) );
		af::array magPM = LUV::Length( vecPM );
		af::array magPP = LUV::Length( vecPM );

		LUV::LuVector< 3, af::array > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		af::array magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		af::array magRM = LUV::Length( vecObs - vecRhoM );
		af::array magRP = LUV::Length( vecObs - vecRhoP );

		//

		af::array magR0S = magR0 * magR0;
		af::array magAbsD = af::abs( magD );
		af::array magDS = magD * magD;

		///

		af::array magA1 = af::atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		af::array magA2 = af::atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		af::array magA3 = af::log( ( magRP + magLP ) / ( magRM + magLM ) );
		af::array magA4 = ( magP0 * ( magR0S + 2.0 * magDS ) ) / 2.0;
		af::array magA5 = ( magP0 * ( magLP * magRP - magLM * magRM ) ) / 2.0;

		GIVV = af::sum( af::sum( magD * ( LUV::Dot( dirP0, dirU ) * ( magAbsD * ( magA1 - magA2 - magP0 * magA3 ) ) ) , 2 ), 3 ) / 2;
		GDVV = dirN * ( LUV::Dot( dirP0, dirU ) * ( magA3 * magA4 + magA5 - magAbsD * magAbsD * magAbsD * ( magA1 - magA2 ) ) );
		GDVV[0] = af::sum( af::sum( GDVV[0], 2 ), 3 ) / 3;
		GDVV[1] = af::sum( af::sum( GDVV[1], 2 ), 3 ) / 3;
		GDVV[2] = af::sum( af::sum( GDVV[2], 2 ), 3 ) / 3;
	}

	/*inline complex< T > TetrahedralIntegral( const UINT_T& idFace, const UINT_T& idTetra ) const
	{
		UINT_T idTetraVertex[ 4 ] = {
			tetraVertexIndexPtr_[ 4 * idTetra ],
			tetraVertexIndexPtr_[ 4 * idTetra + 1 ],
			tetraVertexIndexPtr_[ 4 * idTetra + 2 ],
			tetraVertexIndexPtr_[ 4 * idTetra + 3 ]
		};

		LUV::LuVector3< T >* tetraVertex[ 4 ] = {
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertex[ 0 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertex[ 1 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertex[ 2 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertex[ 3 ] ] )
		};

		T tetraVolume = LUV::TetrahedronVolume< T >( *tetraVertex[ 0 ], *tetraVertex[ 1 ], *tetraVertex[ 2 ], *tetraVertex[ 3 ] );

		UINT_T idSwgVertex = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFace ], &tetraVertexIndexPtr_[ 4 * idTetra ] );

		LUV::LuVector3< T >* swgVertex = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idSwgVertex ] );

		LUV::LuVector3c< T >* tetraQuadFieldE = new LUV::LuVector3c< T >[ quadCount_ ];
		for( UINT_T idQuad = 0; idQuad < quadCount_; ++idQuad )
		{
			tetraQuadFieldE[ idQuad ] = incWave_->FieldE(
				*( reinterpret_cast< LUV::LuVector3< T >* >(
					&tetraQuadDataPtr_[ quadBlockSize_ * idTetra + 4 * idQuad + 1 ]
				) )
			);
		}

		af::array afTetraComputeMatrix = af::array( 3, quadCount_, AF_COMPLEX_T );
		afTetraComputeMatrix.write( reinterpret_cast< T* >( tetraQuadFieldE ), SIZEOF_CT * 3 * quadCount_, afHost );

		delete[] tetraQuadFieldE;

		af::array afTetraQuadData = af::array( 4, quadCount_, AF_FLOAT_T );
		afTetraQuadData.write( &tetraQuadDataPtr_[ quadBlockSize_ * idTetra ], SIZEOF_T * 4 * quadCount_, afHost );

		afTetraComputeMatrix.row( 0 ) *= afTetraQuadData.row( 1 ) - (*swgVertex)[ 0 ];
		afTetraComputeMatrix.row( 1 ) *= afTetraQuadData.row( 2 ) - (*swgVertex)[ 1 ];
		afTetraComputeMatrix.row( 2 ) *= afTetraQuadData.row( 3 ) - (*swgVertex)[ 2 ];
		afTetraComputeMatrix = af::sum( afTetraComputeMatrix, 0 );
		afTetraComputeMatrix *= afTetraQuadData.row( 0 );
		afTetraComputeMatrix = sum( afTetraComputeMatrix, 1 );

		complex< T > tetraResult;
		afTetraComputeMatrix.host( &tetraResult );
		tetraResult /= tetraVolume;

		return tetraResult;
	}*/
	
	inline void CalculateG_VV(
		LUV::LuVector< 3, af::array >& vecV1,
		LUV::LuVector< 3, af::array >& vecV2,
		LUV::LuVector< 3, af::array >& vecV3,
		LUV::LuVector< 3, af::array >& vecV4,
		LUV::LuVector< 3, af::array >& vecObs,
		af::array& GIVV,
		LUV::LuVector< 3, af::array >& GDVV
	)
	{
		LUV::LuVector< 3, af::array > dirN = -LUV::PlaneNormalP( vecV4, vecV1, vecV2, vecV3 );
		LUV::LuVector< 3, af::array > dirU = -LUV::LineNormalP( vecV3, vecV1, vecV2 );
		LUV::LuVector< 3, af::array > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector< 3, af::array > vecS = vecV2 + vecV1;
		LUV::LuVector< 3, af::array > vecM = vecV2 - vecV1;
		LUV::LuVector< 3, af::array > dirM = Unit( vecM );
		LUV::LuVector< 3, af::array > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector< 3, af::array > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector< 3, af::array > vecRhoP = ( vecS + vecB ) / 2;
		LUV::LuVector< 3, af::array > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		
		af::array magD = LUV::Dot( vecObs - vecRho, dirN );

		LUV::LuVector< 3, af::array > vecL = vecRhoP - vecRhoM;
		af::array magL = LUV::Length( vecL );
		
		LUV::LuVector< 3, af::array > vecPM = vecRhoM - vecRho;
		LUV::LuVector< 3, af::array > vecPP = vecRhoP - vecRho;

		af::array magLM = LUV::Dot( vecPM, dirL );
		af::array magLP = LUV::Dot( vecPP, dirL );
		af::array magP0 = af::abs( LUV::Dot( vecPM, dirU ) );
		af::array magPM = LUV::Length( vecPM );
		af::array magPP = LUV::Length( vecPM );

		LUV::LuVector< 3, af::array > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		af::array magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		af::array magRM = LUV::Length( vecObs - vecRhoM );
		af::array magRP = LUV::Length( vecObs - vecRhoP );

		//

		af::array magR0S = magR0 * magR0;
		af::array magAbsD = af::abs( magD );
		af::array magDS = magD * magD;

		///

		af::array magA1 = af::atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		af::array magA2 = af::atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		af::array magA3 = af::log( ( magRP + magLP ) / ( magRM + magLM ) );
		af::array magA4 = ( magP0 * ( magR0S + 2.0 * magDS ) ) / 2.0;
		af::array magA5 = ( magP0 * ( magLP * magRP - magLM * magRM ) ) / 2.0;

		GIVV = af::sum( af::sum( magD * ( LUV::Dot( dirP0, dirU ) * ( magAbsD * ( magA1 - magA2 - magP0 * magA3 ) ) ) , 2 ), 3 ) / 2;
		GDVV = dirN * ( LUV::Dot( dirP0, dirU ) * ( magA3 * magA4 + magA5 - magAbsD * magAbsD * magAbsD * ( magA1 - magA2 ) ) );
		GDVV[0] = af::sum( af::sum( GDVV[0], 2 ), 3 ) / 3;
		GDVV[1] = af::sum( af::sum( GDVV[1], 2 ), 3 ) / 3;
		GDVV[2] = af::sum( af::sum( GDVV[2], 2 ), 3 ) / 3;
	}
	
	inline void CalculateGISV(
		LUV::LuVector< 3, af::array >& vecV1,
		LUV::LuVector< 3, af::array >& vecV2,
		LUV::LuVector< 3, af::array >& vecV3,
		LUV::LuVector< 3, af::array >& vecV4,
		LUV::LuVector< 3, af::array >& vecObs,
		af::array& GISV
	)
	{
		LUV::LuVector< 3, af::array > dirN = -LUV::PlaneNormalP( vecV4, vecV1, vecV2, vecV3 );
		LUV::LuVector< 3, af::array > dirU = -LUV::LineNormalP( vecV3, vecV1, vecV2 );
		LUV::LuVector< 3, af::array > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector< 3, af::array > vecS = vecV2 + vecV1;
		LUV::LuVector< 3, af::array > vecM = vecV2 - vecV1;
		LUV::LuVector< 3, af::array > dirM = Unit( vecM );
		LUV::LuVector< 3, af::array > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector< 3, af::array > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector< 3, af::array > vecRhoP = ( vecS + vecB ) / 2;
		LUV::LuVector< 3, af::array > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		
		af::array magD = LUV::Dot( vecObs - vecRho, dirN );

		LUV::LuVector< 3, af::array > vecL = vecRhoP - vecRhoM;
		af::array magL = LUV::Length( vecL );
		
		LUV::LuVector< 3, af::array > vecPM = vecRhoM - vecRho;
		LUV::LuVector< 3, af::array > vecPP = vecRhoP - vecRho;

		af::array magLM = LUV::Dot( vecPM, dirL );
		af::array magLP = LUV::Dot( vecPP, dirL );
		af::array magP0 = af::abs( LUV::Dot( vecPM, dirU ) );
		af::array magPM = LUV::Length( vecPM );
		af::array magPP = LUV::Length( vecPM );

		LUV::LuVector< 3, af::array > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		af::array magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		af::array magRM = LUV::Length( vecObs - vecRhoM );
		af::array magRP = LUV::Length( vecObs - vecRhoP );

		//

		af::array magR0S = magR0 * magR0;
		af::array magAbsD = af::abs( magD );

		///

		af::array magA1 = af::atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		af::array magA2 = af::atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		af::array magA3 = af::log( ( magRP + magLP ) / ( magRM + magLM ) );

		GISV = af::sum( af::sum( magD * ( LUV::Dot( dirP0, dirU ) * ( magAbsD * ( magA1 - magA2 - magP0 * magA3 ) ) ) , 2 ), 3 ) / 2;
	}
	
	inline void CalculateGI_S(
		LUV::LuVector< 3, af::array >& vecV1,
		LUV::LuVector< 3, af::array >& vecV2,
		LUV::LuVector< 3, af::array >& vecV3,
		LUV::LuVector< 3, af::array >& vecV4,
		LUV::LuVector< 3, af::array >& vecObs,
		af::array& GI_S
	)
	{
		LUV::LuVector< 3, af::array > dirN = -LUV::PlaneNormalP( vecV4, vecV1, vecV2, vecV3 );
		LUV::LuVector< 3, af::array > dirU = -LUV::LineNormalP( vecV3, vecV1, vecV2 );
		LUV::LuVector< 3, af::array > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector< 3, af::array > vecS = vecV2 + vecV1;
		LUV::LuVector< 3, af::array > vecM = vecV2 - vecV1;
		LUV::LuVector< 3, af::array > dirM = Unit( vecM );
		LUV::LuVector< 3, af::array > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector< 3, af::array > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector< 3, af::array > vecRhoP = ( vecS + vecB ) / 2;
		LUV::LuVector< 3, af::array > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		
		af::array magD = LUV::Dot( vecObs - vecRho, dirN );

		LUV::LuVector< 3, af::array > vecL = vecRhoP - vecRhoM;
		af::array magL = LUV::Length( vecL );
		
		LUV::LuVector< 3, af::array > vecPM = vecRhoM - vecRho;
		LUV::LuVector< 3, af::array > vecPP = vecRhoP - vecRho;

		af::array magLM = LUV::Dot( vecPM, dirL );
		af::array magLP = LUV::Dot( vecPP, dirL );
		af::array magP0 = af::abs( LUV::Dot( vecPM, dirU ) );
		af::array magPM = LUV::Length( vecPM );
		af::array magPP = LUV::Length( vecPM );

		LUV::LuVector< 3, af::array > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		af::array magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		af::array magRM = LUV::Length( vecObs - vecRhoM );
		af::array magRP = LUV::Length( vecObs - vecRhoP );

		//

		af::array magR0S = magR0 * magR0;
		af::array magAbsD = af::abs( magD );

		///

		af::array magA1 = af::atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		af::array magA2 = af::atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		af::array magA3 = af::log( ( magRP + magLP ) / ( magRM + magLM ) );

		GI_S = af::sum( magD * ( LUV::Dot( dirP0, dirU ) * ( magP0 * magA3 - magAbsD * ( magA1 - magA2 ) ) ) , 2 );
	}

	inline void CalculateG(
		af::array& afFaceQuadDataM,
		af::array& afTetraQuadDataA,
		af::array& afFaceVertexN,
		af::array& afTetraVertexB,
		LUV::LuVector3< T >& swgVertexN,
		af::array& GIVV,
		af::array& GIVS,
		af::array& GISV,
		af::array& GISS,
		LUV::LuVector< 3, af::array > GDVV
	)
	{
		af::array afFaceVertexN0 = af::reorder( afFaceVertexN, 0, 2, 1, 3 );
		af::array afFaceVertexN1 = af::shift( afFaceVertexN0, 0, 0, -1, 0 );
		af::array afFaceVertexN2 = af::shift( afFaceVertexN1, 0, 0, -1, 0 );

		//////

		af::array afTetraVertexB3 = af::tile( af::reorder( afTetraVertexB, 0, 2, 1, 3 ), 1, 1, 1, 3 );

		af::array afTetraVertexB0 = afTetraVertexB3;
		afTetraVertexB0( af::span, af::span, af::span, 0 ) = af::shift( afTetraVertexB0( af::span, af::span, af::span, 0 ), 0, 0, -1, 0 );
		afTetraVertexB0( af::span, af::span, af::span, 1 ) = af::shift( afTetraVertexB0( af::span, af::span, af::span, 1 ), 0, 0, -2, 0 );
		afTetraVertexB0( af::span, af::span, af::span, 2 ) = af::shift( afTetraVertexB0( af::span, af::span, af::span, 2 ), 0, 0, -3, 0 );
		afTetraVertexB0( af::span, af::span, 1, af::span ) = af::shift( afTetraVertexB0( af::span, af::span, 1, af::span ), 0, 0, 0, 1 );
		afTetraVertexB0( af::span, af::span, 2, af::span ) = af::shift( afTetraVertexB0( af::span, af::span, 2, af::span ), 0, 0, 0, 2 );
		af::array afTetraVertexB1 = af::shift( afTetraVertexB0, 0, 0, 0, -1 );
		af::array afTetraVertexB2 = af::shift( afTetraVertexB1, 0, 0, 0, -1 );

		//////

		af::array afFaceVertexN0_V = af::tile( afFaceVertexN0, 1, tetraQuadCount_, 1, 1 );
		af::array afFaceVertexN1_V = af::tile( afFaceVertexN1, 1, tetraQuadCount_, 1, 1 );
		af::array afFaceVertexN2_V = af::tile( afFaceVertexN2, 1, tetraQuadCount_, 1, 1 );

		af::array afFaceVertexN0_S = af::tile( afFaceVertexN0, 1, faceQuadCount_, 1, 1 );
		af::array afFaceVertexN1_S = af::tile( afFaceVertexN1, 1, faceQuadCount_, 1, 1 );
		af::array afFaceVertexN2_S = af::tile( afFaceVertexN2, 1, faceQuadCount_, 1, 1 );

		//////

		af::array afTetraVertexB0_V = af::tile( afTetraVertexB0, 1, tetraQuadCount_, 1, 1 );
		af::array afTetraVertexB1_V = af::tile( afTetraVertexB1, 1, tetraQuadCount_, 1, 1 );
		af::array afTetraVertexB2_V = af::tile( afTetraVertexB2, 1, tetraQuadCount_, 1, 1 );
		af::array afTetraVertexB3_V = af::tile( afTetraVertexB3, 1, tetraQuadCount_, 1, 1 );

		af::array afTetraVertexB0_S = af::tile( afTetraVertexB0, 1, faceQuadCount_, 1, 1 );
		af::array afTetraVertexB1_S = af::tile( afTetraVertexB1, 1, faceQuadCount_, 1, 1 );
		af::array afTetraVertexB2_S = af::tile( afTetraVertexB2, 1, faceQuadCount_, 1, 1 );
		af::array afTetraVertexB3_S = af::tile( afTetraVertexB3, 1, faceQuadCount_, 1, 1 );

		//////
		
		af::array afFaceQuadWeightM = afFaceQuadDataM.row( 0 );
		af::array afFaceQuadVertexM = afFaceQuadDataM.rows( 1, 3 );

		//////
		
		af::array afTetraQuadWeightA = afTetraQuadDataA.row( 0 );
		af::array afTetraQuadVertexA = afTetraQuadDataA.rows( 1, 3 );

		//////
		
		af::array afFaceQuadVertexM_V = af::tile( afFaceQuadVertexM, 1, 1, 4, 3 );
		af::array afFaceQuadVertexM_S = af::tile( afFaceQuadVertexM, 1, 1, 3, 1 );

		//////
		
		af::array afTetraQuadVertexA_V = af::tile( afTetraQuadVertexA, 1, 1, 4, 3 );
		af::array afTetraQuadVertexA_S = af::tile( afTetraQuadVertexA, 1, 1, 3, 1 );

		////// see THEORY files for references
		// VV part

		LUV::LuVector< 3, af::array > vecV1_VV = LUV::LuVector< 3, af::array >( afTetraVertexB0_V( 0, af::span, af::span, af::span ), afTetraVertexB0_V( 1, af::span, af::span, af::span ), afTetraVertexB0_V( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV2_VV = LUV::LuVector< 3, af::array >( afTetraVertexB1_V( 0, af::span, af::span, af::span ), afTetraVertexB1_V( 1, af::span, af::span, af::span ), afTetraVertexB1_V( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV3_VV = LUV::LuVector< 3, af::array >( afTetraVertexB2_V( 0, af::span, af::span, af::span ), afTetraVertexB2_V( 1, af::span, af::span, af::span ), afTetraVertexB2_V( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV4_VV = LUV::LuVector< 3, af::array >( afTetraVertexB3_V( 0, af::span, af::span, af::span ), afTetraVertexB3_V( 1, af::span, af::span, af::span ), afTetraVertexB3_V( 2, af::span, af::span, af::span ) );

		LUV::LuVector< 3, af::array > vecV1_SV = LUV::LuVector< 3, af::array >( afTetraVertexB0_S( 0, af::span, af::span, af::span ), afTetraVertexB0_S( 1, af::span, af::span, af::span ), afTetraVertexB0_S( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV2_SV = LUV::LuVector< 3, af::array >( afTetraVertexB1_S( 0, af::span, af::span, af::span ), afTetraVertexB1_S( 1, af::span, af::span, af::span ), afTetraVertexB1_S( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV3_SV = LUV::LuVector< 3, af::array >( afTetraVertexB2_S( 0, af::span, af::span, af::span ), afTetraVertexB2_S( 1, af::span, af::span, af::span ), afTetraVertexB2_S( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV4_SV = LUV::LuVector< 3, af::array >( afTetraVertexB3_S( 0, af::span, af::span, af::span ), afTetraVertexB3_S( 1, af::span, af::span, af::span ), afTetraVertexB3_S( 2, af::span, af::span, af::span ) );
		
		LUV::LuVector< 3, af::array > vecV1_VS = LUV::LuVector< 3, af::array >( afFaceVertexN0_V( 0, af::span, af::span, af::span ), afFaceVertexN0_V( 1, af::span, af::span, af::span ), afFaceVertexN0_V( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV2_VS = LUV::LuVector< 3, af::array >( afFaceVertexN1_V( 0, af::span, af::span, af::span ), afFaceVertexN1_V( 1, af::span, af::span, af::span ), afFaceVertexN1_V( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV3_VS = LUV::LuVector< 3, af::array >( afFaceVertexN2_V( 0, af::span, af::span, af::span ), afFaceVertexN2_V( 1, af::span, af::span, af::span ), afFaceVertexN2_V( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV4_VS = LUV::LuVector< 3, af::array >( af::constant( swgVertexN[0], vecV1_VS[0].dims() ), af::constant( swgVertexN[2], vecV1_VS[0].dims() ), af::constant( swgVertexN[2], vecV1_VS[0].dims() ) );
		
		LUV::LuVector< 3, af::array > vecV1_SS = LUV::LuVector< 3, af::array >( afFaceVertexN0_S( 0, af::span, af::span, af::span ), afFaceVertexN0_S( 1, af::span, af::span, af::span ), afFaceVertexN0_S( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV2_SS = LUV::LuVector< 3, af::array >( afFaceVertexN1_S( 0, af::span, af::span, af::span ), afFaceVertexN1_S( 1, af::span, af::span, af::span ), afFaceVertexN1_S( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV3_SS = LUV::LuVector< 3, af::array >( afFaceVertexN2_S( 0, af::span, af::span, af::span ), afFaceVertexN2_S( 1, af::span, af::span, af::span ), afFaceVertexN2_S( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecV4_SS = LUV::LuVector< 3, af::array >( af::constant( swgVertexN[0], vecV1_SS[0].dims() ), af::constant( swgVertexN[2], vecV1_SS[0].dims() ), af::constant( swgVertexN[2], vecV1_SS[0].dims() ) );

		//
		LUV::LuVector< 3, af::array > vecObs_VV = LUV::LuVector< 3, af::array >( afTetraQuadVertexA_V( 0, af::span, af::span, af::span ), afTetraQuadVertexA_V( 1, af::span, af::span, af::span ), afTetraQuadVertexA_V( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecObs_VS = LUV::LuVector< 3, af::array >( afTetraQuadVertexA_S( 0, af::span, af::span, af::span ), afTetraQuadVertexA_S( 1, af::span, af::span, af::span ), afTetraQuadVertexA_S( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecObs_SV = LUV::LuVector< 3, af::array >( afFaceQuadVertexM_V( 0, af::span, af::span, af::span ), afFaceQuadVertexM_V( 1, af::span, af::span, af::span ), afFaceQuadVertexM_V( 2, af::span, af::span, af::span ) );
		LUV::LuVector< 3, af::array > vecObs_SS = LUV::LuVector< 3, af::array >( afFaceQuadVertexM_S( 0, af::span, af::span, af::span ), afFaceQuadVertexM_S( 1, af::span, af::span, af::span ), afFaceQuadVertexM_S( 2, af::span, af::span, af::span ) );

		//cout << "E" << endl;
		CalculateG_VV( vecV1_VV, vecV2_VV, vecV3_VV, vecV4_VV, vecObs_VV, GIVV, GDVV );
		//cout << "E1" << endl;
		CalculateGISV( vecV1_SV, vecV2_SV, vecV3_SV, vecV4_SV, vecObs_SV, GISV );
		
		//cout << "E2" << endl;
		CalculateGI_S( vecV1_VS, vecV2_VS, vecV3_VS, vecV4_VS, vecObs_VS, GIVS );
		//cout << "E3" << endl;
		CalculateGI_S( vecV1_SS, vecV2_SS, vecV3_SS, vecV4_SS, vecObs_SS, GISS );

		

		//cout << "F" << endl;

		//LUV::LuVector< 3, af::array > dirN_VV = -LUV::PlaneNormalP( vecV4_VV, vecV1_VV, vecV2_VV, vecV3_VV );
		//LUV::LuVector< 3, af::array > dirU_VV = -LUV::LineNormalP( vecV3_VV, vecV1_VV, vecV2_VV );
		//LUV::LuVector< 3, af::array > dirL_VV = LUV::Cross( dirN_VV, dirU_VV );

		//LUV::LuVector< 3, af::array > vecS_VV = vecV2_VV + vecV1_VV;
		//LUV::LuVector< 3, af::array > vecM_VV = vecV2_VV - vecV1_VV;
		//LUV::LuVector< 3, af::array > dirM_VV = Unit( vecM_VV );
		//LUV::LuVector< 3, af::array > vecB_VV = LUV::Dot( dirM_VV, dirL_VV ) * vecM_VV;
		//LUV::LuVector< 3, af::array > vecRhoM_VV = ( vecS_VV - vecB_VV ) / 2;
		//LUV::LuVector< 3, af::array > vecRhoP_VV = ( vecS_VV + vecB_VV ) / 2;
		//LUV::LuVector< 3, af::array > vecRho_VV = LUV::ProjPlane( vecObs_VV, vecV1_VV, dirN_VV );
		//
		//af::array magD_VV = LUV::Dot( vecObs_VV - vecRho_VV, dirN_VV );

		//LUV::LuVector< 3, af::array > vecL_VV = vecRhoP_VV - vecRhoM_VV;
		//af::array magL_VV = LUV::Length( vecL_VV );
		//
		//LUV::LuVector< 3, af::array > vecPM_VV = vecRhoM_VV - vecRho_VV;
		//LUV::LuVector< 3, af::array > vecPP_VV = vecRhoP_VV - vecRho_VV;

		//af::array magLM_VV = LUV::Dot( vecPM_VV, dirL_VV );
		//af::array magLP_VV = LUV::Dot( vecPP_VV, dirL_VV );
		//af::array magP0_VV = LUV::Length( LUV::Dot( vecPM_VV, dirU_VV ) );
		//af::array magPM_VV = LUV::Length( vecPM_VV );
		//af::array magPP_VV = LUV::Length( vecPM_VV );

		//LUV::LuVector< 3, af::array > dirP0_VV = ( vecPM_VV - magLM_VV * dirL_VV ) / magP0_VV;
		//
		//af::array magR0_VV = LUV::Length( vecObs_VV - LUV::ProjLine( vecObs_VV, vecRhoM_VV, vecRhoP_VV ) );
		//af::array magRM_VV = LUV::Length( vecObs_VV - vecRhoM_VV );
		//af::array magRP_VV = LUV::Length( vecObs_VV - vecRhoP_VV );

		//af::array magR0S_VV = magR0_VV * magR0_VV;
		//af::array magAbsD_VV = af::abs( magD_VV );

		//af::array magA1_VV = af::atan( ( magP0_VV * magLP_VV ) / ( magR0S_VV + magAbsD_VV * magRP_VV ) );
		//af::array magA2_VV = af::atan( ( magP0_VV * magLM_VV ) / ( magR0S_VV + magAbsD_VV * magRM_VV ) );
		//af::array magA3_VV = af::log( ( magRP_VV + magLP_VV ) / ( magRM_VV + magLM_VV ) );

		//afGIVV = af::sum( af::sum( magD_VV * ( LUV::Dot( dirP0_VV, dirU_VV ) * ( magAbsD_VV * ( magA1_VV - magA2_VV - magP0_VV * magA3_VV ) ) ) , 2 ), 3 ) / 2;














		/*
		v1 v2 v3 - v4
			v1 v2 - v3 - v4
			v2 v3 - v1 - v4
			v3 v1 - v2 - v4

			v1v2v3v4
			
			2 3 4 1
			3 4 2 1
			4 2 3 1

			1 3 4 2
			3 4 1 2
			4 1 3 2

			1 2 4 3
			2 4 1 3
			4 1 2 3

			1 2 3 4
			2 3 1 4
			3 1 2 4

			v1			v2			v3			v4
			2 1 1 1		3 3 2 2		4 4 4 3		1 2 3 4
			3 3 2 2		4 4 4 3		2 1	1 1		1 2 3 4
			4 4 4 3		2 1 1 1		3 3	2 2		1 2 3 4



			1 2 3 4 [3,1,4,1] < --- 1 2 3 4 [3,4,1,1]

			tile

			1 2 3 4 [3,1,4,3]	= v4
			1 2 3 4
			1 2 3 4


			2 3 4 1		shift (:,:,:,0) - 1 in dim3
			3 4 1 2		shift (:,:,:,1) - 2 in dim3
			4 1 2 3		shift (:,:,:,2) - 3 in dim3

			2 1 1 1		shift (:,:,1,:) + 1 in dim4		= v1
			3 3 2 2		shift (:,:,2,:) + 2 in dim4
			4 4 4 3		

			shift (:,:,:,:) -1 in dim4		= v2

			shift (:,:,:,:) -1 in dim4		= v3



			

		v1 v2 v3 v4 [3,4] --- tile --->

		x1 x2 x3 x4 [1,4,1,1] --- tile --->

		x1 [4,m,1,1] --- tile * 4 --->	x1 x1 x1 x1 [4,m,4,1] --- shift in dim3 --->	x1 x4 x3 x2 [4,m,4,1]
		x2								x2 x2 x2 x2										x2 x1 x4 x3
		x3								x3 x3 x3 x3										x3 x2 x1 x4
		x4								x4 x4 x4 x4										x4 x3 x2 x1

		*/

	}

	inline complex< T > TetrahedralIntegral( const UINT_T& idFaceM, const UINT_T& idFaceN, const UINT_T& idTetraA, const UINT_T& idTetraB )
	{
		UINT_T idFaceVertexM[ 3 ] = {
			faceVertexIndexPtr_[ 3 * idFaceM ],
			faceVertexIndexPtr_[ 3 * idFaceM + 1 ],
			faceVertexIndexPtr_[ 3 * idFaceM + 2 ]
		};

		UINT_T idFaceVertexN[ 3 ] = {
			faceVertexIndexPtr_[ 3 * idFaceN ],
			faceVertexIndexPtr_[ 3 * idFaceN + 1 ],
			faceVertexIndexPtr_[ 3 * idFaceN + 2 ]
		};

		UINT_T idTetraVertexA[ 4 ] = {
			tetraVertexIndexPtr_[ 4 * idTetraA ],
			tetraVertexIndexPtr_[ 4 * idTetraA + 1 ],
			tetraVertexIndexPtr_[ 4 * idTetraA + 2 ],
			tetraVertexIndexPtr_[ 4 * idTetraA + 3 ]
		};

		UINT_T idTetraVertexB[ 4 ] = {
			tetraVertexIndexPtr_[ 4 * idTetraB ],
			tetraVertexIndexPtr_[ 4 * idTetraB + 1 ],
			tetraVertexIndexPtr_[ 4 * idTetraB + 2 ],
			tetraVertexIndexPtr_[ 4 * idTetraB + 3 ]
		};

		LUV::LuVector3< T >* faceVertexM[ 3 ] = {
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexM[ 0 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexM[ 1 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexM[ 2 ] ] )
		};

		LUV::LuVector3< T >* faceVertexN[ 3 ] = {
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexN[ 0 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexN[ 1 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexN[ 2 ] ] )
		};

		LUV::LuVector3< T >* tetraVertexA[ 4 ] = {
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexA[ 0 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexA[ 1 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexA[ 2 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexA[ 3 ] ] )
		};

		//LUV::LuVector3< T > faceVertexM[ 3 ] = {
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexM[ 0 ] ], vertexDataPtr_[ 3 * idFaceVertexM[ 0 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexM[ 0 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexM[ 1 ] ], vertexDataPtr_[ 3 * idFaceVertexM[ 1 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexM[ 1 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexM[ 2 ] ], vertexDataPtr_[ 3 * idFaceVertexM[ 2 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexM[ 2 ] + 2 ] )
		//};

		//LUV::LuVector3< T > faceVertexN[ 3 ] = {
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexN[ 0 ] ], vertexDataPtr_[ 3 * idFaceVertexN[ 0 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexN[ 0 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexN[ 1 ] ], vertexDataPtr_[ 3 * idFaceVertexN[ 1 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexN[ 1 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexN[ 2 ] ], vertexDataPtr_[ 3 * idFaceVertexN[ 2 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexN[ 2 ] + 2 ] )
		//};

		//LUV::LuVector3< T > tetraVertexA[ 4 ] = {
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexA[ 0 ] ], vertexDataPtr_[ 3 * idTetraVertexA[ 0 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexA[ 0 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexA[ 1 ] ], vertexDataPtr_[ 3 * idTetraVertexA[ 1 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexA[ 1 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexA[ 2 ] ], vertexDataPtr_[ 3 * idTetraVertexA[ 2 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexA[ 2 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexA[ 3 ] ], vertexDataPtr_[ 3 * idTetraVertexA[ 3 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexA[ 3 ] + 2 ] )
		//};

		//LUV::LuVector3< T > tetraVertexB[ 4 ] = {
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexB[ 0 ] ], vertexDataPtr_[ 3 * idTetraVertexB[ 0 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexB[ 0 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexB[ 1 ] ], vertexDataPtr_[ 3 * idTetraVertexB[ 1 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexB[ 1 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexB[ 2 ] ], vertexDataPtr_[ 3 * idTetraVertexB[ 2 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexB[ 2 ] + 2 ] ),
		//	LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexB[ 3 ] ], vertexDataPtr_[ 3 * idTetraVertexB[ 3 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexB[ 3 ] + 2 ] )
		//};
		
		T faceAreaM = LUV::TriangleArea( faceVertexM[ 0 ], faceVertexM[ 1 ], faceVertexM[ 2 ] );
		T faceAreaN = LUV::TriangleArea( faceVertexN[ 0 ], faceVertexN[ 1 ], faceVertexN[ 2 ] );
		
		T tetraVolumeA = LUV::TetrahedronVolume( tetraVertexA[ 0 ], tetraVertexA[ 1 ], tetraVertexA[ 2 ], tetraVertexA[ 3 ] );
		T tetraVolumeB = LUV::TetrahedronVolume( tetraVertexB[ 0 ], tetraVertexB[ 1 ], tetraVertexB[ 2 ], tetraVertexB[ 3 ] );

		UINT_T idSwgVertexM = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFaceM ], &tetraVertexIndexPtr_[ 4 * idTetraA ] );
		UINT_T idSwgVertexN = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFaceN ], &tetraVertexIndexPtr_[ 4 * idTetraB ] );

		LUV::LuVector3< T > swgVertexM = LUV::LuVector3< T >( vertexDataPtr_[ 3 * idSwgVertexM ], vertexDataPtr_[ 3 * idSwgVertexM + 1 ], vertexDataPtr_[ 3 * idSwgVertexM + 2 ] );
		LUV::LuVector3< T > swgVertexN = LUV::LuVector3< T >( vertexDataPtr_[ 3 * idSwgVertexN ], vertexDataPtr_[ 3 * idSwgVertexN + 1 ], vertexDataPtr_[ 3 * idSwgVertexN + 2 ] );
		
		//////

		//

		complex< T > I1 = 0;

		for( UINT_T idqA = 0; idqA < tetraQuadCount_; ++idqA )
		{
			UINT_T addressQA = tetraQuadBlockSize_ * idTetraA + idqA;
			T* magWA =  &tetraQuadDataPtr_[ addressQA ];
			LUV::LuVector3< T >* vecQA = reinterpret_cast< LUV::LuVector3< T >* >( &tetraQuadDataPtr_[ addressQA + 1 ] );

			complex< T > I1_B = 0;
			for( UINT_T idqB = 0; idqB < tetraQuadCount_; ++idqB )
			{
				UINT_T addressQB = tetraQuadBlockSize_ * idTetraB + idqB;
				T* magWB =  &tetraQuadDataPtr_[ addressQB ];
				LUV::LuVector3< T >* vecQB = reinterpret_cast< LUV::LuVector3< T >* >( &tetraQuadDataPtr_[ addressQB + 1 ] );
				
				T magR = LUV::Length( *vecQA - *vecQB );

				I1_B += *magWB * GreenRegular( R );
			}

			I1 += *magWA * ( I1_B + GreenVolumeIntegralScalar(  );
		}

		//


		
		af::array afFaceQuadDataM = af::array( 4, faceQuadCount_, AF_FLOAT_T );
		af::array afFaceQuadDataN = af::array( 4, faceQuadCount_, AF_FLOAT_T );
		afFaceQuadDataM.write( &faceQuadDataPtr_[ faceQuadBlockSize_ * idFaceM ], SIZEOF_T * faceQuadBlockSize_, afHost );
		afFaceQuadDataN.write( &faceQuadDataPtr_[ faceQuadBlockSize_ * idFaceN ], SIZEOF_T * faceQuadBlockSize_, afHost );

		af::array afTetraQuadDataA = af::array( 4, tetraQuadCount_, AF_FLOAT_T );
		af::array afTetraQuadDataB = af::array( 4, tetraQuadCount_, AF_FLOAT_T );
		afTetraQuadDataA.write( &tetraQuadDataPtr_[ tetraQuadBlockSize_ * idTetraA ], SIZEOF_T * tetraQuadBlockSize_, afHost );
		afTetraQuadDataB.write( &tetraQuadDataPtr_[ tetraQuadBlockSize_ * idTetraB ], SIZEOF_T * tetraQuadBlockSize_, afHost );

		af::array afFaceVertexM = af::array( 3, 3, AF_FLOAT_T );
		af::array afFaceVertexN = af::array( 3, 3, AF_FLOAT_T );
		afFaceVertexM.write( reinterpret_cast< T* >( faceVertexM ), SIZEOF_T * 3 * 3, afHost );
		afFaceVertexN.write( reinterpret_cast< T* >( faceVertexN ), SIZEOF_T * 3 * 3, afHost );
		
		af::array afTetraVertexA = af::array( 3, 4, AF_FLOAT_T );
		af::array afTetraVertexB = af::array( 3, 4, AF_FLOAT_T );
		afTetraVertexA.write( reinterpret_cast< T* >( tetraVertexA ), SIZEOF_T * 3 * 4, afHost );
		afTetraVertexB.write( reinterpret_cast< T* >( tetraVertexB ), SIZEOF_T * 3 * 4, afHost );

		////// see the documents in the THEORY folder
		
		af::array GIVV = af::array( 1, tetraQuadCount_, AF_FLOAT_T );
		af::array GIVS = af::array( 1, tetraQuadCount_, AF_FLOAT_T );
		af::array GISV = af::array( 1, faceQuadCount_, AF_FLOAT_T );
		af::array GISS = af::array( 1, faceQuadCount_, AF_FLOAT_T );

		LUV::LuVector3< af::array > GDVV( GIVV, GIVV, GIVV );

		//cout << "D" << endl;
		CalculateG( afFaceQuadDataM, afTetraQuadDataA, afFaceVertexN, afTetraVertexB, swgVertexN, GIVV, GIVS, GISV, GISS, GDVV ); // [ 1, m, 1, 1 ]

		//CalculateGDVV(  ); // L3A[ 1, m, 1, 1 ]

		//////

		// R

		//cout << "D1" << endl;

		af::array vecRxVVm = af::tile( afTetraQuadDataA.row( 1 ), tetraQuadCount_, 1, 1, 1 );
		af::array vecRyVVm = af::tile( afTetraQuadDataA.row( 2 ), tetraQuadCount_, 1, 1, 1 );
		af::array vecRzVVm = af::tile( afTetraQuadDataA.row( 3 ), tetraQuadCount_, 1, 1, 1 );

		af::array vecRxVVn = af::tile( af::transpose( afTetraQuadDataB.row( 1 ) ), 1, tetraQuadCount_, 1, 1 );
		af::array vecRyVVn = af::tile( af::transpose( afTetraQuadDataB.row( 2 ) ), 1, tetraQuadCount_, 1, 1 );
		af::array vecRzVVn = af::tile( af::transpose( afTetraQuadDataB.row( 3 ) ), 1, tetraQuadCount_, 1, 1 );


		af::array vecRxSVm = af::tile( afFaceQuadDataM.row( 1 ), tetraQuadCount_, 1, 1, 1 );
		af::array vecRySVm = af::tile( afFaceQuadDataM.row( 2 ), tetraQuadCount_, 1, 1, 1 );
		af::array vecRzSVm = af::tile( afFaceQuadDataM.row( 3 ), tetraQuadCount_, 1, 1, 1 );

		af::array vecRxSVn = af::tile( af::transpose( afTetraQuadDataB.row( 1 ) ), 1, faceQuadCount_, 1, 1 );
		af::array vecRySVn = af::tile( af::transpose( afTetraQuadDataB.row( 2 ) ), 1, faceQuadCount_, 1, 1 );
		af::array vecRzSVn = af::tile( af::transpose( afTetraQuadDataB.row( 3 ) ), 1, faceQuadCount_, 1, 1 );


		af::array vecRxVSm = af::tile( afTetraQuadDataA.row( 1 ), faceQuadCount_, 1, 1, 1 );
		af::array vecRyVSm = af::tile( afTetraQuadDataA.row( 2 ), faceQuadCount_, 1, 1, 1 );
		af::array vecRzVSm = af::tile( afTetraQuadDataA.row( 3 ), faceQuadCount_, 1, 1, 1 );

		af::array vecRxVSn = af::tile( af::transpose( afFaceQuadDataN.row( 1 ) ), 1, tetraQuadCount_, 1, 1 );
		af::array vecRyVSn = af::tile( af::transpose( afFaceQuadDataN.row( 2 ) ), 1, tetraQuadCount_, 1, 1 );
		af::array vecRzVSn = af::tile( af::transpose( afFaceQuadDataN.row( 3 ) ), 1, tetraQuadCount_, 1, 1 );


		af::array vecRxSSm = af::tile( afFaceQuadDataM.row( 1 ), faceQuadCount_, 1, 1, 1 );
		af::array vecRySSm = af::tile( afFaceQuadDataM.row( 2 ), faceQuadCount_, 1, 1, 1 );
		af::array vecRzSSm = af::tile( afFaceQuadDataM.row( 3 ), faceQuadCount_, 1, 1, 1 );

		af::array vecRxSSn = af::tile( af::transpose( afFaceQuadDataN.row( 1 ) ), 1, faceQuadCount_, 1, 1 );
		af::array vecRySSn = af::tile( af::transpose( afFaceQuadDataN.row( 2 ) ), 1, faceQuadCount_, 1, 1 );
		af::array vecRzSSn = af::tile( af::transpose( afFaceQuadDataN.row( 3 ) ), 1, faceQuadCount_, 1, 1 );

		//cout << "D2" << endl;


		af::array RVV = af::sqrt( af::pow( vecRxVVm - vecRxVVn, 2.0 ) + af::pow( vecRyVVm - vecRyVVn, 2.0 ) + af::pow( vecRzVVm - vecRzVVn, 2.0 ) );
		af::array RSV = af::sqrt( af::pow( vecRxSVm - vecRxSVn, 2.0 ) + af::pow( vecRySVm - vecRySVn, 2.0 ) + af::pow( vecRzSVm - vecRzSVn, 2.0 ) );
		af::array RVS = af::sqrt( af::pow( vecRxVSm - vecRxVSn, 2.0 ) + af::pow( vecRyVSm - vecRyVSn, 2.0 ) + af::pow( vecRzVSm - vecRzVSn, 2.0 ) );
		af::array RSS = af::sqrt( af::pow( vecRxSSm - vecRxSSn, 2.0 ) + af::pow( vecRySSm - vecRySSn, 2.0 ) + af::pow( vecRzSSm - vecRzSSn, 2.0 ) );


		af::array isZeroRVV = af::iszero( RVV );
		af::array isZeroRSV = af::iszero( RSV );
		af::array isZeroRVS = af::iszero( RVS );
		af::array isZeroRSS = af::iszero( RSS );

		
		//cout << "D3" << endl;


		//af::array GNVV = ( ( ( af::exp( waveNumberJ_ * RVV ) - 1 ) / RVV )( isZeroRVV ) = waveNumberJ_ );
		//af::array GNSV = ( ( ( af::exp( waveNumberJ_ * RSV ) - 1 ) / RVV )( isZeroRSV ) = waveNumberJ_ );
		//af::array GNVS = ( ( ( af::exp( waveNumberJ_ * RVS ) - 1 ) / RVV )( isZeroRVS ) = waveNumberJ_ );
		//af::array GNSS = ( ( ( af::exp( waveNumberJ_ * RSS ) - 1 ) / RSS )( isZeroRSS ) = waveNumberJ_ );

		af::array GNVV = ( af::exp( waveNumberJ_ * RVV ) - 1 ) / RVV;
		af::array GNSV = ( af::exp( waveNumberJ_ * RSV ) - 1 ) / RSV;
		af::array GNVS = ( af::exp( waveNumberJ_ * RVS ) - 1 ) / RVS;
		af::array GNSS = ( af::exp( waveNumberJ_ * RSS ) - 1 ) / RSS;

		GNVV( isZeroRVV ) = waveNumberJ_;
		GNSV( isZeroRSV ) = waveNumberJ_;
		GNVS( isZeroRVS ) = waveNumberJ_;
		GNSS( isZeroRSS ) = waveNumberJ_;


		//cout << "D4" << endl;

		//////

		

		af::array WnVV = af::tile( af::transpose( afTetraQuadDataB.row( 0 ) ), 1, tetraQuadCount_, 1, 1 );
		af::array WnSV = af::tile( af::transpose( afTetraQuadDataB.row( 0 ) ), 1, faceQuadCount_, 1, 1 );
		af::array WnVS = af::tile( af::transpose( afFaceQuadDataN.row( 0 ) ), 1, tetraQuadCount_, 1, 1 );
		af::array WnSS = af::tile( af::transpose( afFaceQuadDataN.row( 0 ) ), 1, faceQuadCount_, 1, 1 );

		af::array WmV = afTetraQuadDataA.row( 0 );
		af::array WmS = afFaceQuadDataN.row( 0 );

		af::array QxmV = afTetraQuadDataA.row( 1 );
		af::array QymV = afTetraQuadDataA.row( 2 );
		af::array QzmV = afTetraQuadDataA.row( 3 );

		af::array QxnVV = af::tile( af::transpose( afTetraQuadDataB.row( 1 ) ), 1, tetraQuadCount_, 1, 1 );
		af::array QynVV = af::tile( af::transpose( afTetraQuadDataB.row( 2 ) ), 1, tetraQuadCount_, 1, 1 );
		af::array QznVV = af::tile( af::transpose( afTetraQuadDataB.row( 3 ) ), 1, tetraQuadCount_, 1, 1 );

		//////

		//cout << "D5" << endl;

		complex< T > I[19];

		sum( WmV * ( af::sum( WnVV * GNVV, 0 ) + GIVV ), 1 ).host( &I[0] );

		sum( WmV * QxmV * ( af::sum( WnVV * GNVV, 0 ) + GIVV ), 1 ).host( &I[1] );
		sum( WmV * QymV * ( af::sum( WnVV * GNVV, 0 ) + GIVV ), 1 ).host( &I[2] );
		sum( WmV * QzmV * ( af::sum( WnVV * GNVV, 0 ) + GIVV ), 1 ).host( &I[3] );

		sum( WmV * ( af::sum( WnVV * QxnVV * GNVV, 0 ) + GDVV[0] + QxmV * GIVV ), 1 ).host( &I[4] );
		sum( WmV * ( af::sum( WnVV * QynVV * GNVV, 0 ) + GDVV[1] + QymV * GIVV ), 1 ).host( &I[5] );
		sum( WmV * ( af::sum( WnVV * QznVV * GNVV, 0 ) + GDVV[2] + QzmV * GIVV ), 1 ).host( &I[6] );

		sum( WmV * QxmV * ( af::sum( WnVV * QxnVV * GNVV, 0 ) + GDVV[0] + QxmV * GIVV ), 1 ).host( &I[7] );
		sum( WmV * QymV * ( af::sum( WnVV * QynVV * GNVV, 0 ) + GDVV[1] + QymV * GIVV ), 1 ).host( &I[8] );
		sum( WmV * QzmV * ( af::sum( WnVV * QznVV * GNVV, 0 ) + GDVV[2] + QzmV * GIVV ), 1 ).host( &I[9] );

		sum( WmS * ( af::sum( WnSV * GNSV, 0 ) + GISV ), 1 ).host( &I[10] );
		sum( WmV * ( af::sum( WnVS * GNVS, 0 ) + GIVS ), 1 ).host( &I[11] );
		sum( WmS * ( af::sum( WnSS * GNSS, 0 ) + GISS ), 1 ).host( &I[12] );

		sum( WmV * QxmV, 1 ).host( &I[13] );
		sum( WmV * QymV, 1 ).host( &I[14] );
		sum( WmV * QzmV, 1 ).host( &I[15] );

		sum( WmV * QxmV * QxmV, 1 ).host( &I[16] );
		sum( WmV * QymV * QymV, 1 ).host( &I[17] );
		sum( WmV * QzmV * QzmV, 1 ).host( &I[18] );

		//cout << "D6" << endl;

		//////
		
		T factorC = faceAreaM * faceAreaN / ( 9 * tetraVolumeA * tetraVolumeB );
		T epsRA = emPropDataPtr_[ 2 * idTetraA ];
		T epsRB = emPropDataPtr_[ 2 * idTetraB ];
		T contrastA = ( 1 - epsRA ) / epsRA;
		T contrastB = ( 1 - epsRB ) / epsRB;
		T swgDot = LUV::Dot( swgVertexM, swgVertexN );
		LUV::LuVector3< T > swgSum = swgVertexM + swgVertexN;

		//cout << "D7" << endl;

		complex< T > j( 0, 1 );

		complex< T > factor1 = ( -j ) * ( factorC / ( waveNumber_ * c0_ * eps0_ * epsRB ) );
		complex< T > factor2 = j * ( waveNumber_ * c0_ * mu0_ * contrastB * factorC );
		complex< T > factor3 = ( -j ) * ( 9 * factorC * contrastB / ( waveNumber_ * c0_ * eps0_ ) );

		T volArRatioM = tetraVolumeA / faceAreaM;
		T volArRatioN = tetraVolumeB / faceAreaN;
		T volArRatio = volArRatioM * volArRatioN;

		complex< T > result =
			factor1 * ( swgDot * tetraVolumeA + I[16] + I[17] + I[18]
				- swgSum[0] * I[13] - swgSum[1] * I[14] - swgSum[2] * I[15] )
			- factor2 * ( swgDot * I[0] + I[7] + I[8] + I[9]
				- swgVertexN[0] * I[1] - swgVertexN[1] * I[2] - swgVertexN[2] * I[3]
				- swgVertexM[0] * I[4] - swgVertexM[1] * I[5] - swgVertexM[2] * I[6] )
			- factor3 * ( I[0] - volArRatioM * I[10] - volArRatioN * I[11] - volArRatio * I[12] )
		;

		//cout << "D8" << endl;

		return result;

	}

	//inline complex< T > TetrahedralIntegral( const UINT_T& idFaceM, const UINT_T& idFaceN, const UINT_T& idTetraA, const UINT_T& idTetraB )
	//{
	//	UINT_T idFaceVertexM[ 3 ] = {
	//		faceVertexIndexPtr_[ 3 * idFaceM ],
	//		faceVertexIndexPtr_[ 3 * idFaceM + 1 ],
	//		faceVertexIndexPtr_[ 3 * idFaceM + 2 ]
	//	};

	//	UINT_T idFaceVertexN[ 3 ] = {
	//		faceVertexIndexPtr_[ 3 * idFaceN ],
	//		faceVertexIndexPtr_[ 3 * idFaceN + 1 ],
	//		faceVertexIndexPtr_[ 3 * idFaceN + 2 ]
	//	};

	//	UINT_T idTetraVertexA[ 4 ] = {
	//		tetraVertexIndexPtr_[ 4 * idTetraA ],
	//		tetraVertexIndexPtr_[ 4 * idTetraA + 1 ],
	//		tetraVertexIndexPtr_[ 4 * idTetraA + 2 ],
	//		tetraVertexIndexPtr_[ 4 * idTetraA + 3 ]
	//	};

	//	UINT_T idTetraVertexB[ 4 ] = {
	//		tetraVertexIndexPtr_[ 4 * idTetraB ],
	//		tetraVertexIndexPtr_[ 4 * idTetraB + 1 ],
	//		tetraVertexIndexPtr_[ 4 * idTetraB + 2 ],
	//		tetraVertexIndexPtr_[ 4 * idTetraB + 3 ]
	//	};

	//	LUV::LuVector3< T >* faceVertexM[ 3 ] = {
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexM[ 0 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexM[ 1 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexM[ 2 ] ] )
	//	};

	//	LUV::LuVector3< T >* faceVertexN[ 3 ] = {
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexN[ 0 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexN[ 1 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertexN[ 2 ] ] )
	//	};

	//	LUV::LuVector3< T >* tetraVertexA[ 4 ] = {
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexA[ 0 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexA[ 1 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexA[ 2 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexA[ 3 ] ] )
	//	};

	//	LUV::LuVector3< T >* tetraVertexB[ 4 ] = {
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexB[ 0 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexB[ 1 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexB[ 2 ] ] ),
	//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexB[ 3 ] ] )
	//	};
	//	
	//	T faceAreaM = LUV::TriangleArea( *faceVertexM[ 0 ], *faceVertexM[ 1 ], *faceVertexM[ 2 ] );
	//	T faceAreaN = LUV::TriangleArea( *faceVertexN[ 0 ], *faceVertexN[ 1 ], *faceVertexN[ 2 ] );
	//	
	//	T tetraVolumeA = LUV::TetrahedronVolume( *tetraVertexA[ 0 ], *tetraVertexA[ 1 ], *tetraVertexA[ 2 ], *tetraVertexA[ 3 ] );
	//	T tetraVolumeB = LUV::TetrahedronVolume( *tetraVertexB[ 0 ], *tetraVertexB[ 1 ], *tetraVertexB[ 2 ], *tetraVertexB[ 3 ] );

	//	UINT_T idSwgVertexM = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFaceM ], &tetraVertexIndexPtr_[ 4 * idTetraA ] );
	//	UINT_T idSwgVertexN = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFaceN ], &tetraVertexIndexPtr_[ 4 * idTetraB ] );

	//	LUV::LuVector3< T >* swgVertexM = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idSwgVertexM ] );
	//	LUV::LuVector3< T >* swgVertexN = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idSwgVertexN ] );

	//	T factorC = faceAreaM * faceAreaN / ( 9 * tetraVolumeA * tetraVolumeB );
	//	
	//	//////
	//	
	//	af::array afFaceQuadDataM = af::array( 4, faceQuadCount_, AF_FLOAT_T );
	//	af::array afFaceQuadDataN = af::array( 4, faceQuadCount_, AF_FLOAT_T );
	//	afFaceQuadDataM.write( &faceQuadDataPtr_[ faceQuadBlockSize_ * idFaceM ], SIZEOF_T * 4 * faceQuadCount_, afHost );
	//	afFaceQuadDataN.write( &faceQuadDataPtr_[ faceQuadBlockSize_ * idFaceN ], SIZEOF_T * 4 * faceQuadCount_, afHost );

	//	af::array afTetraQuadDataA = af::array( 4, tetraQuadCount_, AF_FLOAT_T );
	//	af::array afTetraQuadDataB = af::array( 4, tetraQuadCount_, AF_FLOAT_T );
	//	afTetraQuadDataA.write( &tetraQuadDataPtr_[ tetraQuadBlockSize_ * idTetraA ], SIZEOF_T * 4 * tetraQuadCount_, afHost );
	//	afTetraQuadDataB.write( &tetraQuadDataPtr_[ tetraQuadBlockSize_ * idTetraB ], SIZEOF_T * 4 * tetraQuadCount_, afHost );

	//	////// see the documents in the THEORY folder
	//	
	//	af::array afGIVV = af::array( 1, tetraQuadCount_, AF_FLOAT_T );
	//	af::array afGIVS = af::array( 1, tetraQuadCount_, AF_FLOAT_T );
	//	af::array afGISV = af::array( 1, faceQuadCount_, AF_FLOAT_T );
	//	af::array afGISS = af::array( 1, faceQuadCount_, AF_FLOAT_T );

	//	CalculateAnalyticalPart( afFaceQuadDataM, afTetraQuadDataA,  afGIVV, afGIVV, afGIVV, afGIVV )



	//	af::array afTetraQuadData = af::array( 4, quadCount_, AF_FLOAT_T );
	//	afTetraQuadData.write( &tetraQuadDataPtr_[ quadBlockSize_ * idTetra ], SIZEOF_T * 4 * quadCount_, afHost );







	//}

	inline void GenerateSingleElem( const UINT_T& idFaceM, const UINT_T& idFaceN )
	{
		//UINT_T idFaceNVertex[ 3 ] = {
		//	faceVertexIndexPtr_[ 3 * idFaceN ],
		//	faceVertexIndexPtr_[ 3 * idFaceN + 1 ],
		//	faceVertexIndexPtr_[ 3 * idFaceN + 2 ]
		//};

		//UINT_T idFaceMVertex[ 3 ] = {
		//	faceVertexIndexPtr_[ 3 * idFaceM ],
		//	faceVertexIndexPtr_[ 3 * idFaceM + 1 ],
		//	faceVertexIndexPtr_[ 3 * idFaceM + 2 ]
		//};
			
		UINT_T idTetraM[ 2 ] = {
			faceTetraIndexPtr_[ 2 * idFaceM ],
			faceTetraIndexPtr_[ 2 * idFaceM + 1 ]
		};
			
		UINT_T idTetraN[ 2 ] = {
			faceTetraIndexPtr_[ 2 * idFaceN ],
			faceTetraIndexPtr_[ 2 * idFaceN + 1 ]
		};
		
		complex< T > result;

		//cout << "C" << endl;
		result = TetrahedralIntegral( idFaceM, idFaceN, idTetraM[ 0 ], idTetraN[ 0 ] );

		if( idTetraN[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idFaceM, idFaceN, idTetraM[ 0 ], idTetraN[ 1 ] );
		}

		if( idTetraM[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idFaceM, idFaceN, idTetraM[ 1 ], idTetraN[ 0 ] );

			if( idTetraN[ 1 ] != -1 )
			{
				result += TetrahedralIntegral( idFaceM, idFaceN, idTetraM[ 1 ], idTetraN[ 1 ] );
			}
		}

		//cout << "C1" << endl;



		//LUV::LuVector3< T >* faceNVertex[ 3 ] = {
		//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceNVertex[ 0 ] ] ),
		//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceNVertex[ 1 ] ] ),
		//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceNVertex[ 2 ] ] )
		//};

		//LUV::LuVector3< T >* faceMVertex[ 3 ] = {
		//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceMVertex[ 0 ] ] ),
		//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceMVertex[ 1 ] ] ),
		//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceMVertex[ 2 ] ] )
		//};

		//T faceNArea = LUV::TriangleArea( *faceNVertex[ 0 ], *faceNVertex[ 1 ], *faceNVertex[ 2 ] );

		//T faceMArea = LUV::TriangleArea( *faceNVertex[ 0 ], *faceNVertex[ 1 ], *faceNVertex[ 2 ] );

		//complex< T > result;
		//	
		//UINT_T idTetra[ 2 ] = {
		//	faceTetraIndexPtr_[ 2 * idFace ],
		//	faceTetraIndexPtr_[ 2 * idFace + 1 ]
		//};
		//	
		//result = TetrahedralIntegral( idFace, idTetra[ 0 ] );

		//if( idTetra[ 1 ] != -1 )
		//{
		//	result -= TetrahedralIntegral( idFace, idTetra[ 1 ] );
		//}

		//result *= faceArea / 3.0;
		
		momDataPtr_[ idFaceN * faceCount_ + idFaceM ] = result;
	}

	void PartialGenerate( const UINT_T& idFaceNStart, const UINT_T& idFaceNEnd )
	{
		//cout << "Thread: " << idFaceNStart << " - " << idFaceNEnd << endl;

		UINT_T idFaceNLimit = faceCount_ < idFaceNEnd ? faceCount_ : idFaceNEnd;

		for( UINT_T idFaceN = idFaceNStart; idFaceN < idFaceNLimit; ++idFaceN )
		{
			//
			cout << idFaceN % (UINT_T)( ceil( (double)( faceCount_ ) / (double)( threadCount_ ) ) ) << " / " << (UINT_T)( ceil( (double)( faceCount_ ) / (double)( threadCount_ ) ) ) << endl;
			//
			for( UINT_T idFaceM = 0; idFaceM < faceCount_; ++idFaceM )
			{
				GenerateSingleElem( idFaceM, idFaceN );
			}
		}
	}

	void Generate()
	{
		for( UINT_T idFaceN = 0; idFaceN < faceCount_; ++idFaceN )
		{
			for( UINT_T idFaceM = 0; idFaceM < faceCount_; ++idFaceM )
			{
				//cout << "B" << endl;
				GenerateSingleElem( idFaceM, idFaceN );
			}
		}
	}

	void ThreadedGenerate()
	{
		UINT_T idFaceNDelta = (UINT_T)( ceil( (double)( faceCount_ ) / (double)( threadCount_ ) ) );

		for( UINT_T idThread = 0; idThread < threadCount_; ++idThread )
		{
			UINT_T idFaceNStart = idThread * idFaceNDelta;
			UINT_T idFaceNEnd = idFaceNStart + idFaceNDelta;
			threadList_.push_back( new thread( [=]{ PartialGenerate( idFaceNStart, idFaceNEnd ); } ));
		}
		
		for( UINT_T idThread = 0; idThread < threadCount_; ++idThread )
		{
			threadList_[ idThread ]->join();
		}
		
		for( UINT_T idThread = 0; idThread < threadCount_; ++idThread )
		{
			delete threadList_[ idThread ];
		}
		threadList_.clear();
		threadList_.reserve( threadCount_ );
	}

};

#endif