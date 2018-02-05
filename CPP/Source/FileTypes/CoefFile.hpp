#ifndef COEF_FILE_INCLUDED
#define COEF_FILE_INCLUDED

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

.coef file:
[0] face count
[1 ... A - 1]: coefRE coefIM

A = 1 + 2 * [0]

total size = SIZEOF_CT * [0] byte

*/

// always double
template< class T >
class CoefFile
{
public:
	bool init_;
	UINT_T faceCount_;
	shared_ptr< complex< T > > coefData_;

public:
	CoefFile() :
		init_( false ),
		faceCount_( 0 ),
		coefData_()
	{

	}

	~CoefFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			faceCount_ = 0;
			coefData_.reset();
		}
	}

	void Initialize( const UINT_T& faceCount )
	{
		Reset();

		faceCount_ = faceCount;

		coefData_.reset( new complex< T >[ faceCount_ ], []( complex< T >* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	void Initialize( const UINT_T& faceCount, const shared_ptr< complex< T > >& coefData )
	{
		Reset();

		faceCount_ = faceCount;
		coefData_ = coefData;

		init_ = true;
	}

	bool Load( const string& filePath )
	{
		Reset();

		fstream coefFile( filePath, ios::in | ios::binary );

		if( !coefFile.good() )
		{
			coefFile.close();
			return false;
		}

		coefFile.read( ( char* )&faceCount_, SIZEOF_T );

		coefData_.reset( new complex< T >[ faceCount_ ], []( complex< T >* ptr ){ delete[] ptr; } );

		coefFile.read( ( char* )( coefData_.get() ), SIZEOF_CT * faceCount_ );

		coefFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream coefFile( filePath, ios::trunc | ios::out | ios::binary );
		
		if( !coefFile.good() )
		{
			coefFile.close();
			return false;
		}

		coefFile.write( ( char* )&faceCount_, SIZEOF_T );

		coefFile.write( ( char* )( coefData_.get() ), SIZEOF_CT * faceCount_ );

		coefFile.close();

		return true;
	}

};

#endif