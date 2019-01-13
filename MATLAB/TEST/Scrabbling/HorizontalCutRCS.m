clc;
clear all;
close all;

run( "..\Misc\AddPath_" );

p = load( "..\Misc\pathList.mat" );

geomName = "tet1";
ruleName = "constant";
waveName = "plane";
obspName = "xycircular181";

MakeMesh( geomName );
MakeFace( geomName );
Electrify( geomName, ruleName );
MakeQuad( geomName );
MakeRHS( geomName, waveName );
MakeMoM( geomName, waveName );
SolveMoM( geomName );
ObserveRCS( geomName, waveName, obspName );

run( "..\Misc\RmPath_" );



