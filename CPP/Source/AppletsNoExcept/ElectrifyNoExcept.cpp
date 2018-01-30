#include <_BitDepthDefines.hpp>

#include <iostream>
#include <chrono>

#include <EFVIEMOM.hpp>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();
	
	af::setDevice( 0 );

	cout << "# Electrify #" << endl;

	string meshFileName = argv[ 1 ];
	string ruleFileName = argv[ 2 ];
	string propFileName = argv[ 3 ];

	TetraMeshFile< FLOAT_T > meshFile;
	meshFile.Load_tetramesh( meshFileName );

    EmRuleFile< FLOAT_T > ruleFile;
	ruleFile.Load_emrule( ruleFileName );

	EmPropFile< FLOAT_T > propFile;
	propFile.Initialise( meshFile.tetraCount_ );

	EmRuleFactory< FLOAT_T >& ruleFactory = EmRuleFactory< FLOAT_T >::GetInstance();
	shared_ptr< EmRule< FLOAT_T > > emRule = ruleFactory.GetRule( ruleFile.ruleType_ );
	emRule->SetRuleData( ruleFile.ruleData_ );
	emRule->GenerateProps( propFile.emPropData_, meshFile.vertexData_, meshFile.tetraVertexIndex_, propFile.tetraCount_ );
	propFile.Save_emprop( propFileName );

	auto tEnd = chrono::high_resolution_clock::now();
	auto tDiff = tEnd - tStart;

	chrono::duration< FLOAT_T > tMsec = tDiff;

	cout << "EmRule Type: " << ruleFile.ruleType_ << endl;
	cout << "Tetrahedron Count: " << propFile.tetraCount_ << endl;

	cout << "# Finished in " << tMsec.count() << " sec. #" << endl;
	cout << endl;

	return 0;
}