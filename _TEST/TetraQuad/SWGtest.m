clc;
clear all;
close all;
format long;

vert =[ ...
	1/sqrt(3), 0, 0; ...
	-sqrt(3)/6, 1/2, 0; ...
	-sqrt(3)/6, -1/2, 0; ....
	0, 0, sqrt(6)/3 ...
];

obsR = [ -1, -1, -1 ];
lambda = 300;
k = 2*pi/lambda;

vert1 =[ ...
	0, 0, 0; ...
	10, 0, 0; ...
	0, 20, 0; ....
	0, 0, 30 ...
];

vert2 =[ ...
	0, 0, 0; ...
	20, 0, 0; ...
	0, 30, 0; ....
	0, 0, 10 ...
] + 50;

Q = 0;
Qp = 0;
for n = 1 : 6
	n1 = n;
	n2 = n;
	Qp = Q;
	[ X1, Y1, Z1, W1 ] = tetraquad( n1, vert1 );
	[ X2, Y2, Z2, W2 ] = tetraquad( n2, vert2 );
	
	L1 = n1.^3;
	L2 = n2.^3;
	
	X1 = repmat( X1, 1, L2 );
	Y1 = repmat( Y1, 1, L2 );
	Z1 = repmat( Z1, 1, L2 );
	W1 = repmat( W1, 1, L2 );
	
	X2 = repmat( X2', L1, 1 );
	Y2 = repmat( Y2', L1, 1 );
	Z2 = repmat( Z2', L1, 1 );
	W2 = repmat( W2', L1, 1 );
	
	R = sqrt( ( X1 - X2 ).^2 + ( Y1 - Y2 ).^2 + ( Z1 - Z2 ).^2 );
	
	F = X2 .* ( exp( -1i .* k .* R ) - 1 ) ./ R;
	
	Q = sum( sum( W1 .* W2 .* F ) );
	
	disp( n );
	disp( Q );
	disp( 100 * real( Q - Qp ) / real(Q)  );
	disp( 100 * imag( Q - Qp ) / imag(Q)  );
end


% Q = 0;
% Qp = 0;
% for n = 1 : 6
% 	Qp = Q;
% 	[ X, Y, Z, W ] = tetraquad( n, vert );
% 	%R = sqrt( (obsR(1) - X).^2 + (obsR(2) - Y).^2 + (obsR(3) - Z).^2 );
% 	F = ones( size(Y) );
% 	Q = W' * F;
% 	disp( n );
% 	disp( Q );
% 	disp( Q - Qp );
% end

% Q = 0;
% Qp = 0;
% for n = 1 : 6
% 	Qp = Q;
% 	[ X, Y, Z, W ] = tetraquad( n, vert );
% 	F = Z.^2;
% 	Q = W' * F;
% 	disp( n );
% 	disp( Q );
% 	disp( Q - Qp );
% end


% [ X, Y, Z, W ] = tetraquad( 1, vert );
% F = Z.^1;
% Q1 = W' * F;
% disp( Q1 );
% 
% [ X, Y, Z, W ] = tetraquad( 2, vert );
% F = Z.^1;
% Q2 = W' * F;
% disp( Q2 );
% 
% 
% [ X, Y, Z, W ] = tetraquad( 3, vert );
% F = Z.^1;
% Q3 = W' * F;
% disp( Q3 );
% 
% 
% [ X, Y, Z, W ] = tetraquad( 4, vert );
% F = Z.^1;
% Q4 = W' * F;
% disp( Q4 );
% 
% [ X, Y, Z, W ] = tetraquad( 5, vert );
% F = Z.^1;
% Q5 = W' * F;
% disp( Q5 );