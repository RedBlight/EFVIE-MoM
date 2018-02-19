#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	af::setDevice( 0 );

	cout << "# MakeMoM #" << endl;

	string meshFileName = argv[ 1 ];
	string faceFileName = argv[ 2 ];
	string tetqFileName = argv[ 3 ];
	string triqFileName = argv[ 4 ];
	string propFileName = argv[ 5 ];
	string waveFileName = argv[ 6 ];
	string momFileName = argv[ 7 ];

    TetraMeshFile< FLOAT_T > meshFile;
	meshFile.Load( meshFileName );

    TetraFaceFile< FLOAT_T > faceFile;
	faceFile.Load( faceFileName );

    TetraQuadFile< FLOAT_T > tetqFile;
	tetqFile.Load( tetqFileName );

    TetraQuadFile< FLOAT_T > triqFile;
	triqFile.Load( triqFileName );

    EmPropFile< FLOAT_T > propFile;
	propFile.Load( propFileName );

    IncWaveFile< FLOAT_T > waveFile;
	waveFile.Load( waveFileName );

	/////////////////////

	
	//cout << "vertCount: " << meshFile.vertexCount_ << endl;
	//cout << "tetCount: " << meshFile.tetraCount_ << endl;
	//cout << "faceCount: " << faceFile.faceCount_ << endl;
	//cout << "tetCount: " << faceFile.tetraCount_ << endl;
	//cout << "tetVert:" << endl;
	//for( size_t idt = 0; idt < meshFile.tetraCount_; ++idt )
	//{
	//	cout << meshFile.tetraVertexIndex_.get()[4*idt] << " ";
	//	cout << meshFile.tetraVertexIndex_.get()[4*idt+1] << " ";
	//	cout << meshFile.tetraVertexIndex_.get()[4*idt+2] << " ";
	//	cout << meshFile.tetraVertexIndex_.get()[4*idt+3] << endl;
	//}
	//
	//cout << "faceVert:" << endl;
	//for( size_t idt = 0; idt < faceFile.faceCount_; ++idt )
	//{
	//	cout << faceFile.faceVertexIndex_.get()[3*idt] << " ";
	//	cout << faceFile.faceVertexIndex_.get()[3*idt+1] << " ";
	//	cout << faceFile.faceVertexIndex_.get()[3*idt+2] << endl;
	//}
	//
	//cout << "faceTet:" << endl;
	//for( size_t idt = 0; idt < faceFile.faceCount_; ++idt )
	//{
	//	cout << faceFile.faceTetraIndex_.get()[2*idt] << " ";
	//	cout << faceFile.faceTetraIndex_.get()[2*idt+1] << endl;
	//}


	/////////////////////

	IncWaveFactory< FLOAT_T >& waveFac = IncWaveFactory< FLOAT_T >::GetInstance();
	shared_ptr< IncWave< FLOAT_T > > incWave = waveFac.GetWave( waveFile.waveType_ );
	incWave->SetWaveData( waveFile.waveData_ );

	MomMatrixFile< FLOAT_T > momFile;
	momFile.Initialize( faceFile.faceCount_ );

	MomGeneratorCpuRdc< FLOAT_T > momGenerator(
		incWave->WaveNumber(),
		meshFile.tetraCount_,
		faceFile.faceCount_,
		tetqFile.quadCount_,
		triqFile.quadCount_,
		meshFile.vertexData_,
		tetqFile.quadData_,
		triqFile.quadData_,
		propFile.emPropData_,
		meshFile.tetraVertexIndex_,
		faceFile.faceVertexIndex_,
		faceFile.faceTetraIndex_,
		faceFile.tetraFaceIndex_,
		momFile.momData_
	);

	

	//MomGeneratorCpu< FLOAT_T > momGenerator(
	//	incWave->WaveNumber(),
	//	faceFile.faceCount_,
	//	tetqFile.quadCount_,
	//	triqFile.quadCount_,
	//	meshFile.vertexData_,
	//	tetqFile.quadData_,
	//	triqFile.quadData_,
	//	propFile.emPropData_,
	//	meshFile.tetraVertexIndex_,
	//	faceFile.faceVertexIndex_,
	//	faceFile.faceTetraIndex_,
	//	faceFile.tetraFaceIndex_,
	//	momFile.momData_
	//);

	//cout << "A" << endl;

	momGenerator.ThreadedGenerate();
	//momGenerator.Generate();

	momFile.Save( momFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;
	
	cout << "Tetrahedron count: " << meshFile.tetraCount_ << endl;
	cout << "TetraQuad count: " << tetqFile.quadCount_ << endl;
	cout << "Face count: " << faceFile.faceCount_ << endl;
	cout << "Thread count: " << momGenerator.threadCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	/*
	cout << "MOMMAT:" << endl;
	for( size_t idx = 0; idx < faceFile.faceCount_; ++idx )
	{
		cout << momFile.momData_.get()[7*idx] << " ";
		cout << momFile.momData_.get()[7*idx+1] << " ";
		cout << momFile.momData_.get()[7*idx+2] << " ";
		cout << momFile.momData_.get()[7*idx+3] << " ";
		cout << momFile.momData_.get()[7*idx+4] << " ";
		cout << momFile.momData_.get()[7*idx+5] << " ";
		cout << momFile.momData_.get()[7*idx+6] << endl;
	}
	*/

	return 0;
}
