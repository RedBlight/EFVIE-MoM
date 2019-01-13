clc;
clear all;
close all;

run( "..\Misc\AddPath_" );
p = load( "..\Misc\pathList.mat" );

%%% INPUT %%%
freqStart = 1;
freqEnd = 50;
freqCount = 64;
epsr = 10;

%%% FEKO ENTRY STR %%%
nl_ = string( newline );
GenerateFekoEntryStr = @( f_, xr_, xi_, yr_, yi_, zr_, zi_ ) ...
"#Request Name: MidPoint" + nl_ + ...
"#Frequency: " + string( sprintf('%1.8E', f_) ) + nl_ + ...
"#Coordinate System: Cartesian" + nl_ + ...
"#No. of X Samples: 1" + nl_ + ...
"#No. of Y Samples: 1" + nl_ + ...
"#No. of Z Samples: 1" + nl_ + ...
"#Result Type: Electric Field Values" + nl_ + ...
"#No. of Header Lines: 1" + nl_ + ...
string( '#                 "X"               "Y"               "Z"               "Re(Ex)"          "Im(Ex)"          "Re(Ey)"          "Im(Ey)"          "Re(Ez)"          "Im(Ez)"          ' ) + nl_ + ...
"                   2.50000000E-01    2.50000000E-01    2.50000000E-01   " + ...
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

geomName = "tet1";
ruleName = "constant";
waveName = "plane";

eps0 = 8.85418781761e-12;
mu0 = pi * 4e-7;
c = 1 / sqrt( eps0 * mu0 );

MakeMesh( geomName );
MakeFace( geomName );
MakeQuad( geomName );

for freq = freqList
	
	
	epsrStart = 1;
	epsrEnd = 50;
	
	epsr = epsrStart + (epsrEnd-epsrStart) * (freq - freqStart) / (freqEnd-freqStart);
	
	SetEmrule( epsr );

	Electrify( geomName, ruleName );
	
	lambda = c / freq;
	w = 2 * pi * freq;
	
	SetIncwave( ...
		lambda, ...
		-1, 0, 0, ...
		0, 1, 0 ...
	);
	
	MakeRHS( geomName, waveName );
	MakeMoM( geomName, waveName );
	SolveMoM( geomName );
	
	[ faceCount, coefVector ] = LoadCoef( geomName );
	
	Cz = coefVector(1);
	Cy = coefVector(2);
	Cx = coefVector(3);
	C0 = coefVector(4);

	D = sqrt(3) .* C0 .* ( m - v0 ) ...
		+ Cx .* ( m - vx ) ...
		+ Cy .* ( m - vy ) ...
		+ Cz .* ( m - vz );
	D = D ./ ( 1i * w );

	E = D ./ ( eps0 * epsr );
	Er = real( E );
	Ei = imag( E );
	
	fileStr = fileStr + GenerateFekoEntryStr( freq, Er(1), Ei(1), Er(2), Ei(2), Er(3), Ei(3) );
	
end

fileId = fopen('MAHO.efe','w');
fprintf( fileId, '%s', fileStr );
fclose( fileId );

%%% END %%%

run( "..\Misc\RmPath_" );



