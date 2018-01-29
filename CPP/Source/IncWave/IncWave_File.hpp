#ifndef INC_WAVE_FILE_INCLUDED
#define INC_WAVE_FILE_INCLUDED

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
	EmRuleFile() :
		init_( false ),
		ruleType_( "" ),
		ruleData_()
	{

	}

	~EmRuleFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			ruleType_ = "";
			ruleData_.clear();
		}
	}

	bool Load_emrule( const string& filePath )
	{
		if( !init_ )
		{
			Reset();
		}

		fstream ruleFile( filePath, ios::in );

		if( !ruleFile.good() )
		{
			ruleFile.close();
			return false;
		}

		ruleFile >> ruleType_;
		
		T lineData;
		while( !ruleFile.eof() )
		{
			ruleFile >> lineData;
			ruleData_.push_back( lineData );
		}

		init_ = true;

		return true;
	}

};


#endif