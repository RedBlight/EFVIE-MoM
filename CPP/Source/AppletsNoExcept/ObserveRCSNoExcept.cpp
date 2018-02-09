#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	af::setDevice( 0 );

	cout << "# ObserveRCS #" << endl;

	string meshFileName = argv[ 1 ];
	string faceFileName = argv[ 2 ];
	string tetqFileName = argv[ 3 ];
	string propFileName = argv[ 4 ];
	string waveFileName = argv[ 5 ];
	string coefFileName = argv[ 6 ];
	string obsaFileName = argv[ 7 ];
	string rcsrFileName = argv[ 8 ];

    TetraMeshFile< FLOAT_T > meshFile;
	meshFile.Load( meshFileName );

    TetraFaceFile< FLOAT_T > faceFile;
	faceFile.Load( faceFileName );

	TetraQuadFile< FLOAT_T > tetqFile;
	tetqFile.Load( tetqFileName );

	EmPropFile< FLOAT_T > propFile;
	propFile.Load( propFileName );

    IncWaveFile< FLOAT_T > waveFile;
	waveFile.Load( waveFileName );

	CoefFile< FLOAT_T > coefFile;
	coefFile.Load( coefFileName );

	ObsAngleFile< FLOAT_T > obsaFile;
	obsaFile.Load( obsaFileName );

	RcsResultFile< FLOAT_T > rcsrFile;
	rcsrFile.Initialize( obsaFile.obsCount_ );

	IncWaveFactory< FLOAT_T >& waveFac = IncWaveFactory< FLOAT_T >::GetInstance();
	shared_ptr< IncWave< FLOAT_T > > incWave = waveFac.GetWave( waveFile.waveType_ );
	incWave->SetWaveData( waveFile.waveData_ );

	RcsObserver< FLOAT_T > rcsObserver(
		incWave->WaveNumber(),
		obsaFile.obsCount_,
		faceFile.faceCount_,
		tetqFile.quadCount_,
		obsaFile.obsData_,
		meshFile.vertexData_,
		tetqFile.quadData_,
		propFile.emPropData_,
		coefFile.coefData_,
		meshFile.tetraVertexIndex_,
		faceFile.faceVertexIndex_,
		faceFile.faceTetraIndex_,
		faceFile.tetraFaceIndex_,
		rcsrFile.resultData_
	);

	rcsObserver.ThreadedGenerate();

	rcsrFile.Save( rcsrFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;

	cout << "Obs count: " << obsaFile.obsCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	//cout << "COEFVEC:" << endl;
	//for( size_t idx = 0; idx < 1; ++idx )
	//{
	//	cout << coefFile.coefData_.get()[7*idx] << " ";
	//	cout << coefFile.coefData_.get()[7*idx+1] << " ";
	//	cout << coefFile.coefData_.get()[7*idx+2] << " ";
	//	cout << coefFile.coefData_.get()[7*idx+3] << " ";
	//	cout << coefFile.coefData_.get()[7*idx+4] << " ";
	//	cout << coefFile.coefData_.get()[7*idx+5] << " ";
	//	cout << coefFile.coefData_.get()[7*idx+6] << endl;
	//}

	return 0;
}
