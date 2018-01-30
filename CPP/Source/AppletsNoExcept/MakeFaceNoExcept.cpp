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

    TetraFaceFile< FLOAT_T > faceFile;

	faceFile.Load_tetramesh( meshFileName );
	faceFile.Save_tetraface( faceFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;

	cout << "Tetrahedron count: " << faceFile.tetraCount_ << endl;
	cout << "Triangle count: " << faceFile.faceCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}