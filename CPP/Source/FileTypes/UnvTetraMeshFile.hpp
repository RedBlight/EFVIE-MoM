#ifndef UNV_TETRA_MESH_FILE_INCLUDED
#define UNV_TETRA_MESH_FILE_INCLUDED

#include <fstream>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <utility>
#include <cstdint>
#include <memory>
#include <StringFuncs.hpp>

using namespace std;

/*

.unv file for tetrahedral mesh:
### FILE START ###		-> ASCII file.
***
vertexStartStr			-> Coordinates of vertices.
	//useless line
	{}v1x{}v1y{}v2z{}
	//useless line
	{}v2x{}v2y{}v2z{}	-> repeat for the number of vertices.
vertexEndStr
***
tetraStartStr			-> Indices of tetrahedra vertices
	//useless line
	{}t1v1{}t1v2{}v2z{}
	//useless line
	{}v2x{}v2y{}v2z{}	-> Repeat for the number of tetrahedra.
tetraEndStr
***
### FILE END ###

{}	-> denotes varying amounts of whitespace
***	-> denotes various amount of useless line

string vertexStartStr = "  2411";
string vertexEndStr = "    -1";
string tetraStartStr = "  2412";
string tetraEndStr = "    -1";

*/

// always double
template< class T >
class UnvTetraMeshFile
{
public:
	bool init_;
	uint64_t vertexCount_;
	uint64_t tetraCount_;
	shared_ptr< T > vertexData_;
	shared_ptr< uint64_t > tetraVertexIndex_;

	static const string vertexStartStr_ = "  2411";
	static const string vertexEndStr_ = "    -1";
	static const string tetraStartStr_ = "  2412";
	static const string tetraEndStr_ = "    -1";

public:

	UnvTetraMeshFile() :
		init_( false ),
		vertexCount_( 0 ),
		tetraCount_( 0 ),
		vertexData_(),
		tetraVertexIndex_()
	{

	}

	~UnvTetraMeshFile()
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
		tetraVertexIndex_ = new size_t[ 4 * tetraCount_ ];

		unvFile.clear();
		unvFile.seekg( 0, ios::beg );

		while( lineStr.compare( vertexStartStr ) != 0 )
		{
			getline( unvFile, lineStr );
		}

		for( size_t idv = 0; idv < vertexCount_; ++idv )
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

			size_t idx = 3 * idv;
			size_t idy = idx + 1;
			size_t idz = idy + 1;
			vertexData_[ idx ] = ( T )( stod( vCoords[ 0 ] ) );
			vertexData_[ idy ] = ( T )( stod( vCoords[ 1 ] ) );
			vertexData_[ idz ] = ( T )( stod( vCoords[ 2 ] ) );
		}

		while( lineStr.compare( tetraStartStr ) != 0 )
		{
			getline( unvFile, lineStr );
		}

		for( size_t idt = 0; idt < tetraCount_; ++idt )
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

			size_t idv1 = 4 * idt;
			size_t idv2 = idv1 + 1;
			size_t idv3 = idv2 + 1;
			size_t idv4 = idv3 + 1;
			tetraVertexIndex_[ idv1 ] = ( size_t )( stoull( vIndices[ 0 ] ) );
			tetraVertexIndex_[ idv2 ] = ( size_t )( stoull( vIndices[ 1 ] ) );
			tetraVertexIndex_[ idv3 ] = ( size_t )( stoull( vIndices[ 2 ] ) );
			tetraVertexIndex_[ idv4 ] = ( size_t )( stoull( vIndices[ 3 ] ) );
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

		meshFile.read( ( char* )&vertexCount_, 8 );
		meshFile.read( ( char* )&tetraCount_, 8 );

		vertexData_ = new T[ 3 * vertexCount_ ];
		tetraVertexIndex_ = new size_t[ 4 * tetraCount_ ];

		meshFile.read( ( char* )vertexData_, 8 * 3 * vertexCount_ );
		meshFile.read( ( char* )tetraVertexIndex_, 8 * 4 * tetraCount_ );

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

		meshFile.write( ( char* )&vertexCount_, 8 );
		meshFile.write( ( char* )&tetraCount_, 8 );

		meshFile.write( ( char* )vertexData_, 8 * 3 * vertexCount_ );
		meshFile.write( ( char* )tetraVertexIndex_, 8 * 4 * tetraCount_ );

		meshFile.close();

		return true;
	}

};

#endif