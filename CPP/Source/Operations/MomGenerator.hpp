#ifndef MOM_GENERATOR_INCLUDED
#define MOM_GENERATOR_INCLUDED

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
class MomGenerator
{
public:
	UINT_T threadCount_;
	vector< thread* > threadList_;

	T eps0_;
	T mu0_;

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
	MomGenerator(
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
		eps0_( 8.85418782E-12 ),
		mu0_( 1.25663706E-6 ),
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

	~MomGenerator()
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

	inline complex< T > TetrahedralIntegral( const UINT_T& idFace, const UINT_T& idTetra ) const
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
	}

	inline void CalculateGI(
		af::array& afFaceQuadDataM,
		af::array& afTetraQuadDataA,
		af::array& afFaceVertexN,
		af::array& afTetraVertexB,
		af::array& afGIVV,
		af::array& afGIVS,
		af::array& afGISV,
		af::array& afGISS
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

		LUV::LuVector< 3, af::array > vecObs_VV = LUV::LuVector< 3, af::array >( afTetraQuadVertexA_V( 0, af::span, af::span, af::span ), afTetraQuadVertexA_V( 1, af::span, af::span, af::span ), afTetraQuadVertexA_V( 2, af::span, af::span, af::span ) );

		LUV::LuVector< 3, af::array > dirN_VV = -LUV::PlaneNormalP( vecV4_VV, vecV1_VV, vecV2_VV, vecV3_VV );
		LUV::LuVector< 3, af::array > dirU_VV = -LUV::LineNormalP( vecV3_VV, vecV1_VV, vecV2_VV );
		LUV::LuVector< 3, af::array > dirL_VV = LUV::Cross( dirN_VV, dirU_VV );

		LUV::LuVector< 3, af::array > vecS_VV = vecV2_VV + vecV1_VV;
		LUV::LuVector< 3, af::array > vecM_VV = vecV2_VV - vecV1_VV;
		LUV::LuVector< 3, af::array > dirM_VV = Unit( vecM_VV );
		LUV::LuVector< 3, af::array > vecB_VV = LUV::Dot( dirM_VV, dirL_VV ) * vecM_VV;
		LUV::LuVector< 3, af::array > vecRhoM_VV = ( vecS_VV - vecB_VV ) / 2;
		LUV::LuVector< 3, af::array > vecRhoP_VV = ( vecS_VV + vecB_VV ) / 2;
		LUV::LuVector< 3, af::array > vecRho_VV = LUV::ProjPlane( vecObs_VV, vecV1_VV, dirN_VV );
		
		af::array magD_VV = LUV::Dot( vecObs_VV - vecRho_VV, dirN_VV );

		LUV::LuVector< 3, af::array > vecL_VV = vecRhoP_VV - vecRhoM_VV;
		af::array magL_VV = LUV::Length( vecL_VV );
		
		LUV::LuVector< 3, af::array > vecPM_VV = vecRhoM_VV - vecRho_VV;
		LUV::LuVector< 3, af::array > vecPP_VV = vecRhoP_VV - vecRho_VV;

		af::array magLM_VV = LUV::Dot( vecPM_VV, dirL_VV );
		af::array magLP_VV = LUV::Dot( vecPP_VV, dirL_VV );
		af::array magP0_VV = LUV::Length( LUV::Dot( vecPM_VV, dirU_VV ) );
		af::array magPM_VV = LUV::Length( vecPM_VV );
		af::array magPP_VV = LUV::Length( vecPM_VV );

		LUV::LuVector< 3, af::array > dirP0_VV = ( vecPM_VV - magLM_VV * dirL_VV ) / magP0_VV;
		
		af::array magR0_VV = LUV::Length( vecObs_VV - LUV::ProjLine( vecObs_VV, vecRhoM_VV, vecRhoP_VV ) );
		af::array magRM_VV = LUV::Length( vecObs_VV - vecRhoM_VV );
		af::array magRP_VV = LUV::Length( vecObs_VV - vecRhoP_VV );

		af::array magR0S_VV = magR0_VV * magR0_VV;
		af::array magAbsD_VV = af::abs( magD_VV );

		af::array magA1_VV = af::atan( ( magP0_VV * magLP_VV ) / ( magR0S_VV + magAbsD_VV * magRP_VV ) );
		af::array magA2_VV = af::atan( ( magP0_VV * magLM_VV ) / ( magR0S_VV + magAbsD_VV * magRM_VV ) );
		af::array magA3_VV = af::log( ( magRP_VV + magLP_VV ) / ( magRM_VV + magLM_VV ) );

		afGIVV = af::sum( af::sum( magD_VV * ( LUV::Dot( dirP0_VV, dirU_VV ) * ( magAbsD_VV * ( magA1_VV - magA2_VV - magP0_VV * magA3_VV ) ) ) , 2 ), 3 ) / 2;














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

		LUV::LuVector3< T > faceVertexM[ 3 ] = {
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexM[ 0 ] ], vertexDataPtr_[ 3 * idFaceVertexM[ 0 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexM[ 0 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexM[ 1 ] ], vertexDataPtr_[ 3 * idFaceVertexM[ 1 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexM[ 1 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexM[ 2 ] ], vertexDataPtr_[ 3 * idFaceVertexM[ 2 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexM[ 2 ] + 2 ] )
		};

		LUV::LuVector3< T > faceVertexN[ 3 ] = {
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexN[ 0 ] ], vertexDataPtr_[ 3 * idFaceVertexN[ 0 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexN[ 0 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexN[ 1 ] ], vertexDataPtr_[ 3 * idFaceVertexN[ 1 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexN[ 1 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertexN[ 2 ] ], vertexDataPtr_[ 3 * idFaceVertexN[ 2 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertexN[ 2 ] + 2 ] )
		};

		LUV::LuVector3< T > tetraVertexA[ 4 ] = {
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexA[ 0 ] ], vertexDataPtr_[ 3 * idTetraVertexA[ 0 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexA[ 0 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexA[ 1 ] ], vertexDataPtr_[ 3 * idTetraVertexA[ 1 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexA[ 1 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexA[ 2 ] ], vertexDataPtr_[ 3 * idTetraVertexA[ 2 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexA[ 2 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexA[ 3 ] ], vertexDataPtr_[ 3 * idTetraVertexA[ 3 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexA[ 3 ] + 2 ] )
		};

		LUV::LuVector3< T > tetraVertexB[ 4 ] = {
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexB[ 0 ] ], vertexDataPtr_[ 3 * idTetraVertexB[ 0 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexB[ 0 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexB[ 1 ] ], vertexDataPtr_[ 3 * idTetraVertexB[ 1 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexB[ 1 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexB[ 2 ] ], vertexDataPtr_[ 3 * idTetraVertexB[ 2 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexB[ 2 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertexB[ 3 ] ], vertexDataPtr_[ 3 * idTetraVertexB[ 3 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertexB[ 3 ] + 2 ] )
		};
		
		T faceAreaM = LUV::TriangleArea( faceVertexM[ 0 ], faceVertexM[ 1 ], faceVertexM[ 2 ] );
		T faceAreaN = LUV::TriangleArea( faceVertexN[ 0 ], faceVertexN[ 1 ], faceVertexN[ 2 ] );
		
		T tetraVolumeA = LUV::TetrahedronVolume( tetraVertexA[ 0 ], tetraVertexA[ 1 ], tetraVertexA[ 2 ], tetraVertexA[ 3 ] );
		T tetraVolumeB = LUV::TetrahedronVolume( tetraVertexB[ 0 ], tetraVertexB[ 1 ], tetraVertexB[ 2 ], tetraVertexB[ 3 ] );

		UINT_T idSwgVertexM = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFaceM ], &tetraVertexIndexPtr_[ 4 * idTetraA ] );
		UINT_T idSwgVertexN = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFaceN ], &tetraVertexIndexPtr_[ 4 * idTetraB ] );

		LUV::LuVector3< T > swgVertexM = LUV::LuVector3< T >( vertexDataPtr_[ 3 * idSwgVertexM ], vertexDataPtr_[ 3 * idSwgVertexM + 1 ], vertexDataPtr_[ 3 * idSwgVertexM + 2 ] );
		LUV::LuVector3< T > swgVertexN = LUV::LuVector3< T >( vertexDataPtr_[ 3 * idSwgVertexN ], vertexDataPtr_[ 3 * idSwgVertexN + 1 ], vertexDataPtr_[ 3 * idSwgVertexN + 2 ] );

		T factorC = faceAreaM * faceAreaN / ( 9 * tetraVolumeA * tetraVolumeB );
		
		//////
		
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
		
		af::array afGIVV = af::array( 1, tetraQuadCount_, AF_FLOAT_T );
		af::array afGIVS = af::array( 1, tetraQuadCount_, AF_FLOAT_T );
		af::array afGISV = af::array( 1, faceQuadCount_, AF_FLOAT_T );
		af::array afGISS = af::array( 1, faceQuadCount_, AF_FLOAT_T );

		CalculateGI( afFaceQuadDataM, afTetraQuadDataA, afFaceVertexN, afTetraVertexB, afGIVV, afGIVS, afGISV, afGISS );



		af::array afTetraQuadData = af::array( 4, quadCount_, AF_FLOAT_T );
		afTetraQuadData.write( &tetraQuadDataPtr_[ quadBlockSize_ * idTetra ], SIZEOF_T * 4 * quadCount_, afHost );







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

	inline void GenerateSingleElem( const UINT_T& idFaceN, const UINT_T& idFaceM ) const
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
			
		UINT_T idTetraN[ 2 ] = {
			faceTetraIndexPtr_[ 2 * idFaceN ],
			faceTetraIndexPtr_[ 2 * idFaceN + 1 ]
		};
			
		UINT_T idTetraM[ 2 ] = {
			faceTetraIndexPtr_[ 2 * idFaceM ],
			faceTetraIndexPtr_[ 2 * idFaceM + 1 ]
		};
		
		complex< T > result;

		result = TetrahedralIntegral( idFaceN, idFaceM, idTetraN[ 0 ], idTetraM[ 0 ] );

		if( idTetraM[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idFaceN, idFaceM, idTetraN[ 0 ], idTetraM[ 1 ] );
		}

		if( idTetraN[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idFaceN, idFaceM, idTetraN[ 1 ], idTetraM[ 0 ] );

			if( idTetraM[ 1 ] != -1 )
			{
				result += TetrahedralIntegral( idFaceN, idFaceM, idTetraN[ 1 ], idTetraM[ 1 ] );
			}
		}



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
		cout << "Thread: " << idFaceNStart << " - " << idFaceNEnd << endl;

		UINT_T idFaceNLimit = faceCount_ < idFaceNEnd ? faceCount_ : idFaceNEnd;

		for( UINT_T idFaceN = idFaceStart; idFaceN < idFaceLimit; ++idFaceN )
		{
			for( UINT_T idFaceM = 0; idFaceM < faceCount_; ++idFaceM )
			{
				GenerateSingleElem( idFaceN, idFaceM );
			}
		}
	}

	void Generate() const
	{
		for( UINT_T idFaceN = 0; idFaceN < faceCount_; ++idFaceN )
		{
			for( UINT_T idFaceM = 0; idFaceM < faceCount_; ++idFaceM )
			{
				GenerateSingleElem( idFaceN, idFaceM );
			}
		}
	}

	void ThreadedGenerate()
	{
		UINT_T idFaceNDelta = (UINT_T)( ceil( (double)( faceCount_ ) / (double)( threadCount_ ) ) );

		for( UINT_T idThread = 0; idThread < threadCount_; ++idThread )
		{
			UINT_T idFaceNStart = idThread * idFaceNDelta;
			UINT_T idFaceNEnd = idFaceSNtart + idFaceNDelta;
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