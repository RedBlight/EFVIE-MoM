#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	af::setDevice( 0 );

	cout << "# Make RHS #" << endl;

		//const UINT_T& faceCount,
		//const shared_ptr< T >& vertexData,
		//const shared_ptr< UINT_T >& tetraVertexIndex,
		//const shared_ptr< UINT_T >& faceVertexIndex,
		//const shared_ptr< UINT_T >& faceTetraIndex,
		//const shared_ptr< UINT_T >& tetraFaceIndex,
		//const UINT_T& quadCount,
		//const shared_ptr< T >& tetraQuadData,
		//const IncWave< T >* incWave,
		//shared_ptr< complex< T > >& rhsData // preallocated

	/*
	.tetramesh
	.tetraface
	.tetraquad
	.incwave

	save
	.rhs
	*/

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

	RhsGenerator< FLOAT_T > rhsGenerator;
	rhsGenerator.Generate(
		faceFile.faceCount_,
		meshFile.vertexData_,
		meshFile.tetraVertexIndex_,
		faceFile.faceVertexIndex_,
		faceFile.faceTetraIndex_,
		faceFile.tetraFaceIndex_,
		tetqFile.quadCount_,
		tetqFile.quadData_,
		incWave.get(),
		rhsFile.rhsData_
	);

	rhsFile.Save( rhsFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;
	
	cout << "Tetrahedron count: " << meshFile.tetraCount_ << endl;
	cout << "TetraQuad count: " << tetqFile.quadCount_ << endl;
	cout << "Face count: " << faceFile.faceCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}