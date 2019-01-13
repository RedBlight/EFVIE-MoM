clc;
clear all;
close all;

%%% INPUT %%%
freqStart = 100000;
freqEnd = 30000000;
freqCount = 4;

incDir = [ -1; 0; 0 ];
polDir = [ 0; 1; 0 ];
phaseOrigin = [ 0; 0; 0 ];

%%% FEKO ENTRY STR %%%
nl_ = string( newline );
GenerateFekoEntryStr = @( f_, xr_, xi_, yr_, yi_, zr_, zi_ ) ...
"#Request Name: MidPoint" + nl_ + ...
"#Frequency: " + string( sprintf('%1.14E', f_) ) + nl_ + ...
"#Coordinate System: Cartesian" + nl_ + ...
"#No. of X Samples: 1" + nl_ + ...
"#No. of Y Samples: 1" + nl_ + ...
"#No. of Z Samples: 1" + nl_ + ...
"#Result Type: Electric Field Values" + nl_ + ...
"#No. of Header Lines: 1" + nl_ + ...
string( '#                 "X"               "Y"               "Z"               "Re(Ex)"          "Im(Ex)"          "Re(Ey)"          "Im(Ey)"          "Re(Ez)"          "Im(Ez)"          ' ) + nl_ + ...
"                   2.00000000E-00    2.00000000E-00    2.00000000E-00   " + ...
string( sprintf('%1.8E', xr_) ) + "   " + ...
string( sprintf('%1.8E', xi_) ) + "    " + ...
string( sprintf('%1.8E', yr_) ) + "    " + ...
string( sprintf('%1.8E', yi_) ) + "   " + ...
string( sprintf('%1.8E', zr_) ) + "   " + ...
string( sprintf('%1.8E', zi_) ) + "   " + ...
nl_ + nl_ ...
;

%%% FEKO START STR %%%
fekoStartStr = ...
"##File Type: Electric near field" + nl_ + ...
"##File Format: 3" + nl_ + ...
"##Source: tet1" + nl_ + ...
"##Date: 2018-07-01 12:34:53" + nl_ + ...
"** File exported by POSTFEKO version 7.0.1-256428 (x64)" + nl_ + ...
nl_ ...
;

%%% BEGIN %%%

fileStr = fekoStartStr;
freqList = linspace( freqStart, freqEnd, freqCount );

v0 = [ 0; 0; 0 ];
vx = [ 1; 0; 0 ];
vy = [ 0; 1; 0 ];
vz = [ 0; 0; 1 ];
m = ( v0 + vx + vy + vz ) ./ 4;

m = [ 2; 2; 2 ];

eps0 = 8.85418781761e-12;
mu0 = pi * 4e-7;
c = 1 / sqrt( eps0 * mu0 );

%%%

[ ...
	vertexCount, faceCount, tetraCount, ...
	vertexList, tetraVertexIndex, faceVertexIndex, ...
	tetraFaceIndex, faceTetraIndex ...
] = GenerateMesh_UnvFile("sphere1r5p591t");

%%%

[ faceArea ] = GenerateFaceArea( faceCount, vertexList, faceVertexIndex );
[ tetraVol ] = GenerateTetraVol( tetraCount, vertexList, tetraVertexIndex );

[ faceMidPoint ] = GenerateFaceMidPoint( faceCount, vertexList, faceVertexIndex );
[ tetraMidPoint ] = GenerateTetraMidPoint( tetraCount, vertexList, tetraVertexIndex );

[ swgVertexIndex ] = GenerateSwgVertexIndex( faceCount, tetraVertexIndex, faceVertexIndex, faceTetraIndex );

[ faceQuadCount, faceQuadW, faceQuadR ] = GenerateFaceQuad( faceCount, vertexList, faceVertexIndex, 2 );
[ tetraQuadCount, tetraQuadW, tetraQuadR ] = GenerateTetraQuad( tetraCount, vertexList, tetraVertexIndex, 4 );

for freq = freqList
	
	epsrStart = 1;
	epsrEnd = 50;
	
	epsr = 20;
	fprintf( '%f\n', freq );
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
angFreq = 2 * pi * freq;
waveLen = M.c0 / freq;
waveNum = 2 * pi / waveLen;

%%%

incWave = PlaneWave( ...
	waveNum, ...
	incDir, ...
	polDir, ...
	phaseOrigin, ...
	1000, 0, ...
	0, 0 ...
);



[ epsRel ] = GenerateEpsRel_Const( tetraCount, epsr );
[ kontrast ] = GenerateKontrast( epsRel );
[ kontrastDiff ] = GenerateKontrastDiff( kontrast, faceCount, faceTetraIndex );

% [ I21 ] = GenerateI21( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR );
% [ I22 ] = GenerateI22( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR );
% [ I23 ] = GenerateI23( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR );
% 
% [ I3SS ] = GenerateI3SS( waveNum, vertexList, faceCount, faceVertexIndex, faceQuadCount, faceQuadW, faceQuadR );
% [ I3VS ] = GenerateI3VS( waveNum, vertexList, faceCount, tetraCount, tetraVertexIndex, faceQuadCount, faceQuadW, faceQuadR, tetraQuadW, tetraQuadR );
% [ I3SV ] = GenerateI3SV( waveNum, vertexList, faceCount, tetraCount, faceVertexIndex, faceQuadW, faceQuadR, tetraQuadCount, tetraQuadW, tetraQuadR );
% [ I3VV ] = GenerateI3VV( waveNum, vertexList, tetraCount, tetraVertexIndex, tetraQuadCount, tetraQuadW, tetraQuadR );
% 
% [ C21 ] = GenerateC21( angFreq, faceCount, faceTetraIndex, kontrast, faceArea, tetraVol );
% 
% [ C31 ] = GenerateC31( faceCount, faceTetraIndex, kontrast, faceArea, tetraVol );
% [ C32 ] = GenerateC32( faceCount, kontrastDiff );
% [ C33 ] = GenerateC33( faceCount, faceTetraIndex, kontrast, faceArea, tetraVol );
% [ C34 ] = GenerateC34( faceCount, faceTetraIndex, kontrastDiff, faceArea, tetraVol );

[ I2 ] = GenerateI2( waveNum, vertexList, tetraCount, tetraMidPoint, tetraVertexIndex, tetraQuadW, tetraQuadR );
[ I3SS ] = GenerateI3SS( waveNum, faceCount, vertexList, faceVertexIndex, faceMidPoint, faceQuadW, faceQuadR );
[ I3ST ] = GenerateI3ST( waveNum, tetraCount, faceCount, vertexList, tetraVertexIndex, faceMidPoint, tetraQuadW, tetraQuadR );
[ I3TS ] = GenerateI3TS( waveNum, tetraCount, faceCount, vertexList, faceVertexIndex, tetraMidPoint, faceQuadW, faceQuadR );
[ I3TT ] = GenerateI3TT( waveNum, tetraCount, vertexList, tetraVertexIndex, tetraMidPoint, tetraQuadW, tetraQuadR );

[ momTerm1 ] = GenerateMoM_Term1( ...
	faceCount, vertexList, tetraVertexIndex, ...
	faceTetraIndex, swgVertexIndex, ...
	epsRel, faceArea, tetraVol, ...
	tetraMidPoint ...
);

[ momTerm2 ] = GenerateMoM_Term2( ...
	angFreq, faceCount, ...
	vertexList, swgVertexIndex, faceTetraIndex, ...
	faceArea, tetraVol, kontrast, ...
	tetraMidPoint, ...
	I2, I3TT ...
);

[ momTerm3 ] = GenerateMoM_Term3( ...
	faceCount, faceTetraIndex, ...
	faceArea, tetraVol, kontrast, kontrastDiff, ...
	I3TT, I3TS, I3ST, I3SS ...
);

% momTerm1c = momTerm1 .* epsr;
% momterm3c = momTerm3 ./ kontrast;
% 
% scale = 1 / 2;
% 
% K1 = scale * 200 / 100 ;
% 
% K2 = -scale * 401 / 100 ;
% 
% % 405
% 
% momMatrix = ( K1 ) .* momTerm1 + ( K2 ) .* momTerm3;


momMatrix = momTerm1 + momTerm2 + momTerm3;

[ RHS ] = GenerateRHS( ...
	incWave, ...
	faceCount, faceArea, tetraVol, ...
	vertexList, faceTetraIndex, swgVertexIndex, ...
	tetraQuadCount, tetraQuadW, tetraQuadR ...
);

%%%

coefVector = linsolve( momMatrix, RHS );

%%%
	
	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	
	% D = GetD( m, tetraCount, faceArea, tetraVol, vertexList, tetraVertexIndex, tetraFaceIndex, faceTetraIndex, swgVertexIndex, coefVector );
	
	[ Es ] = GetEs( ...
		m, freq, ...
		faceCount, faceArea, tetraVol, ...
		vertexList, swgVertexIndex, faceTetraIndex, ...
		kontrast, kontrastDiff, coefVector, ...
		faceQuadW, faceQuadR, tetraQuadW, tetraQuadR ...
	);
	
	%%%%%%%%%%%%%%%
	
% 	Cz = coefVector(1);
% 	Cy = coefVector(2);
% 	Cx = coefVector(3);
% 	C0 = coefVector(4);
% 
% 	D = sqrt(3) .* C0 .* ( m - v0 ) ...
% 		+ Cx .* ( m - vx ) ...
% 		+ Cy .* ( m - vy ) ...
% 		+ Cz .* ( m - vz );
	%D = D ./ ( 1i * w );

	%E = D ./ ( eps0 * epsr );
	E = Es;
	Er = real( E );
	Ei = imag( E );
	
	fileStr = fileStr + GenerateFekoEntryStr( freq, Er(1), Ei(1), Er(2), Ei(2), Er(3), Ei(3) );
	
end

fileId = fopen('MAHO.efe','w');
fprintf( fileId, '%s', fileStr );
fclose( fileId );

%%% END %%%



