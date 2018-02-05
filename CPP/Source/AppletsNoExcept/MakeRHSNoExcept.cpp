#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	af::setDevice( 0 );

	cout << "# MakeRHS #" << endl;

	string meshFileName = argv[ 1 ];
	string faceFileName = argv[ 2 ];
	string tetqFileName = argv[ 3 ];
	string waveFileName = argv[ 4 ];
	string rhsFileName = argv[ 5 ];

    TetraMeshFile< FLOAT_T > meshFile;
	meshFile.Load( meshFileName );

    TetraFaceFile< FLOAT_T > faceFile;
	faceFile.Load( faceFileName );

    TetraQuadFile< FLOAT_T > tetqFile;
	tetqFile.Load( tetqFileName );

    IncWaveFile< FLOAT_T > waveFile;
	waveFile.Load( waveFileName );

	IncWaveFactory< FLOAT_T >& waveFac = IncWaveFactory< FLOAT_T >::GetInstance();
	shared_ptr< IncWave< FLOAT_T > > incWave = waveFac.GetWave( waveFile.waveType_ );
	incWave->SetWaveData( waveFile.waveData_ );

	RhsFile< FLOAT_T > rhsFile;
	rhsFile.Initialize( faceFile.faceCount_ );

	RhsGenerator< FLOAT_T > rhsGenerator(
		faceFile.faceCount_,
		tetqFile.quadCount_,
		meshFile.vertexData_,
		tetqFile.quadData_,
		meshFile.tetraVertexIndex_,
		faceFile.faceVertexIndex_,
		faceFile.faceTetraIndex_,
		faceFile.tetraFaceIndex_,
		incWave.get(),
		rhsFile.rhsData_
	);

	rhsGenerator.ThreadedGenerate();

	rhsFile.Save( rhsFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;
	
	cout << "Tetrahedron count: " << meshFile.tetraCount_ << endl;
	cout << "TetraQuad count: " << tetqFile.quadCount_ << endl;
	cout << "Face count: " << faceFile.faceCount_ << endl;
	cout << "Thread count: " << rhsGenerator.threadCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	cout << "RHS:" << endl;
	for( size_t idx = 0; idx < 1; ++idx )
	{
		cout << rhsFile.rhsData_.get()[7*idx] << " ";
		cout << rhsFile.rhsData_.get()[7*idx+1] << " ";
		cout << rhsFile.rhsData_.get()[7*idx+2] << " ";
		cout << rhsFile.rhsData_.get()[7*idx+3] << " ";
		cout << rhsFile.rhsData_.get()[7*idx+4] << " ";
		cout << rhsFile.rhsData_.get()[7*idx+5] << " ";
		cout << rhsFile.rhsData_.get()[7*idx+6] << endl;
	}

	return 0;
}