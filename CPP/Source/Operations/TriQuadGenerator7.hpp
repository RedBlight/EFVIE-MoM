#ifndef TRI_QUAD_GENERATOR7_INCLUDED
#define TRI_QUAD_GENERATOR7_INCLUDED

#include <fstream>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <utility>
#include <arrayfire.h>
#include "_LuVector.hpp"

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

	void Generate( T* quadData, const size_t* indexList, const T* vertexList, const size_t& faceCount ) const
	{
		for( size_t idf = 0; idf < faceCount; ++idf )
		{
			size_t idx1 = 3 * idf;
			size_t idx2 = idx1 + 1;
			size_t idx3 = idx1 + 2;

			size_t idv1x = 3 * indexList[ idx1 ];
			size_t idv1y = idv1x + 1;
			size_t idv1z = idv1x + 2;

			size_t idv2x = 3 * indexList[ idx2 ];
			size_t idv2y = idv2x + 1;
			size_t idv2z = idv2x + 2;

			size_t idv3x = 3 * indexList[ idx3 ];
			size_t idv3y = idv3x + 1;
			size_t idv3z = idv3x + 2;

			size_t idQuad = 4 * 7 * idf;

			LUV::LuVector< 3, T > v1( vertexList[ idv1x ], vertexList[ idv1x ], vertexList[ idv1x ] );
			LUV::LuVector< 3, T > v2( vertexList[ idv2y ], vertexList[ idv2y ], vertexList[ idv2y ] );
			LUV::LuVector< 3, T > v3( vertexList[ idv3z ], vertexList[ idv3z ], vertexList[ idv3z ] );

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

			for( size_t idq = 0; idq < 7; ++idq )
			{
				size_t idq1 = idQuad + 4 * idq;
				size_t idq2 = idq1 + 1;
				size_t idq3 = idq1 + 2;
				size_t idq4 = idq1 + 3;
				quadData[ idq1 ] = w[ idq ];
				quadData[ idq2 ] = q[ idq ][0];
				quadData[ idq3 ] = q[ idq ][1];
				quadData[ idq4 ] = q[ idq ][2]; 
			}

		}
	}

	~TriQuadGenerator7()
	{

	}

};

#endif