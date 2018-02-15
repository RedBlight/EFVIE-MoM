clc;
clear all;
close all;
format long;

obsR = [ -1, -1, -1 ];
x = -1;
y = -1;
z = -1;

vert =[ ...
	0, 0, 0; ...
	1, 0, 0; ...
	0, 2, 0; ....
	0, 0, 3 ...
];

[ X, Y, Z, W ] = tetraquad( 6, vert );
F = 1 ./ sqrt( (x - X).^2 + (y-Y).^2 + (z-Z).^2 );
Q = W' * F;

disp(Q);