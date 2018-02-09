#ifndef OBS_ANGLE_FILE_INCLUDED
#define OBS_ANGLE_FILE_INCLUDED

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

.obsangle file:
[0] = angle count (uint)
[1 ... A-1] = phi theta (float) X 2 X [0] -> observation angles 

A = 1 + [0]*2

phi = angle between x axis - y axis
theta = angle between z axis - xy plane

*/

template< class T >
class ObsAngleFile
{
public:
	bool init_;
	UINT_T obsCount_;
	shared_ptr< T > obsData_;

public:

	ObsAngleFile() :
		init_( false ),
		obsCount_( 0 ),
		obsData_()
	{

	}

	~ObsAngleFile()
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

		obsData_.reset( new T[ 2 * obsCount_ ], []( T* ptr ){ delete[] ptr; } );

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

		obsData_.reset( new T[ 2 * obsCount_ ], []( T* ptr ){ delete[] ptr; } );

		obsFile.read( ( char* )( obsData_.get() ), SIZEOF_T * 2 * obsCount_ );

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

		obsFile.write( ( char* )( obsData_.get() ), SIZEOF_T * 2 * obsCount_ );

		obsFile.close();

		return true;
	}

};

#endif