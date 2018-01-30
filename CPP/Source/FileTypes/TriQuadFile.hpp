#ifndef TRI_QUAD_FILE_INCLUDED
#define TRI_QUAD_FILE_INCLUDED

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

.triquad file:
[0] face count
[1] quad count = 7;
[2 ... A - 1]:

w1 x1 y1 z1 |
....        | -> for 1 face, all double, 4 * [1] cell total
wN xN yN zN |

repeat for all faces

A = 2 + 4 * [1] * [0]

total size = 8 * ( 2 + 4 * [0] * [1] ) byte

*/

// always double
template< class T >
class TriQuadFile
{
public:
	bool init_;
	size_t faceCount_;
	size_t quadCount_;
	T* quadData_;

public:

	TriQuadFile() :
		init_( false ),
		faceCount_( 0 ),
		quadCount_( 0 ),
		quadData_( nullptr )
	{

	}

	~TriQuadFile()
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
			faceCount_ = 0;
			quadCount_ = 0;
			delete[] quadData_;
			quadData_ = nullptr;
		}
	}

	void Initialise( const size_t& faceCount, const size_t& quadCount )
	{
		if( !init_ )
		{
			faceCount_ = faceCount;
			quadCount_ = quadCount;
			quadData_ = new T[ 4 * quadCount_ * faceCount_ ];
			init_ = true;
		}
	}

	bool Load_triquad( const string& filePath )
	{
		Reset();

		fstream quadFile( filePath, ios::in | ios::binary );

		if( !quadFile.good() )
		{
			quadFile.close();
			return false;
		}

		quadFile.read( ( char* )&faceCount_, 8 );
		quadFile.read( ( char* )&quadCount_, 8 );

		quadData_ = new T[ 4 * quadCount_ * faceCount_ ];

		quadFile.read( ( char* )quadData_, 8 * 4 * quadCount_ * faceCount_ );

		quadFile.close();

		init_ = true;

		return true;
	}

	bool Save_triquad( const string& filePath )
	{
		fstream quadFile( filePath, ios::trunc | ios::out | ios::binary );
		
		if( !quadFile.good() )
		{
			quadFile.close();
			return false;
		}

		quadFile.write( ( char* )&faceCount_, 8 );
		quadFile.write( ( char* )&quadCount_, 8 );

		quadFile.write( ( char* )quadData_, 8 * 4 * quadCount_ * faceCount_ );

		quadFile.close();

		return true;
	}

};

#endif