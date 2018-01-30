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



	/**
		Some Sanity checks first.
	**/

	if( argc < 3 )
	{
		cout << "Not enough arguments! Please specify 'input.tetramesh' and 'output.tetraface' filenames as 1st and 2nd arguments.";
		return 1;
	}

	if( argc > 3 )
	{
		cout << "More than 2 arguments!?! There is something fishy here! Please specify 'input.tetramesh' and 'output.tetraface' filenames as 1st and 2nd arguments, and nothing more.";
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
		Load .tetramesh file.
	**/

	cout << "Loading " << inputFileName << " file... ";

    TetraFaceFile< FLOAT_T > faceFile;

	bool faceLoaded = faceFile.Load_tetramesh( inputFileName );

	if( !faceLoaded )
	{
		cout << "An error occured while loading!" << endl;
		return 1;
	}

	cout << "Done!" << endl;
	cout << "Tetrahedron count: " << faceFile.tetraCount_ << endl;
	cout << "Triangle count: " << faceFile.faceCount_ << endl;



	/**
		Save .tetraface file.
	**/

	cout << "Generating " << outputFileName << " file... ";

	bool faceSaved = faceFile.Save_tetraface( outputFileName );

	if( !faceSaved )
	{
		cout << "An error occured!" << endl;
		return 1;
	}

	cout << "Done!" << endl;

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}