clc;
clear all;
close all;

%%%

freq = 30E+6;
epsr = 50;

angFreq = 2 * pi * freq;
waveLen = M.c0 / freq;
waveNum = 2 * pi * waveLen;

%%%

incDir = [ -1; 0; 0 ];
polDir = [ 0; 1; 0 ];
phaseOrigin = [ 0; 0; 0 ];

incWave = PlaneWave( ...
	waveNum, ...
	incDir, ...
	polDir, ...
	phaseOrigin, ...
	1, 0, ...
	0, 0 ...
);

%%%

[ ...
	vertexCount, faceCount, tetraCount, ...
	vertexList, tetraVertexIndex, faceVertexIndex, ...
	tetraFaceIndex, faceTetraIndex ...
] = GenerateMesh_UnvFile("tet23");

%%%

[ faceArea ] = GenerateFaceArea( faceCount, vertexList, faceVertexIndex );
[ tetraVol ] = GenerateTetraVol( tetraCount, vertexList, tetraVertexIndex );

[ faceMidPoint ] = GenerateFaceMidPoint( faceCount, vertexList, faceVertexIndex );
[ tetraMidPoint ] = GenerateTetraMidPoint( tetraCount, vertexList, tetraVertexIndex );

[ swgVertexIndex ] = GenerateSwgVertexIndex( faceCount, tetraVertexIndex, faceVertexIndex, faceTetraIndex );

[ faceQuadCount, faceQuadW, faceQuadR ] = GenerateFaceQuad( faceCount, vertexList, faceVertexIndex, 2 );
[ tetraQuadCount, tetraQuadW, tetraQuadR ] = GenerateTetraQuad( tetraCount, vertexList, tetraVertexIndex, 4 );

[ epsRel ] = GenerateEpsRel_Const( tetraCount, epsr );
[ kontrast ] = GenerateKontrast( epsRel );
[ kontrastDiff ] = GenerateKontrastDiff( kontrast, faceCount, faceTetraIndex );

[ I21 ] = GenerateI21( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR );
[ I22 ] = GenerateI22( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR );
[ I23 ] = GenerateI23( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR );

[ I3SS ] = GenerateI3SS( waveNum, vertexList, faceCount, faceVertexIndex, faceQuadCount, faceQuadW, faceQuadR );
[ I3VS ] = GenerateI3VS( waveNum, vertexList, faceCount, tetraCount, tetraVertexIndex, faceQuadCount, faceQuadW, faceQuadR, tetraQuadW, tetraQuadR );
[ I3SV ] = GenerateI3SV( waveNum, vertexList, faceCount, tetraCount, faceVertexIndex, faceQuadW, faceQuadR, tetraQuadCount, tetraQuadW, tetraQuadR );
[ I3VV ] = GenerateI3VV( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR );

[ C21 ] = GenerateC21( angFreq, faceCount, faceTetraIndex, kontrast, faceArea, tetraVol );

[ C31 ] = GenerateC31( faceCount, faceTetraIndex, kontrast, faceArea, tetraVol );
[ C32 ] = GenerateC32( faceCount, kontrastDiff );
[ C33 ] = GenerateC33( faceCount, faceTetraIndex, kontrast, faceArea, tetraVol );
[ C34 ] = GenerateC34( faceCount, faceTetraIndex, kontrastDiff, faceArea, tetraVol );

[ momTerm1 ] = GenerateMoM_Term1( ...
	faceCount, vertexList, tetraVertexIndex, ...
	faceTetraIndex, swgVertexIndex, ...
	epsRel, faceArea, tetraVol, ...
	tetraMidPoint ...
);

[ momTerm2 ] = GenerateMoM_Term2( ...
	faceCount, faceTetraIndex, ...
	vertexList, swgVertexIndex, ...
	I21, I22, I23, I3VV, ...
	C21 ...
);

[ momTerm3 ] = GenerateMoM_Term3( ...
	faceCount, faceTetraIndex, ...
	I3VV, I3VS, I3SV, I3SS, ...
	C31, C32, C33, C34 ...
);

momMatrix = momTerm1 + momTerm2 + momTerm3;

[ RHS ] = GenerateRHS( ...
	incWave, ...
	faceCount, faceArea, tetraVol, ...
	vertexList, faceTetraIndex, swgVertexIndex, ...
	tetraQuadCount, tetraQuadW, tetraQuadR ...
);

%%%

coefVec = linsolve( momMatrix, RHS );

%%%





figure();
imagesc( momTerm1 );
axis ij;

figure();
imagesc( abs( momTerm2 ) );
axis ij;

figure();
imagesc( abs( momTerm3 ) );
axis ij;


figure();
imagesc( abs( momMatrix ) );
axis ij;

% figure();
% imagesc( C31 );
% axis ij;
% 
% figure();
% imagesc( C32 );
% axis ij;
% 
% figure();
% imagesc( C33 );
% axis ij;
% 
% figure();
% imagesc( C34 );
% axis ij;

% figure();
% imagesc( abs( momTerm3 ) );
% axis ij;
% 
% figure();
% imagesc( abs( momTerm3 + momTerm1 ) );
% axis ij;

% 
% 
% 
% figure();
% imagesc( abs( I3SS ) );
% axis ij;
% 
% figure();
% imagesc( abs( I3VS ) );
% axis ij;
% 
% figure();
% imagesc( abs( I3SV ) );
% axis ij;
% 
% figure();
% imagesc( abs( I3VV ) );
% axis ij;

% figure();
% pcolor( imag( momTerm3  ) );
% axis ij;
% 
% figure();
% pcolor( abs( momTerm3 ) );
% axis ij;

