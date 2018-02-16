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
		threadCount_( thread::hardware_concurrency() ),
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
		return exp( -waveNumberJ_ * R ) / R;
	}

	inline complex< T > GreenRegular( const T& R )
	{
		return R == 0 ? (waveNumberJ_) : ( ( exp( waveNumberJ_ * R ) - 1.0 ) / R );
	}

	inline T GreenSingular( const T& R )
	{
		return 1.0 / R;
	}










	//////////////////////////////////////////////////////////////////////////
	
	inline void GreenVolVolIntegralEdge(
		T& edgeGIVV,
		T& edgeGDVV,
		const LUV::LuVector3< T >& dirN,
		const LUV::LuVector3< T >& vecRho,
		const T& magD,
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecTetra,
		const LUV::LuVector3< T >& vecFace,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2
	)
	{
		LUV::LuVector3< T > dirU = -LUV::LineNormalP( vecFace, vecV1, vecV2 );
		LUV::LuVector3< T > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector3< T > vecS = vecV2 + vecV1;
		LUV::LuVector3< T > vecM = vecV2 - vecV1;
		LUV::LuVector3< T > dirM = Unit( vecM );
		LUV::LuVector3< T > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector3< T > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector3< T > vecRhoP = ( vecS + vecB ) / 2;
		
		LUV::LuVector3< T > vecPM = vecRhoM - vecRho;
		LUV::LuVector3< T > vecPP = vecRhoP - vecRho;

		T magLM = LUV::Dot( vecPM, dirL );
		T magLP = LUV::Dot( vecPP, dirL );
		T magP0 = abs( LUV::Dot( vecPM, dirU ) );

		LUV::LuVector3< T > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		T magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		T magRM = LUV::Length( vecObs - vecRhoM );
		T magRP = LUV::Length( vecObs - vecRhoP );

		T magR0S = magR0 * magR0;
		T magAbsD = abs( magD );
		T magDS = magD * magD;
		T magAbsDC = magAbsD * magDS;

		T magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		T magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		T magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );
		T magA4 = ( magP0 * ( magR0S + 2.0 * magDS ) ) / 2.0;
		T magA5 = ( magP0 * ( magLP * magRP - magLM * magRM ) ) / 2.0;

		T magA6 = LUV::Dot( dirP0, dirU );
		T magA7 = magA1 - magA2;

		edgeGIVV += magA6 * ( magAbsD * magA7 - magP0 * magA3 );
		edgeGDVV += magA6 * ( magA3 * magA4 + magA5 - magAbsDC * magA7 );
	}

	inline void GreenVolVolIntegralFace(
		T& GIVV,
		LUV::LuVector3< T >& GDVV,
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
		
		T edgeGIVV = 0;
		T edgeGDVV = 0;
		
		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV1, vecV2, vecV3 );
		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV2, vecV3, vecV1 );
		GreenVolVolIntegralEdge( edgeGIVV, edgeGDVV, dirN, vecRho, magD, vecObs, vecTetra, vecV3, vecV1, vecV2 );
		
		GIVV += magD * edgeGIVV;
		GDVV += dirN * edgeGDVV;
	}

	inline void GreenVolVolIntegral(
		T& GIVV,
		LUV::LuVector3< T >& GDVV,
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2,
		const LUV::LuVector3< T >& vecV3,
		const LUV::LuVector3< T >& vecV4
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
		T& edgeGISV,
		const LUV::LuVector3< T >& dirN,
		const LUV::LuVector3< T >& vecRho,
		const T& magAbsD,
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecTetra,
		const LUV::LuVector3< T >& vecFace,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2
	)
	{
		LUV::LuVector3< T > dirU = -LUV::LineNormalP( vecFace, vecV1, vecV2 );
		LUV::LuVector3< T > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector3< T > vecS = vecV2 + vecV1;
		LUV::LuVector3< T > vecM = vecV2 - vecV1;
		LUV::LuVector3< T > dirM = Unit( vecM );
		LUV::LuVector3< T > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector3< T > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector3< T > vecRhoP = ( vecS + vecB ) / 2;
		
		LUV::LuVector3< T > vecPM = vecRhoM - vecRho;
		LUV::LuVector3< T > vecPP = vecRhoP - vecRho;

		T magLM = LUV::Dot( vecPM, dirL );
		T magLP = LUV::Dot( vecPP, dirL );
		T magP0 = abs( LUV::Dot( vecPM, dirU ) );

		LUV::LuVector3< T > dirP0 = ( vecPM - magLM * dirL ) / magP0;
		
		T magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		T magRM = LUV::Length( vecObs - vecRhoM );
		T magRP = LUV::Length( vecObs - vecRhoP );

		T magR0S = magR0 * magR0;

		T magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		T magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		T magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );

		T magA6 = LUV::Dot( dirP0, dirU );
		T magA7 = magA1 - magA2;
		
		//cout << "magP0: " << magP0 << endl;
		//cout << "edgeGISV: " << ( magAbsD * magA7 - magP0 * magA3 ) << endl;

		edgeGISV += magP0 == 0 ? 0 : magA6 * ( magAbsD * magA7 - magP0 * magA3 );
	}

	inline void GreenVolIntegralFace(
		T& GISV,
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
		
		T edgeGISV = 0;
		
		GreenVolIntegralEdge( edgeGISV, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV1, vecV2, vecV3 );
		GreenVolIntegralEdge( edgeGISV, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV2, vecV3, vecV1 );
		GreenVolIntegralEdge( edgeGISV, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV3, vecV1, vecV2 );
		
		GISV += magD * edgeGISV;
	}

	inline void GreenVolIntegral(
		T& GISV,
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2,
		const LUV::LuVector3< T >& vecV3,
		const LUV::LuVector3< T >& vecV4
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
		T& edgeGI_S,
		const LUV::LuVector3< T >& dirN,
		const LUV::LuVector3< T >& vecRho,
		const T& magAbsD,
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecTetra,
		const LUV::LuVector3< T >& vecFace,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2
	)
	{
		LUV::LuVector3< T > dirU = -LUV::LineNormalP( vecFace, vecV1, vecV2 );
		LUV::LuVector3< T > dirL = LUV::Cross( dirN, dirU );

		LUV::LuVector3< T > vecS = vecV2 + vecV1;
		LUV::LuVector3< T > vecM = vecV2 - vecV1;
		LUV::LuVector3< T > dirM = Unit( vecM );
		LUV::LuVector3< T > vecB = LUV::Dot( dirM, dirL ) * vecM;
		LUV::LuVector3< T > vecRhoM = ( vecS - vecB ) / 2;
		LUV::LuVector3< T > vecRhoP = ( vecS + vecB ) / 2;
		
		LUV::LuVector3< T > vecPM = vecRhoM - vecRho;
		LUV::LuVector3< T > vecPP = vecRhoP - vecRho;

		T magLM = LUV::Dot( vecPM, dirL );
		T magLP = LUV::Dot( vecPP, dirL );
		T magP0 = abs( LUV::Dot( vecPM, dirU ) );

		LUV::LuVector3< T > dirP0 = ( vecPM - magLM * dirL ) / magP0;

		//cout << magP0 << endl;
		//if( isnan( magP0 ) ){ cout << ">>>>NaN @ magP0" << endl; char stopper; cin >> stopper; };
		//if( isnan( magLM ) ){ cout << ">>>>NaN @ magLM" << endl; char stopper; cin >> stopper; };
		//if( isnan( LUV::Sum( vecPM ) ) ){ cout << ">>>>NaN @ vecPM" << endl; char stopper; cin >> stopper; };
		//if( isnan( LUV::Sum( dirL ) ) ){ cout << ">>>>NaN @ dirL" << endl; char stopper; cin >> stopper; };
		
		T magR0 = LUV::Length( vecObs - LUV::ProjLine( vecObs, vecRhoM, vecRhoP ) );
		T magRM = LUV::Length( vecObs - vecRhoM );
		T magRP = LUV::Length( vecObs - vecRhoP );

		T magR0S = magR0 * magR0;

		T magA1 = atan( ( magP0 * magLP ) / ( magR0S + magAbsD * magRP ) );
		T magA2 = atan( ( magP0 * magLM ) / ( magR0S + magAbsD * magRM ) );
		T magA3 = log( ( magRP + magLP ) / ( magRM + magLM ) );
		
		//if( isnan( LUV::Sum( dirP0 ) ) ){ cout << ">>>>NaN @ dirP0" << endl; char stopper; cin >> stopper; };
		//if( isnan( LUV::Sum( dirU ) ) ){ cout << ">>>>NaN @ dirP0" << endl; char stopper; cin >> stopper; };
		//if( isnan( magA1 ) ){ cout << ">>>>NaN @ magA1" << endl; char stopper; cin >> stopper; };
		//if( isnan( magA2 ) ){ cout << ">>>>NaN @ magA2" << endl; char stopper; cin >> stopper; };
		//if( isnan( LUV::Dot( dirP0, dirU ) ) ){ cout << ">>>>NaN @ LUV::Dot( dirP0, dirU )" << endl; char stopper; cin >> stopper; };

		edgeGI_S += magP0 == 0 ? 0 : LUV::Dot( dirP0, dirU ) * ( magP0 * magA3 - magAbsD * ( magA1 - magA2 ) );
	}

	inline void GreenSurIntegral(
		T& GI_S,
		const LUV::LuVector3< T >& vecObs,
		const LUV::LuVector3< T >& vecTetra,
		const LUV::LuVector3< T >& vecV1,
		const LUV::LuVector3< T >& vecV2,
		const LUV::LuVector3< T >& vecV3
	)
	{
		LUV::LuVector3< T > dirN = -LUV::PlaneNormalP( vecTetra, vecV1, vecV2, vecV3 );
		LUV::LuVector3< T > vecRho = LUV::ProjPlane( vecObs, vecV1, dirN );
		T magAbsD = abs( LUV::Dot( vecObs - vecRho, dirN ) );

		
		if( isnan( LUV::Sum( dirN ) ) ){ cout << ">>>NaN @ dirN" << endl; char stopper; cin >> stopper; };
		if( isnan( LUV::Sum( vecRho ) ) ){ cout << ">>>NaN @ vecRho" << endl; char stopper; cin >> stopper; };

		GreenSurIntegralEdge( GI_S, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV1, vecV2, vecV3 );
		GreenSurIntegralEdge( GI_S, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV2, vecV3, vecV1 );
		GreenSurIntegralEdge( GI_S, dirN, vecRho, magAbsD, vecObs, vecTetra, vecV3, vecV1, vecV2 );
	}

	//////////////////////////////////////////////////////////////////////////



	inline complex< T > TetrahedralIntegral( const UINT_T& idFaceM, const UINT_T& idFaceN, const UINT_T& idTetraA, const UINT_T& idTetraB )
	{
		UINT_T idFaceM3 = 3 * idFaceM;
		UINT_T* idFaceVertexM = &faceVertexIndexPtr_[ idFaceM3 ];

		UINT_T idFaceN3 = 3 * idFaceN;
		UINT_T* idFaceVertexN = &faceVertexIndexPtr_[ idFaceN3 ];

		UINT_T idTetraA4 = 4 * idTetraA;
		UINT_T* idTetraVertexA = &tetraVertexIndexPtr_[ idTetraA4 ];

		UINT_T idTetraB4 = 4 * idTetraB;
		UINT_T* idTetraVertexB = &tetraVertexIndexPtr_[ idTetraB4 ];

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

		LUV::LuVector3< T >* tetraVertexB[ 4 ] = {
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexB[ 0 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexB[ 1 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexB[ 2 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertexB[ 3 ] ] )
		};
		
		T faceAreaM = LUV::TriangleArea( *faceVertexM[ 0 ], *faceVertexM[ 1 ], *faceVertexM[ 2 ] );
		T faceAreaN = LUV::TriangleArea( *faceVertexN[ 0 ], *faceVertexN[ 1 ], *faceVertexN[ 2 ] );
		
		T tetraVolumeA = LUV::TetrahedronVolume( *tetraVertexA[ 0 ], *tetraVertexA[ 1 ], *tetraVertexA[ 2 ], *tetraVertexA[ 3 ] );
		T tetraVolumeB = LUV::TetrahedronVolume( *tetraVertexB[ 0 ], *tetraVertexB[ 1 ], *tetraVertexB[ 2 ], *tetraVertexB[ 3 ] );

		UINT_T idSwgVertexM = GetSwgVert( &faceVertexIndexPtr_[ idFaceM3 ], &tetraVertexIndexPtr_[ idTetraA4 ] );
		UINT_T idSwgVertexN = GetSwgVert( &faceVertexIndexPtr_[ idFaceN3 ], &tetraVertexIndexPtr_[ idTetraB4 ] );

		LUV::LuVector3< T >* swgVertexM = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idSwgVertexM ] );
		LUV::LuVector3< T >* swgVertexN = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idSwgVertexN ] );

		complex< T > I1( 0, 0 );
		LUV::LuVector3< complex< T > > I2( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) );
		LUV::LuVector3< complex< T > > I3( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) );
		LUV::LuVector3< complex< T > > I4( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) );
		LUV::LuVector3< complex< T > > I5( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) );
		LUV::LuVector3< T > I6( 0, 0, 0 );
		LUV::LuVector3< T > I7( 0, 0, 0 );

		UINT_T addressStartQA = tetraQuadBlockSize_ * idTetraA;
		UINT_T addressStartQB = tetraQuadBlockSize_ * idTetraB;
		UINT_T addressStartQM = faceQuadBlockSize_ * idFaceM;
		UINT_T addressStartQN = faceQuadBlockSize_ * idFaceN;
		UINT_T addressQA = 0;
		UINT_T addressQB = 0;
		UINT_T addressQM = 0;
		UINT_T addressQN = 0;

		for( UINT_T idqA = 0; idqA < tetraQuadCount_; ++idqA )
		{
			addressQA = addressStartQA + 4 * idqA;
			T* magWA = &tetraQuadDataPtr_[ addressQA ];
			LUV::LuVector3< T >* vecQA = reinterpret_cast< LUV::LuVector3< T >* >( &tetraQuadDataPtr_[ addressQA + 1 ] );

			complex< T >					magGNVV( 0, 0 );
			complex< T >					magGNVS( 0, 0 );
			LUV::LuVector3< complex< T > >	vecGNVV( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) );
			T								magGIVV( 0 );
			T								magGIVS( 0 );
			LUV::LuVector3< T >				vecGDVV( 0, 0, 0 );

			for( UINT_T idqB = 0; idqB < tetraQuadCount_; ++idqB )
			{
				addressQB = addressStartQB + 4 * idqB;
				T* magWB =  &tetraQuadDataPtr_[ addressQB ];
				LUV::LuVector3< T >* vecQB = reinterpret_cast< LUV::LuVector3< T >* >( &tetraQuadDataPtr_[ addressQB + 1 ] );
				
				T magR = LUV::Length( *vecQA - *vecQB );
				complex< T > magGRw = GreenRegular( magR ) * *magWB;

				magGNVV += magGRw;
				vecGNVV += magGRw * *vecQB;
			}

			for( UINT_T idqN = 0; idqN < faceQuadCount_; ++idqN )
			{
				addressQN = addressStartQN + 4 * idqN;
				T* magWN =  &faceQuadDataPtr_[ addressQN ];
				LUV::LuVector3< T >* vecQN = reinterpret_cast< LUV::LuVector3< T >* >( &faceQuadDataPtr_[ addressQN + 1 ] );
				
				T magR = LUV::Length( *vecQA - *vecQN );
				complex< T > magGRw = GreenRegular( magR ) * *magWN;

				magGNVS += magGRw;
			}
			
			GreenVolVolIntegral( magGIVV, vecGDVV, *vecQA, *tetraVertexB[ 0 ], *tetraVertexB[ 1 ], *tetraVertexB[ 2 ], *tetraVertexB[ 3 ] );
			GreenSurIntegral( magGIVS, *vecQA, *swgVertexN, *faceVertexN[ 0 ], *faceVertexN[ 1 ], *faceVertexN[ 2 ] );

			LUV::LuVector3< T > vecQAw = *vecQA * *magWA;

			complex< T > magGTVV = magGNVV + magGIVV;
			LUV::LuVector3< complex< T > > vecGTVV = ( vecGNVV + ( vecGDVV + *vecQA * magGIVV ) );

			I1 += magGTVV * *magWA;
			I2 += magGTVV * vecQAw;
			I3 += vecGTVV * *magWA;
			I4 += vecGTVV * vecQAw;
			I5[ 1 ] += ( magGNVS + magGIVS ) * *magWA;
			I6 += vecQAw;
			I7 += vecQAw * *vecQA;
		}

		//cout << faceQuadCount_ << endl;
		for( UINT_T idqM = 0; idqM < faceQuadCount_; ++idqM )
		{
			//cout << "!!!!!!!!MMMM!!!!!!" << endl;
			addressQM = addressStartQM + 4 * idqM;
			T* magWM = &faceQuadDataPtr_[ addressQM ];
			LUV::LuVector3< T >* vecQM = reinterpret_cast< LUV::LuVector3< T >* >( &faceQuadDataPtr_[ addressQM + 1 ] );

			complex< T >					magGNSV( 0, 0 );
			complex< T >					magGNSS( 0, 0 );
			T								magGISV( 0 );
			T								magGISS( 0 );

			for( UINT_T idqB = 0; idqB < tetraQuadCount_; ++idqB )
			{
				addressQB = addressStartQB + 4 * idqB;
				T* magWB =  &tetraQuadDataPtr_[ addressQB ];
				LUV::LuVector3< T >* vecQB = reinterpret_cast< LUV::LuVector3< T >* >( &tetraQuadDataPtr_[ addressQB + 1 ] );
				
				T magR = LUV::Length( *vecQM - *vecQB );
				complex< T > magGRw = GreenRegular( magR ) * *magWB;

				magGNSV += magGRw;
			}

			for( UINT_T idqN = 0; idqN < faceQuadCount_; ++idqN )
			{
				addressQN = addressStartQN + 4 * idqN;
				T* magWN =  &faceQuadDataPtr_[ addressQN ];
				LUV::LuVector3< T >* vecQN = reinterpret_cast< LUV::LuVector3< T >* >( &faceQuadDataPtr_[ addressQN + 1 ] );
				
				T magR = LUV::Length( *vecQM - *vecQN );
				complex< T > magGRw = GreenRegular( magR ) * *magWN;

				magGNSS += magGRw;
			}

			GreenVolIntegral( magGISV, *vecQM, *tetraVertexB[ 0 ], *tetraVertexB[ 1 ], *tetraVertexB[ 2 ], *tetraVertexB[ 3 ] );
			GreenSurIntegral( magGISS, *vecQM, *swgVertexN, *faceVertexN[ 0 ], *faceVertexN[ 1 ], *faceVertexN[ 2 ] );
			
			I5[ 0 ] += ( magGNSV + magGISV ) * *magWM;
			I5[ 2 ] += ( magGNSS + magGISS ) * *magWM;
			
		
			if( isnan( abs( magGNSV ) ) ){ cout << ">>NaN @ magGNSV" << endl; char stopper; cin >> stopper; };
			if( isnan( abs( magGISV ) ) ){ cout << ">>NaN @ magGISV" << endl; char stopper; cin >> stopper; };
			if( isnan( abs( magGNSS ) ) ){ cout << ">>NaN @ magGNSS" << endl; char stopper; cin >> stopper; };
			if( isnan( abs( magGISS ) ) ){ cout << ">>NaN @ magGISS" << endl; char stopper; cin >> stopper; };
			if( isnan( abs( *magWM ) ) ){ cout << ">>NaN @ *magWM" << endl; char stopper; cin >> stopper; };
		}
		
		T factorC = faceAreaM * faceAreaN / ( 9 * tetraVolumeA * tetraVolumeB );
		T epsRA = emPropDataPtr_[ 2 * idTetraA ];
		T epsRB = emPropDataPtr_[ 2 * idTetraB ];
		T contrastA = ( 1 - epsRA ) / epsRA;
		T contrastB = ( 1 - epsRB ) / epsRB;
		T swgDot = LUV::Dot( *swgVertexM, *swgVertexN );
		LUV::LuVector3< T > swgSum = *swgVertexM + *swgVertexN;

		complex< T > j( 0, 1 );

		complex< T > factor1 = ( -j ) * ( factorC / ( waveNumber_ * c0_ * eps0_ * epsRB ) );
		complex< T > factor2 = j * ( waveNumber_ * c0_ * mu0_ * contrastB * factorC );
		complex< T > factor3 = ( -j ) * ( 9 * factorC * contrastB / ( waveNumber_ * c0_ * eps0_ ) );

		T volArRatioM = tetraVolumeA / faceAreaM;
		T volArRatioN = tetraVolumeB / faceAreaN;
		T volArRatio = volArRatioM * volArRatioN;

		/*
		complex< T > I1( 0, 0 ); //0
		LUV::LuVector3< complex< T > > I2( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) ); // 1 2 3
		LUV::LuVector3< complex< T > > I3( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) ); // 4 5 6
		LUV::LuVector3< complex< T > > I4( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) ); // 7 8 9
		LUV::LuVector3< complex< T > > I5( ( 0, 0 ), ( 0, 0 ), ( 0, 0 ) ); // 10 11 12
		LUV::LuVector3< T > I6( 0, 0, 0 ); // 13 14 15
		LUV::LuVector3< T > I7( 0, 0, 0 ); // 16 17 18
		*/

		
		if( isnan( abs( factor1 ) ) ){ cout << ">NaN @ factor1" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( factor2 ) ) ){ cout << ">NaN @ factor2" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( factor3 ) ) ){ cout << ">NaN @ factor3" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( swgDot ) ) ){ cout << ">NaN @ swgDot" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( tetraVolumeA ) ) ){ cout << ">NaN @ tetraVolumeA" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( LUV::Sum( I7 ) ) ) ){ cout << ">NaN @ LUV::Sum( I7 )" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( LUV::Dot( I6, swgSum ) ) ) ){ cout << ">NaN @ LUV::Dot( I6, swgSum )" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( I1 ) ) ){ cout << ">NaN @ I1" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( LUV::Sum( I4 ) ) ) ){ cout << ">NaN @ LUV::Sum( I4 )" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( LUV::Dot( I2, *swgVertexN ) ) ) ){ cout << ">NaN @ LUV::Dot( I2, *swgVertexN )" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( LUV::Dot( I3, *swgVertexM ) ) ) ){ cout << ">NaN @  LUV::Dot( I3, *swgVertexM )" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( I5[0] ) ) ){ cout << ">NaN @ I5[0]" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( I5[1] ) ) ){ cout << ">NaN @ I5[1]" << endl; char stopper; cin >> stopper; };
		if( isnan( abs( I5[2] ) ) ){ cout << ">NaN @ I5[2]" << endl; char stopper; cin >> stopper; };


		// !!! book formula
		//return
		//	factor1 * ( swgDot * tetraVolumeA + LUV::Sum( I7 )
		//		- LUV::Dot( I6, swgSum ) )
		//	- factor2 * ( swgDot * I1 + LUV::Sum( I4 )
		//		- LUV::Dot( I2, *swgVertexN )
		//		- LUV::Dot( I3, *swgVertexM ) )
		//	- factor3 * ( I1 - I5[0] * volArRatioM - I5[1] * volArRatioN + I5[2] * volArRatio );
		// !!!


		//return
		//	( idTetraA == idTetraB ? factor1 * ( swgDot * tetraVolumeA + LUV::Sum( I7 )
		//		- LUV::Dot( I6, swgSum ) ) : 0 );

		return
			( idTetraA == idTetraB ? factor1 * ( swgDot * tetraVolumeA + LUV::Sum( I7 )
				- LUV::Dot( I6, swgSum ) ) : 0 )
			- factor2 * ( swgDot * I1 + LUV::Sum( I4 )
				- LUV::Dot( I2, *swgVertexN )
				- LUV::Dot( I3, *swgVertexM ) )
			- factor3 * ( I1 - I5[0] * volArRatioM - I5[1] * volArRatioN + I5[2] * volArRatio );

		//return
		//	( idFaceM == idFaceN ? factor1 * ( swgDot * tetraVolumeA + LUV::Sum( I7 )
		//		- LUV::Dot( I6, swgSum ) ) : 0 )
		//	- factor2 * ( swgDot * I1 + LUV::Sum( I4 )
		//		- LUV::Dot( I2, *swgVertexN )
		//		- LUV::Dot( I3, *swgVertexM ) )
		//	- factor3 * ( I1 - I5[0] * volArRatioM - I5[1] * volArRatioN + I5[2] * volArRatio );

				/*
		complex< T > result =
			factor1 * ( swgDot * tetraVolumeA + I[16] + I[17] + I[18]
				- swgSum[0] * I[13] - swgSum[1] * I[14] - swgSum[2] * I[15] )
			- factor2 * ( swgDot * I[0] + I[7] + I[8] + I[9]
				- swgVertexN[0] * I[1] - swgVertexN[1] * I[2] - swgVertexN[2] * I[3]
				- swgVertexM[0] * I[4] - swgVertexM[1] * I[5] - swgVertexM[2] * I[6] )
			- factor3 * ( I[0] - volArRatioM * I[10] - volArRatioN * I[11] - volArRatio * I[12] )
		;
		*/
	}

	inline void GenerateSingleElem( const UINT_T& idFaceM, const UINT_T& idFaceN )
	{
		UINT_T* idTetraM = &faceTetraIndexPtr_[ 2 * idFaceM ];
		UINT_T* idTetraN = &faceTetraIndexPtr_[ 2 * idFaceN ];
		
		complex< T > result;

		result = TetrahedralIntegral( idFaceM, idFaceN, idTetraM[ 0 ], idTetraN[ 0 ] );
		if( isnan( abs( result ) ) )
		{
			cout << "NaN @ M = " << idFaceM << ", N = " << idFaceN << ", A = " << idTetraM[ 0 ] << ", B = " << idTetraN[ 0 ] << endl;
			char stopper;
			cin >> stopper;
		}

		if( idTetraN[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idFaceM, idFaceN, idTetraM[ 0 ], idTetraN[ 1 ] );
			if( isnan( abs( result ) ) )
			{
				cout << "NaN @ M = " << idFaceM << ", N = " << idFaceN << ", A = " << idTetraM[ 0 ] << ", B = " << idTetraN[ 1 ] << endl;
				char stopper;
				cin >> stopper;
			}
		}

		if( idTetraM[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idFaceM, idFaceN, idTetraM[ 1 ], idTetraN[ 0 ] );
			if( isnan( abs( result ) ) )
			{
				cout << "NaN @ M = " << idFaceM << ", N = " << idFaceN << ", A = " << idTetraM[ 1 ] << ", B = " << idTetraN[ 0 ] << endl;
				char stopper;
				cin >> stopper;
			}

			if( idTetraN[ 1 ] != -1 )
			{
				result += TetrahedralIntegral( idFaceM, idFaceN, idTetraM[ 1 ], idTetraN[ 1 ] );
				if( isnan( abs( result ) ) )
				{
					cout << "NaN @ M = " << idFaceM << ", N = " << idFaceN << ", A = " << idTetraM[ 1 ] << ", B = " << idTetraN[ 1 ] << endl;
					char stopper;
					cin >> stopper;
				}
			}
		}
		
		momDataPtr_[ idFaceN * faceCount_ + idFaceM ] = result; // column major
	}

	void PartialGenerate( const UINT_T& idFaceNStart, const UINT_T& idFaceNEnd )
	{
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