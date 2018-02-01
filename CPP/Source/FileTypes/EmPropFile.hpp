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
#include <memory>

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
	shared_ptr< T > emPropData_;

public:

	EmPropFile() :
		init_( false ),
		tetraCount_( 0 ),
		emPropData_()
	{

	}

	~EmPropFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			tetraCount_ = 0;
			emPropData_.reset();
		}
	}

	void Initialize( const UINT_T& tetraCount )
	{
		Reset();

		tetraCount_ = tetraCount;

		emPropData_.reset( new T[ 2 * tetraCount_  ], []( T* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	void Initialize( const UINT_T& tetraCount, const shared_ptr< T >& emPropData )
	{
		Reset();

		tetraCount_ = tetraCount;
		emPropData_ = emPropData;

		init_ = true;
	}

	bool Load( const string& filePath )
	{
		Reset();

		fstream propFile( filePath, ios::in | ios::binary );

		if( !propFile.good() )
		{
			propFile.close();
			return false;
		}

		propFile.read( ( char* )&tetraCount_, SIZEOF_T );

		emPropData_.reset( new T[ 2 * tetraCount_ ], []( T* ptr ){ delete[] ptr; } );
		propFile.read( ( char* )( emPropData_.get() ), SIZEOF_T * 2 * tetraCount_ );

		propFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream propFile( filePath, ios::trunc | ios::out | ios::binary );

		if( !propFile.good() )
		{
			propFile.close();
			return false;
		}

		propFile.write( ( char* )&tetraCount_, SIZEOF_T );
		propFile.write( ( char* )( emPropData_.get() ), SIZEOF_T * 2 * tetraCount_ );

		propFile.close();

		return true;
	}

};

#endif