#ifndef MOM_SOLVER_INCLUDED
#define MOM_SOLVER_INCLUDED

#include <_BitDepthDefines.hpp>

#include <fstream>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <utility>
#include <memory>
#include <map>
#include <complex>
#include <thread>

#include <arrayfire.h>

#include <_LuVector.hpp>

using namespace std;

template< class T >
class MomSolver
{
public:
	UINT_T faceCount_;
	complex< T >* momDataPtr_;
	complex< T >* rhsDataPtr_;
	complex< T >* coefDataPtr_;

public:
	MomSolver(
		const UINT_T& faceCount,
		const shared_ptr< complex< T > >& momData,
		const shared_ptr< complex< T > >& rhsData,
		shared_ptr< complex< T > >& coefData // preallocated
	) :
		faceCount_( faceCount ),
		momDataPtr_( momData.get() ),
		rhsDataPtr_( rhsData.get() ),
		coefDataPtr_( coefData.get() )
	{

	}

	~MomSolver()
	{

	}

	void Solve()
	{
		af::array afMomMatrix = af::array( faceCount_, faceCount_, AF_COMPLEX_T );
		afMomMatrix.write( reinterpret_cast< T* >( momDataPtr_ ), SIZEOF_CT * faceCount_ * faceCount_, afHost );

		af::array afRhsVector = af::array( faceCount_, 1, AF_COMPLEX_T );
		afRhsVector.write( reinterpret_cast< T* >( rhsDataPtr_ ), SIZEOF_CT * faceCount_, afHost );

		af::array afCoefVector = af::solve( afMomMatrix, afRhsVector );
		afCoefVector.host( coefDataPtr_ );
	}

};

#endif