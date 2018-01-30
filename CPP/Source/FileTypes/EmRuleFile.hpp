#ifndef EM_RULE_FILE_INCLUDED
#define EM_RULE_FILE_INCLUDED

#include <_BitDepthDefines.hpp>

#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

template< class T >
class EmRuleFile
{
public:

	bool init_;
	string ruleType_;
	vector< T > ruleData_;

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