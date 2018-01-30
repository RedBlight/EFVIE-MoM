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

#include <StringFuncs.hpp>

using namespace std;

/*

.tetramesh file:
[0] = vertex count (uint)
[1] = tetrahedron count (uint)
[2 ... A-1] = x y z (float) 8 byte X 3 X [0] -> vertex coords
[A ... B-1] = v1 v2 v3 v4 (uint) 8 byte X 4 X [1] -> tetrahedron vertex indices

A = 2 + [0]*3
B = A + [1]*4

*/

// always double
template< class T >
class TetraMeshFile
{
public:
	bool init_;
	UINT_T vertexCount_;
	UINT_T tetraCount_;
	T* vertexData_;
	UINT_T* tetraVertexIndex_;

public:

	TetraMeshFile() :
		init_( false ),
		vertexCount_( 0 ),
		tetraCount_( 0 ),
		vertexData_( nullptr ),
		tetraVertexIndex_( nullptr )
	{

	}

	~TetraMeshFile()
	{
		if( init_ )
		{
			delete[] vertexData_;
			delete[] tetraVertexIndex_;
		}
	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			vertexCount_ = 0;
			tetraCount_ = 0;
			delete[] vertexData_;
			delete[] tetraVertexIndex_;
			vertexData_ = nullptr;
			tetraVertexIndex_ = nullptr;
		}
	}

	bool Load_unv( const string& filePath )
	{
		using namespace __StringFuncs;

		Reset();

		fstream unvFile( filePath, ios::in );
		
		/*
			Perhaps do some sanity checks here etc.
		*/

		string vertexStartStr = "  2411";
		string vertexEndStr = "    -1";
		string tetraStartStr = "  2412";
		string tetraEndStr = "    -1";

		string lineStr = "";

		while( lineStr.compare( vertexStartStr ) != 0 )
		{
			getline( unvFile, lineStr );
		}

		while( lineStr.compare( vertexEndStr ) != 0 )
		{
			getline( unvFile, lineStr );
			++vertexCount_;
		}

		vertexCount_ = ( vertexCount_ - 1 ) / 2;

		while( lineStr.compare( tetraStartStr ) != 0 )
		{
			getline( unvFile, lineStr );
		}

		while( lineStr.compare( tetraEndStr ) != 0 )
		{
			getline( unvFile, lineStr );
			++tetraCount_;
		}

		tetraCount_ = ( tetraCount_ - 1 ) / 2;
		
		vertexData_ = new T[ 3 * vertexCount_ ];
		tetraVertexIndex_ = new UINT_T[ 4 * tetraCount_ ];

		unvFile.clear();
		unvFile.seekg( 0, ios::beg );

		while( lineStr.compare( vertexStartStr ) != 0 )
		{
			getline( unvFile, lineStr );
		}

		for( UINT_T idv = 0; idv < vertexCount_; ++idv )
		{
			getline( unvFile, lineStr );
			getline( unvFile, lineStr );
			Trim( lineStr );
			SearchAndReplace( lineStr, "    ", " " );
			SearchAndReplace( lineStr, "   ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "D", "E" );
			vector< string > vCoords = Explode( lineStr, ' ' );

			UINT_T idx = 3 * idv;
			UINT_T idy = idx + 1;
			UINT_T idz = idy + 1;
			vertexData_[ idx ] = ( T )( stod( vCoords[ 0 ] ) );
			vertexData_[ idy ] = ( T )( stod( vCoords[ 1 ] ) );
			vertexData_[ idz ] = ( T )( stod( vCoords[ 2 ] ) );
		}

		while( lineStr.compare( tetraStartStr ) != 0 )
		{
			getline( unvFile, lineStr );
		}

		for( UINT_T idt = 0; idt < tetraCount_; ++idt )
		{
			getline( unvFile, lineStr );
			getline( unvFile, lineStr );
			Trim( lineStr );
			SearchAndReplace( lineStr, "    ", " " );
			SearchAndReplace( lineStr, "   ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			SearchAndReplace( lineStr, "  ", " " );
			vector< string > vIndices = Explode( lineStr, ' ' );

			UINT_T idv1 = 4 * idt;
			UINT_T idv2 = idv1 + 1;
			UINT_T idv3 = idv2 + 1;
			UINT_T idv4 = idv3 + 1;
			tetraVertexIndex_[ idv1 ] = ( UINT_T )( stoull( vIndices[ 0 ] ) );
			tetraVertexIndex_[ idv2 ] = ( UINT_T )( stoull( vIndices[ 1 ] ) );
			tetraVertexIndex_[ idv3 ] = ( UINT_T )( stoull( vIndices[ 2 ] ) );
			tetraVertexIndex_[ idv4 ] = ( UINT_T )( stoull( vIndices[ 3 ] ) );
		}

		unvFile.close();

		init_ = true;

		return true;
	}

	bool Load_tetramesh( const string& filePath )
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

		vertexData_ = new T[ 3 * vertexCount_ ];
		tetraVertexIndex_ = new UINT_T[ 4 * tetraCount_ ];

		meshFile.read( ( char* )vertexData_, SIZEOF_T * 3 * vertexCount_ );
		meshFile.read( ( char* )tetraVertexIndex_, SIZEOF_T * 4 * tetraCount_ );

		meshFile.close();

		init_ = true;

		return true;
	}

	bool Save_tetramesh( const string& filePath )
	{
		fstream meshFile( filePath, ios::trunc | ios::out | ios::binary );

		if( !meshFile.good() )
		{
			meshFile.close();
			return false;
		}

		meshFile.write( ( char* )&vertexCount_, SIZEOF_T );
		meshFile.write( ( char* )&tetraCount_, SIZEOF_T );

		meshFile.write( ( char* )vertexData_, SIZEOF_T * 3 * vertexCount_ );
		meshFile.write( ( char* )tetraVertexIndex_, SIZEOF_T * 4 * tetraCount_ );

		meshFile.close();

		return true;
	}

};

#endif