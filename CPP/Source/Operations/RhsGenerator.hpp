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

#include <arrayfire.h>

#include <_IncWave.hpp>

using namespace std;

template< class T >
class RhsGenerator
{

public:
	RhsGenerator()
	{

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

	void Generate(
		const UINT_T& faceCount,
		const shared_ptr< T >& vertexData,
		const shared_ptr< UINT_T >& tetraVertexIndex,
		const shared_ptr< UINT_T >& faceVertexIndex,
		const shared_ptr< UINT_T >& faceTetraIndex,
		const shared_ptr< UINT_T >& tetraFaceIndex,
		const UINT_T& quadCount,
		const shared_ptr< T >& tetraQuadData,
		const IncWave< T >* incWave,
		shared_ptr< complex< T > >& rhsData // preallocated
	) const
	{
		T* vertexDataPtr = vertexData.get();
		UINT_T* tetraVertexIndexPtr = tetraVertexIndex.get();
		UINT_T* faceVertexIndexPtr = faceVertexIndex.get();
		UINT_T* faceTetraIndexPtr = faceTetraIndex.get();
		UINT_T* tetraFaceIndexPtr = tetraFaceIndex.get();
		T* tetraQuadDataPtr = tetraQuadData.get();
		complex< T >* rhsDataPtr = rhsData.get();

		UINT_T quadBlockSize = 4 * quadCount;

		for( UINT_T idFace = 0; idFace < faceCount; ++idFace )
		{
			UINT_T idFaceVertex[ 3 ] = {
				faceVertexIndexPtr[ 3 * idFace ],
				faceVertexIndexPtr[ 3 * idFace + 1 ],
				faceVertexIndexPtr[ 3 * idFace + 2 ]
			};

			LUV::LuVector3< T >* faceVertex[ 3 ] = {
				reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idFaceVertex[ 0 ] ] ),
				reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idFaceVertex[ 1 ] ] ),
				reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idFaceVertex[ 2 ] ] )
			};

			T faceArea = LUV::TriangleArea( *faceVertex[ 0 ], *faceVertex[ 1 ], *faceVertex[ 2 ] );
			
			//////

			UINT_T idTetra1 = faceTetraIndexPtr[ 2 * idFace ];
			
			UINT_T idTetra1Vertex[ 4 ] = {
				tetraVertexIndexPtr[ 4 * idTetra1 ],
				tetraVertexIndexPtr[ 4 * idTetra1 + 1 ],
				tetraVertexIndexPtr[ 4 * idTetra1 + 2 ],
				tetraVertexIndexPtr[ 4 * idTetra1 + 3 ]
			};

			LUV::LuVector3< T >* tetra1Vertex[ 4 ] = {
				reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra1Vertex[ 0 ] ] ),
				reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra1Vertex[ 1 ] ] ),
				reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra1Vertex[ 2 ] ] ),
				reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra1Vertex[ 3 ] ] )
			};

			T tetra1Volume = LUV::TetrahedronVolume< T >( *tetra1Vertex[ 0 ], *tetra1Vertex[ 1 ], *tetra1Vertex[ 2 ], *tetra1Vertex[ 3 ] );

			T swgFactor1 = faceArea / ( 3.0 * tetra1Volume );

			UINT_T idSwgVertex1 = GetSwgVert( &faceVertexIndexPtr[ 3 * idFace ], &tetraVertexIndexPtr[ 4 * idTetra1 ] );

			LUV::LuVector3< T >* swgVertex1 = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idSwgVertex1 ] );

			///

			LUV::LuVector3c< T >* tetra1QuadFieldE = new LUV::LuVector3c< T >[ quadCount ];
			for( UINT_T idQuad = 0; idQuad < quadCount; ++idQuad )
			{
				tetra1QuadFieldE[ idQuad ] = incWave->FieldE(
					*( reinterpret_cast< LUV::LuVector3< T >* >(
						&tetraQuadDataPtr[ quadBlockSize * idTetra1 + 4 * idQuad + 1 ]
					) )
				);
			}

			af::array afTetra1ComputeMatrix = af::array( 3, quadCount, AF_COMPLEX_T );
			afTetra1ComputeMatrix.write( reinterpret_cast< T* >( tetra1QuadFieldE ), SIZEOF_CT * 3 * quadCount, afHost );

			delete[] tetra1QuadFieldE;

			af::array afTetra1QuadData = af::array( 4, quadCount, AF_FLOAT_T );
			afTetra1QuadData.write( &tetraQuadDataPtr[ quadBlockSize * idTetra1 ], SIZEOF_T * 4 * quadCount, afHost );

			afTetra1ComputeMatrix.row( 0 ) *= afTetra1QuadData.row( 1 ) - (*swgVertex1)[ 0 ];
			afTetra1ComputeMatrix.row( 1 ) *= afTetra1QuadData.row( 2 ) - (*swgVertex1)[ 1 ];
			afTetra1ComputeMatrix.row( 2 ) *= afTetra1QuadData.row( 3 ) - (*swgVertex1)[ 2 ];
			afTetra1ComputeMatrix = af::sum( afTetra1ComputeMatrix, 0 );
			afTetra1ComputeMatrix *= afTetra1QuadData.row( 0 );
			afTetra1ComputeMatrix = sum( afTetra1ComputeMatrix, 1 );

			complex< T > tetra1Result;
			afTetra1ComputeMatrix.host( &tetra1Result );
			tetra1Result *= swgFactor1;

			complex< T > result = tetra1Result;

			UINT_T idTetra2 = faceTetraIndexPtr[ 2 * idFace + 1 ];
			if( idTetra2 != -1 )
			{			
				UINT_T idTetra2Vertex[ 4 ] = {
					tetraVertexIndexPtr[ 4 * idTetra2 ],
					tetraVertexIndexPtr[ 4 * idTetra2 + 1 ],
					tetraVertexIndexPtr[ 4 * idTetra2 + 2 ],
					tetraVertexIndexPtr[ 4 * idTetra2 + 3 ]
				};

				LUV::LuVector3< T >* tetra2Vertex[ 4 ] = {
					reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra2Vertex[ 0 ] ] ),
					reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra2Vertex[ 1 ] ] ),
					reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra2Vertex[ 2 ] ] ),
					reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idTetra2Vertex[ 3 ] ] )
				};

				T tetra2Volume = LUV::TetrahedronVolume< T >( *tetra2Vertex[ 0 ], *tetra2Vertex[ 1 ], *tetra2Vertex[ 2 ], *tetra2Vertex[ 3 ] );

				T swgFactor2 = faceArea / ( 3.0 * tetra2Volume );

				UINT_T idSwgVertex2 = GetSwgVert( &faceVertexIndexPtr[ 3 * idFace ], &tetraVertexIndexPtr[ 4 * idTetra2 ] );

				LUV::LuVector3< T >* swgVertex2 = reinterpret_cast< LUV::LuVector3< T >* >( &vertexDataPtr[ 3 * idSwgVertex2 ] );

				///

				LUV::LuVector3c< T >* tetra2QuadFieldE = new LUV::LuVector3c< T >[ quadCount ];
				for( UINT_T idQuad = 0; idQuad < quadCount; ++idQuad )
				{
					tetra2QuadFieldE[ idQuad ] = incWave->FieldE(
						*( reinterpret_cast< LUV::LuVector3< T >* >(
							&tetraQuadDataPtr[ quadBlockSize * idTetra2 + 4 * idQuad + 1 ]
						) )
					);
				}

				af::array afTetra2ComputeMatrix = af::array( 3, quadCount, AF_COMPLEX_T );
				afTetra2ComputeMatrix.write( reinterpret_cast< T* >( tetra2QuadFieldE ), SIZEOF_CT * 3 * quadCount, afHost );

				delete[] tetra2QuadFieldE;

				af::array afTetra2QuadData = af::array( 4, quadCount, AF_FLOAT_T );
				afTetra2QuadData.write( &tetraQuadDataPtr[ quadBlockSize * idTetra2 ], SIZEOF_T * 4 * quadCount, afHost );

				afTetra2ComputeMatrix.row( 0 ) *= afTetra2QuadData.row( 1 ) - (*swgVertex2)[ 0 ];
				afTetra2ComputeMatrix.row( 1 ) *= afTetra2QuadData.row( 2 ) - (*swgVertex2)[ 1 ];
				afTetra2ComputeMatrix.row( 2 ) *= afTetra2QuadData.row( 3 ) - (*swgVertex2)[ 2 ];
				afTetra2ComputeMatrix = af::sum( afTetra2ComputeMatrix, 0 );
				afTetra2ComputeMatrix *= afTetra2QuadData.row( 0 );
				afTetra2ComputeMatrix = sum( afTetra2ComputeMatrix, 1 );

				complex< T > tetra2Result;
				afTetra2ComputeMatrix.host( &tetra2Result );
				tetra2Result *= swgFactor2;

				result += tetra2Result;
			}

			rhsDataPtr[ idFace ] = result;
		}

	}

};

#endif