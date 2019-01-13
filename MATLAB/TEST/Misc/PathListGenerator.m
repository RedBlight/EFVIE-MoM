clc;
clear all;
close all;

rootFolder = "D:\___WORKSPACE\EFVIE-MoM\MATLAB\TEST\";

applFolderName = "Applet";
dataFolderName = "Data";
execFolderName = "Executer";
loadFolderName = "Loader";
miscFolderName = "Misc";
scrbFolderName = "Scrabbling";
settFolderName = "Setter";

meshAppName = "MakeMesh.exe";
faceAppName = "MakeFace.exe";
elecAppName = "Electrify.exe";
quadAppName = "MakeQuad.exe";
rhsvAppName = "MakeRHS.exe";
mommAppName = "MakeMoM.exe";
solvAppName = "SolveMoM.exe";
orcsAppName = "ObserveRCS.exe";
scatAppName = "ObserveScat.exe";

ruleExt = ".emrule";
waveExt = ".incwave";
obspExt = ".obsangle";

unvmExt = ".unv";
meshExt = ".tetramesh";
faceExt = ".tetraface";
propExt = ".emprops";
tetqExt = ".tetraquad";
triqExt = ".triquad";
rhsvExt = ".rhs";
mommExt = ".mommatrix";
coefExt = ".coef";
rcsrExt = ".rcsresult";

applFolder = rootFolder + applFolderName + "\";
dataFolder = rootFolder + dataFolderName + "\";
execFolder = rootFolder + execFolderName + "\";
loadFolder = rootFolder + loadFolderName + "\";
miscFolder = rootFolder + miscFolderName + "\";
scrbFolder = rootFolder + scrbFolderName + "\";
settFolder = rootFolder + settFolderName + "\";

meshApp = applFolder + meshAppName;
faceApp = applFolder + faceAppName;
elecApp = applFolder + elecAppName;
quadApp = applFolder + quadAppName;
rhsvApp = applFolder + rhsvAppName;
mommApp = applFolder + mommAppName;
solvApp = applFolder + solvAppName;
orcsApp = applFolder + orcsAppName;
scatApp = applFolder + scatAppName;

save( "pathList.mat" );


















