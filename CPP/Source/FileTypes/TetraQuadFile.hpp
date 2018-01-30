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

using namespace std;

/*

.tetraquad file:
[0] tetrahedron count
[1] quad level = 4,
[2] quad count = level^3 = 64;
[3 ... A - 1]:

w1 x1 y1 z1 |
....        | -> for 1 tetrahedron, all double, 4 * [2] cell total
wN xN yN zN |

repeat for all tetrahedra

A = 3 + 4 * [2] * [0]

total size = 8 * ( 3 + 4 * [0] * [2] ) byte

*/

// always double
template< class T >
class TetraQuadFile
{
public:
	bool init_;
	UINT_T tetraCount_;
	UINT_T quadLevel_;
	UINT_T quadCount_;
	T* quadData_;

public:

	TetraQuadFile() :
		init_( false ),
		tetraCount_( 0 ),
		quadLevel_( 0 ),
		quadCount_( 0 ),
		quadData_( nullptr )
	{

	}

	~TetraQuadFile()
	{
		if( init_ )
		{
			delete[] quadData_;
		}
	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			tetraCount_ = 0;
			quadLevel_ = 0;
			quadCount_ = 0;
			delete[] quadData_;
			quadData_ = nullptr;
		}
	}

	void Initialise( const UINT_T& tetraCount, const UINT_T& quadLevel )
	{
		if( !init_ )
		{
			tetraCount_ = tetraCount;
			quadLevel_ = quadLevel;
			quadCount_ = quadLevel * quadLevel * quadLevel;
			quadData_ = new T[ 4 * quadCount_ * tetraCount_ ];
			init_ = true;
		}
	}

	bool Load_tetraquad( const string& filePath )
	{
		Reset();

		fstream quadFile( filePath, ios::in | ios::binary );

		if( !quadFile.good() )
		{
			quadFile.close();
			return false;
		}

		quadFile.read( ( char* )&tetraCount_, SIZEOF_T );
		quadFile.read( ( char* )&quadLevel_, SIZEOF_T );
		quadFile.read( ( char* )&quadCount_, SIZEOF_T );

		quadData_ = new T[ 4 * quadCount_ * tetraCount_ ];

		quadFile.read( ( char* )quadData_, SIZEOF_T * 4 * quadCount_ * tetraCount_ );

		quadFile.close();

		init_ = true;

		return true;
	}

	bool Save_tetraquad( const string& filePath )
	{
		fstream quadFile( filePath, ios::trunc | ios::out | ios::binary );
		
		if( !quadFile.good() )
		{
			quadFile.close();
			return false;
		}

		quadFile.write( ( char* )&tetraCount_, SIZEOF_T );
		quadFile.write( ( char* )&quadLevel_, SIZEOF_T );
		quadFile.write( ( char* )&quadCount_, SIZEOF_T );

		quadFile.write( ( char* )quadData_, SIZEOF_T * 4 * quadCount_ * tetraCount_ );

		quadFile.close();

		return true;
	}

};

#endif