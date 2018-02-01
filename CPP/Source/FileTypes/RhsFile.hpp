#ifndef RHS_FILE_INCLUDED
#define RHS_FILE_INCLUDED

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

.rhs file:
[0] face count
[1 ... A - 1]: rhsRE rhsIM

A = 1 + 2 * [0]

total size = SIZEOF_CT * [0] byte

*/

// always double
template< class T >
class RhsFile
{
public:
	bool init_;
	UINT_T faceCount_;
	shared_ptr< complex< T > > rhsData_;

public:
	RhsFile() :
		init_( false ),
		faceCount_( 0 ),
		rhsData_()
	{

	}

	~RhsFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			faceCount_ = 0;
			rhsData_.reset();
		}
	}

	void Initialize( const UINT_T& faceCount )
	{
		Reset();

		faceCount_ = faceCount;

		rhsData_.reset( new complex< T >[ faceCount_ ], []( complex< T >* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	void Initialize( const UINT_T& faceCount, const shared_ptr< complex< T > >& rhsData )
	{
		Reset();

		faceCount_ = faceCount;
		rhsData_ = rhsData;

		init_ = true;
	}

	bool Load( const string& filePath )
	{
		Reset();

		fstream rhsFile( filePath, ios::in | ios::binary );

		if( !rhsFile.good() )
		{
			rhsFile.close();
			return false;
		}

		rhsFile.read( ( char* )&faceCount_, SIZEOF_T );

		rhsData_.reset( new complex< T >[ faceCount_ ], []( complex< T >* ptr ){ delete[] ptr; } );

		rhsFile.read( ( char* )( rhsData_.get() ), SIZEOF_CT * faceCount_ );

		rhsFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream rhsFile( filePath, ios::trunc | ios::out | ios::binary );
		
		if( !rhsFile.good() )
		{
			rhsFile.close();
			return false;
		}

		rhsFile.write( ( char* )&faceCount_, SIZEOF_T );

		rhsFile.write( ( char* )( rhsData_.get() ), SIZEOF_CT * faceCount_ );

		rhsFile.close();

		return true;
	}

};

#endif