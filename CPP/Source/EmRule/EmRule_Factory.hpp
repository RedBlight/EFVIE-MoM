#ifndef EM_RULE_FACTORY_INCLUDED
#define EM_RULE_FACTORY_INCLUDED

#include <_BitDepthDefines.hpp>

#include <string>
#include <list>
#include <utility>
#include <iterator>
#include <map>
#include <iostream>
#include <memory>

#include <EmRule.hpp>
#include <EmRule_Constant.hpp>

using namespace std;

/*
	USAGE:
	EmRuleFactory< T >& myFac = EmRuleFactory< T >::GetInstance();
	shared_ptr< EmRule< T > > myRule = myFac.GetRule( "MyRule" );
*/
template< class T >
class EmRuleFactory
{
public:
	static EmRuleFactory< T >& GetInstance()
	{
		static EmRuleFactory< T > instance_;
		return instance_;
	}

	map< string, shared_ptr< EmRule< T > > > ruleMap_;
	
private:
	EmRuleFactory()
	{
		// Register all kinds of EmRule variants here

		ruleMap_.clear();
		ruleMap_[ "Constant" ] = shared_ptr< EmRule< T > >( new EmRule_Constant< T >() );
		//ruleMap_[ "RadialLinear" ] = new EmRule_RadialLinear< T >();
		//ruleMap_[ "CartesianLinear" ] = new EmRule_CartesianLinear< T >();
	}

	~EmRuleFactory()
	{

	}

public:
	EmRuleFactory( const EmRuleFactory< T >& ) = delete;
    void operator=( const EmRuleFactory< T >& ) = delete;

	shared_ptr< EmRule< T > > GetRule( const string& name )
	{
		// Fecking Visual Studio, can compile this, but can't identify the type in IDE. 
		auto it = ruleMap_.find( name );
		if( it == ruleMap_.end() )
		{
			return nullptr;
		}
		else
		{
			return it->second->Clone();
		}
	}

};




// FOR OLD
/*
	This definition of the map had some internal problems, that I was not able to figure out:
	map< string, EmRule< T >* > ruleMap_;

	So instead, I did it like this:
	map< string, unsigned long long int > ruleMap_;

	... and then casted that int to the pointer.

	Dayumn C++, you're scary.

	USAGE:
	EmRuleFactory< T >& myFac = EmRuleFactory< T >::GetInstance();
	EmRule< T >* myRule = myFac.GetRule( "MyRule" );
*/

////////////////////////
// OLD V3
////////////////////////
//template< class T >
//class EmRuleFactory
//{
//public:
//	static EmRuleFactory< T >& GetInstance()
//	{
//		static EmRuleFactory< T > instance_;
//		return instance_;
//	}
//
//	map< string, unsigned long long int > ruleMap_;
//	
//private:
//	EmRuleFactory()
//	{
//		
//		cout << "!!EMRULEFACTORY Created!! " << endl;
//
//		ruleMap_.clear();
//
//		// Register all kinds of EmRule variants here
//
//		ruleMap_[ "Constant" ] = reinterpret_cast< unsigned long long int >( new EmRule_Constant< T >() );
//
//		//ruleMap_[ "RadialLinear" ] = new EmRule_RadialLinear< T >();
//		//ruleMap_[ "CartesianLinear" ] = new EmRule_CartesianLinear< T >();
//	}
//
//	~EmRuleFactory()
//	{
//		cout << "!!EMRULEFACTORY Destroyed!! " << endl;
//
//		for( const auto& elem : ruleMap_ )
//		{
//			delete reinterpret_cast< EmRule< T >* >( elem.second );
//		}
//	}
//
//public:
//	EmRuleFactory( const EmRuleFactory< T >& ) = delete;
//    void operator=( const EmRuleFactory< T >& ) = delete;
//
//	shared_ptr< EmRule< T > > GetRule( const string& name )
//	{
//		auto it = ruleMap_.find( name );
//		if( it == ruleMap_.end() )
//		{
//			return nullptr;
//		}
//		else
//		{
//			return reinterpret_cast< EmRule< T >* >( it->second )->Clone();
//		}
//	}
//
//};




////////////////////////
// OLD V2
////////////////////////
//template< class T >
//class EmRuleFactory
//{
//public:
//	static EmRuleFactory< T >* pInstance_;
//	static size_t refCount_;
//	
//public:
//	map< string, unsigned long long int > ruleMap_;
//
//public:
//	static EmRuleFactory< T >& GetInstance()
//	{
//		++refCount_;
//
//		if( pInstance_ == nullptr )
//		{
//			pInstance_ = new EmRuleFactory< T >();
//		}
//
//		return *pInstance_;
//	}
//
//	EmRuleFactory()
//	{
//		
//		cout << "!!EMRULEFACTORY Created!! " << refCount_ << endl;
//
//		ruleMap_.clear();
//
//		// Register all kinds of EmRule variants here
//
//		ruleMap_[ "Constant" ] = reinterpret_cast< unsigned long long int >( new EmRule_Constant< T >() );
//		//ruleMap_[ "RadialLinear" ] = new EmRule_RadialLinear< T >();
//		//ruleMap_[ "CartesianLinear" ] = new EmRule_CartesianLinear< T >();
//	}
//
//	~EmRuleFactory()
//	{
//		--refCount_;
//		cout << "!!EMRULEFACTORY Destroyed!! " << refCount_ << endl;
//		if( refCount_ == 0 )
//		{
//			cout << "!!EMRULEFACTORY DELETED!!" << endl;
//			for( const auto& elem : ruleMap_ )
//			{
//				delete reinterpret_cast< EmRule< T >* >( elem.second );
//			}
//
//			delete pInstance_; // also calls the destructor, so refCount goes to -1.
//			++refCount_;
//			pInstance_ = nullptr;
//		}
//	}
//
//	EmRule< T >* GetRule( const string& name )
//	{
//		auto it = ruleMap_.find( name );
//		if( it == ruleMap_.end() )
//		{
//			return nullptr;
//		}
//		else
//		{
//			return reinterpret_cast< EmRule< T >* >( it->second )->Clone();
//		}
//	}
//
//};
//
//template< class T >
//EmRuleFactory< T >* EmRuleFactory< T >::pInstance_ = nullptr;
//
//template< class T >
//size_t EmRuleFactory< T >::refCount_ = 0;



////////////////////////
// OLD V1
////////////////////////


//template< class T >
//class EmRuleFactory
//{
//public:
//	static EmRuleFactory< T >* pInstance_;
//
//public:
//	map< string, EmRule< T >* > ruleMap_;
//
//public:
//	static EmRuleFactory< T >* GetInstance()
//	{
//		if (pInstance_ == nullptr)
//		{
//			pInstance_ = new EmRuleFactory< T >();
//		}
//
//		return pInstance_;
//	}
//
//	EmRuleFactory()
//	{
//		ruleMap_.clear();
//
//		// Register all kinds of EmRule variants here
//
//		ruleMap_["Constant"] = new EmRule_Constant< T >();
//		//ruleMap_[ "RadialLinear" ] = new EmRule_RadialLinear< T >();
//		//ruleMap_[ "CartesianLinear" ] = new EmRule_CartesianLinear< T >();
//	}
//
//	~EmRuleFactory()
//	{
//		delete pInstance_;
//		typename map< string, EmRule< T >* >::iterator it;
//
//		for (it = ruleMap_.begin(); it != ruleMap_.end(); it++)
//		{
//			delete it->second;
//		}
//	}
//
//	EmRule< T >* GetRule(const string& name)
//	{
//		auto it = EmRuleFactory< T >::ruleMap_.find(name);
//		if (it == ruleMap_.end())
//		{
//			return nullptr;
//		}
//		else
//		{
//			return it->second;
//		}
//	}
//
//	//	static EmRule< T >* GetRule( const string& name )
//	//	{
//	//		ruleMap_;
//	//		auto it = EmRuleFactory< T >::ruleMap_.find( name );
//	//		//if( it == ruleMap_.end() )
//	//		//{
//	//		//	return nullptr;
//	//		//}
//	//		//else
//	//		//{
//	//		//	return it.second;
//	//		//}
//	//	}
//
//};











//template< class T >
//class EmRuleFactory
//{
//public:
//	typedef EmRule< T >* EmRuleTp;
//	typedef map< string, EmRuleTp > MapT;
//
//private:
//	
//	static MapT ruleMap_;
//
//public:
//	
//
//	static void Initialize();
//	static EmRuleTp GetRule( const string& name );
//
//};
//
//template< class T >
//map< string, EmRule< T >* > EmRuleFactory< T >::ruleMap_;
//
//template< class T >
//static void EmRuleFactory< T >::Initialize()
//{
//		// Register all kinds of EmRule variants here
//
//	EmRuleFactory< T >::ruleMap_[ "Constant" ] = new EmRule_Constant< T >();
//		//ruleMap_[ "RadialLinear" ] = new EmRule_RadialLinear< T >();
//		//ruleMap_[ "CartesianLinear" ] = new EmRule_CartesianLinear< T >();
//};
//
//template< class T >
//static EmRuleFactory<T>::EmRuleTp EmRuleFactory< T >::GetRule( const string& name )
//{
//	MapT::iterator it = EmRuleFactory< T >::ruleMap_.find( name );
//};

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