#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	af::setDevice( 0 );

	cout << "# SolveMoM #" << endl;

	string momFileName = argv[ 1 ];
	string rhsFileName = argv[ 2 ];
	string coefFileName = argv[ 3 ];

    MomMatrixFile< FLOAT_T > momFile;
	momFile.Load( momFileName );

    RhsFile< FLOAT_T > rhsFile;
	rhsFile.Load( rhsFileName );

	CoefFile< FLOAT_T > coefFile;
	coefFile.Initialize( momFile.faceCount_ );

	MomSolver< FLOAT_T > momSolver(
		momFile.faceCount_,
		momFile.momData_,
		rhsFile.rhsData_,
		coefFile.coefData_
	);

	momSolver.Solve();

	coefFile.Save( coefFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;

	cout << "Face count: " << momFile.faceCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	/*
	cout << "COEFVEC:" << endl;
	for( size_t idx = 0; idx < 1; ++idx )
	{
		cout << coefFile.coefData_.get()[7*idx] << " ";
		cout << coefFile.coefData_.get()[7*idx+1] << " ";
		cout << coefFile.coefData_.get()[7*idx+2] << " ";
		cout << coefFile.coefData_.get()[7*idx+3] << " ";
		cout << coefFile.coefData_.get()[7*idx+4] << " ";
		cout << coefFile.coefData_.get()[7*idx+5] << " ";
		cout << coefFile.coefData_.get()[7*idx+6] << endl;
	}
	*/

	return 0;
}
