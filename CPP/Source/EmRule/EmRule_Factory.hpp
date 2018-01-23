#ifndef EM_RULE_FACTORY_INCLUDED
#define EM_RULE_FACTORY_INCLUDED

#include <string>
#include <list>
#include <utility>
#include <iterator>
#include <map>
#include "EmRule_Base.hpp"
#include "EmRule_Constant.hpp"

using namespace std;

template< class T >
class EmRuleFactory
{
public:
	typedef EmRule< T >* EmRuleTp;
	typedef map< string, EmRuleTp > MapT;

private:
	
	static MapT ruleMap_;

public:
	

	static void Initialize();
	static EmRuleTp GetRule( const string& name );

};

template< class T >
map< string, EmRule< T >* > EmRuleFactory< T >::ruleMap_;

template< class T >
static void EmRuleFactory< T >::Initialize()
{
		// Register all kinds of EmRule variants here

	EmRuleFactory< T >::ruleMap_[ "Constant" ] = new EmRule_Constant< T >();
		//ruleMap_[ "RadialLinear" ] = new EmRule_RadialLinear< T >();
		//ruleMap_[ "CartesianLinear" ] = new EmRule_CartesianLinear< T >();
};

template< class T >
static EmRuleFactory<T>::EmRuleTp EmRuleFactory< T >::GetRule( const string& name )
{
	MapT::iterator it = EmRuleFactory< T >::ruleMap_.find( name );
};

//template< class T >  
//class EmRuleFactory 
//{
//public:
//
//	static void Initialize()
//	{
//		// Register all kinds of EmRule variants here
//
//		ruleMap_[ "Constant" ] = new EmRule_Constant< T >();
//		//ruleMap_[ "RadialLinear" ] = new EmRule_RadialLinear< T >();
//		//ruleMap_[ "CartesianLinear" ] = new EmRule_CartesianLinear< T >();
//	}
//
//	static EmRule< T >* GetRule( const string& name )
//	{
//		ruleMap_;
//		auto it = EmRuleFactory< T >::ruleMap_.find( name );
//		//if( it == ruleMap_.end() )
//		//{
//		//	return nullptr;
//		//}
//		//else
//		//{
//		//	return it.second;
//		//}
//	}
//
//	static void Destroy()
//	{
//		for( auto& elem : ruleMap_ )
//		{
//			delete elem.second;
//		}
//	}
//};

#endif