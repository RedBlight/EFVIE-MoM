#ifndef EM_RULE_CONSTANT_INCLUDED
#define EM_RULE_CONSTANT_INCLUDED

#include <_BitDepthDefines.hpp>

#include <EmRule.hpp>

template< class T >
class EmRule_Constant: public EmRule< T >
{
protected:
	vector< T > ruleData_;

	enum RuleDataIndex
	{
		EPS,
		MU
	};

	bool SetRuleData_( const vector< T >& ruleData ) override
	{
		if( ruleData.size() != 2 )
		{
			return false;
		}
		ruleData_ = ruleData;

		return true;
	}

	bool GenerateProps_( T* propArr, const T* vertexList, const UINT_T* tetraList, const UINT_T& tetraCount ) const override
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

	shared_ptr< EmRule< T > > Clone_() const override
	{
		return shared_ptr< EmRule< T > >( new EmRule_Constant< T >( *this ) );
	}

public:
	EmRule_Constant()
	{
		//cout << "!!! EMRULE_CONSTANT CREATED !!!" << endl;
	}
	
	~EmRule_Constant()
	{
		//cout << "!!! EMRULE_CONSTANT DESTROYED !!!" << endl;
	}

};

#endif