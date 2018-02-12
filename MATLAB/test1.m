clc;
clear all;
close all;

dataN = "Data/";
geomN = "sphere86";
ruleN = "eps2";
waveN = "PlaneZinYpol";
obspN = "xycircular361";

ruleF = " " + dataN + ruleN + ".emrule";
waveF = " " + dataN + waveN + ".incwave";
obspF = " " + dataN + obspN + ".obsangle";

unvmF = " " + dataN + geomN + ".unv";
meshF = " " + dataN + geomN + ".tetramesh";
faceF = " " + dataN + geomN + ".tetraface";
propF = " " + dataN + geomN + ".emprops";
tetqF = " " + dataN + geomN + ".tetraquad";
triqF = " " + dataN + geomN + ".triquad";
rhsvF = " " + dataN + geomN + ".rhs";
mommF = " " + dataN + geomN + ".mommatrix";
coefF = " " + dataN + geomN + ".coef";
rsltF = " " + dataN + geomN + ".rcsresult";

system( "MakeMesh.exe" + unvmF + meshF );
system( "MakeFace.exe" + meshF + faceF );
system( "Electrify.exe" + meshF + ruleF + propF );
system( "MakeQuad.exe" + meshF + faceF + tetqF + triqF );
system( "MakeRHS.exe" + meshF + faceF + tetqF + waveF + rhsvF );
system( "MakeMoM.exe" + meshF + faceF + tetqF + triqF + propF + waveF + mommF );
system( "SolveMoM.exe" + mommF + rhsvF + coefF );
system( "ObserveRCS.exe" + meshF + faceF + tetqF + propF + waveF + coefF + obspF + rsltF );


% system( "MakeMesh.exe Data/sphere52r1.unv Data/sphere52r1.tetramesh" );
% system( "MakeFace.exe Data/sphere52r1.tetramesh Data/sphere52r1.tetraface" );
% system( "Electrify.exe Data/sphere52r1.tetramesh Data/eps2.emrule Data/sphere52r1.emprops" );
% system( "MakeQuad.exe Data/sphere52r1.tetramesh Data/sphere52r1.tetraface Data/sphere52r1.tetraquad Data/sphere52r1.triquad" );
% system( "MakeRHS.exe Data/sphere52r1.tetramesh Data/sphere52r1.tetraface Data/sphere52r1.tetraquad Data/PlaneXinZpol.incwave Data/sphere52r1.rhs" );
% system( "MakeMoM.exe Data/sphere52r1.tetramesh Data/sphere52r1.tetraface Data/sphere52r1.tetraquad Data/sphere52r1.triquad Data/sphere52r1.emprops Data/PlaneXinZpol.incwave Data/sphere52r1.mommatrix" );
% system( "SolveMoM.exe Data/sphere52r1.mommatrix Data/sphere52r1.rhs Data/sphere52r1.coef" );
% system( "ObserveRCS.exe Data/sphere52r1.tetramesh Data/sphere52r1.tetraface Data/sphere52r1.tetraquad Data/sphere52r1.emprops Data/PlaneXinZpol.incwave Data/sphere52r1.coef Data/xycircular361.obsangle Data/sphere52r1.rcsresult" );

% 	string meshFileName = argv[ 1 ];
% 	string faceFileName = argv[ 2 ];
% 	string tetqFileName = argv[ 3 ];
% 	string propFileName = argv[ 4 ];
% 	string waveFileName = argv[ 5 ];
% 	string coefFileName = argv[ 6 ];
% 	string obsaFileName = argv[ 7 ];
% 	string rcsrFileName = argv[ 8 ];
    
    