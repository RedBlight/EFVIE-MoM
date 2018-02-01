#ifndef TRI_QUAD_FILE_INCLUDED
#define TRI_QUAD_FILE_INCLUDED

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

.triquad file:
[0] face count
[1] quad count = 7;
[2 ... A - 1]:

w1 x1 y1 z1 |
....        | -> for 1 face, all double, 4 * [1] cell total
wN xN yN zN |

repeat for all faces

A = 2 + 4 * [1] * [0]

total size = SIZEOF_T * ( 2 + 4 * [0] * [1] ) byte

*/

// always double
template< class T >
class TriQuadFile
{
public:
	bool init_;
	UINT_T faceCount_;
	UINT_T quadCount_;
	shared_ptr< T > quadData_;

public:

	TriQuadFile() :
		init_( false ),
		faceCount_( 0 ),
		quadCount_( 0 ),
		quadData_()
	{

	}

	~TriQuadFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			faceCount_ = 0;
			quadCount_ = 0;
			quadData_.reset();
		}
	}

	void Initialize( const UINT_T& faceCount, const UINT_T& quadCount )
	{
		Reset();

		faceCount_ = faceCount;
		quadCount_ = quadCount;

		quadData_.reset( new T[ 4 * quadCount_ * faceCount_  ], []( T* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	void Initialize( const UINT_T& faceCount, const UINT_T& quadCount, const shared_ptr< T >& quadData )
	{
		Reset();

		faceCount_ = faceCount;
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

		quadFile.read( ( char* )&faceCount_, SIZEOF_T );
		quadFile.read( ( char* )&quadCount_, SIZEOF_T );

		quadData_.reset( new T[ 4 * quadCount_ * faceCount_ ], []( T* ptr ){ delete[] ptr; } );

		quadFile.read( ( char* )( quadData_.get() ), SIZEOF_T * 4 * quadCount_ * faceCount_ );

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

		quadFile.write( ( char* )&faceCount_, SIZEOF_T );
		quadFile.write( ( char* )&quadCount_, SIZEOF_T );

		quadFile.write( ( char* )( quadData_.get() ), SIZEOF_T * 4 * quadCount_ * faceCount_ );

		quadFile.close();

		return true;
	}

};

#endif