#ifndef SCATTER_RESULT_FILE_INCLUDED
#define SCATTER_RESULT_FILE_INCLUDED

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

.scatterresult file:
[0] observation point count
[1 ... A - 1]: Ex Ey Ez (complex) x 3

A = 1 + 3 * 2 * [0] 

total size = 1 + 3 * SIZEOF_CT * [0] byte

*/

template< class T >
class ScatterResultFile
{
public:
	bool init_;
	UINT_T obsCount_;
	shared_ptr< complex< T > > resultData_;

public:
	ScatterResultFile() :
		init_( false ),
		obsCount_( 0 ),
		resultData_()
	{

	}

	~ScatterResultFile()
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

		resultData_.reset( new complex< T >[ 3 * obsCount_ ], []( complex< T >* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	void Initialize( const UINT_T& obsCount, const shared_ptr< complex< T > >& resultData )
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

		resultData_.reset( new complex< T >[ 3 * obsCount_ ], []( complex< T >* ptr ){ delete[] ptr; } );

		resultFile.read( ( char* )( resultData_.get() ), SIZEOF_CT * 3 * obsCount_ );

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

		resultFile.write( ( char* )( resultData_.get() ), SIZEOF_CT * 3 * obsCount_ );

		resultFile.close();

		return true;
	}

};

#endif