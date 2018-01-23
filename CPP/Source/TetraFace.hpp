#ifndef TETRA_FACE_INCLUDED
#define TETRA_FACE_INCLUDED

#include <string>

template< class T >
class TetraFace
{
public:
	T v1_;
	T v2_;
	T v3_;
	T t1_;
	T t2_;

public:

	TetraFace( )
	{

	}

	TetraFace( const T& v1, const T& v2, const T& v3 ) :
		v1_( v1 ),
		v2_( v2 ),
		v3_( v3 ),
		t1_( -1 ),
		t2_( -1 )
	{

	}

	
	TetraFace( const T& v1, const T& v2, const T& v3, const T& t1 ) :
		v1_( v1 ),
		v2_( v2 ),
		v3_( v3 ),
		t1_( t1 ),
		t2_( -1 )
	{

	}

	string MakeHash() const
	{
		return string( ( char* )&v1_, sizeof( T ) )
			+ string( ( char* )&v2_, sizeof( T ) )
			+ string( ( char* )&v3_, sizeof( T ) )
		;
	}

};

#endif