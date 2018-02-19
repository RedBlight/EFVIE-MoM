clc;
clear all;
close all;

A = [ 2 .* pi .* (0 : 2 : 360) ./ 360; repmat( pi/2, 1, 181 ) ];

A = double( reshape( A, 1, 362 ) );

fileId = fopen( 'Data/xycircular181.obsangle', 'w' );

fwrite( fileId, 181, 'uint64' );
fwrite( fileId, A, 'double' );

fclose( fileId );


fileId = fopen( 'Data/sphere45x90.obsangle', 'w' );
N = 45;
tMaxN = (N+1);
pMaxN = ((2*N)+1);
fwrite( fileId, tMaxN*pMaxN, 'uint64' );
for thetaN = 1:tMaxN
	for phiN = 1:pMaxN
		fwrite( fileId, ((phiN-1)/(pMaxN-1))*(2 * pi), 'double' );
		fwrite( fileId, ((thetaN-1)/(tMaxN-1))*( pi), 'double' );
	end
end
fclose( fileId );