#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	af::setDevice( 0 );

	cout << "# MakeMesh #" << endl;

	string unvFileName = argv[ 1 ];
	string meshFileName = argv[ 2 ];

	UnvTetraMeshFile< FLOAT_T > unvFile;
	unvFile.Load( unvFileName );

    TetraMeshFile< FLOAT_T > meshFile;
	meshFile.Initialize( unvFile.vertexCount_, unvFile.tetraCount_, unvFile.vertexData_, unvFile.tetraVertexIndex_ );
	meshFile.Save( meshFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;
	
	cout << "Vertex count: " << meshFile.vertexCount_ << endl;
	cout << "Tetrahedron count: " << meshFile.tetraCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}