%clc;
%clear all;
%close all;

eps0 = 8.854187817E-12;
epsr = 2;

epsFactor = 1 / ( eps0 * epsr );

IntJP = @( rj ) ...
	0.25 .* ( 0.4 - dot( rj, [1/6, 1/6, 1/6] ) + dot( rj, -[ 1/3, 1/3, 1/3 ] ) ) ...
;

IntJM = @( rj ) ...
	0.25 .* ( -0.6 - dot( rj, [1/2, 1/2, 1/2] ) + dot( rj, [ 1, 1, 1 ] ) ) ...
;

rp = [ -1/3, -1/3, -1/3 ];
rm = [ 1, 1, 1 ];

mat11 = epsFactor * IntJP( rp ) + epsFactor * IntJM( rm );

rx = [ 1, 0, 0 ];
ry = [ 0, 1, 0 ];
rz = [ 0, 0, 1 ];


mat12 = epsFactor * IntJP( rx )
mat13 = epsFactor * IntJP( ry )
mat14 = epsFactor * IntJP( rz )


