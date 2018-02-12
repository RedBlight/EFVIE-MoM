clc;
clear all;
close all;

A = [ 2 .* pi .* (-180 : 2 : 180) ./ 360; repmat( pi/2, 1, 181 ) ];

A = double( reshape( A, 1, 362 ) );

fileId = fopen( 'Data/xycircular361.obsangle', 'w' );

fwrite( fileId, 181, 'int64' );
fwrite( fileId, A, 'double' );

fclose( fileId );
