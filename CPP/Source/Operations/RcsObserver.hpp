#ifndef RCS_OBSERVER_INCLUDED
#define RCS_OBSERVER_INCLUDED

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
class RcsObserver
{
public:
	UINT_T threadCount_;
	vector< thread* > threadList_;
	
	AF_CFLOAT j_;

	T eps0_;
	T mu0_;
	T c0_;

	T waveNumber_;
	AF_CFLOAT waveNumberJ_;

	//temporary list of pointers to use across functions.
	
	UINT_T obsCount_;
	UINT_T faceCount_;
	UINT_T tetraQuadCount_;
	UINT_T tetraQuadBlockSize_;
	
	T* obsDataPtr_;
	T* vertexDataPtr_;
	T* tetraQuadDataPtr_;
	T* emPropDataPtr_;
	complex< T >* swgCoefDataPtr_;

	UINT_T* tetraVertexIndexPtr_;
	UINT_T* faceVertexIndexPtr_;
	UINT_T* faceTetraIndexPtr_;
	UINT_T* tetraFaceIndexPtr_;

	T* rcsDataPtr_;

public:
	RcsObserver(
		const T& waveNumber,
		const UINT_T& obsCount,
		const UINT_T& faceCount,
		const UINT_T& tetraQuadCount,
		const shared_ptr< T >& obsData,
		const shared_ptr< T >& vertexData,
		const shared_ptr< T >& tetraQuadData,
		const shared_ptr< T >& emPropData,
		const shared_ptr< complex< T > >& swgCoefData,
		const shared_ptr< UINT_T >& tetraVertexIndex,
		const shared_ptr< UINT_T >& faceVertexIndex,
		const shared_ptr< UINT_T >& faceTetraIndex,
		const shared_ptr< UINT_T >& tetraFaceIndex,
		shared_ptr< T >& rcsData // preallocated
	) :
		threadCount_( thread::hardware_concurrency() * 2 ),
		threadList_(),
		j_( 0.0, 1.0 ),
		eps0_( 8.85418782E-12 ),
		mu0_( 1.25663706E-6 ),
		c0_( 299792458.0 ),
		waveNumber_( waveNumber ),
		waveNumberJ_( waveNumber * j_ ),
		obsCount_( obsCount ),
		faceCount_( faceCount ),
		tetraQuadCount_( tetraQuadCount ),
		tetraQuadBlockSize_( tetraQuadCount_ * 4 ),
		obsDataPtr_( obsData.get() ),
		vertexDataPtr_( vertexData.get() ),
		tetraQuadDataPtr_( tetraQuadData.get() ),
		emPropDataPtr_( emPropData.get() ),
		swgCoefDataPtr_( swgCoefData.get() ),
		tetraVertexIndexPtr_( tetraVertexIndex.get() ),
		faceVertexIndexPtr_( faceVertexIndex.get() ),
		faceTetraIndexPtr_( faceTetraIndex.get() ),
		tetraFaceIndexPtr_( tetraFaceIndex.get() ),
		rcsDataPtr_( rcsData.get() )
	{
		threadList_.reserve( threadCount_ );
	}

	~RcsObserver()
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
	
	inline LUV::LuVector3< complex< T > > TetrahedralIntegral( const UINT_T& idObs, const UINT_T& idFace, const UINT_T& idTetra )
	{
		UINT_T idFaceVertex[ 3 ] = {
			faceVertexIndexPtr_[ 3 * idFace ],
			faceVertexIndexPtr_[ 3 * idFace + 1 ],
			faceVertexIndexPtr_[ 3 * idFace + 2 ]
		};

		UINT_T idTetraVertex[ 4 ] = {
			tetraVertexIndexPtr_[ 4 * idTetra ],
			tetraVertexIndexPtr_[ 4 * idTetra + 1 ],
			tetraVertexIndexPtr_[ 4 * idTetra + 2 ],
			tetraVertexIndexPtr_[ 4 * idTetra + 3 ]
		};
		
		LUV::LuVector3< T > faceVertex[ 3 ] = {
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertex[ 0 ] ], vertexDataPtr_[ 3 * idFaceVertex[ 0 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertex[ 0 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertex[ 1 ] ], vertexDataPtr_[ 3 * idFaceVertex[ 1 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertex[ 1 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idFaceVertex[ 2 ] ], vertexDataPtr_[ 3 * idFaceVertex[ 2 ] + 1 ], vertexDataPtr_[ 3 * idFaceVertex[ 2 ] + 2 ] )
		};

		LUV::LuVector3< T > tetraVertex[ 4 ] = {
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertex[ 0 ] ], vertexDataPtr_[ 3 * idTetraVertex[ 0 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertex[ 0 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertex[ 1 ] ], vertexDataPtr_[ 3 * idTetraVertex[ 1 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertex[ 1 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertex[ 2 ] ], vertexDataPtr_[ 3 * idTetraVertex[ 2 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertex[ 2 ] + 2 ] ),
			LUV::LuVector3< T >( vertexDataPtr_[ 3 * idTetraVertex[ 3 ] ], vertexDataPtr_[ 3 * idTetraVertex[ 3 ] + 1 ], vertexDataPtr_[ 3 * idTetraVertex[ 3 ] + 2 ] )
		};

		T faceArea = LUV::TriangleArea( faceVertex[ 0 ], faceVertex[ 1 ], faceVertex[ 2 ] );
		
		T tetraVolume = LUV::TetrahedronVolume( tetraVertex[ 0 ], tetraVertex[ 1 ], tetraVertex[ 2 ], tetraVertex[ 3 ] );

		UINT_T idSwgVertex = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFace ], &tetraVertexIndexPtr_[ 4 * idTetra ] );
		
		LUV::LuVector3< T > swgVertex = LUV::LuVector3< T >( vertexDataPtr_[ 3 * idSwgVertex ], vertexDataPtr_[ 3 * idSwgVertex + 1 ], vertexDataPtr_[ 3 * idSwgVertex + 2 ] );

		af::array afTetraQuadData = af::array( 4, tetraQuadCount_, AF_FLOAT_T );
		afTetraQuadData.write( &tetraQuadDataPtr_[ tetraQuadBlockSize_ * idTetra ], SIZEOF_T * tetraQuadBlockSize_, afHost );

		//cout << "AAA" << endl;

		LUV::LuVector3< af::array > vecQ( afTetraQuadData.row( 1 ), afTetraQuadData.row( 2 ), afTetraQuadData.row( 3 ) );
		
		//cout << "BBB" << endl;

		af::array magQ = LUV::Length( vecQ );

		//cout << "CCC" << endl;

		LUV::LuVector3< T > vecObsSph( 1, obsDataPtr_[ 2 * idObs ], obsDataPtr_[ 2 * idObs + 1 ] );
		LUV::LuVector3< T > vecObs = LUV::SphToCts( vecObsSph );

		//cout << "vecSph:" << vecObsSph << endl;
		//cout << "vecObs:" << vecObs << endl;
		//cout << endl;

		//cout << "DDD" << endl;

		//vecQ[ 0 ] *= vecObs[ 0 ];

		//cout << "D1" << endl;

		af::array qDot = vecQ[ 0 ] * vecObs[ 0 ] + vecQ[ 1 ] * vecObs[ 1 ] + vecQ[ 2 ] * vecObs[ 2 ];  //LUV::Dot( vecQ, vecObs );
		//af_print( qDot );

		qDot = qDot * waveNumberJ_;
		//qDot *= waveNumberJ_;
		//af_print( qDot );
		//cout << "F" << endl;

		af::array expPart = af::exp( qDot ); //* afTetraQuadData.row( 0 );
		//cout << "G" << endl;

		LUV::LuVector3< af::array > vecDiff = vecQ - swgVertex;
		//cout << "H" << endl;

		LUV::LuVector3< af::array > sumVector = vecDiff * afTetraQuadData.row( 0 ) * expPart;
		//cout << "I" << endl;

		
		//af::array cosAlpha = LUV::Dot( vecQ, vecObs ) / ( 1 * magQ );
		//af::array expPart = af::exp( waveNumberJ_ * ( cosAlpha * magQ ) );
		//
		//af::array expPart = af::exp( LUV::Dot( vecQ, waveNumberJ_ * LUV::SphToCts( vecObsSph ) ) );

		//af::array =

		//LUV::LuVector3< af::array > sumVector = ( vecQ - swgVertex ) * ( afTetraQuadData.row( 0 ) * af::exp( LUV::Dot( vecQ, //god dayummm
		//	*reinterpret_cast< LUV::LuVector3< AF_CFLOAT >* >(
		//		&( *reinterpret_cast< complex< T >* >( &waveNumberJ_ ) * LUV::SphToCts( vecObsSph ) )
		//	)
		//) ) );

		//cout << "EEE" << endl;

		LUV::LuVector3< complex< T > > result;
		af::sum( sumVector[0], 1 ).host( &result[0] );
		af::sum( sumVector[1], 1 ).host( &result[1] );
		af::sum( sumVector[2], 1 ).host( &result[2] );

		T tetraEps = emPropDataPtr_[ 2 * idTetra ];
		T contrast = ( tetraEps - 1.0 ) / tetraEps;

		
		//cout << "resul: " << result << endl;
		//cout << "coef: " << swgCoefDataPtr_[ idFace ] << endl;
		//cout << "contrast: " << contrast << endl;
		//cout << "faceArea: " << faceArea << endl;
		//cout << "tetraVolume: " << tetraVolume << endl;

		result *= swgCoefDataPtr_[ idFace ] * ( contrast * faceArea / ( 3.0 * tetraVolume ) );

		//cout << "resul2: " << result << endl;
		//cout << endl;




		return result;
	}

	inline LUV::LuVector3< complex< T > > CalculateSingleSwg( const UINT_T& idObs, const UINT_T& idFace )
	{
		UINT_T idTetra[ 2 ] = {
			faceTetraIndexPtr_[ 2 * idFace ],
			faceTetraIndexPtr_[ 2 * idFace + 1 ]
		};

		LUV::LuVector3< complex< T > > result = TetrahedralIntegral( idObs, idFace, idTetra[ 0 ] );

		if( idTetra[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idObs, idFace, idTetra[ 1 ] );

			//cout << "resul3: " << result << endl;
			//cout << endl;
		}

		//////
		//if( idFace == 0 )
		//{
		//	cout << "OBS: " << idObs << endl;
		//	cout << "idTetra+: " << idTetra[ 0 ] << endl;
		//	cout << "idTetra-: " << idTetra[ 1 ] << endl;
		//	cout << "result: " << idObs << endl;
		//}
		//////

		return result;
	}

	inline void GenerateSingleElem( const UINT_T& idObs )
	{
		LUV::LuVector3< complex< T > > result = 0;

		for( UINT_T idFace = 0; idFace < faceCount_; ++idFace )
		{
			result += CalculateSingleSwg( idObs, idFace );
			//cout << "res@" << idFace << ": " << result << endl;
		}
		//cout << "resFinal" << result << endl;
		//cout << "resMag" << pow( LUV::Length( result ), 2 ) << endl;
		//cout << endl;


		rcsDataPtr_[ idObs ] = pow( LUV::Length( result ), 2 );
	}

	void PartialGenerate( const UINT_T& idObsStart, const UINT_T& idObsEnd )
	{
		UINT_T idObsLimit = obsCount_ < idObsEnd ? obsCount_ : idObsEnd;

		for( UINT_T idObs = idObsStart; idObs < idObsLimit; ++idObs )
		{
			GenerateSingleElem( idObs );
		}
	}

	void Generate()
	{
		for( UINT_T idObs = 0; idObs < obsCount_; ++idObs )
		{
			GenerateSingleElem( idObs );
		}
	}

	void ThreadedGenerate()
	{
		UINT_T idObsDelta = (UINT_T)( ceil( (double)( obsCount_ ) / (double)( threadCount_ ) ) );

		for( UINT_T idThread = 0; idThread < threadCount_; ++idThread )
		{
			UINT_T idObsStart = idThread * idObsDelta;
			UINT_T idObsEnd = idObsStart + idObsDelta;
			threadList_.push_back( new thread( [=]{ PartialGenerate( idObsStart, idObsEnd ); } ));
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