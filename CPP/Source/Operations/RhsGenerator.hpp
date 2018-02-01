#ifndef RHS_GENERATOR_INCLUDED
#define RHS_GENERATOR_INCLUDED

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

#include <_IncWave.hpp>

using namespace std;

template< class T >
class RhsGenerator
{
public:
	UINT_T threadCount_;
	vector< thread* > threadList_;

	//temporary list of pointers to use across functions.
	UINT_T faceCount_;
	UINT_T quadCount_;
	UINT_T quadBlockSize_;

	T* vertexDataPtr_;
	T* tetraQuadDataPtr_;

	UINT_T* tetraVertexIndexPtr_;
	UINT_T* faceVertexIndexPtr_;
	UINT_T* faceTetraIndexPtr_;
	UINT_T* tetraFaceIndexPtr_;

	const IncWave< T >* incWave_;

	complex< T >* rhsDataPtr_;


public:
	RhsGenerator(
		const UINT_T& faceCount,
		const UINT_T& quadCount,
		const shared_ptr< T >& vertexData,
		const shared_ptr< T >& tetraQuadData,
		const shared_ptr< UINT_T >& tetraVertexIndex,
		const shared_ptr< UINT_T >& faceVertexIndex,
		const shared_ptr< UINT_T >& faceTetraIndex,
		const shared_ptr< UINT_T >& tetraFaceIndex,
		const IncWave< T >* incWave,
		shared_ptr< complex< T > >& rhsData // preallocated
	) :
		threadCount_( thread::hardware_concurrency() ),
		threadList_(),
		faceCount_( faceCount ),
		quadCount_( quadCount ),
		quadBlockSize_( quadCount * 4 ),
		vertexDataPtr_( vertexData.get() ),
		tetraQuadDataPtr_( tetraQuadData.get() ),
		tetraVertexIndexPtr_( tetraVertexIndex.get() ),
		faceVertexIndexPtr_( faceVertexIndex.get() ),
		faceTetraIndexPtr_( faceTetraIndex.get() ),
		tetraFaceIndexPtr_( tetraFaceIndex.get() ),
		incWave_( incWave ),
		rhsDataPtr_( rhsData.get() )
	{
		threadList_.reserve( threadCount_ );
	}

	~RhsGenerator()
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

	inline void GenerateSingleElem( const UINT_T& idFace ) const
	{
		UINT_T idFaceVertex[ 3 ] = {
			faceVertexIndexPtr_[ 3 * idFace ],
			faceVertexIndexPtr_[ 3 * idFace + 1 ],
			faceVertexIndexPtr_[ 3 * idFace + 2 ]
		};

		LUV::LuVector3< T >* faceVertex[ 3 ] = {
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 0 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 1 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 2 ] ] )
		};

		T faceArea = LUV::TriangleArea( *faceVertex[ 0 ], *faceVertex[ 1 ], *faceVertex[ 2 ] );

		complex< T > result;
			
		UINT_T idTetra[ 2 ] = {
			faceTetraIndexPtr_[ 2 * idFace ],
			faceTetraIndexPtr_[ 2 * idFace + 1 ]
		};
			
		result = TetrahedralIntegral( idFace, idTetra[ 0 ] );

		if( idTetra[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idFace, idTetra[ 1 ] );
		}

		result *= faceArea / 3.0;
	}

	void PartialGenerate( const UINT_T& idFaceStart, const UINT_T& idFaceEnd )
	{
		cout << "Thread: " << idFaceStart << " - " << idFaceEnd << endl;

		UINT_T idFaceLimit = faceCount_ < idFaceEnd ? faceCount_ : idFaceEnd;

		for( UINT_T idFace = idFaceStart; idFace < idFaceLimit; ++idFace )
		{
			GenerateSingleElem( idFace );
		}
	}

	void Generate() const
	{
		for( UINT_T idFace = 0; idFace < faceCount_; ++idFace )
		{
			GenerateSingleElem( idFace );
		}
	}

	void ThreadedGenerate()
	{
		UINT_T idFaceDelta = (UINT_T)( ceil( (double)( faceCount_ ) / (double)( threadCount_ ) ) );

		for( UINT_T idThread = 0; idThread < threadCount_; ++idThread )
		{
			UINT_T idFaceStart = idThread * idFaceDelta;
			UINT_T idFaceEnd = idFaceStart + idFaceDelta;
			threadList_.push_back( new thread( [=]{ PartialGenerate( idFaceStart, idFaceEnd ); } ));
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

			//UINT_T idFaceVertex[ 3 ] = {
			//	faceVertexIndexPtr_[ 3 * idFace ],
			//	faceVertexIndexPtr_[ 3 * idFace + 1 ],
			//	faceVertexIndexPtr_[ 3 * idFace + 2 ]
			//};

			//LUV::LuVector3< T >* faceVertex[ 3 ] = {
			//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 0 ] ] ),
			//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 1 ] ] ),
			//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 2 ] ] )
			//};

			//T faceArea = LUV::TriangleArea( *faceVertex[ 0 ], *faceVertex[ 1 ], *faceVertex[ 2 ] );

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

			//rhsDataPtr_[ idFace ] = result;


			////////////////////////////////

			//UINT_T idTetra1 = faceTetraIndexPtr[ 2 * idFace ];
			//
			//UINT_T idTetra1Vertex[ 4 ] = {
			//	tetraVertexIndexPtr[ 4 * idTetra1 ],
			//	tetraVertexIndexPtr[ 4 * idTetra1 + 1 ],
			//	tetraVertexIndexPtr[ 4 * idTetra1 + 2 ],
			//	tetraVertexIndexPtr[ 4 * idTetra1 + 3 ]
			//};

			//LUV::LuVector3< T >* tetra1Vertex[ 4 ] = {
			//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra1Vertex[ 0 ] ] ),
			//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra1Vertex[ 1 ] ] ),
			//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra1Vertex[ 2 ] ] ),
			//	reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra1Vertex[ 3 ] ] )
			//};

			//T tetra1Volume = LUV::TetrahedronVolume< T >( *tetra1Vertex[ 0 ], *tetra1Vertex[ 1 ], *tetra1Vertex[ 2 ], *tetra1Vertex[ 3 ] );

			//T swgFactor1 = faceArea / ( 3.0 * tetra1Volume );

			//UINT_T idSwgVertex1 = GetSwgVert( &faceVertexIndexPtr[ 3 * idFace ], &tetraVertexIndexPtr[ 4 * idTetra1 ] );

			//LUV::LuVector3< T >* swgVertex1 = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idSwgVertex1 ] );

			/////

			//LUV::LuVector3c< T >* tetra1QuadFieldE = new LUV::LuVector3c< T >[ quadCount ];
			//for( UINT_T idQuad = 0; idQuad < quadCount; ++idQuad )
			//{
			//	tetra1QuadFieldE[ idQuad ] = incWave->FieldE(
			//		*( reinterpret_cast< LUV::LuVector3< T >* >(
			//			&tetraQuadDataPtr[ quadBlockSize * idTetra1 + 4 * idQuad + 1 ]
			//		) )
			//	);
			//}

			//af::array afTetra1ComputeMatrix = af::array( 3, quadCount, AF_COMPLEX_T );
			//afTetra1ComputeMatrix.write( reinterpret_cast< T* >( tetra1QuadFieldE ), SIZEOF_CT * 3 * quadCount, afHost );

			//delete[] tetra1QuadFieldE;

			//af::array afTetra1QuadData = af::array( 4, quadCount, AF_FLOAT_T );
			//afTetra1QuadData.write( &tetraQuadDataPtr[ quadBlockSize * idTetra1 ], SIZEOF_T * 4 * quadCount, afHost );

			//afTetra1ComputeMatrix.row( 0 ) *= afTetra1QuadData.row( 1 ) - (*swgVertex1)[ 0 ];
			//afTetra1ComputeMatrix.row( 1 ) *= afTetra1QuadData.row( 2 ) - (*swgVertex1)[ 1 ];
			//afTetra1ComputeMatrix.row( 2 ) *= afTetra1QuadData.row( 3 ) - (*swgVertex1)[ 2 ];
			//afTetra1ComputeMatrix = af::sum( afTetra1ComputeMatrix, 0 );
			//afTetra1ComputeMatrix *= afTetra1QuadData.row( 0 );
			//afTetra1ComputeMatrix = sum( afTetra1ComputeMatrix, 1 );

			//complex< T > tetra1Result;
			//afTetra1ComputeMatrix.host( &tetra1Result );
			//tetra1Result *= swgFactor1;

			//complex< T > result = tetra1Result;

			//UINT_T idTetra2 = faceTetraIndexPtr[ 2 * idFace + 1 ];
			//if( idTetra2 != -1 )
			//{			
			//	UINT_T idTetra2Vertex[ 4 ] = {
			//		tetraVertexIndexPtr[ 4 * idTetra2 ],
			//		tetraVertexIndexPtr[ 4 * idTetra2 + 1 ],
			//		tetraVertexIndexPtr[ 4 * idTetra2 + 2 ],
			//		tetraVertexIndexPtr[ 4 * idTetra2 + 3 ]
			//	};

			//	LUV::LuVector3< T >* tetra2Vertex[ 4 ] = {
			//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra2Vertex[ 0 ] ] ),
			//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra2Vertex[ 1 ] ] ),
			//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra2Vertex[ 2 ] ] ),
			//		reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra2Vertex[ 3 ] ] )
			//	};

			//	T tetra2Volume = LUV::TetrahedronVolume< T >( *tetra2Vertex[ 0 ], *tetra2Vertex[ 1 ], *tetra2Vertex[ 2 ], *tetra2Vertex[ 3 ] );

			//	T swgFactor2 = faceArea / ( 3.0 * tetra2Volume );

			//	UINT_T idSwgVertex2 = GetSwgVert( &faceVertexIndexPtr[ 3 * idFace ], &tetraVertexIndexPtr[ 4 * idTetra2 ] );

			//	LUV::LuVector3< T >* swgVertex2 = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idSwgVertex2 ] );

			//	///

			//	LUV::LuVector3c< T >* tetra2QuadFieldE = new LUV::LuVector3c< T >[ quadCount ];
			//	for( UINT_T idQuad = 0; idQuad < quadCount; ++idQuad )
			//	{
			//		tetra2QuadFieldE[ idQuad ] = incWave->FieldE(
			//			*( reinterpret_cast< LUV::LuVector3< T >* >(
			//				&tetraQuadDataPtr[ quadBlockSize * idTetra2 + 4 * idQuad + 1 ]
			//			) )
			//		);
			//	}

			//	af::array afTetra2ComputeMatrix = af::array( 3, quadCount, AF_COMPLEX_T );
			//	afTetra2ComputeMatrix.write( reinterpret_cast< T* >( tetra2QuadFieldE ), SIZEOF_CT * 3 * quadCount, afHost );

			//	delete[] tetra2QuadFieldE;

			//	af::array afTetra2QuadData = af::array( 4, quadCount, AF_FLOAT_T );
			//	afTetra2QuadData.write( &tetraQuadDataPtr[ quadBlockSize * idTetra2 ], SIZEOF_T * 4 * quadCount, afHost );

			//	afTetra2ComputeMatrix.row( 0 ) *= afTetra2QuadData.row( 1 ) - (*swgVertex2)[ 0 ];
			//	afTetra2ComputeMatrix.row( 1 ) *= afTetra2QuadData.row( 2 ) - (*swgVertex2)[ 1 ];
			//	afTetra2ComputeMatrix.row( 2 ) *= afTetra2QuadData.row( 3 ) - (*swgVertex2)[ 2 ];
			//	afTetra2ComputeMatrix = af::sum( afTetra2ComputeMatrix, 0 );
			//	afTetra2ComputeMatrix *= afTetra2QuadData.row( 0 );
			//	afTetra2ComputeMatrix = sum( afTetra2ComputeMatrix, 1 );

			//	complex< T > tetra2Result;
			//	afTetra2ComputeMatrix.host( &tetra2Result );
			//	tetra2Result *= swgFactor2;

			//	result += tetra2Result;
			//}

		//}

	//}

};

#endif