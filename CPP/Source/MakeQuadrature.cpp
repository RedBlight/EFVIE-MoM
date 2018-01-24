#include "TetraMeshFile.hpp"
#include "TetraFaceFile.hpp"
#include "TetraQuadFile.hpp"
#include "TriQuadFile.hpp"
#include "TetraQuadGenerator4.hpp"
#include "TriQuadGenerator7.hpp"
#include <iostream>
#include <chrono>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	cout << "# Make Quadrature #" << endl;

	af::setDevice(0);



	/**
		Some Sanity checks first.
	**/

	if( argc < 5 )
	{
		cout << "Not enough arguments! Please specify 'input.tetramesh', 'input.tetraface', 'output.tetraquad', 'output.triquad' filenames as 1st and 2rd arguments.";
		return 1;
	}

	if( argc > 5 )
	{
		cout << "More than 4 arguments!?! There is something fishy here!";
		return 1;
	}

	string meshFileName = argv[ 1 ];
	string faceFileName = argv[ 2 ];
	string tetqFileName = argv[ 3 ];
	string triqFileName = argv[ 4 ];

	ifstream meshFileTest( meshFileName ); 
	if( !meshFileTest )
	{
		cout << meshFileName << " file doesn't exist." << endl;
		return 1;
	}
	meshFileTest.close();

	ifstream faceFileTest( faceFileName ); 
	if( !faceFileTest )
	{
		cout << faceFileName << " file doesn't exist." << endl;
		return 1;
	}
	faceFileTest.close();



	/**
		Load .tetramesh file.
	**/

	cout << "Loading " << meshFileName << " file... ";

    TetraMeshFile< double > meshFile;

	bool meshLoaded = meshFile.Load_tetramesh( meshFileName );

	if( !meshLoaded )
	{
		cout << "An error occured while loading!" << endl;
		return 1;
	}

	cout << "Done!" << endl;



	/**
		Load .tetraface file.
	**/

	cout << "Loading " << faceFileName << " file... ";

    TetraFaceFile< double > faceFile;

	bool faceLoaded = faceFile.Load_tetraface( faceFileName );

	if( !faceLoaded )
	{
		cout << "An error occured while loading!" << endl;
		return 1;
	}

	cout << "Done!" << endl;



	/**
		Generate .tetraquad
	**/

	cout << "Generating " << tetqFileName << " file... ";

	// add more checks here

	TetraQuadGenerator4< double > tetQuadGen;
	TetraQuadFile< double > tetqFile;
	tetqFile.Initialise( meshFile.tetraCount_, 4 );
	tetQuadGen.Generate( tetqFile.quadData_, meshFile.tetraVertexIndex_, meshFile.vertexData_, meshFile.tetraCount_ );
	tetqFile.Save_tetraquad( tetqFileName );

	cout << "Done!" << endl;



	/**
		Generate .triquad
	**/

	cout << "Generating " << triqFileName << " file... ";

	// add more checks here

	TriQuadGenerator7< double > triQuadGen;
	TriQuadFile< double > triqFile;
	triqFile.Initialise( faceFile.faceCount_, 7 );
	triQuadGen.Generate( triqFile.quadData_, faceFile.faceVertexIndex_, meshFile.vertexData_, faceFile.faceCount_ );
	triqFile.Save_triquad( triqFileName );

	cout << "Done!" << endl;


	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< double > tMsec = tDiff;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}