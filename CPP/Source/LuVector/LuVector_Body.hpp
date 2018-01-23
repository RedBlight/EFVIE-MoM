#ifndef LU_VECTOR_BODY_INCLUDED
#define LU_VECTOR_BODY_INCLUDED

#include "LuVector.hpp"

namespace LUV
{

	template< size_t N, class T >
	class LuVector
	{
	private:
		T _[ N ];

	public:
		LuVector( )
		{

		}

		~LuVector()
		{

		}

		template< class... S > 
		LuVector( const S&... elem ) : _{ static_cast< T >(elem) ...}
		{

		}

		LuVector( const LuVector< N, const T >& other ) : _{ other._ }
		{

		}

		template< class S > 
		LuVector( const LuVector< N, S >& other )
		{
			UNROLL( *this, other, OP_COPY< T, S >(), LOOP_INDEX< N - 1 >() );
		}

		template< class S > 
		LuVector( const S& scalar )
		{
			UNROLL( *this, scalar, OP_COPY< T, S >(), LOOP_INDEX< N - 1 >() );
		}

		template< class S >
		inline LuVector< N, T >& operator =( const LuVector< N, S >& other )
		{
			UNROLL( *this, other, OP_COPY< T, S >(), LOOP_INDEX< N - 1 >() );
			return *this;
		}

		inline const T& operator []( const size_t& idx ) const
		{
			return _[ idx ];
		}

		inline T& operator []( const size_t& idx )
		{
			return _[ idx ];
		}

		inline string ToString() const
		{
			stringstream ss;
			ss << "(";
			UNROLL( ss, *this, ",", OP_OSTREAM< T >(), LOOP_INDEX< N - 2 >() );
			ss << _[ N - 1 ] << ")";
			return string( ss.str() );
		}

	};
};

#endif