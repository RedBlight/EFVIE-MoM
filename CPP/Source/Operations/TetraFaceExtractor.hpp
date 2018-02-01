#ifndef TETRA_FACE_EXTRACTOR_INCLUDED
#define TETRA_FACE_EXTRACTOR_INCLUDED

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

#include <TetraFace.hpp>

#include <arrayfire.h>

using namespace std;

template< class T >
class TetraFaceExtractor
{

public:
	TetraFaceExtractor()
	{

	}

	~TetraFaceExtractor()
	{

	}

	void Extract(
		const UINT_T& tetraCount,
		shared_ptr< UINT_T >& tetraVertexIndex,

		UINT_T& faceCount,
		shared_ptr< UINT_T >& faceVertexIndex,
		shared_ptr< UINT_T >& faceTetraIndex,
		shared_ptr< UINT_T >& tetraFaceIndex
	) const
	{
		tetraFaceIndex.reset( new UINT_T[ 4 * tetraCount ], []( UINT_T* ptr ){ delete[] ptr; } );
		UINT_T* tetraFaceIndexPtr = tetraFaceIndex.get();


		tetraVertexIndex.reset(
			af::sort( af::array( 4, tetraCount, tetraVertexIndex.get(), afHost ) ).host< UINT_T >(),
			[]( UINT_T* ptr ){ delete[] ptr; }
		);

		vector< TetraFace< UINT_T > > faceList;
		faceList.reserve( 4 * tetraCount );
		
		map< string, UINT_T > faceMap;

		UINT_T* iPtr = tetraVertexIndex.get();

		for( UINT_T idt = 0; idt < tetraCount; ++idt )
		{
			UINT_T iVert[4];
			iVert[0] = 4 * idt;
			iVert[1] = iVert[0] + 1;
			iVert[2] = iVert[0] + 2;
			iVert[3] = iVert[0] + 3;
			
			TetraFace< UINT_T > tetraFace[4];
			tetraFace[0] = TetraFace< UINT_T >( iPtr[ iVert[0] ], iPtr[ iVert[1] ], iPtr[ iVert[2] ] );
			tetraFace[1] = TetraFace< UINT_T >( iPtr[ iVert[0] ], iPtr[ iVert[1] ], iPtr[ iVert[3] ] );
			tetraFace[2] = TetraFace< UINT_T >( iPtr[ iVert[0] ], iPtr[ iVert[2] ], iPtr[ iVert[3] ] );
			tetraFace[3] = TetraFace< UINT_T >( iPtr[ iVert[1] ], iPtr[ iVert[2] ], iPtr[ iVert[3] ] );
			
			string faceHash[4];
			faceHash[0] = tetraFace[0].MakeHash();
			faceHash[1] = tetraFace[1].MakeHash();
			faceHash[2] = tetraFace[2].MakeHash();
			faceHash[3] = tetraFace[3].MakeHash();

			for( UINT_T idf = 0; idf < 4; ++idf )
			{
				auto itMap = faceMap.find( faceHash[ idf ] );
				if( itMap == faceMap.end() )
				{
					tetraFace[ idf ].t1_ = idt;
					faceList.push_back( tetraFace[ idf ] );
					UINT_T faceIndex = faceList.size();
					faceMap[ faceHash[ idf ] ] = faceIndex;
					tetraFaceIndexPtr[ iVert[ idf ] ] = faceIndex;
				}
				else
				{
					UINT_T faceIndex = itMap->second;
					faceList[ faceIndex ].t2_ = idt;
					tetraFaceIndexPtr[ iVert[ idf ] ] = faceIndex;
				}
			}
		}

		faceList.shrink_to_fit();
		faceCount = faceList.size();

		faceVertexIndex.reset( new UINT_T[ 3 * faceCount ], []( UINT_T* ptr ){ delete[] ptr; } );
		faceTetraIndex.reset( new UINT_T[ 2 * faceCount ], []( UINT_T* ptr ){ delete[] ptr; } );

		UINT_T* faceVertexIndexPtr = faceVertexIndex.get();
		UINT_T* faceTetraIndexPtr = faceTetraIndex.get();

		/**
			Think of a different way to alleviate this extra copy below.
		**/
		for( UINT_T idf = 0; idf < faceCount; ++idf )
		{
			UINT_T iVert[3];
			iVert[0] = 3 * idf;
			iVert[1] = iVert[0] + 1;
			iVert[2] = iVert[0] + 2;

			UINT_T iTet[2];
			iTet[0] = 2 * idf;
			iTet[1] = iTet[0] + 1;

			faceVertexIndexPtr[ iVert[0] ] = faceList[ idf ].v1_;
			faceVertexIndexPtr[ iVert[1] ] = faceList[ idf ].v2_;
			faceVertexIndexPtr[ iVert[2] ] = faceList[ idf ].v3_;

			faceTetraIndexPtr[ iTet[0] ] = faceList[ idf ].t1_;
			faceTetraIndexPtr[ iTet[1] ] = faceList[ idf ].t2_;
		}
	}

};

#endif