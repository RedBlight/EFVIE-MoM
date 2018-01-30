#ifndef TETRA_FACE_FILE_INCLUDED
#define TETRA_FACE_FILE_INCLUDED

#include <_BitDepthDefines.hpp>

#include <fstream>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <utility>
#include <map>

using namespace std;

/*
.tetraface file: each cell is 8 byte
[0] = trig count (uint)
[1] = tetrahedron count (uint)
[A ... B-1] = v1 v2 v3 (uint) X 3 X [0] -> face vertex indices
[B ... C-1] = t1 t2 (uint) X 2 X [0] -> face tetrahedron indices, -1 means null
[C ... D-1] = f1 f2 f3 f4 (uint) X 4 X [1] -> tetrahedron face indices

A = 2
B = A + [0]*3
C = B + [0]*2
D = C + [1]*4
*/

// always double
template< class T >
class TetraFaceFile
{
public:
	bool init_;
	UINT_T faceCount_;
	UINT_T tetraCount_;
	shared_ptr< UINT_T > faceVertexIndex_;
	shared_ptr< UINT_T > faceTetraIndex_;
	shared_ptr< UINT_T > tetraFaceIndex_;

public:

	TetraFaceFile() :
		init_( false ),
		faceCount_( 0 ),
		tetraCount_( 0 ),
		faceVertexIndex_(),
		faceTetraIndex_(),
		tetraFaceIndex_()
	{

	}

	~TetraFaceFile()
	{

	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			faceCount_ = 0;
			tetraCount_ = 0;
			faceVertexIndex_.reset();
			faceTetraIndex_.reset();
			tetraFaceIndex_.reset();
		}
	}

	void Initialize( const UINT_T& faceCount, const UINT_T& tetraCount )
	{
		Reset();

		faceCount_ = faceCount;
		tetraCount_ = tetraCount;
		
		faceVertexIndex_.reset( new UINT_T[ 3 * faceCount_ ], []( UINT_T* ptr ){ delete[] ptr; } );
		faceTetraIndex_.reset( new UINT_T[ 2 * faceCount_ ], []( UINT_T* ptr ){ delete[] ptr; } );
		tetraFaceIndex_.reset( new UINT_T[ 4 * tetraCount_ ], []( UINT_T* ptr ){ delete[] ptr; } );

		init_ = true;
	}

	
	void Initialize(
		const UINT_T& faceCount,
		const UINT_T& tetraCount,
		const shared_ptr< UINT_T >& faceVertexIndex,
		const shared_ptr< UINT_T >& faceTetraIndex,
		const shared_ptr< UINT_T >& tetraFaceIndex
	)
	{
		Reset();

		faceCount_ = faceCount;
		tetraCount_ = tetraCount;

		faceVertexIndex_ = faceVertexIndex;
		faceTetraIndex_ = faceTetraIndex;
		tetraFaceIndex_ = tetraFaceIndex;

		init_ = true;
	}

	//bool Load_tetramesh( const string& filePath )
	//{
	//	using namespace af;

	//	Reset();

	//	TetraMeshFile< T > meshFile;

	//	bool meshLoaded = meshFile.Load_tetramesh( filePath );

	//	if( !meshLoaded )
	//	{
	//		return false;
	//	}

	//	

	//	////// DEBUG
	//	//cout << endl;
	//	//cout << meshFile.tetraVertexIndex_[0] << " " << meshFile.tetraVertexIndex_[1] << " " << meshFile.tetraVertexIndex_[2] << " " << meshFile.tetraVertexIndex_[3] << " " << endl;
	//	//cout << meshFile.tetraVertexIndex_[4] << " " << meshFile.tetraVertexIndex_[5] << " " << meshFile.tetraVertexIndex_[6] << " " << meshFile.tetraVertexIndex_[7] << " " << endl;
	//	//cout << meshFile.tetraVertexIndex_[8] << " " << meshFile.tetraVertexIndex_[9] << " " << meshFile.tetraVertexIndex_[10] << " " << meshFile.tetraVertexIndex_[11] << " " << endl;
	//	//cout << meshFile.tetraVertexIndex_[12] << " " << meshFile.tetraVertexIndex_[13] << " " << meshFile.tetraVertexIndex_[14] << " " << meshFile.tetraVertexIndex_[15] << " " << endl;
	//	//cout << meshFile.tetraVertexIndex_[16] << " " << meshFile.tetraVertexIndex_[17] << " " << meshFile.tetraVertexIndex_[18] << " " << meshFile.tetraVertexIndex_[19] << " " << endl;
	//	//cout << endl;
	//	//////

	//	tetraCount_ = meshFile.tetraCount_;
	//	tetraFaceIndex_ = new UINT_T[ 4 * tetraCount_ ];
	//	meshFile.tetraVertexIndex_ = af::sort( af::array( 4, meshFile.tetraCount_, meshFile.tetraVertexIndex_, afHost ) ).host< UINT_T >();
	//	


	//	
	//	//af::array indexData( 4, meshFile.tetraCount_, meshFile.tetraVertexIndex_, afHost );

	//	//af::array sortedData = indexData;
	//	//sortedData = af::sort( sortedData );

	//	//meshFile.tetraVertexIndex_ = sortedData.host< size_t >();

	//	////////////////////////////

	//	

	//	//size_t arr[] = {1,8,99,2, 3,83,356,8, 32,1,24,457, 8,7,3,9};


	//	//
	//	//af::array aba( 4, 5, arr, afHost );
	//	////meshFile.tetraVertexIndex_ = af::sort( indexData, 0, true ).host< size_t >();

	//	//af_print(aba.cols(0,3));

	//	//af::array ana = af::sort( aba );

	//	//
	//	//af_print(ana.cols(0,3));


	//	//
	//	//af::array indexData( 4, meshFile.tetraCount_, meshFile.tetraVertexIndex_, afHost );
	//	////meshFile.tetraVertexIndex_ = af::sort( indexData, 0, true ).host< size_t >();

	//	//size_t N = meshFile.tetraCount_-1;

	//	//af_print(indexData.cols(0,8) );
	//	//af_print(indexData.cols(N-8,N) );

	//	//af::array sortedData = indexData;
	//	//sortedData = af::sort( sortedData.cols(0,N) );

	//	//af_print(sortedData.cols(0,8));
	//	//af_print(sortedData.cols(N-8,N));

	//	//meshFile.tetraVertexIndex_ = sortedData.host< size_t >();
	//	

	//	////// DEBUG
	//	//cout << endl;
	//	//cout << meshFile.tetraVertexIndex_[0] << " " << meshFile.tetraVertexIndex_[1] << " " << meshFile.tetraVertexIndex_[2] << " " << meshFile.tetraVertexIndex_[3] << " " << endl;
	//	//cout << meshFile.tetraVertexIndex_[4] << " " << meshFile.tetraVertexIndex_[5] << " " << meshFile.tetraVertexIndex_[6] << " " << meshFile.tetraVertexIndex_[7] << " " << endl;
	//	//cout << meshFile.tetraVertexIndex_[8] << " " << meshFile.tetraVertexIndex_[9] << " " << meshFile.tetraVertexIndex_[10] << " " << meshFile.tetraVertexIndex_[11] << " " << endl;
	//	//cout << meshFile.tetraVertexIndex_[12] << " " << meshFile.tetraVertexIndex_[13] << " " << meshFile.tetraVertexIndex_[14] << " " << meshFile.tetraVertexIndex_[15] << " " << endl;
	//	//cout << meshFile.tetraVertexIndex_[16] << " " << meshFile.tetraVertexIndex_[17] << " " << meshFile.tetraVertexIndex_[18] << " " << meshFile.tetraVertexIndex_[19] << " " << endl;
	//	//cout << endl;
	//	//////

	//	vector< TetraFace< UINT_T > > faceList;
	//	faceList.reserve( 4 * meshFile.tetraCount_ );

	//	map< string, UINT_T > faceMap;

	//	UINT_T* iPtr = meshFile.tetraVertexIndex_;

	//	for( UINT_T idt = 0; idt < tetraCount_; ++idt )
	//	{
	//		UINT_T iVert[4];
	//		iVert[0] = 4 * idt;
	//		iVert[1] = iVert[0] + 1;
	//		iVert[2] = iVert[0] + 2;
	//		iVert[3] = iVert[0] + 3;
	//		
	//		TetraFace< UINT_T > tetraFace[4];
	//		tetraFace[0] = TetraFace< UINT_T >( iPtr[ iVert[0] ], iPtr[ iVert[1] ], iPtr[ iVert[2] ] );
	//		tetraFace[1] = TetraFace< UINT_T >( iPtr[ iVert[0] ], iPtr[ iVert[1] ], iPtr[ iVert[3] ] );
	//		tetraFace[2] = TetraFace< UINT_T >( iPtr[ iVert[0] ], iPtr[ iVert[2] ], iPtr[ iVert[3] ] );
	//		tetraFace[3] = TetraFace< UINT_T >( iPtr[ iVert[1] ], iPtr[ iVert[2] ], iPtr[ iVert[3] ] );
	//		
	//		string faceHash[4];
	//		faceHash[0] = tetraFace[0].MakeHash();
	//		faceHash[1] = tetraFace[1].MakeHash();
	//		faceHash[2] = tetraFace[2].MakeHash();
	//		faceHash[3] = tetraFace[3].MakeHash();

	//		for( UINT_T idf = 0; idf < 4; ++idf )
	//		{
	//			auto itMap = faceMap.find( faceHash[ idf ] );
	//			if( itMap == faceMap.end() )
	//			{
	//				tetraFace[ idf ].t1_ = idt + 1;
	//				faceList.push_back( tetraFace[ idf ] );
	//				UINT_T faceIndex = faceList.size();
	//				faceMap[ faceHash[ idf ] ] = faceIndex;
	//				tetraFaceIndex_[ iVert[ idf ] ] = faceIndex;
	//			}
	//			else
	//			{
	//				UINT_T faceIndex = itMap->second;
	//				faceList[ faceIndex ].t2_ = idt + 1;
	//				tetraFaceIndex_[ iVert[ idf ] ] = faceIndex;
	//			}
	//		}
	//	}

	//	faceList.shrink_to_fit();
	//	faceCount_ = faceList.size();

	//	faceVertexIndex_ = new UINT_T[ 3 * faceCount_ ];
	//	faceTetraIndex_ = new UINT_T[ 2 * faceCount_ ];

	//	/**
	//		Think of a different way to alleviate this extra copy below.
	//	**/

	//	//cout << "C" << endl;

	//	for( UINT_T idf = 0; idf < faceCount_; ++idf )
	//	{
	//		UINT_T iVert[3];
	//		iVert[0] = 3 * idf;
	//		iVert[1] = iVert[0] + 1;
	//		iVert[2] = iVert[0] + 2;

	//		UINT_T iTet[2];
	//		iTet[0] = 2 * idf;
	//		iTet[1] = iTet[0] + 1;

	//		faceVertexIndex_[ iVert[0] ] = faceList[ idf ].v1_;
	//		faceVertexIndex_[ iVert[1] ] = faceList[ idf ].v2_;
	//		faceVertexIndex_[ iVert[2] ] = faceList[ idf ].v3_;

	//		faceTetraIndex_[ iTet[0] ] = faceList[ idf ].t1_;
	//		faceTetraIndex_[ iTet[1] ] = faceList[ idf ].t2_;
	//	}

	//	init_ = true;

	//	return true;
	//}

	bool Load( const string& filePath )
	{
		Reset();

		fstream faceFile( filePath, ios::in | ios::binary );

		if( !faceFile.good() )
		{
			faceFile.close();
			return false;
		}

		faceFile.read( ( char* )&faceCount_, SIZEOF_T );
		faceFile.read( ( char* )&tetraCount_, SIZEOF_T );
		
		faceVertexIndex_.reset( new UINT_T[ 3 * faceCount_ ], []( UINT_T* ptr ){ delete[] ptr; } );
		faceTetraIndex_.reset( new UINT_T[ 2 * faceCount_ ], []( UINT_T* ptr ){ delete[] ptr; } );
		tetraFaceIndex_.reset( new UINT_T[ 4 * tetraCount_ ], []( UINT_T* ptr ){ delete[] ptr; } );

		faceFile.read( ( char* )( faceVertexIndex_.get() ), SIZEOF_T * 3 * faceCount_ );
		faceFile.read( ( char* )( faceTetraIndex_.get() ), SIZEOF_T * 2 * faceCount_ );
		faceFile.read( ( char* )( tetraFaceIndex_.get() ), SIZEOF_T * 4 * tetraCount_ );

		faceFile.close();

		init_ = true;

		return true;
	}

	bool Save( const string& filePath )
	{
		fstream faceFile( filePath, ios::trunc | ios::out | ios::binary );

		if( !faceFile.good() )
		{
			faceFile.close();
			return false;
		}

		faceFile.write( ( char* )&faceCount_, SIZEOF_T );
		faceFile.write( ( char* )&tetraCount_, SIZEOF_T );

		faceFile.write( ( char* )( faceVertexIndex_.get() ), SIZEOF_T * 3 * faceCount_ );
		faceFile.write( ( char* )( faceTetraIndex_.get() ), SIZEOF_T * 2 * faceCount_ );
		faceFile.write( ( char* )( tetraFaceIndex_.get() ), SIZEOF_T * 4 * tetraCount_ );

		faceFile.close();

		return true;
	}

};

#endif