#ifndef INC_WAVE_INCLUDED
#define INC_WAVE_INCLUDED

#include <_BitDepthDefines.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>

#include <arrayfire.h>

#include <_LuVector.hpp>

using namespace std;

template< class T >
using PairEM = pair< LUV::LuVector3c< T >, LUV::LuVector3c< T > >;

template< class T >
class IncWave
{
protected:
	virtual bool SetWaveData_( const vector< T >& waveData ) = 0;
	virtual LUV::LuVector3c< T > FieldE_( const LUV::LuVector3< T >& obsPoint ) const = 0;
	virtual LUV::LuVector3c< T > FieldM_( const LUV::LuVector3< T >& obsPoint ) const = 0;
	virtual PairEM< T > FieldEM_( const LUV::LuVector3< T >& obsPoint ) const = 0;
	virtual shared_ptr< IncWave< T > > Clone_() const = 0;

public:

	bool SetWaveData( const vector< T >& waveData )
	{
		return SetWaveData_( waveData );
	}

	LUV::LuVector3c< T > FieldE( const LUV::LuVector3< T >& obsPoint ) const
	{
		return FieldE_( obsPoint );
	}

	LUV::LuVector3c< T > FieldM( const LUV::LuVector3< T >& obsPoint ) const
	{
		return FieldM_( obsPoint );
	}

	PairEM< T > FieldEM( const LUV::LuVector3< T >& obsPoint ) const
	{
		return FieldEM_( obsPoint );
	}

	shared_ptr< IncWave< T > > Clone( const LUV::LuVector3< T >& obsPoint ) const
	{
		return Clone_();
	}

};



#endif