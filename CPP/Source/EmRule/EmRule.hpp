#ifndef EM_RULE_INCLUDED
#define EM_RULE_INCLUDED

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace std;

template< class T >
class EmRule
{
protected:
	virtual bool SetRuleData_( const vector< T >& ruleData ) = 0;
	virtual bool GenerateProps_( T* propArr, const T* vertexList, const size_t* tetraList, const size_t& tetraCount ) const = 0;
	virtual shared_ptr< EmRule< T > > Clone_() const = 0;

public:

	bool SetRuleData( const vector< T >& ruleData )
	{
		return SetRuleData_( ruleData );
	}

	bool GenerateProps( T* propArr, const T* vertexList, const size_t* tetraList, const size_t& tetraCount ) const
	{
		return GenerateProps_( propArr, vertexList, tetraList, tetraCount );
	}

	shared_ptr< EmRule< T > > Clone() const
	{
		return Clone_();
	}

};



#endif