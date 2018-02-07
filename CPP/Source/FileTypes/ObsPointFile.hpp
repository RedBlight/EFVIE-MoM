#ifndef OBS_POINT_FILE_INCLUDED
#define OBS_POINT_FILE_INCLUDED

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

.obspoint file:
[0] = point count (uint)
[1 ... A-1] = x y z (float) X 3 X [0] -> obs point coords

A = 1 + [0]*3

*/

template< class T >
class ObsPointFile
{
public:
	bool init_;
	UINT_T obsCount_;
	shared_ptr< T > obsData_;

public:

	ObsPointFile() :
		init_( false ),
		obsCount_( 0 ),
		obsData_()
	{

	}

	~ObsPointFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			obsCount_ = 0;
			obsData_.reset();
		}
	}

	void Initialize( const UINT_T& obsCount )
	{
		Reset();

		obsCount_ = obsCount;

		obsData_.reset( new T[ 3 * obsCount_ ], []( T* ptr ){ delete[] ptr; } );

		init_ = true;
	}
	
	void Initialize( const UINT_T& obsCount, const shared_ptr< T >& obsData )
	{
		Reset();

		obsCount_ = obsCount;
		obsData_ = obsData;

		init_ = true;
	}

	bool Load( const string& filePath )
	{
		Reset();

		fstream obsFile( filePath, ios::in | ios::binary );

		if( !obsFile.good() )
		{
			obsFile.close();
			return false;
		}

		obsFile.read( ( char* )&obsCount_, SIZEOF_T );
		obsFile.read( ( char* )&tetraCount_, SIZEOF_T );

		obsData_.reset( new T[ 3 * obsCount_ ], []( T* ptr ){ delete[] ptr; } );

		obsFile.read( ( char* )( obsData_.get() ), SIZEOF_T * 3 * obsCount_ );

		obsFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream obsFile( filePath, ios::trunc | ios::out | ios::binary );

		if( !obsFile.good() )
		{
			obsFile.close();
			return false;
		}

		obsFile.write( ( char* )&obsCount_, SIZEOF_T );

		obsFile.write( ( char* )( obsData_.get() ), SIZEOF_T * 3 * obsCount_ );

		obsFile.close();

		return true;
	}

};

#endif