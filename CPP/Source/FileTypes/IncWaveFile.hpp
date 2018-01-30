#ifndef INC_WAVE_FILE_INCLUDED
#define INC_WAVE_FILE_INCLUDED

#include <_BitDepthDefines.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

template< class T >
class IncWaveFile
{
public:

	bool init_;
	string waveType_;
	vector< T > waveData_;

public:
	IncWaveFile() :
		init_( false ),
		waveType_( "" ),
		waveData_()
	{

	}

	~IncWaveFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			waveType_ = "";
			waveData_.clear();
		}
	}

	bool Load_incwave( const string& filePath )
	{
		if( !init_ )
		{
			Reset();
		}

		fstream waveFile( filePath, ios::in );

		if( !waveFile.good() )
		{
			waveFile.close();
			return false;
		}

		waveFile >> ruleType_;
		
		T lineData;
		while( !waveFile.eof() )
		{
			waveFile >> lineData;
			waveData_.push_back( lineData );
		}

		init_ = true;

		return true;
	}

};


#endif