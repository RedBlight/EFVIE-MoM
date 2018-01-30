#ifndef LU_VECTOR_MATH_INCLUDED
#define LU_VECTOR_MATH_INCLUDED

#include "LuVector.hpp"

namespace LUV
{

	template< size_t N, class T >
	inline T Sum( const LuVector< N, T >& vec )
	{
		T result = 0;
		UNROLL( result, vec, OP_ADD< T, T >(), LOOP_INDEX< N - 1 >() );
		return result;
	}

	// Abs
	template< size_t N, class T >
	inline LuVector< N, T > Abs( const LuVector< N, T >& vec )
	{
		LuVector< N, T > absVec;
		UNROLL( absVec, vec, OP_ABS< T, T >(), LOOP_INDEX< N - 1 >() );
		return absVec;
	}

	// Length
	template< size_t N, class T >
	inline T Length( const LuVector< N, T >& vec )
	{
		T result = 0;
		UNROLL( result, vec, vec, OP_DOT< N, T, T >(), LOOP_INDEX< N - 1 >() );
		return sqrt( result );
	}

	// Unit
	template< size_t N, class T >
	inline LuVector< N, T > Unit( const LuVector< N, T >& vec )
	{
		return vec / Length( vec );
	}

	//////////////////////////////////////////////////////////////

	// Abs Complex
	template< size_t N, class T >
	inline LuVector< N, T > Abs( const LuVector< N, complex< T > >& vec )
	{
		LuVector< N, T > absVec;
		UNROLL( absVec, vec, OP_ABS< T, complex< T > >(), LOOP_INDEX< N - 1 >() );
		return absVec;
	}

	// Arg Complex
	template< size_t N, class T >
	inline LuVector< N, T > Arg( const LuVector< N, complex< T > >& vec )
	{
		LuVector< N, T > argVec;
		UNROLL( argVec, vec, OP_ARG< T, complex< T > >(), LOOP_INDEX< N - 1 >() );
		return argVec;
	}

	// Length Complex
	template< size_t N, class T >
	inline T Length( const LuVector< N, complex< T > >& vec )
	{
		return Length( Abs( vec ) ); 
	}

	// Unit Complex
	template< size_t N, class T >
	inline LuVector< N, complex< T > > Unit( const LuVector< N, complex< T > >& vec )
	{
		return vec / Length( vec );
	}

	//////////////////////////////////////////////////////////////

	// Min
	template< size_t N, class T, class S >
	inline LuVector< N, T > Min( const LuVector< N, T >& lhs, const LuVector< N, S >& rhs )
	{
		LuVector< N, T > result;
		UNROLL( result, lhs, rhs, OP_MIN< N, T, S >(), LOOP_INDEX< N - 1 >() );
		return result;
	}

	// Max
	template< size_t N, class T, class S >
	inline LuVector< N, T > Max( const LuVector< N, T >& lhs, const LuVector< N, S >& rhs )
	{
		LuVector< N, T > result;
		UNROLL( result, lhs, rhs, OP_MAX< N, T, S >(), LOOP_INDEX< N - 1 >() );
		return result;
	}

	// Dot
	template< size_t N, class T, class S >
	inline T Dot( const LuVector< N, T >& lhs, const LuVector< N, S >& rhs )
	{
		T result = 0;
		UNROLL( result, lhs, rhs, OP_DOT< N, T, S >(), LOOP_INDEX< N - 1 >() );
		return result;
	}

	// Cross
	template< class T, class S >
	inline LuVector< 3, T > Cross( const LuVector< 3, T >& lhs, const LuVector< 3, S >& rhs )
	{
		return LuVector< 3, T >(
			lhs[1] * rhs[2] - lhs[2] * rhs[1],
			lhs[2] * rhs[0] - lhs[0] * rhs[2],
			lhs[0] * rhs[1] - lhs[1] * rhs[0]
		);
	}

	//////////////////////////////////////////////////////////////

	// Reflect
	template< size_t N, class T, class S >
	inline LuVector< N, T > Reflect( const LuVector< N, T >& vec, const LuVector< N, S >& normal )
	{
		return vec - normal * Dot( vec, normal ) * 2;
	}

	// Rotate...

	//////////////////////////////////////////////////////////////

	// CtsToSph
	template< class T >
	inline LuVector< 3, T > CtsToSph( const LuVector< 3, T >& cts )
	{
		T len = Length( cts );
		return LuVector< 3, T >(
			len,
			cts[0] == 0 && cts[1] == 0 ? 0 : atan2( cts[1], cts[0] ),
			acos( cts[2] / len )
		);
	}

	// SphToCts
	template< class T >
	inline LuVector< 3, T > SphToCts( const LuVector< 3, T >& sph )
	{
		T planarVal = sph[0] * sin( sph[2] );
		return LuVector< 3, T >(
			planarVal * cos( sph[1] ),
			planarVal * sin( sph[1] ),
			sph[0] * cos( sph[2] )
		);
	}

	// CtsToPol
	template< class T >
	inline LuVector< 2, T > CtsToPol( const LuVector< 2, T >& cts )
	{
		T len = Length( cts );
		return LuVector< 2, T >(
			len,
			cts[0] == 0 && cts[1] == 0 ? 0 : atan2( cts[1], cts[0] )
		);
	}

	// PolToCts
	template< class T >
	inline LuVector< 2, T > PolToCts( const LuVector< 2, T >& pol )
	{
		T planarVal = sph[0] * sin( sph[2] );
		return LuVector< 2, T >(
			pol[0] * cos( pol[1] ),
			pol[0] * sin( pol[1] )
		);
	}

	//////////////////////////////////////////////////////////////

	// OrthonormalSet N=3
	// N: looking outside the sphere at given angles
	// U: up vector
	// R: right vector, parallel to XY plane
	// N = U X R 
	template< class T >
	inline void OrthonormalSet( const T& angP, const T& angT, LuVector< 3, T >& dirN, LuVector< 3, T >& dirU, LuVector< 3, T >& dirR )
	{
		T cp = cos( angP );
		T sp = sin( angP );
		T ct = cos( angT );
		T st = sin( angT );

		dirN[0] = st * cp;
		dirN[1] = st * sp;
		dirN[2] = ct;

		dirR[0] = sp;
		dirR[1] = - cp;
		dirR[2] = 0;

		dirU = Cross( dirR, dirN );
	}

	// OrthonormalSet N=2
	template< class T >
	inline void OrthonormalSet( const T& ang, LuVector< 2, T >& dirN, LuVector< 2, T >& dirR )
	{
		T c = cos( ang );
		T s = sin( ang );

		dirN[0] = c;
		dirN[1] = s;

		dirR[0] = s;
		dirR[1] = - c;
	}

	// OrthogonalR N=3
	template< class T >
	inline LuVector< 3, T > OrthonormalR( const LuVector< 3, T >& dirN )
	{
		T ang = dirN[0] == 0 && dirN[1] == 0 ? 0 : atan2( dirN[1], dirN[0] );
		T c = cos( ang );
		T s = sin( ang );

		return LuVector< 3, T >( s, -c, 0 ); 
	}

	// OrthogonalR N=2
	template< class T >
	inline LuVector< 2, T > OrthonormalR( const LuVector< 2, T >& dirN )
	{
		return LuVector< 2, T >( dirN[1], -dirN[0] ); 
	}

	// OrthogonalU N=3
	template< class T >
	inline LuVector< 3, T > OrthonormalU( const LuVector< 3, T >& dirN )
	{
		return Cross( OrthogonalR( dirN ), dirN ); 
	}

	// OrthonormalU N=3
	template< class T >
	inline LuVector< 3, T > OrthonormalR( const LuVector< 3, T >& dirN, const LuVector< 3, T >& dirU )
	{
		return Cross( dirN, dirU ); 
	}

	// Orthonormalize N=3
	// Makes sure that given vectors are absolutely othogonal and unit length.
	template< class T >
	inline void Orthonormalize( LuVector< 3, T >& dirN, LuVector< 3, T >& dirU, LuVector< 3, T >& dirR )
	{
		dirN = Unit( dirN );
		dirU = Unit( dirU );
		dirR = Unit( dirR );

		dirU = Unit( dirU - Dot( dirU, dirN ) * dirN );

		dirR = Unit( dirR - Dot( dirR, dirN ) * dirN );
		dirR = Unit( dirR - Dot( dirR, dirU ) * dirU );
	}

};

#endif