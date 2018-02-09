#ifndef RCS_RESULT_FILE_INCLUDED
#define RCS_RESULT_FILE_INCLUDED

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

.rcsresult file:
[0] observation angle count
[1 ... A - 1]: rcsVal (float)

A = 1 + 1 * [0]

total size = 1 + SIZEOF_T * [0] byte

*/

template< class T >
class RcsResultFile
{
public:
	bool init_;
	UINT_T obsCount_;
	shared_ptr< T > resultData_;

public:
	RcsResultFile() :
		init_( false ),
		obsCount_( 0 ),
		resultData_()
	{

	}

	~RcsResultFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			obsCount_ = 0;
			resultData_.reset();
		}
	}

	void Initialize( const UINT_T& obsCount )
	{
		Reset();

		obsCount_ = obsCount;

		resultData_.reset( new T[ obsCount_ ], []( T* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	void Initialize( const UINT_T& obsCount, const shared_ptr< T >& resultData )
	{
		Reset();

		obsCount_ = obsCount;
		resultData_ = resultData;

		init_ = true;
	}

	bool Load( const string& filePath )
	{
		Reset();

		fstream resultFile( filePath, ios::in | ios::binary );

		if( !resultFile.good() )
		{
			resultFile.close();
			return false;
		}

		resultFile.read( ( char* )&obsCount_, SIZEOF_T );

		resultData_.reset( new T[ obsCount_ ], []( T* ptr ){ delete[] ptr; } );

		resultFile.read( ( char* )( resultData_.get() ), SIZEOF_T * obsCount_ );

		resultFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream resultFile( filePath, ios::trunc | ios::out | ios::binary );
		
		if( !resultFile.good() )
		{
			resultFile.close();
			return false;
		}

		resultFile.write( ( char* )&obsCount_, SIZEOF_T );

		resultFile.write( ( char* )( resultData_.get() ), SIZEOF_T * obsCount_ );

		resultFile.close();

		return true;
	}

};

#endif