#ifndef TRI_QUAD_GENERATOR_7_INCLUDED
#define TRI_QUAD_GENERATOR_7_INCLUDED

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

#include <arrayfire.h>

#include <_LuVector.hpp>

using namespace std;

template< class T >
class TriQuadGenerator7
{
public:
	// barycentric coordinate factors
	T bFactor1_;
	T bFactor2_;
	T bFactor3_;
	T bFactor4_;
	T bFactor5_;

	// weight factors:
	T wFactor1_;
	T wFactor2_;
	T wFactor3_;

public:
	TriQuadGenerator7()
	{
		bFactor1_ = 1.0 / 3.0;
		bFactor2_ = ( 6.0 - sqrt( 15.0 ) ) / 21.0;
		bFactor3_ = ( 9.0 + 2.0 * sqrt( 15.0 ) ) / 21.0;
		bFactor4_ = ( 6.0 + sqrt( 15.0 ) ) / 21.0;
		bFactor5_ = ( 9.0 - 2.0 * sqrt( 15.0 ) ) / 21.0;

		wFactor1_ = 9.0 / 40.0;
		wFactor2_ = ( 155.0 - sqrt( 15.0 ) ) / 1200.0;
		wFactor3_ = ( 155.0 + sqrt( 15.0 ) ) / 1200.0;
	}
	
		//const UINT_T& tetraCount,
		//const shared_ptr< T >& vertexData,
		//const shared_ptr< UINT_T >& tetraVertexIndex,

		//shared_ptr< T >& quadData

	void Generate(
		const UINT_T& faceCount,
		const shared_ptr< T >& vertexData,
		const shared_ptr< UINT_T >& faceVertexIndex,

		shared_ptr< T >& quadData // must be preallocated

	) const
	{
		T* vertexDataPtr = vertexData.get();
		UINT_T* tetraVertexIndexPtr = faceVertexIndex.get();
		T* quadDataPtr = quadData.get();

		for( UINT_T idf = 0; idf < faceCount; ++idf )
		{
			UINT_T idx1 = 3 * idf;
			UINT_T idx2 = idx1 + 1;
			UINT_T idx3 = idx1 + 2;

			UINT_T idv1x = 3 * tetraVertexIndexPtr[ idx1 ];
			UINT_T idv1y = idv1x + 1;
			UINT_T idv1z = idv1x + 2;

			UINT_T idv2x = 3 * tetraVertexIndexPtr[ idx2 ];
			UINT_T idv2y = idv2x + 1;
			UINT_T idv2z = idv2x + 2;

			UINT_T idv3x = 3 * tetraVertexIndexPtr[ idx3 ];
			UINT_T idv3y = idv3x + 1;
			UINT_T idv3z = idv3x + 2;

			UINT_T idQuad = 4 * 7 * idf;

			LUV::LuVector< 3, T > v1( vertexDataPtr[ idv1x ], vertexDataPtr[ idv1x ], vertexDataPtr[ idv1x ] );
			LUV::LuVector< 3, T > v2( vertexDataPtr[ idv2y ], vertexDataPtr[ idv2y ], vertexDataPtr[ idv2y ] );
			LUV::LuVector< 3, T > v3( vertexDataPtr[ idv3z ], vertexDataPtr[ idv3z ], vertexDataPtr[ idv3z ] );

			LUV::LuVector< 3, T > q[7];

			q[0] = bFactor1_ * ( v1 + v2 + v3 );

			q[1] = bFactor2_ * v1 + bFactor2_ * v2 + bFactor3_ * v3;
			q[2] = bFactor2_ * v1 + bFactor3_ * v2 + bFactor2_ * v3;
			q[3] = bFactor3_ * v1 + bFactor2_ * v2 + bFactor2_ * v3;

			q[4] = bFactor4_ * v1 + bFactor4_ * v2 + bFactor5_ * v3;
			q[5] = bFactor4_ * v1 + bFactor5_ * v2 + bFactor4_ * v3;
			q[6] = bFactor5_ * v1 + bFactor4_ * v2 + bFactor4_ * v3;

			T area = LUV::Length( LUV::Cross( v2 - v1, v3 - v1 ) ) / 2.0;

			T w[7];

			w[0] = wFactor1_ * area;
			w[1] = wFactor2_ * area;
			w[2] = wFactor2_ * area;
			w[3] = wFactor2_ * area;
			w[4] = wFactor3_ * area;
			w[5] = wFactor3_ * area;
			w[6] = wFactor3_ * area;

			for( UINT_T idq = 0; idq < 7; ++idq )
			{
				UINT_T idq1 = idQuad + 4 * idq;
				UINT_T idq2 = idq1 + 1;
				UINT_T idq3 = idq1 + 2;
				UINT_T idq4 = idq1 + 3;
				quadDataPtr[ idq1 ] = w[ idq ];
				quadDataPtr[ idq2 ] = q[ idq ][0];
				quadDataPtr[ idq3 ] = q[ idq ][1];
				quadDataPtr[ idq4 ] = q[ idq ][2]; 
			}

		}
	}

	~TriQuadGenerator7()
	{

	}

};

#endif