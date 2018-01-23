#ifndef LU_VECTOR_INCLUDED
#define LU_VECTOR_INCLUDED

#include <sstream>
#include <string>
#include <complex>

namespace LUV
{
	using namespace std;

	template< size_t N, class T >
	class LuVector;

	template< class T >
	using LuVector2 = LuVector< 2, T >;

	template< class T >
	using LuVector3 = LuVector< 3, T >;

	using LuVector2f = LuVector< 2, float >;
	using LuVector2d = LuVector< 2, double >;
	using LuVector2c = LuVector< 2, complex< double > >;

	using LuVector3f = LuVector< 3, float >;
	using LuVector3d = LuVector< 3, double >;
	using LuVector3c = LuVector< 3, complex< double > >;

};

#include "LuVector_Op.hpp"
#include "LuVector_Unroll.hpp"
#include "LuVector_Body.hpp"
#include "LuVector_Overload.hpp"
#include "LuVector_Math.hpp"
#include "LuVector_Geometry.hpp"

#endif