#ifndef TETRA_QUAD_GENERATOR64_INCLUDED
#define TETRA_QUAD_GENERATOR64_INCLUDED

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
	
	// precalculated values end here

	//af::array vertMat_;
	//af::array vertDiffX_;
	//
	//af::array W_;

	//af::array WXYZ_; // transpose and generate a host pointer

public:
	TetraQuadGenerator64() :
		q1_( 4, f64 ),
		q2_( 4, f64 ),
		q3_( 4, f64 ),
		w1_( 4, f64 ),
		w2_( 4, f64 ),
		w3_( 4, f64 ),

		Q1_( 64, f64 ),
		Q2_( 64, f64 ),
		Q3_( 64, f64 ),
		Wr_( 64, f64 ),

		Qx_( 64, f64 ),
		Qy_( 64, f64 ),
		Qz_( 64, f64 ),

		C1_( 64, f64 ),

		diffMat_( 4, 4, f64 )

		//vertMat_( 4, 3, f64 ),
		//vertDiffX_( 4, 3, f64 ),

		//W_( 64, f64 ),
		//
		//WXYZ_( 64, 4, f64 )
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

		q1_.write( q1, 8 * 4 );
		q2_.write( q2, 8 * 4 );
		q3_.write( q3, 8 * 4 );
		w1_.write( w1, 8 * 4 );
		w2_.write( w2, 8 * 4 );
		w3_.write( w3, 8 * 4 );

		Q1_ = af::tile( af::flat( af::tile( q1_.T(), 4 ) ), 4 );
		Q2_ = af::tile( af::tile( q2_, 4 ), 4 );
		Q3_ = af::flat( af::tile( af::tile( q3_.T(), 4 ), 4 ) );

		Wr_ = af::tile( af::flat( af::tile( w1_.T(), 4 ) ), 4 )
			* af::tile( af::tile( w2_, 4 ), 4 )
			* af::flat( af::tile( af::tile( w3_.T(), 4 ), 4 ) );

		Qx_ = 1.0 - Q1_;
		Qy_ = ( 1.0 - Q2_ ) * Q1_;
		Qz_ = Q1_ * Q2_ * Q3_;

		C1_ = af::constant( 1.0, 64, f64 );

		diffMat_.write( diffMat, 8 * 16 );
	}

	~TetraQuadGenerator64()
	{

	}

	void Generate( T* quadData, const size_t* indexList, const T* vertexList, const size_t& tetraCount ) const
	{
		for( size_t idt = 0; idt < tetraCount; ++idt )
		{
			size_t idx1 = 4 * idt;
			size_t idx2 = idx1 + 1;
			size_t idx3 = idx1 + 2;
			size_t idx4 = idx1 + 3;

			size_t idv1x = 3 * indexList[ idx1 ];
			size_t idv1y = idv1x + 1;
			size_t idv1z = idv1x + 2;

			size_t idv2x = 3 * indexList[ idx2 ];
			size_t idv2y = idv2x + 1;
			size_t idv2z = idv2x + 2;

			size_t idv3x = 3 * indexList[ idx3 ];
			size_t idv3y = idv3x + 1;
			size_t idv3z = idv3x + 2;

			size_t idv4x = 3 * indexList[ idx4 ];
			size_t idv4y = idv4x + 1;
			size_t idv4z = idv4x + 2;

			size_t idQuad = 4 * 64 * idt;

			T vertMat[12] = { // col major
				vertexList[ idv1x ], vertexList[ idv2x ], vertexList[ idv3x ], vertexList[ idv4x ],
				vertexList[ idv1y ], vertexList[ idv2y ], vertexList[ idv3y ], vertexList[ idv4y ],
				vertexList[ idv1z ], vertexList[ idv2z ], vertexList[ idv3z ], vertexList[ idv4z ]
			};

			af::array vertMat_( 4, 3, f64 );
			vertMat_.write( vertMat, 8 * 12 );

			af::array vertDiffX_ = af::matmul( diffMat_, vertMat_ );
			af::array W_ = abs( af::det< double >( vertDiffX_.rows( 1, 3 ) ) ) * Wr_;
			af::array WXYZ_ = af::join( 1, W_, af::matmul( af::join( 1, C1_, Qx_, Qy_, Qz_ ), vertDiffX_ ) );

			shared_ptr< T > hostQuadData( transpose( WXYZ_ ).host< T >() );

			copy( hostQuadData, hostQuadData + 4 * 64, &( quadData[ idQuad ] ) );

		}
	}

};

#endif