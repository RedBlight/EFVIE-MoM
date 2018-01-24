#include "TetraMeshFile.hpp"
#include "TetraQuadFile.hpp"
#include "TetraQuadGenerator4.hpp"
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

	if( argc < 3 )
	{
		cout << "Not enough arguments! Please specify 'input.tetramesh', and 'output.tetraquad' filenames as 1st and 2rd arguments.";
		return 1;
	}

	if( argc > 3 )
	{
		cout << "More than 3 arguments!?! There is something fishy here! Please specify 'input.tetramesh', and 'output.tetraquad' filenames as 1st and 2rd arguments, and nothing more.";
		return 1;
	}

	string meshFileName = argv[ 1 ];
	string quadFileName = argv[ 2 ];

	ifstream meshFileTest( meshFileName ); 
	if( !meshFileTest )
	{
		cout << meshFileName << " file doesn't exist." << endl;
		return 1;
	}
	meshFileTest.close();



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
		Generate .tetraquad
	**/

	cout << "Generating " << quadFileName << " file... ";

	// add more checks here

	TetraQuadGenerator4< double > quadGen;
	TetraQuadFile< double > quadFile;
	quadFile.Initialise( meshFile.tetraCount_, 4 );
	quadGen.Generate( quadFile.quadData_, meshFile.tetraVertexIndex_, meshFile.vertexData_, meshFile.tetraCount_ );
	quadFile.Save_tetraquad( quadFileName );

	cout << "Done!" << endl;



	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< double > tMsec = tDiff;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}