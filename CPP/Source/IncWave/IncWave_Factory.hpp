#ifndef INC_WAVE_FACTORY_INCLUDED
#define INC_WAVE_FACTORY_INCLUDED

#include <_BitDepthDefines.hpp>

#include <string>
#include <list>
#include <utility>
#include <iterator>
#include <map>
#include <iostream>
#include <memory>

#include <IncWave.hpp>
#include <IncWave_Plane.hpp>

using namespace std;

/*
	USAGE:
	IncWaveFactory< T >& myFac = IncWaveFactory< T >::GetInstance();
	shared_ptr< IncWave< T > > myWave = myFac.GetWave( "MyWave" );
*/
template< class T >
class IncWaveFactory
{
public:
	static IncWaveFactory< T >& GetInstance()
	{
		static IncWaveFactory< T > instance_;
		return instance_;
	}

	map< string, shared_ptr< IncWave< T > > > waveMap_;
	
private:
	IncWaveFactory()
	{
		// Register all kinds of EmRule variants here
		
		waveMap_.clear();
		waveMap_[ "Plane" ] = shared_ptr< IncWave< T > >( new IncWave_Plane< T >() );
		//waveMap_[ "HertzianDipole" ] = shared_ptr< IncWave< T > >( new IncWave_HertzianDipole< T >() );
	}

	~IncWaveFactory()
	{

	}

public:
	IncWaveFactory( const IncWaveFactory< T >& ) = delete;
    void operator=( const IncWaveFactory< T >& ) = delete;

	shared_ptr< IncWave< T > > GetWave( const string& name )
	{
		// Fecking Visual Studio, can compile this, but can't identify the type in IDE. 
		auto it = waveMap_.find( name );
		if( it == waveMap_.end() )
		{
			return nullptr;
		}
		else
		{
			return it->second->Clone();
		}
	}

};

#endif