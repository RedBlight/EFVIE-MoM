clc;
clear all;
close all;

run( "..\Misc\AddPath_" );
p = load( "..\Misc\pathList.mat" );

%%% BEGIN %%%

geomName = "tet1";
ruleName = "constant";
waveName = "plane";

MakeMesh( geomName );
MakeFace( geomName );
Electrify( geomName, ruleName );
MakeQuad( geomName );
MakeRHS( geomName, waveName );
MakeMoM( geomName, waveName );
SolveMoM( geomName );

%%%

epsr = 2;
lambda = 30;

eps0 = 8.85418781761e-12;
mu0 = pi * 4e-7;
c = 1 / sqrt( eps0 * mu0 );
f = c / lambda;
w = 2 * pi * f;

v0 = [ 0; 0; 0 ];
vx = [ 1; 0; 0 ];
vy = [ 0; 1; 0 ];
vz = [ 0; 0; 1 ];
m = ( v0 + vx + vy + vz ) ./ 4;

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

%%%

%epsr=2 lam=30
Ef = [ ...
	(-4.24118324E-02) + 1i * (-2.21718325E-03); ...
	(7.77199837E-01) + 1i * (4.06871519E-02); ...
	(-4.24118324E-02) + 1i * (-2.21718325E-03) ...
];

%epsr=10 lam=30
% Ef = [ ...
% 	(-4.47450628E-02) + 1i * (-2.32836832E-03); ...
% 	(4.24001142E-01) + 1i * (2.21534909E-02); ...
% 	(-4.47450628E-02) + 1i * (-2.32836832E-03) ...
% ];
%%%


E_abs = abs( E );
Ef_abs = abs( Ef );

disp( E_abs ./ Ef_abs );

%%% END %%%

run( "..\Misc\RmPath_" );



