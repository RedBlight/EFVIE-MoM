clc;
clear all;
close all;

tic;

%%% INPUT %%%

geomName = "sphere1r6p1110t";
epsR = 50 * ( 1 - 1i * 0.3 );

obsRho = 3;
obsZ = 0;
phiCount = 64;

freq = 30E+6;
incDir = M.VecUnit( [ -1; 0; 0 ] );
polDir = M.VecUnit( [ 0; 1; 1 ] );
phaseOrigin = [ 0; 0; 0 ];

faceQuadLevel = 0; % 4^n
tetraQuadLevel = 1; % n^3

%%% BEGIN %%%

angFreq = 2 * pi * freq;
waveLen = M.c0 / freq;
waveNum = 2 * pi / waveLen;

incWave = PlaneWave( ...
	waveNum, ...
	incDir, ...
	polDir, ...
	phaseOrigin, ...
	1, 0, ...
	0, 0 ...
);

[ ...
	vertexCount, faceCount, tetraCount, ...
	vertexList, tetraVertexIndex, faceVertexIndex, ...
	tetraFaceIndex, faceTetraIndex ...
] = GenerateMesh_UnvFile( geomName );

[ faceArea ] = GenerateFaceArea( faceCount, vertexList, faceVertexIndex );
[ tetraVol ] = GenerateTetraVol( tetraCount, vertexList, tetraVertexIndex );

[ faceMidPoint ] = GenerateFaceMidPoint( faceCount, vertexList, faceVertexIndex );
[ tetraMidPoint ] = GenerateTetraMidPoint( tetraCount, vertexList, tetraVertexIndex );

[ swgVertexIndex ] = GenerateSwgVertexIndex( faceCount, tetraVertexIndex, faceVertexIndex, faceTetraIndex );

% [ faceQuadCount, faceQuadW, faceQuadR ] = GenerateFaceQuad( faceCount, vertexList, faceVertexIndex, faceQuadLevel );
% [ tetraQuadCount, tetraQuadW, tetraQuadR ] = GenerateTetraQuad( tetraCount, vertexList, tetraVertexIndex, tetraQuadLevel );

[ epsRel ] = GenerateEpsRel_Const( tetraCount, epsR );
[ kontrast ] = GenerateKontrast( epsRel );
[ kontrastDiff ] = GenerateKontrastDiff( kontrast, faceCount, faceTetraIndex );

[ lastSingleBaseId ] = M.FindMin1( faceTetraIndex( 2, : ) );

disp('Init'); toc; tic;

[ relTT, relTS, relSS ] = GenerateElemRel( faceCount, tetraCount, tetraVertexIndex, faceVertexIndex );

disp('Rel'); toc; tic;

%[ I2 ] = GenerateI2( waveNum, vertexList, tetraCount, tetraVol, tetraMidPoint, tetraVertexIndex );
[ I3SS ] = GenerateI3SS( lastSingleBaseId, waveNum, faceCount, vertexList, faceVertexIndex, faceMidPoint, faceArea, relSS );
[ I3ST ] = GenerateI3ST( lastSingleBaseId, waveNum, tetraCount, faceCount, vertexList, tetraVertexIndex, faceMidPoint, tetraMidPoint, tetraVol, relTS );
[ I3TS ] = GenerateI3TS( waveNum, tetraCount, faceCount, vertexList, faceVertexIndex, tetraMidPoint, faceMidPoint, faceArea, relTS );
[ I2, I3TT ] = GenerateI3TT( waveNum, tetraCount, vertexList, tetraVertexIndex, tetraMidPoint, tetraVol, relTT );

disp('I2 I3'); toc; tic;

[ momTerm1 ] = GenerateMoM_Term1( ...
	faceCount, vertexList, tetraVertexIndex, ...
	faceTetraIndex, swgVertexIndex, ...
	epsRel, faceArea, tetraVol, ...
	tetraMidPoint ...
);

disp('momTerm1'); toc; tic;

[ momTerm2 ] = GenerateMoM_Term2( ...
	angFreq, faceCount, ...
	vertexList, swgVertexIndex, faceTetraIndex, ...
	faceArea, tetraVol, kontrast, ...
	tetraMidPoint, ...
	I2, I3TT ...
);

disp('momTerm2'); toc; tic;

[ momTerm3 ] = GenerateMoM_Term3( ...
	faceCount, faceTetraIndex, ...
	faceArea, tetraVol, kontrast, kontrastDiff, ...
	I3TT, I3TS, I3ST, I3SS ...
);

disp('momTerm3'); toc; tic;

momMatrix = momTerm1 + momTerm2 + momTerm3;


disp('Mom Mat'); toc; tic;

% [ momMatrix ] = GenerateMomMatrix( ...
% 	lastSingleBaseId, faceCount, angFreq, vertexList, ...
% 	tetraVertexIndex, faceTetraIndex, swgVertexIndex, ...
% 	epsRel, faceArea, tetraVol, tetraMidPoint, ...
% 	kontrast, kontrastDiff, relSS, ...
% 	I2, I3TT, I3TS, I3ST, I3SS ...
% );

[ RHS ] = GenerateRHS( ...
	incWave, ...
	faceCount, faceArea, ...
	vertexList, faceTetraIndex, swgVertexIndex, ...
	tetraMidPoint ...
);

coefVector = linsolve( momMatrix, RHS );

%%% POST

fekoFile = FekoFile_Phi( "MAHO", freq );

phiVec = linspace( 0, 360, phiCount );
rhoVec = repmat( obsRho, 1, phiCount );
zVec = repmat( obsZ, 1, phiCount );

[ xVec, yVec, zVec ] = pol2cart( degtorad( phiVec ), rhoVec, zVec );

for idp = 1 : phiCount
	
	x = xVec( idp );
	y = yVec( idp );
	z = zVec( idp );
	
	[ Es ] = GetEs( ...
		[ x; y; z ], freq, ...
		faceCount, faceArea, tetraVol, ...
		vertexList, swgVertexIndex, faceTetraIndex, ...
		kontrast, kontrastDiff, coefVector, ...
		faceMidPoint, tetraMidPoint ...
	);

	Er = real( Es );
	Ei = imag( Es );
	
	%[ rhoF, phiF, zF ] = M.CartToPolBasis( x, y, z, xF, yF, zF );
	
	[ Err, Epr, Ezr ] = M.CartToPolBasis( x, y, z, Er( 1 ), Er( 2 ), Er( 3 ) );
	[ Eri, Epi, Ezi ] = M.CartToPolBasis( x, y, z, Ei( 1 ), Ei( 2 ), Ei( 3 ) );
	
	fekoFile.InsertEntry( rhoVec( idp ), phiVec( idp ), zVec( idp ), Err, Eri, Epr, Epi, Ezr, Ezi );
	
end

fekoFile.SaveFile();



toc;






% 
% 
% 
% 
% for freq = freqList
% 	
% 	epsrStart = 1;
% 	epsrEnd = 50;
% 	
% 	epsr = 20;
% 	fprintf( '%f\n', freq );
% 	
% 	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 	
% 
% %%%
% 
% 
% 
% 
% 
% [ epsRel ] = GenerateEpsRel_Const( tetraCount, epsr );
% [ kontrast ] = GenerateKontrast( epsRel );
% [ kontrastDiff ] = GenerateKontrastDiff( kontrast, faceCount, faceTetraIndex );
% 
% [ I2 ] = GenerateI2( waveNum, vertexList, tetraCount, tetraMidPoint, tetraVertexIndex, tetraQuadW, tetraQuadR );
% [ I3SS ] = GenerateI3SS( waveNum, faceCount, vertexList, faceVertexIndex, faceTetraIndex, faceMidPoint, faceQuadW, faceQuadR );
% [ I3ST ] = GenerateI3ST( waveNum, tetraCount, faceCount, vertexList, tetraVertexIndex, faceTetraIndex, faceMidPoint, tetraQuadW, tetraQuadR );
% [ I3TS ] = GenerateI3TS( waveNum, tetraCount, faceCount, vertexList, faceVertexIndex, tetraMidPoint, faceQuadW, faceQuadR );
% [ I3TT ] = GenerateI3TT( waveNum, tetraCount, vertexList, tetraVertexIndex, tetraMidPoint, tetraQuadW, tetraQuadR );
% 
% [ momTerm1 ] = GenerateMoM_Term1( ...
% 	faceCount, vertexList, tetraVertexIndex, ...
% 	faceTetraIndex, swgVertexIndex, ...
% 	epsRel, faceArea, tetraVol, ...
% 	tetraMidPoint ...
% );
% 
% [ momTerm2 ] = GenerateMoM_Term2( ...
% 	angFreq, faceCount, ...
% 	vertexList, swgVertexIndex, faceTetraIndex, ...
% 	faceArea, tetraVol, kontrast, ...
% 	tetraMidPoint, ...
% 	I2, I3TT ...
% );
% 
% [ momTerm3 ] = GenerateMoM_Term3( ...
% 	faceCount, faceTetraIndex, ...
% 	faceArea, tetraVol, kontrast, kontrastDiff, ...
% 	I3TT, I3TS, I3ST, I3SS ...
% );
% 
% % momTerm1c = momTerm1 .* epsr;
% % momterm3c = momTerm3 ./ kontrast;
% % 
% % scale = 1 / 2;
% % 
% % K1 = scale * 200 / 100 ;
% % 
% % K2 = -scale * 401 / 100 ;
% % 
% % % 405
% % 
% % momMatrix = ( K1 ) .* momTerm1 + ( K2 ) .* momTerm3;
% 
% 
% momMatrix = momTerm1 + momTerm2 + momTerm3;
% 
% [ RHS ] = GenerateRHS( ...
% 	incWave, ...
% 	faceCount, faceArea, tetraVol, ...
% 	vertexList, faceTetraIndex, swgVertexIndex, ...
% 	tetraQuadCount, tetraQuadW, tetraQuadR ...
% );
% 
% %%%
% 
% coefVector = linsolve( momMatrix, RHS );
% 
% %%%
% 	
% 	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% 	
% 	% D = GetD( m, tetraCount, faceArea, tetraVol, vertexList, tetraVertexIndex, tetraFaceIndex, faceTetraIndex, swgVertexIndex, coefVector );
% 	
% 	[ Es ] = GetEs( ...
% 		m, freq, ...
% 		faceCount, faceArea, tetraVol, ...
% 		vertexList, swgVertexIndex, faceTetraIndex, ...
% 		kontrast, kontrastDiff, coefVector, ...
% 		faceQuadW, faceQuadR, tetraQuadW, tetraQuadR ...
% 	);
% 	
% 	%%%%%%%%%%%%%%%
% 	
% % 	Cz = coefVector(1);
% % 	Cy = coefVector(2);
% % 	Cx = coefVector(3);
% % 	C0 = coefVector(4);
% % 
% % 	D = sqrt(3) .* C0 .* ( m - v0 ) ...
% % 		+ Cx .* ( m - vx ) ...
% % 		+ Cy .* ( m - vy ) ...
% % 		+ Cz .* ( m - vz );
% 	%D = D ./ ( 1i * w );
% 
% 	%E = D ./ ( eps0 * epsr );
% 	E = Es;
% 	Er = real( E );
% 	Ei = imag( E );
% 	
% 	fileStr = fileStr + GenerateFekoEntryStr( freq, Er(1), Ei(1), Er(2), Ei(2), Er(3), Ei(3) );
% 	
% end
% 
% fileId = fopen('MAHO.efe','w');
% fprintf( fileId, '%s', fileStr );
% fclose( fileId );
% 
% %%% END %%%



