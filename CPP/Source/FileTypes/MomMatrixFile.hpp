#ifndef MOM_MATRIX_FILE_INCLUDED
#define MOM_MATRIX_FILE_INCLUDED

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
#include <complex>

using namespace std;

/*

.mommatrix file:
[0] face count
[1 ... A - 1]: momRE momIM // row mayor !!!

A = 1 + 2 * [0] * [0]

total size = SIZEOF_T + SIZEOF_CT * [0] * [0] byte

*/

// always double
template< class T >
class MomMatrixFile
{
public:
	bool init_;
	UINT_T faceCount_;
	shared_ptr< complex< T > > momData_;

public:
	MomMatrixFile() :
		init_( false ),
		faceCount_( 0 ),
		momData_()
	{

	}

	~MomMatrixFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			faceCount_ = 0;
			momData_.reset();
		}
	}

	void Initialize( const UINT_T& faceCount )
	{
		Reset();

		faceCount_ = faceCount;

		momData_.reset( new complex< T >[ faceCount_ * faceCount_ ], []( complex< T >* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	void Initialize( const UINT_T& faceCount, const shared_ptr< complex< T > >& momData_ )
	{
		Reset();

		faceCount_ = faceCount;
		momData_ = rhsData;

		init_ = true;
	}

	bool Load( const string& filePath )
	{
		Reset();

		fstream momFile( filePath, ios::in | ios::binary );

		if( !momFile.good() )
		{
			momFile.close();
			return false;
		}

		momFile.read( ( char* )&faceCount_, SIZEOF_T );

		momData_.reset( new complex< T >[ faceCount_ * faceCount_ ], []( complex< T >* ptr ){ delete[] ptr; } );

		momFile.read( ( char* )( momData_.get() ), SIZEOF_CT * faceCount_ * faceCount_ );

		momFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream momFile( filePath, ios::trunc | ios::out | ios::binary );
		
		if( !momFile.good() )
		{
			momFile.close();
			return false;
		}

		momFile.write( ( char* )&faceCount_, SIZEOF_T );

		momFile.write( ( char* )( momData_.get() ), SIZEOF_CT * faceCount_ * faceCount_ );

		momFile.close();

		return true;
	}

};

#endif