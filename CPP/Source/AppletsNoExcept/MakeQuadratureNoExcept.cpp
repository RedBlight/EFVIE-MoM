#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	af::setDevice( 0 );

	cout << "# Make Quadrature #" << endl;

	string meshFileName = argv[ 1 ];
	string faceFileName = argv[ 2 ];
	string tetqFileName = argv[ 3 ];
	string triqFileName = argv[ 4 ];

    TetraMeshFile< FLOAT_T > meshFile;
	meshFile.Load( meshFileName );

    TetraFaceFile< FLOAT_T > faceFile;
	faceFile.Load( faceFileName );

	TetraQuadGenerator64< FLOAT_T > tetQuadGen;
	TetraQuadFile< FLOAT_T > tetqFile;
	tetqFile.Initialise( meshFile.tetraCount_, 4 );
	tetQuadGen.Generate( tetqFile.quadData_, meshFile.tetraVertexIndex_.operator->(), meshFile.vertexData_.operator->(), meshFile.tetraCount_ );
	tetqFile.Save_tetraquad( tetqFileName );

	TriQuadGenerator7< FLOAT_T > triQuadGen;
	TriQuadFile< FLOAT_T > triqFile;
	triqFile.Initialise( faceFile.faceCount_, 7 );
	triQuadGen.Generate( triqFile.quadData_, faceFile.faceVertexIndex_, meshFile.vertexData_, faceFile.faceCount_ );
	triqFile.Save_triquad( triqFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;
	
	cout << "Tetrahedron count: " << meshFile.tetraCount_ << endl;
	cout << "TetraQuad count: " << tetqFile.quadCount_ << endl;
	cout << "Face count: " << faceFile.faceCount_ << endl;
	cout << "TriQuad count: " << triqFile.quadCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}