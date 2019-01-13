clc;
clear all;
close all;

%%%

epsr = 2;

[ ...
	vertexCount, faceCount, tetraCount, ...
	vertexList, tetraVertexIndex, faceVertexIndex, ...
	tetraFaceIndex, faceTetraIndex ...
] = GenerateCaltropMesh();

%%%

[ epsRel ] = GenerateEpsRel_Const( tetraCount, epsr );
[ kontrast ] = GenerateKontrast( epsRel );

[ faceArea ] = GenerateFaceArea( faceCount, vertexList, faceVertexIndex );
[ tetraVol ] = GenerateTetraVol( tetraCount, vertexList, tetraVertexIndex );

[ faceMidPoint ] = GenerateFaceMidPoint( faceCount, vertexList, faceVertexIndex );
[ tetraMidPoint ] = GenerateTetraMidPoint( tetraCount, vertexList, tetraVertexIndex );

[ swgVertexIndex ] = GenerateSwgVertexIndex( faceCount, tetraVertexIndex, faceVertexIndex, faceTetraIndex );

[ momTerm1 ] = GenerateMoM_Term1( ...
	faceCount, ...
	vertexList, tetraVertexIndex, ...
	faceTetraIndex, ...
	swgVertexIndex, ...
	epsRel, faceArea, tetraVol, ...
	tetraMidPoint ...
);

%%%

figure();
pcolor( momTerm1 );
axis ij;

