#include "TetraMeshFile.hpp"
#include <iostream>
#include <chrono>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	cout << "# MakeMesh #" << endl;

	/**
		Some Sanity checks first.
	**/

	if( argc < 3 )
	{
		cout << "Not enough arguments! Please specify 'input.unv' and 'output.tetramesh' filenames as 1st and 2nd arguments.";
		return 1;
	}

	if( argc > 3 )
	{
		cout << "More than 2 arguments!?! There is something fishy here! Please specify 'input.unv' and 'output.tetramesh' filenames as 1st and 2nd arguments, and nothing more.";
		return 1;
	}

	string inputFileName = argv[ 1 ];
	string outputFileName = argv[ 2 ];

	ifstream inputTest( inputFileName ); 
	if( !inputTest )
	{
		cout << "Input file doesn't exist." << endl;
		return 1;
	}
	inputTest.close();



	/**
		Load .unv file.
	**/

	cout << "Loading " << inputFileName << " file... ";

    TetraMeshFile< double > meshFile;

	bool unvLoaded = meshFile.Load_unv( inputFileName );

	if( !unvLoaded )
	{
		cout << "An error occured while loading!" << endl;
		return 1;
	}

	cout << "Done!" << endl;
	cout << "Vertex count: " << meshFile.vertexCount_ << endl;
	cout << "Tetrahedron count: " << meshFile.tetraCount_ << endl;



	/**
		Save .tetramesh file.
	**/

	cout << "Generating " << outputFileName << " file... ";

	bool meshSaved = meshFile.Save_tetramesh( outputFileName );

	if( !meshSaved )
	{
		cout << "An error occured!" << endl;
		return 1;
	}

	cout << "Done!" << endl;

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< double > tMsec = tDiff;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}