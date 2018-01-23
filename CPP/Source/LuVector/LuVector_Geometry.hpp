#ifndef LU_VECTOR_GEOMETRY_INCLUDED
#define LU_VECTOR_GEOMETRY_INCLUDED

#include "LuVector.hpp"

namespace LUV
{
	//////////////////////////////////////////////////////////////
	// LUV GEOMETRY
	//////////////////////////////////////////////////////////////

	// ProjLine N=3
	template< class T >
	inline LuVector< 3, T > ProjLine( const LuVector< 3, T >& vec, const LuVector< 3, T >&v1, const LuVector< 3, T >& v2 ) 
	{
		LuVector< 3, T > v12 = Unit( v2 - v1 );
		return v1 + Dot( v12, vec - v1 ) * v12;
	}

	// ProjLine N=2
	template< class T >
	inline LuVector< 2, T > ProjLine( const LuVector< 2, T >& vec, const LuVector< 2, T >&v1, const LuVector< 2, T >& v2 ) 
	{
		LuVector< 2, T > v12 = Unit( v2 - v1 );
		return v1 + Dot( v12, vec - v1 ) * v12;
	}

	// ProjLineL N=3
	template< class T >
	inline LuVector< 3, T > ProjLineL( const LuVector< 3, T >& vec, const LuVector< 3, T >&v1, const LuVector< 3, T >& lineDir ) 
	{
		return v1 + Dot( lineDir, vec - v1 ) * lineDir;
	}

	// ProjLineL N=2
	template< class T >
	inline LuVector< 2, T > ProjLineL( const LuVector< 2, T >& vec, const LuVector< 2, T >&v1, const LuVector< 2, T >& lineDir )  
	{
		return v1 + Dot( lineDir, vec - v1 ) * lineDir;
	}

	// LineNormal N=3
	template< class T >
	inline LuVector< 3, T > LineNormal( const LuVector< 3, T >& v1, const LuVector< 3, T >& v2 ) 
	{
		LuVector< 3, T > v12 = Unit( v2 - v1 );
		return OrthogonalR( v12 );
	}

	// LineNormal N=2
	template< class T >
	inline LuVector< 2, T > LineNormal( const LuVector< 2, T >& v1, const LuVector< 2, T >& v2 ) 
	{
		LuVector< 2, T > v12 = Unit( v2 - v1 );
		return OrthogonalR( v12 );
	}

	// LineNormalL N=3
	template< class T >
	inline LuVector< 3, T > LineNormalL( const LuVector< 3, T >& lineDir ) 
	{
		return OrthogonalR( lineDir );
	}

	// LineNormalL N=2
	template< class T >
	inline LuVector< 2, T > LineNormalL( const LuVector< 2, T >& lineDir ) 
	{
		return OrthogonalR( lineDir );
	}

	// LineNormalP N=3
	template< class T >
	inline LuVector< 3, T > LineNormalP( const LuVector< 3, T >& vec, const LuVector< 3, T >& v1, const LuVector< 3, T >& v2 ) 
	{
		LuVector< 3, T > v12 = Unit( v2 - v1 );
		LuVector< 3, T > ort = vec - ProjLineL( vec, v1, v12 );
		T len = Length( ort );
		return len > 0 ? ort / len : LineNormalL( v12 );
	}

	// LineNormalP N=2
	template< class T >
	inline LuVector< 2, T > LineNormalP( const LuVector< 2, T >& vec, const LuVector< 2, T >& v1, const LuVector< 2, T >& v2 ) 
	{
		LuVector< 2, T > v12 = Unit( v2 - v1 );
		LuVector< 2, T > ort = vec - ProjLineL( vec, v1, v12 );
		T len = Length( ort );
		return len > 0 ? ort / len : LineNormalL( v12 );
	}

	// LineNormalPL N=3
	template< class T >
	inline LuVector< 3, T > LineNormalPL( const LuVector< 3, T >& vec, const LuVector< 3, T >& v1, const LuVector< 3, T >& lineDir )  
	{
		LuVector< 3, T > ort = vec - ProjLineL( vec, v1, lineDir );
		T len = Length( ort );
		return len > 0 ? ort / len : LineNormalL( lineDir );
	}

	// LineNormalPL N=2
	template< class T >
	inline LuVector< 2, T > LineNormalPL( const LuVector< 2, T >& vec, const LuVector< 2, T >& v1, const LuVector< 2, T >& lineDir ) 
	{
		LuVector< 2, T > ort = vec - ProjLineL( vec, v1, lineDir );
		T len = Length( ort );
		return len > 0 ? ort / len : LineNormalL( lineDir );
	}

	//////////////////////////////////////////////////////////////

	// ProjPlane
	template< class T >
	inline LuVector< 3, T > ProjPlane( const LuVector< 3, T >& vec, const LuVector< 3, T >& v1, const LuVector< 3, T >& n ) 
	{
		return vec - Dot( vec - v1, n ) * n;
	}

	// PlaneNormal
	template< class T >
	inline LuVector< 3, T > PlaneNormal( const LuVector< 3, T >& v1, const LuVector< 3, T >& v2, const LuVector< 3, T >& v3 )
	{
		return Unit( Cross( v2 - v1, v3 - v1 ) );
	}

	// PlaneNormalP
	template< class T >
	inline LuVector< 3, T > PlaneNormalP(  const LuVector< 3, T >& vec, const LuVector< 3, T >& v1, const LuVector< 3, T >& v2, const LuVector< 3, T >& v3 ) 
	{
		LuVector< 3, T > n = PlaneNormal( v1, v2, v3 );
		return Dot( n, vec - v1 ) > 0 ? n : -n;
	}

	// PlaneNormalP
	template< class T >
	inline LuVector< 3, T > PlaneNormalPN(  const LuVector< 3, T >& vec, const LuVector< 3, T >& v1, const LuVector< 3, T >& n ) 
	{
		return Dot( n, vec - v1 ) > 0 ? n : -n;
	}

};

#endif