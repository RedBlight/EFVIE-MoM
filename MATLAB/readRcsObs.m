clc;
clear all;
close all;

% A = [ 2 .* pi .* (0 : 360) ./ 360; repmat( pi, 1, 361 ) ];
% 
% A = double( reshape( A, 1, 722 ) );
% 
% fileId = fopen( 'Data/xycircular361.obsangle', 'w' );
% 
% fwrite( fileId, 361, 'int64' );
% fwrite( fileId, A, 'double' );



fileID = fopen( 'Data/sphere52r1.rcsresult' );
A = fread( fileID, 182, 'double' );

figure();
plot( -180:2:180, A( 2 : 182 ) )

fclose( fileID );
