clc;
clear all;
close all;

system( "Applets/MakeMesh.exe Data/sphere.unv Data/sphere.tetramesh" );

system( "Applets/MakeFace.exe Data/sphere.tetramesh Data/sphere.tetraface" );

system( "Applets/Electrify.exe Data/sphere.tetramesh Data/sphere.emrule Data/sphere.emprops" );

system( "Applets/MakeQuadrature.exe Data/sphere.tetramesh Data/sphere.tetraface Data/sphere.tetraquad Data/sphere.triquad" );

