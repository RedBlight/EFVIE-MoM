#include "TetraMeshFile.hpp"
#include "_EmRule.hpp"
#include "EmPropFile.hpp"
#include "EmRuleFile.hpp"
#include <iostream>
#include <chrono>

using namespace std;

int main( int argc, char *argv[] )
{
	auto tStart = chrono::high_resolution_clock::now();

	cout << "# Electrify #" << endl;



	/**
		Some Sanity checks first.
	**/

	if( argc < 4 )
	{
		cout << "Not enough arguments! Please specify 'input.tetramesh', 'input.emrule', and 'output.emprops' filenames as 1st, 2nd, and 3rd arguments.";
		return 1;
	}

	if( argc > 4 )
	{
		cout << "More than 3 arguments!?! There is something fishy here! Please specify 'input.tetramesh', 'input.emrule', and 'output.emprops' filenames as 1st, 2nd, and 3rd arguments, and nothing more.";
		return 1;
	}

	string meshFileName = argv[ 1 ];
	string ruleFileName = argv[ 2 ];
	string propFileName = argv[ 3 ];

	ifstream meshFileTest( meshFileName ); 
	if( !meshFileTest )
	{
		cout << meshFileName << " file doesn't exist." << endl;
		return 1;
	}
	meshFileTest.close();

	ifstream ruleFileTest( ruleFileName ); 
	if( !ruleFileTest )
	{
		cout << ruleFileName << " file doesn't exist." << endl;
		return 1;
	}
	ruleFileTest.close();



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
		Load .emrule file.
	**/

	cout << "Loading " << ruleFileName << " file... ";

    EmRuleFile< double > ruleFile;

	bool ruleLoaded = ruleFile.Load_emrule( ruleFileName );

	if( !ruleLoaded )
	{
		cout << "An error occured while loading!" << endl;
		return 1;
	}

	cout << "Done!" << endl;



	/**
		Generate .emprops
	**/

	cout << "Generating " << propFileName << " file... ";

	EmPropFile< double > propFile;
	propFile.Initialise( meshFile.tetraCount_ );

	EmRuleFactory< double >& ruleFactory = EmRuleFactory< double >::GetInstance();

	shared_ptr< EmRule< double > > emRule = ruleFactory.GetRule( ruleFile.ruleType_ );
	if( emRule == nullptr )
	{
		cout << "Error!" << endl;
		cout << "Specified EmRule type is not defined: " << ruleFile.ruleType_ << endl;
		return 1;
	}
	
	bool emRuleSet = emRule->SetRuleData( ruleFile.ruleData_ );
	if( !emRuleSet )
	{
		cout << "Error!" << endl;
		cout << "There are some proplems in the EmRule parameters!" << endl;
		return 1;
	}
	
	bool propsGenerated = emRule->GenerateProps( propFile.emPropData_, meshFile.vertexData_, meshFile.tetraVertexIndex_, propFile.tetraCount_ );
	if( !propsGenerated )
	{
		cout << "Error!" << endl;
		cout << "A proplem encountered whlie generating EmProp data!" << endl;
		return 1;
	}
	
	bool propfileSaved = propFile.Save_emprop( propFileName );
	if( !propfileSaved )
	{
		cout << "Error!" << endl;
		cout << "A proplem encountered whlie saving " << propFileName << " file!" << endl;
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