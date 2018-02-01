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
		threadCount_( thread::hardware_concurrency() * 4 ),
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

		UINT_T idSwgVertexM = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFaceM ], &tetraVertexIndexPtr_[ 4 * idTetraA ] );
		UINT_T idSwgVertexN = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFaceN ], &tetraVertexIndexPtr_[ 4 * idTetraB ] );

		LUV::LuVector3< T >* swgVertexM = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idSwgVertexM ] );
		LUV::LuVector3< T >* swgVertexN = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idSwgVertexN ] );

		T factorC = faceAreaM * faceAreaN / ( 9 * tetraVolumeA * tetraVolumeB );







	}

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