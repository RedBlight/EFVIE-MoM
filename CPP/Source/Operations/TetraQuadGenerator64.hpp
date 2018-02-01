#ifndef TETRA_QUAD_GENERATOR_64_INCLUDED
#define TETRA_QUAD_GENERATOR_64_INCLUDED

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

using namespace std;

/*
C++ implementation of tetraquad.m file, where N=4, so there N^3 = 64 quadrature points for each tetrahedra.
*/

template< class T >
class TetraQuadGenerator64
{
public:
	af::array q1_; // these are taken directly from tetraquad.m file
	af::array q2_;
	af::array q3_;
	af::array w1_;
	af::array w2_;
	af::array w3_;

	af::array Q1_;
	af::array Q2_;
	af::array Q3_;
	af::array Wr_;

	af::array Qx_;
	af::array Qy_;
	af::array Qz_;

	af::array C1_; // column of ones
	af::array diffMat_;

public:
	TetraQuadGenerator64() :
		q1_( 4, AF_FLOAT_T ),
		q2_( 4, AF_FLOAT_T ),
		q3_( 4, AF_FLOAT_T ),
		w1_( 4, AF_FLOAT_T ),
		w2_( 4, AF_FLOAT_T ),
		w3_( 4, AF_FLOAT_T ),

		Q1_( 64, AF_FLOAT_T ),
		Q2_( 64, AF_FLOAT_T ),
		Q3_( 64, AF_FLOAT_T ),
		Wr_( 64, AF_FLOAT_T ),

		Qx_( 64, AF_FLOAT_T ),
		Qy_( 64, AF_FLOAT_T ),
		Qz_( 64, AF_FLOAT_T ),

		C1_( 64, AF_FLOAT_T ),

		diffMat_( 4, 4, AF_FLOAT_T )
	{
		T q1[4] = {
			0.204148582103227,
			0.482952704895632,
			0.761399262448138,
			0.951499450553003
		};

		T q2[4] = {
			0.139759864343781,
			0.416409567631083,
			0.723156986361876,
			0.942895803885482
		}; 

		T q3[4] = {
			0.069431844202974,
			0.330009478207572,
			0.669990521792428,
			0.930568155797026
		};

		T w1[4] = {
			0.010352240749918,
			0.068633887172923,
			0.143458789799214,
			0.110888415611278
		};

		T w2[4] = {
			0.031180970950008,
			0.129847547608232,
			0.203464568010271,
			0.135506913431488
		}; 

		T w3[4] = {
			0.173927422568727,
			0.326072577431273,
			0.326072577431273,
			0.173927422568727
		};

		T diffMat[16] = { // lets not forget that af::array is column major.
			1,	-1,	-1,	-1,
			0,	1,	0,	0,
			0,	0,	1,	0,
			0,	0,	0,	1
		};

		q1_.write( q1, SIZEOF_T * 4 );
		q2_.write( q2, SIZEOF_T * 4 );
		q3_.write( q3, SIZEOF_T * 4 );
		w1_.write( w1, SIZEOF_T * 4 );
		w2_.write( w2, SIZEOF_T * 4 );
		w3_.write( w3, SIZEOF_T * 4 );

		Q1_ = af::tile( af::flat( af::tile( q1_.T(), 4 ) ), 4 );
		Q2_ = af::tile( af::tile( q2_, 4 ), 4 );
		Q3_ = af::flat( af::tile( af::tile( q3_.T(), 4 ), 4 ) );

		Wr_ = af::tile( af::flat( af::tile( w1_.T(), 4 ) ), 4 )
			* af::tile( af::tile( w2_, 4 ), 4 )
			* af::flat( af::tile( af::tile( w3_.T(), 4 ), 4 ) );

		Qx_ = 1.0 - Q1_;
		Qy_ = ( 1.0 - Q2_ ) * Q1_;
		Qz_ = Q1_ * Q2_ * Q3_;

		C1_ = af::constant( 1.0, 64, AF_FLOAT_T );

		diffMat_.write( diffMat, SIZEOF_T * 16 );
	}

	~TetraQuadGenerator64()
	{

	}

	void Generate(
		const UINT_T& tetraCount,
		const shared_ptr< T >& vertexData,
		const shared_ptr< UINT_T >& tetraVertexIndex,

		shared_ptr< T >& quadData // must be preallocated

	) const
	{
		T* vertexDataPtr = vertexData.get();
		UINT_T* tetraVertexIndexPtr = tetraVertexIndex.get();
		T* quadDataPtr = quadData.get();

		for( UINT_T idt = 0; idt < tetraCount; ++idt )
		{
			UINT_T idx1 = 4 * idt;
			UINT_T idx2 = idx1 + 1;
			UINT_T idx3 = idx1 + 2;
			UINT_T idx4 = idx1 + 3;

			UINT_T idv1x = 3 * tetraVertexIndexPtr[ idx1 ];
			UINT_T idv1y = idv1x + 1;
			UINT_T idv1z = idv1x + 2;

			UINT_T idv2x = 3 * tetraVertexIndexPtr[ idx2 ];
			UINT_T idv2y = idv2x + 1;
			UINT_T idv2z = idv2x + 2;

			UINT_T idv3x = 3 * tetraVertexIndexPtr[ idx3 ];
			UINT_T idv3y = idv3x + 1;
			UINT_T idv3z = idv3x + 2;

			UINT_T idv4x = 3 * tetraVertexIndexPtr[ idx4 ];
			UINT_T idv4y = idv4x + 1;
			UINT_T idv4z = idv4x + 2;

			UINT_T idQuad = 4 * 64 * idt;

			T vertMat[12] = { // col major
				vertexDataPtr[ idv1x ], vertexDataPtr[ idv2x ], vertexDataPtr[ idv3x ], vertexDataPtr[ idv4x ],
				vertexDataPtr[ idv1y ], vertexDataPtr[ idv2y ], vertexDataPtr[ idv3y ], vertexDataPtr[ idv4y ],
				vertexDataPtr[ idv1z ], vertexDataPtr[ idv2z ], vertexDataPtr[ idv3z ], vertexDataPtr[ idv4z ]
			};

			af::array vertMatAf( 4, 3, AF_FLOAT_T );
			vertMatAf.write( vertMat, SIZEOF_T * 12 );

			af::array vertDiffX = af::matmul( diffMat_, vertMatAf );
			af::array W = abs( af::det< FLOAT_T >( vertDiffX.rows( 1, 3 ) ) ) * Wr_;
			af::array WXYZ = af::join( 1, W, af::matmul( af::join( 1, C1_, Qx_, Qy_, Qz_ ), vertDiffX ) );

			// fix the unnecessary copy here
			T* quadDataHost = transpose( WXYZ ).host< T >();
			copy( quadDataHost, quadDataHost + 4 * 64, &( quadDataPtr[ idQuad ] ) );
			delete[] quadDataHost;
		}
	}

};

#endif