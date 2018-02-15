#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	af::setDevice( 0 );

	cout << "# MakeFace #" << endl;

	string meshFileName = argv[ 1 ];
	string faceFileName = argv[ 2 ];

    TetraMeshFile< FLOAT_T > meshFile;
	meshFile.Load( meshFileName );

	UINT_T faceCount = 0;
	shared_ptr< UINT_T > faceVertexIndex;
	shared_ptr< UINT_T > faceTetraIndex;
	shared_ptr< UINT_T > tetraFaceIndex;

	TetraFaceExtractor< FLOAT_T > extractor;

	extractor.Extract(
		meshFile.tetraCount_,
		meshFile.tetraVertexIndex_,
		
		faceCount,
		faceVertexIndex,
		faceTetraIndex,
		tetraFaceIndex
	);

    TetraFaceFile< FLOAT_T > faceFile;
	faceFile.Initialize(
		faceCount,
		meshFile.tetraCount_,
		faceVertexIndex,
		faceTetraIndex,
		tetraFaceIndex
	);

	faceFile.Save( faceFileName );
	meshFile.Save( meshFileName );


	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;

	cout << "Tetrahedron count: " << faceFile.tetraCount_ << endl;
	cout << "Triangle count: " << faceFile.faceCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}