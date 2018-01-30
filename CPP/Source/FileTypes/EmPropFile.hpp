#ifndef EM_PROP_FILE_INCLUDED
#define EM_PROP_FILE_INCLUDED

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
.emprop file: each cell is 8 byte
[0] = tetrahedron count (uint64)
[A ... B-1] = eps mu (double) 8 byte X 2 X [0] -> electromagnetic material properties, eps and mu relative to eps0 and mu0

A = 1
B = A + [0]*2
*/

// always double
template< class T >
class EmPropFile
{
public:
	bool init_;
	UINT_T tetraCount_;
	FLOAT_T* emPropData_;

public:

	EmPropFile() :
		init_( false ),
		tetraCount_( 0 ),
		emPropData_( nullptr )
	{

	}

	~EmPropFile()
	{
		if( init_ )
		{
			delete[] emPropData_;
		}
	}

	void Initialise( const UINT_T& tetraCount )
	{
		if( !init_ )
		{
			tetraCount_ = tetraCount;
			emPropData_ = new T[ 2 * tetraCount_ ];
			init_ = true;
		}
	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			tetraCount_ = 0;
			delete[] emPropData_;
			emPropData_ = nullptr;
		}
	}

	bool Load_emprop( const string& filePath )
	{
		Reset();

		fstream propFile( filePath, ios::in | ios::binary );

		if( !propFile.good() )
		{
			propFile.close();
			return false;
		}

		propFile.read( ( char* )&tetraCount_, SIZEOF_T );
		emPropData_ = new T[ 2 * tetraCount_ ];

		propFile.read( ( char* )emPropData_, SIZEOF_T * 2 * tetraCount_ );

		propFile.close();

		init_ = true;

		return true;
	}

	bool Save_emprop( const string& filePath )
	{
		fstream propFile( filePath, ios::trunc | ios::out | ios::binary );

		if( !propFile.good() )
		{
			propFile.close();
			return false;
		}

		propFile.write( ( char* )&tetraCount_, SIZEOF_T );
		propFile.write( ( char* )emPropData_, SIZEOF_T * 2 * tetraCount_ );

		propFile.close();

		return true;
	}

};

#endif