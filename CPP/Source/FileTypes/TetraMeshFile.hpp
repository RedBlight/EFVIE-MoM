#ifndef TETRA_MESH_FILE_INCLUDED
#define TETRA_MESH_FILE_INCLUDED

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

.tetramesh file:
[0] = vertex count (uint)
[1] = tetrahedron count (uint)
[2 ... A-1] = x y z (float) X 3 X [0] -> vertex coords
[A ... B-1] = v1 v2 v3 v4 (uint) X 4 X [1] -> tetrahedron vertex indices

A = 2 + [0]*3
B = A + [1]*4

*/

template< class T >
class TetraMeshFile
{
public:
	bool init_;
	UINT_T vertexCount_;
	UINT_T tetraCount_;
	shared_ptr< T > vertexData_;
	shared_ptr< UINT_T > tetraVertexIndex_;

public:

	TetraMeshFile() :
		init_( false ),
		vertexCount_( 0 ),
		tetraCount_( 0 ),
		vertexData_(),
		tetraVertexIndex_()
	{

	}

	~TetraMeshFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			vertexCount_ = 0;
			tetraCount_ = 0;
			vertexData_.reset();
			tetraVertexIndex_.reset();
		}
	}

	void Initialize( const UINT_T& vertexCount, const UINT_T& tetraCount )
	{
		Reset();

		vertexCount_ = vertexCount;
		tetraCount_ = tetraCount;

		vertexData_.reset( new T[ 3 * vertexCount_ ], []( T* ptr ){ delete[] ptr; } );
		tetraVertexIndex_.reset( new UINT_T[ 4 * tetraCount_ ], []( UINT_T* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	
	void Initialize( const UINT_T& vertexCount, const UINT_T& tetraCount, const shared_ptr< T >& vertexData, const shared_ptr< UINT_T >& tetraVertexIndex )
	{
		Reset();

		vertexCount_ = vertexCount;
		tetraCount_ = tetraCount;
		vertexData_ = vertexData;
		tetraVertexIndex_ = tetraVertexIndex;

		init_ = true;
	}

	bool Load( const string& filePath )
	{
		Reset();

		fstream meshFile( filePath, ios::in | ios::binary );

		if( !meshFile.good() )
		{
			meshFile.close();
			return false;
		}

		meshFile.read( ( char* )&vertexCount_, SIZEOF_T );
		meshFile.read( ( char* )&tetraCount_, SIZEOF_T );

		vertexData_.reset( new T[ 3 * vertexCount_ ], []( T* ptr ){ delete[] ptr; } );
		tetraVertexIndex_.reset( new UINT_T[ 4 * tetraCount_ ], []( UINT_T* ptr ){ delete[] ptr; } );

		meshFile.read( ( char* )( vertexData_.get() ), SIZEOF_T * 3 * vertexCount_ );
		meshFile.read( ( char* )( tetraVertexIndex_.get() ), SIZEOF_T * 4 * tetraCount_ );

		meshFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream meshFile( filePath, ios::trunc | ios::out | ios::binary );

		if( !meshFile.good() )
		{
			meshFile.close();
			return false;
		}

		meshFile.write( ( char* )&vertexCount_, SIZEOF_T );
		meshFile.write( ( char* )&tetraCount_, SIZEOF_T );

		meshFile.write( ( char* )( vertexData_.get() ), SIZEOF_T * 3 * vertexCount_ );
		meshFile.write( ( char* )( tetraVertexIndex_.get() ), SIZEOF_T * 4 * tetraCount_ );

		meshFile.close();

		return true;
	}

};

#endif