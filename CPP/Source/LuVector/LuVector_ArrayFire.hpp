#ifndef LU_VECTOR_ARRAYFIRE_INCLUDED
#define LU_VECTOR_ARRAYFIRE_INCLUDED

#include <cstdint>
#include <complex>

#include <LuVector.hpp>

#include <arrayfire.h>

/*

ArrayFire array type specializations for LuVector.

Extend as needed.

*/

namespace LUV
{
	template< af::dtype dType >
	struct afType
	{

	};

	template<>
	struct afType< f32 >
	{
		typedef float cType;
	};
	
	template<>
	struct afType< f64 >
	{
		typedef double cType;
	};
	
	template<>
	struct afType< c32 >
	{
		typedef std::complex< float > cType;
	};
	
	template<>
	struct afType< c64 >
	{
		typedef std::complex< double > cType;
	};

	//continue if needed

	//////////////////////////////
	// LuVector_Op
	//////////////////////////////

	//////////////////////////////
	// LuVector_Unroll
	//////////////////////////////

	//////////////////////////////
	// LuVector_Body
	//////////////////////////////

	//////////////////////////////
	// LuVector_Overload
	//////////////////////////////

	// - VECTOR
	template< size_t N >
	inline LuVector< N, af::array > operator -( const LuVector< N, af::array >& vec )
	{
		return vec * (-1);
	}

	//////////////////////////////
	// LuVector_Math
	//////////////////////////////

	template< size_t N >
	inline af::array Sum( const LuVector< N, af::array >& vec )
	{
		af::array result = af::constant( 0, vec[0].dims(), vec[0].type() );
		UNROLL( result, vec, OP_ADD< af::array, af::array >(), LOOP_INDEX< N - 1 >() );
		return result;
	}
	
	template< size_t N >
	inline af::array Length( const LuVector< N, af::array >& vec )
	{
		af::array result = af::constant( 0, vec[0].dims(), vec[0].type() );
		UNROLL( result, vec, vec, OP_DOT< N, af::array, af::array >(), LOOP_INDEX< N - 1 >() );
		return af::sqrt( result );
	}

	template< size_t N >
	inline af::array Dot( const LuVector< N, af::array >& lhs, const LuVector< N, af::array >& rhs )
	{
		af::array result = af::constant( 0, lhs[0].dims(), lhs[0].type() );
		UNROLL( result, lhs, rhs, OP_DOT< N, af::array, af::array >(), LOOP_INDEX< N - 1 >() );
		return result;
	}

	//////////////////////////////
	// LuVector_Geometry
	//////////////////////////////

	template<>
	inline LuVector< 3, af::array > PlaneNormalP( const LuVector< 3, af::array >& vec, const LuVector< 3, af::array >& v1, const LuVector< 3, af::array >& v2, const LuVector< 3, af::array >& v3 ) 
	{
		LuVector< 3, af::array > n = PlaneNormal( v1, v2, v3 );
		af::array d = af::sign( Dot( n, vec - v1 ) );
		return n * ( 1 - 2 * d );
	}

	// make sure triangle is not degenerate
	template<>
	inline LuVector< 2, af::array > LineNormalP( const LuVector< 2, af::array >& vec, const LuVector< 2, af::array >& v1, const LuVector< 2, af::array >& v2 ) 
	{
		LuVector< 2, af::array > v12 = Unit( v2 - v1 );
		LuVector< 2, af::array > ort = vec - ProjLineL( vec, v1, v12 );
		return ort / Length( ort );
	}



};

#endif