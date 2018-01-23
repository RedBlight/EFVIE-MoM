#ifndef TETRA_FACE_FILE_INCLUDED
#define TETRA_FACE_FILE_INCLUDED

#include <fstream>
#include <string>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>
#include <utility>
#include <map>
#include "arrayfire.h"
#include "StringFuncs.hpp"
#include "TetraMeshFile.hpp"
#include "TetraFace.hpp"

using namespace std;

// some string functions
//namespace __STRF
//{
//	void SearchAndReplace( string& source, const string& find, const string& replace )
//	{
//		for( size_t i = 0; ( i = source.find( find, i ) ) != string::npos; )
//		{
//			source.replace( i, find.length(), replace );
//			i += replace.length();
//		}
//	}
//
//	// trim from start (in place)
//	static inline void ltrim(std::string &s) {
//		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
//			return !std::isspace(ch);
//		}));
//	}
//
//	// trim from end (in place)
//	static inline void rtrim(std::string &s) {
//		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
//			return !std::isspace(ch);
//		}).base(), s.end());
//	}
//
//	// trim from both ends (in place)
//	static inline void Trim(std::string &s) {
//		ltrim(s);
//		rtrim(s);
//	}
//
//	std::vector<std::string> Explode( const string& s, char delim )
//	{
//		std::vector<std::string> result;
//		std::istringstream iss(s);
//
//		for (std::string token; std::getline(iss, token, delim); )
//		{
//			result.push_back(std::move(token));
//		}
//
//		return result;
//	}
//}

/*
.tetraface file: each cell is 8 byte
[0] = trig count (size_t)
[1] = tetrahedron count (size_t)
[A ... B-1] = v1 v2 v3 (uint64) 8 byte X 3 X [0] -> face vertex indices
[B ... C-1] = t1 t2 (uint64) 8 byte X 2 X [0] -> face tetrahedron indices, -1 means null
[C ... D-1] = f1 f2 f3 f4 (uint64) 8 byte X 4 X [1] -> tetrahedron face indices

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
	size_t faceCount_;
	size_t tetraCount_;
	size_t* faceVertexIndex_;
	size_t* faceTetraIndex_;
	size_t* tetraFaceIndex_;

public:

	TetraFaceFile() :
		init_( false ),
		faceCount_( 0 ),
		tetraCount_( 0 ),
		faceVertexIndex_( nullptr ),
		faceTetraIndex_( nullptr ),
		tetraFaceIndex_( nullptr )
	{

	}

	~TetraFaceFile()
	{
		if( init_ )
		{
			delete[] faceVertexIndex_;
			delete[] faceTetraIndex_;
			delete[] tetraFaceIndex_;
		}
	}

	void Reset()
	{
		if( init_ )
		{
			init_ = false;
			faceCount_ = 0;
			tetraCount_ = 0;
			delete[] faceVertexIndex_;
			delete[] faceTetraIndex_;
			delete[] tetraFaceIndex_;
			faceVertexIndex_ = nullptr;
			faceTetraIndex_ = nullptr;
			tetraFaceIndex_ = nullptr;
		}
	}

	bool Load_tetramesh( const string& filePath )
	{
		using namespace af;

		Reset();

		TetraMeshFile< T > meshFile;

		bool meshLoaded = meshFile.Load_tetramesh( filePath );

		if( !meshLoaded )
		{
			return false;
		}

		

		////// DEBUG
		//cout << endl;
		//cout << meshFile.tetraVertexIndex_[0] << " " << meshFile.tetraVertexIndex_[1] << " " << meshFile.tetraVertexIndex_[2] << " " << meshFile.tetraVertexIndex_[3] << " " << endl;
		//cout << meshFile.tetraVertexIndex_[4] << " " << meshFile.tetraVertexIndex_[5] << " " << meshFile.tetraVertexIndex_[6] << " " << meshFile.tetraVertexIndex_[7] << " " << endl;
		//cout << meshFile.tetraVertexIndex_[8] << " " << meshFile.tetraVertexIndex_[9] << " " << meshFile.tetraVertexIndex_[10] << " " << meshFile.tetraVertexIndex_[11] << " " << endl;
		//cout << meshFile.tetraVertexIndex_[12] << " " << meshFile.tetraVertexIndex_[13] << " " << meshFile.tetraVertexIndex_[14] << " " << meshFile.tetraVertexIndex_[15] << " " << endl;
		//cout << meshFile.tetraVertexIndex_[16] << " " << meshFile.tetraVertexIndex_[17] << " " << meshFile.tetraVertexIndex_[18] << " " << meshFile.tetraVertexIndex_[19] << " " << endl;
		//cout << endl;
		//////

		tetraCount_ = meshFile.tetraCount_;
		tetraFaceIndex_ = new size_t[ 4 * tetraCount_ ];
		meshFile.tetraVertexIndex_ = af::sort( af::array( 4, meshFile.tetraCount_, meshFile.tetraVertexIndex_, afHost ) ).host< size_t >();
		


		
		//af::array indexData( 4, meshFile.tetraCount_, meshFile.tetraVertexIndex_, afHost );

		//af::array sortedData = indexData;
		//sortedData = af::sort( sortedData );

		//meshFile.tetraVertexIndex_ = sortedData.host< size_t >();

		////////////////////////////

		

		//size_t arr[] = {1,8,99,2, 3,83,356,8, 32,1,24,457, 8,7,3,9};


		//
		//af::array aba( 4, 5, arr, afHost );
		////meshFile.tetraVertexIndex_ = af::sort( indexData, 0, true ).host< size_t >();

		//af_print(aba.cols(0,3));

		//af::array ana = af::sort( aba );

		//
		//af_print(ana.cols(0,3));


		//
		//af::array indexData( 4, meshFile.tetraCount_, meshFile.tetraVertexIndex_, afHost );
		////meshFile.tetraVertexIndex_ = af::sort( indexData, 0, true ).host< size_t >();

		//size_t N = meshFile.tetraCount_-1;

		//af_print(indexData.cols(0,8) );
		//af_print(indexData.cols(N-8,N) );

		//af::array sortedData = indexData;
		//sortedData = af::sort( sortedData.cols(0,N) );

		//af_print(sortedData.cols(0,8));
		//af_print(sortedData.cols(N-8,N));

		//meshFile.tetraVertexIndex_ = sortedData.host< size_t >();
		

		////// DEBUG
		//cout << endl;
		//cout << meshFile.tetraVertexIndex_[0] << " " << meshFile.tetraVertexIndex_[1] << " " << meshFile.tetraVertexIndex_[2] << " " << meshFile.tetraVertexIndex_[3] << " " << endl;
		//cout << meshFile.tetraVertexIndex_[4] << " " << meshFile.tetraVertexIndex_[5] << " " << meshFile.tetraVertexIndex_[6] << " " << meshFile.tetraVertexIndex_[7] << " " << endl;
		//cout << meshFile.tetraVertexIndex_[8] << " " << meshFile.tetraVertexIndex_[9] << " " << meshFile.tetraVertexIndex_[10] << " " << meshFile.tetraVertexIndex_[11] << " " << endl;
		//cout << meshFile.tetraVertexIndex_[12] << " " << meshFile.tetraVertexIndex_[13] << " " << meshFile.tetraVertexIndex_[14] << " " << meshFile.tetraVertexIndex_[15] << " " << endl;
		//cout << meshFile.tetraVertexIndex_[16] << " " << meshFile.tetraVertexIndex_[17] << " " << meshFile.tetraVertexIndex_[18] << " " << meshFile.tetraVertexIndex_[19] << " " << endl;
		//cout << endl;
		//////

		vector< TetraFace< size_t > > faceList;
		faceList.reserve( 4 * meshFile.tetraCount_ );

		map< string, size_t > faceMap;

		size_t* iPtr = meshFile.tetraVertexIndex_;

		for( size_t idt = 0; idt < tetraCount_; ++idt )
		{
			size_t iVert[4];
			iVert[0] = 4 * idt;
			iVert[1] = iVert[0] + 1;
			iVert[2] = iVert[0] + 2;
			iVert[3] = iVert[0] + 3;
			
			TetraFace< size_t > tetraFace[4];
			tetraFace[0] = TetraFace< size_t >( iPtr[ iVert[0] ], iPtr[ iVert[1] ], iPtr[ iVert[2] ] );
			tetraFace[1] = TetraFace< size_t >( iPtr[ iVert[0] ], iPtr[ iVert[1] ], iPtr[ iVert[3] ] );
			tetraFace[2] = TetraFace< size_t >( iPtr[ iVert[0] ], iPtr[ iVert[2] ], iPtr[ iVert[3] ] );
			tetraFace[3] = TetraFace< size_t >( iPtr[ iVert[1] ], iPtr[ iVert[2] ], iPtr[ iVert[3] ] );

			//cout << "A" << endl;
			
			string faceHash[4];
			faceHash[0] = tetraFace[0].MakeHash();
			faceHash[1] = tetraFace[1].MakeHash();
			faceHash[2] = tetraFace[2].MakeHash();
			faceHash[3] = tetraFace[3].MakeHash();
			
			//cout << "A1" << endl;

			for( size_t idf = 0; idf < 4; ++idf )
			{
				auto itMap = faceMap.find( faceHash[ idf ] );
				//cout << "A2" << endl;
				if( itMap == faceMap.end() )
				{
					//cout << "A3" << endl;
					tetraFace[ idf ].t1_ = idt + 1;
					//cout << "A4" << endl;
					faceList.push_back( tetraFace[ idf ] );
					//cout << "A5" << endl;
					size_t faceIndex = faceList.size();
					//cout << "A6" << endl;
					faceMap[ faceHash[ idf ] ] = faceIndex;
					//cout << "A7" << endl;
					tetraFaceIndex_[ iVert[ idf ] ] = faceIndex;
				}
				else
				{
					size_t faceIndex = itMap->second;
					faceList[ faceIndex ].t2_ = idt + 1;
					tetraFaceIndex_[ iVert[ idf ] ] = faceIndex;
				}
			}

			//cout << "B" << endl;
		}

		faceList.shrink_to_fit();
		faceCount_ = faceList.size();

		faceVertexIndex_ = new size_t[ 3 * faceCount_ ];
		faceTetraIndex_ = new size_t[ 2 * faceCount_ ];

		/**
			Think of a different way to alleviate this extra copy below.
		**/

		//cout << "C" << endl;

		for( size_t idf = 0; idf < faceCount_; ++idf )
		{
			size_t iVert[3];
			iVert[0] = 3 * idf;
			iVert[1] = iVert[0] + 1;
			iVert[2] = iVert[0] + 2;

			size_t iTet[2];
			iTet[0] = 2 * idf;
			iTet[1] = iTet[0] + 1;

			faceVertexIndex_[ iVert[0] ] = faceList[ idf ].v1_;
			faceVertexIndex_[ iVert[1] ] = faceList[ idf ].v2_;
			faceVertexIndex_[ iVert[2] ] = faceList[ idf ].v3_;

			faceTetraIndex_[ iTet[0] ] = faceList[ idf ].t1_;
			faceTetraIndex_[ iTet[1] ] = faceList[ idf ].t2_;
		}

		init_ = true;

		return true;
	}

	bool Load_tetraface( const string& filePath )
	{
		Reset();

		fstream tetrafaceFile( filePath, ios::in | ios::binary );

		if( !tetrafaceFile.good() )
		{
			tetrafaceFile.close();
			return false;
		}

		tetrafaceFile.read( ( char* )&faceCount_, 8 );
		tetrafaceFile.read( ( char* )&tetraCount_, 8 );

		faceVertexIndex_ = new size_t[ 3 * faceCount_ ];
		faceTetraIndex_ = new size_t[ 2 * faceCount_ ];
		tetraFaceIndex_ = new size_t[ 4 * tetraCount_ ];

		tetrafaceFile.read( ( char* )faceVertexIndex_, 8 * 3 * faceCount_ );
		tetrafaceFile.read( ( char* )faceTetraIndex_, 8 * 2 * faceCount_ );
		tetrafaceFile.read( ( char* )tetraFaceIndex_, 8 * 4 * tetraCount_ );

		tetrafaceFile.close();

		init_ = true;

		return true;
	}

	bool Save_tetraface( const string& filePath )
	{
		fstream tetrafaceFile( filePath, ios::trunc | ios::out | ios::binary );

		if( !tetrafaceFile.good() )
		{
			tetrafaceFile.close();
			return false;
		}

		tetrafaceFile.write( ( char* )&faceCount_, 8 );
		tetrafaceFile.write( ( char* )&tetraCount_, 8 );

		tetrafaceFile.write( ( char* )faceVertexIndex_, 8 * 3 * faceCount_ );
		tetrafaceFile.write( ( char* )faceTetraIndex_, 8 * 2 * faceCount_ );
		tetrafaceFile.write( ( char* )tetraFaceIndex_, 8 * 4 * tetraCount_ );

		tetrafaceFile.close();

		return true;
	}

};

#endif