#ifndef EM_RULE_CONSTANT_INCLUDED
#define EM_RULE_CONSTANT_INCLUDED

#include "EmRule_Base.hpp"

template< class T >
class EmRule_Constant: public EmRule< T >
{
protected:
	vector< T > ruleData_;

	enum RuleDataIndex
	{
		EPS = 0,
		MU = 1
	};

	bool SetRuleData_( const vector< T >& ruleData ) override
	{
		if( ruleData.size() != 2 )
		{
			return false;
		}
		ruleData_ = ruleData;
		
	}

	bool GenerateProps_( T* propArr, const T* vertexList, const size_t* tetraList, const size_t& tetraCount ) const override
	{
		for( size_t idt = 0; idt < tetraCount; ++idt )
		{
			size_t idEps = 2 * idt;
			size_t idMu = idEps + 1;
			propArr[ idEps ] = ruleData_[ EPS ];
			propArr[ idMu ] = ruleData_[ MU ];
		}

		return true;
	}

public:
	
	EmRule_Constant()
	{

	}
	
	~EmRule_Constant()
	{

	}

};

#endif