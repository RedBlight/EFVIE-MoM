clc;
clear all;
close all;

system( "MakeMesh.exe Data/sphere.unv Data/sphere.tetramesh" );

system( "MakeFace.exe Data/sphere.tetramesh Data/sphere.tetraface" );

system( "Electrify.exe Data/sphere.tetramesh Data/sphere.emrule Data/sphere.emprops" );

system( "MakeQuad.exe Data/sphere.tetramesh Data/sphere.tetraface Data/sphere.tetraquad Data/sphere.triquad" );

