clc;
clear all;
close all;

A = [ 2 .* pi .* (0 : 2 : 360) ./ 360; repmat( pi/2, 1, 181 ) ];

A = double( reshape( A, 1, 362 ) );

fileId = fopen( 'Data/xycircular181.obsangle', 'w' );

fwrite( fileId, 181, 'uint64' );
fwrite( fileId, A, 'double' );

fclose( fileId );