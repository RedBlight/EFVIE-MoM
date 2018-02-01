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
		EPS,	// 0	Relative electric permittivity
		MU		// 1	Relative magnetic permeability
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

	bool GenerateProps_(
		const UINT_T& tetraCount,
		const shared_ptr< T >& vertexData,
		const shared_ptr< UINT_T >& tetraVertexIndex,
		shared_ptr< T >& emPropData // must be preallocated
	) const override
	{
		T* vertexDataPtr = vertexData.get();
		UINT_T* tetraVertexIndexPtr = tetraVertexIndex.get();
		T* emPropDataPtr = emPropData.get();

		for( size_t idt = 0; idt < tetraCount; ++idt )
		{
			size_t idEps = 2 * idt;
			size_t idMu = idEps + 1;
			emPropDataPtr[ idEps ] = ruleData_[ EPS ];
			emPropDataPtr[ idMu ] = ruleData_[ MU ];
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

	}
	
	~EmRule_Constant()
	{

	}

};

#endif