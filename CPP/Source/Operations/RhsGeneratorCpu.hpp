#ifndef RHS_GENERATOR_CPU_INCLUDED
#define RHS_GENERATOR_CPU_INCLUDED

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
#include <_IncWave.hpp>

using namespace std;

template< class T >
class RhsGeneratorCpu
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
	RhsGeneratorCpu(
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

	~RhsGeneratorCpu()
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
		UINT_T idTetra4 = 4 * idTetra;
		UINT_T* idTetraVertex = &tetraVertexIndexPtr_[ idTetra4 ];

		LUV::LuVector3< T >* tetraVertex[ 4 ] = {
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertex[ 0 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertex[ 1 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertex[ 2 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idTetraVertex[ 3 ] ] )
		};

		T tetraVolume = LUV::TetrahedronVolume( *tetraVertex[ 0 ], *tetraVertex[ 1 ], *tetraVertex[ 2 ], *tetraVertex[ 3 ] );

		UINT_T idSwgVertex = GetSwgVert( &faceVertexIndexPtr_[ 3 * idFace ], &tetraVertexIndexPtr_[ 4 * idTetra ] );

		LUV::LuVector3< T >* swgVertex = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idSwgVertex ] );

		complex< T > result( 0, 0 );

		UINT_T addressStartQ = quadBlockSize_ * idTetra;

		for( UINT_T idq = 0; idq < quadCount_; ++idq )
		{
			UINT_T addressQ = addressStartQ + 4 * idq;
			T* magW =  &tetraQuadDataPtr_[ addressQ ];
			LUV::LuVector3< T >* vecQ = reinterpret_cast< LUV::LuVector3< T >* >( &tetraQuadDataPtr_[ addressQ + 1 ] );

			result += LUV::Dot( incWave_->FieldE( *vecQ ), *vecQ - *swgVertex ) * *magW;
		}

		return result / tetraVolume;






		//LUV::LuVector3c< T >* tetraQuadFieldE = new LUV::LuVector3c< T >[ quadCount_ ];
		//for( UINT_T idQuad = 0; idQuad < quadCount_; ++idQuad )
		//{
		//	tetraQuadFieldE[ idQuad ] = incWave_->FieldE(
		//		*( reinterpret_cast< LUV::LuVector3< T >* >(
		//			&tetraQuadDataPtr_[ quadBlockSize_ * idTetra + 4 * idQuad + 1 ]
		//		) )
		//	);
		//}

		//af::array afTetraComputeMatrix = af::array( 3, quadCount_, AF_COMPLEX_T );
		//afTetraComputeMatrix.write( reinterpret_cast< T* >( tetraQuadFieldE ), SIZEOF_CT * 3 * quadCount_, afHost );

		//delete[] tetraQuadFieldE;

		//af::array afTetraQuadData = af::array( 4, quadCount_, AF_FLOAT_T );
		//afTetraQuadData.write( &tetraQuadDataPtr_[ quadBlockSize_ * idTetra ], SIZEOF_T * 4 * quadCount_, afHost );

		//afTetraComputeMatrix.row( 0 ) *= afTetraQuadData.row( 1 ) - (*swgVertex)[ 0 ];
		//afTetraComputeMatrix.row( 1 ) *= afTetraQuadData.row( 2 ) - (*swgVertex)[ 1 ];
		//afTetraComputeMatrix.row( 2 ) *= afTetraQuadData.row( 3 ) - (*swgVertex)[ 2 ];
		//afTetraComputeMatrix = af::sum( afTetraComputeMatrix, 0 );
		//afTetraComputeMatrix *= afTetraQuadData.row( 0 );
		//afTetraComputeMatrix = sum( afTetraComputeMatrix, 1 );

		//complex< T > tetraResult;
		//afTetraComputeMatrix.host( &tetraResult );
		//tetraResult /= tetraVolume;

		//return tetraResult;
	}

	inline void GenerateSingleElem( const UINT_T& idFace ) const
	{
		UINT_T idFace3 = 3 * idFace;
		UINT_T* idFaceVertex = &faceVertexIndexPtr_[ idFace3 ];

		UINT_T idFace2 = 2 * idFace;
		UINT_T* idTetra = &faceTetraIndexPtr_[ idFace2 ];

		LUV::LuVector3< T >* faceVertex[ 3 ] = {
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 0 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 1 ] ] ),
			reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr_[ 3 * idFaceVertex[ 2 ] ] )
		};

		T faceArea = LUV::TriangleArea( *faceVertex[ 0 ], *faceVertex[ 1 ], *faceVertex[ 2 ] );

		complex< T > result;
			
		result = TetrahedralIntegral( idFace, idTetra[ 0 ] );

		if( idTetra[ 1 ] != -1 )
		{
			result -= TetrahedralIntegral( idFace, idTetra[ 1 ] );
		}

		result *= faceArea / 3.0; // why did I do this? godd dammit bitch.
		
		rhsDataPtr_[ idFace ] = result;
	}

	void PartialGenerate( const UINT_T& idFaceStart, const UINT_T& idFaceEnd )
	{
		//cout << "Thread: " << idFaceStart << " - " << idFaceEnd << endl;

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

};

#endif