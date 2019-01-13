clc;
clear all;
close all;

%%%

freq = 300E+6;
epsr = 1;

angFreq = 2 * pi * freq;
waveLen = M.c0 / freq;
waveNum = 2 * pi / waveLen;

%%%

incDir = M.VecUnit( [ -1; 1; 0 ] );
polDir = M.VecUnit( [ 1; 1; 0 ] );
phaseOrigin = [ 0; 0; 0 ];

incWave = PlaneWave( ...
	waveNum, ...
	incDir, ...
	polDir, ...
	phaseOrigin, ...
	1, 0, ...
	0, 0 ...
);

Nx = 128;
Ny = 128;

z = 5;
r = waveLen * 2;


rZ = repmat( z, Ny, Nx );
rX = repmat( linspace(-r,r,Nx), Ny, 1 );
rY = repmat( linspace(-r,r,Ny)', 1, Nx );

E = zeros( 3, Ny, Nx );

for m = 1 : Ny
	for k = 1 : Nx
		E( :, m, k ) = incWave.FieldE( [ rX( m, k ); rY( m, k ); rZ( m, k ) ] );
	end
end





figure();
imagesc( squeeze( real( E( 2, :, : ) ) ) );
axis xy;


