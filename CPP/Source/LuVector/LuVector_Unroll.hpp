#ifndef LU_VECTOR_UNROLL_INCLUDED
#define LU_VECTOR_UNROLL_INCLUDED

#include "LuVector.hpp"

namespace LUV
{
	//////////////////////////////////////////////////////////////
	// TEMPLATE LOOP UNROLLING
	//////////////////////////////////////////////////////////////

	// VECTOR OP SCALAR
	template< size_t I, size_t N, class T, class S, template< class, class > class OP >
	inline void UNROLL( LuVector< N, T >& lhs, const S& rhs, OP< T, S > operation, LOOP_INDEX< I > )
	{
		UNROLL( lhs, rhs, operation, LOOP_INDEX< I - 1 >() );
		operation( lhs[ I ], rhs );
	}

	template< size_t N, class T, class S, template< class, class > class OP >
	inline void UNROLL( LuVector< N, T >& lhs, const S& rhs, OP< T, S > operation, LOOP_INDEX< 0 > )
	{
		operation( lhs[ 0 ], rhs );
	}

	// SCALAR OP VECTOR
	template< size_t I, size_t N, class T, class S, template< class, class > class OP >
	inline void UNROLL( S& lhs, const LuVector< N, T >& rhs, OP< T, S > operation, LOOP_INDEX< I > )
	{
		UNROLL( lhs, rhs, operation, LOOP_INDEX< I - 1 >() );
		operation( lhs, rhs[ I ] );
	}

	template< size_t N, class T, class S, template< class, class > class OP >
	inline void UNROLL( S& lhs, const LuVector< N, T >& rhs, OP< T, S > operation, LOOP_INDEX< 0 > )
	{
		operation( lhs, rhs[ 0 ] );
	}

	// VECTOR OP VECTOR
	template< size_t I, size_t N, class T, class S, template< class, class > class OP >
	inline void UNROLL( LuVector< N, T >& lhs, const LuVector< N, S >& rhs, OP< T, S > operation, LOOP_INDEX< I > )
	{
		UNROLL( lhs, rhs, operation, LOOP_INDEX< I - 1 >() );
		operation( lhs[ I ], rhs[ I ] );
	}

	template< size_t N, class T, class S, template< class, class > class OP >
	inline void UNROLL( LuVector< N, T >& lhs, const LuVector< N, S >& rhs, OP< T, S > operation, LOOP_INDEX< 0 > )
	{
		operation( lhs[ 0 ], rhs[ 0 ] );
	}

	// SCALAR = VECTOR OP VECTOR (could be cross-element)
	template< size_t I, size_t N, class T, class S, template< size_t, class, class > class OP >
	inline void UNROLL( T& result, const LuVector< N, T >& lhs, const LuVector< N, S >& rhs, OP< N, T, S > operation, LOOP_INDEX< I > )
	{
		UNROLL( result, lhs, rhs, operation, LOOP_INDEX< I - 1 >() );
		operation( result, lhs, rhs, I );
	}

	template< size_t N, class T, class S, template< size_t, class, class > class OP >
	inline void UNROLL( T& result, const LuVector< N, T >& lhs, const LuVector< N, S >& rhs, OP< N, T, S > operation, LOOP_INDEX< 0 > )
	{
		operation( result, lhs, rhs, 0 );
	}

	// VECTOR = VECTOR OP VECTOR (could be cross-element)
	template< size_t I, size_t N, class T, class S, template< size_t, class, class > class OP >
	inline void UNROLL( LuVector< N, T >& result, const LuVector< N, T >& lhs, const LuVector< N, S >& rhs, OP< N, T, S > operation, LOOP_INDEX< I > )
	{
		UNROLL( result, lhs, rhs, operation, LOOP_INDEX< I - 1 >() );
		operation( result, lhs, rhs, I );
	}

	template< size_t N, class T, class S, template< size_t, class, class > class OP >
	inline void UNROLL( LuVector< N, T >& result, const LuVector< N, T >& lhs, const LuVector< N, S >& rhs, OP< N, T, S > operation, LOOP_INDEX< 0 > )
	{
		operation( result, lhs, rhs, 0 );
	}

	// OSTREAM OP VECTOR
	template< size_t I, size_t N, class T, template< class > class OP >
	inline void UNROLL( ostream& lhs, const LuVector< N, T >& rhs, const string& delimeter, OP< T > operation, LOOP_INDEX< I > )
	{
		UNROLL( lhs, rhs, delimeter, operation, LOOP_INDEX< I - 1 >() );
		operation( lhs, rhs[ I ], delimeter );
	}

	template< size_t N, class T, template< class > class OP >
	inline void UNROLL( ostream& lhs, const LuVector< N, T >& rhs, const string& delimeter, OP< T > operation, LOOP_INDEX< 0 > )
	{
		operation( lhs, rhs[ 0 ], delimeter );
	}

};

#endif