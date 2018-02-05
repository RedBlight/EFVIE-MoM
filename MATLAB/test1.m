clc;
clear all;
close all;

system( "MakeMesh.exe Data/tetraPair.unv Data/tetraPair.tetramesh" );

system( "MakeFace.exe Data/tetraPair.tetramesh Data/tetraPair.tetraface" );

system( "Electrify.exe Data/tetraPair.tetramesh Data/tetraPair.emrule Data/tetraPair.emprops" );

system( "MakeQuad.exe Data/tetraPair.tetramesh Data/tetraPair.tetraface Data/tetraPair.tetraquad Data/tetraPair.triquad" );

system( "MakeRHS.exe Data/tetraPair.tetramesh Data/tetraPair.tetraface Data/tetraPair.tetraquad Data/tetraPair.incwave Data/tetraPair.rhs" );

system( "MakeMoM.exe Data/tetraPair.tetramesh Data/tetraPair.tetraface Data/tetraPair.tetraquad Data/tetraPair.triquad Data/tetraPair.emprops Data/tetraPair.incwave Data/tetraPair.mommatrix" );

system( "SolveMoM.exe Data/tetraPair.mommatrix Data/tetraPair.rhs Data/tetraPair.coef" );

% 	string meshFileName = argv[ 1 ];
% 	string faceFileName = argv[ 2 ];
% 	string tetqFileName = argv[ 3 ];
% 	string triqFileName = argv[ 4 ];
% 	string propFileName = argv[ 5 ];
% 	string waveFileName = argv[ 6 ];
% 	string momFileName = argv[ 7 ];
    
    