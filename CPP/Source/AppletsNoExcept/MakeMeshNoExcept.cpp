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

    TetraMeshFile< FLOAT_T > meshFile;
	meshFile.Load_unv( unvFileName );
	meshFile.Save_tetramesh( meshFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;
	
	cout << "Vertex count: " << meshFile.vertexCount_ << endl;
	cout << "Tetrahedron count: " << meshFile.tetraCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}