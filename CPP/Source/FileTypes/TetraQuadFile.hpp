#ifndef TETRA_QUAD_FILE_INCLUDED
#define TETRA_QUAD_FILE_INCLUDED

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

using namespace std;

/*

.tetraquad file:
[0] tetrahedron count
[1] quad count //= 64 for now
[2 ... A - 1]:

w1 x1 y1 z1 |
....        | -> for 1 tetrahedron, FLOAT_T, 4 * [2] cell total
wN xN yN zN |

repeat for all tetrahedra

A = 2 + 4 * [2] * [0]

total size = SIZEOF_T * ( 2 + 4 * [0] * [2] ) byte

*/

// always double
template< class T >
class TetraQuadFile
{
public:
	bool init_;
	UINT_T tetraCount_;
	UINT_T quadCount_;
	shared_ptr< T > quadData_;

public:

	TetraQuadFile() :
		init_( false ),
		tetraCount_( 0 ),
		quadCount_( 0 ),
		quadData_()
	{

	}

	~TetraQuadFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			tetraCount_ = 0;
			quadCount_ = 0;
			quadData_.reset();
		}
	}

	void Initialize( const UINT_T& tetraCount, const UINT_T& quadCount )
	{
		Reset();

		tetraCount_ = tetraCount;
		quadCount_ = quadCount;

		quadData_.reset( new T[ 4 * quadCount_ * tetraCount_  ], []( T* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	void Initialize( const UINT_T& tetraCount, const UINT_T& quadCount, const shared_ptr< T >& quadData )
	{
		Reset();

		tetraCount_ = tetraCount;
		quadCount_ = quadCount;
		quadData_ = quadData;

		init_ = true;
	}

	bool Load( const string& filePath )
	{
		Reset();

		fstream quadFile( filePath, ios::in | ios::binary );

		if( !quadFile.good() )
		{
			quadFile.close();
			return false;
		}

		quadFile.read( ( char* )&tetraCount_, SIZEOF_T );
		quadFile.read( ( char* )&quadCount_, SIZEOF_T );

		quadData_.reset( new T[ 4 * quadCount_ * tetraCount_ ], []( T* ptr ){ delete[] ptr; } );

		quadFile.read( ( char* )( quadData_.get() ), SIZEOF_T * 4 * quadCount_ * tetraCount_ );

		quadFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream quadFile( filePath, ios::trunc | ios::out | ios::binary );
		
		if( !quadFile.good() )
		{
			quadFile.close();
			return false;
		}

		quadFile.write( ( char* )&tetraCount_, SIZEOF_T );
		quadFile.write( ( char* )&quadCount_, SIZEOF_T );

		quadFile.write( ( char* )( quadData_.get() ), SIZEOF_T * 4 * quadCount_ * tetraCount_ );

		quadFile.close();

		return true;
	}

};

#endif